//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "TransportLayer.h"
#include "ASSDriver.h"
#include "ASSUtils.h"

#include <asl/Assure.h>

using namespace std;
using namespace asl;

static const char * DriverStateStrings[] = {
    "not_connected",
    "synchronizing",
    "running",
    "configuring",
    ""
};

IMPLEMENT_ENUM( y60::DriverState, DriverStateStrings );

namespace y60 {

extern const char * CMD_ENTER_CONFIG_MODE;
extern const char * CMD_QUERY_CONFIG_MODE;
extern const char * CMD_LEAVE_CONFIG_MODE;

extern const char * CMD_CALLIBRATE_TRANSMISSION_LEVEL;
extern const char * CMD_PERFORM_TARA;

const unsigned char MAGIC_TOKEN( 255 );

const std::string OK_STRING("\r\nOK\r\n");
const std::string ERROR_STRING("\r\nERR");
const double COMMAND_TIMEOUT( 10 );

// TODO: clean up this stuff ...
const unsigned int MAX_NUM_STATUS_TOKENS( 10 );
const unsigned int BYTES_PER_STATUS_TOKEN( 5 );
const unsigned int MAX_STATUS_LINE_LENGTH( MAX_NUM_STATUS_TOKENS * BYTES_PER_STATUS_TOKEN + 2 );

// XXX Workaround Bug 595
DEFINE_EXCEPTION( ASSStatusTokenException, asl::Exception );

TransportLayer::TransportLayer(const char * theTransportName, const dom::NodePtr & theSettings) :
    asl::PosixThread( threadMain ),
    _myGridSize(-1, -1),
    _myState( NOT_CONNECTED ),
    _myReceiveBuffer( 1024 ),
    _myMagicTokenFlag( false ),
    _myExpectedLine( 0 ),
    _myFirmwareVersion( -1 ),
    _myFirmwareStatus( -1 ),
    _myControllerId( -1 ),
    _myFirmwareMode( -1 ),
    _myFramerate( -1 ),
    _myFrameNo( -1 ),
    _myChecksum( 0 ),
    _mySyncLostCounter( 0 ),
    _myDeviceLostCounter( 0 ),
    _myChecksumErrorCounter( 0 ),
    _myReceivedFrames( 0 ),
    _myFirstFrameFlag( true ),
    _myLastCommandTime( asl::Time() ),
    _myFrameBuffer( 0 ),
    _myRunningFlag( true ),
    _myTransportName( theTransportName )
{
    getConfigSetting( theSettings, "EventQueueSize", _myEventQueueSize, 100);

    fork();
}

TransportLayer::~TransportLayer() {

    AC_PRINT << "=== ASS Transport Layer Stats =========" << endl
             << "Frames received       : " << _myReceivedFrames << endl
             << "Synchronization errors: " << _mySyncLostCounter << endl
             << "Device lost           : " << _myDeviceLostCounter << endl
             << "Checksum errors       : " << _myChecksumErrorCounter << endl
             << "=======================================";
}


void 
TransportLayer::stopThread() {
    AC_PRINT << "Waiting for IO thread shutdown.";
    _myRunningFlag = false;
    join();
}

void 
TransportLayer::poll() {

    _myFrameQueueLock.lock();
    if (_myFrameQueue.size() > _myEventQueueSize) {
        AC_WARNING << "Event queue has more than " << _myEventQueueSize << " items. Flushing.";
        while ( ! _myFrameQueue.empty() ) {
            _myFrameQueue.pop();
        }
    }
    _myFrameQueueLock.unlock();

    switch (_myState) {
        case NOT_CONNECTED:
            establishConnection();
            // do not use a whole core while waiting for connection
            msleep( 10 ); 
            break;
        case SYNCHRONIZING:
            readData();
            synchronize();
            break;
        case RUNNING:
            readData();
            readSensorValues();
            break;
        case CONFIGURING:
            readData();
            handleConfigurationCommand();
            break;
    }
}

unsigned
TransportLayer::readStatusToken( std::vector<unsigned char>::iterator & theIt, const char theToken ) {
    if ( * theIt != theToken ) {
        throw ASSStatusTokenException(string("Failed to parse status token '") + theToken + "'. Got '" +
                char(* theIt) + "'", PLUS_FILE_LINE );
    }
    theIt += 1;
    istringstream myStream;
    myStream.setf( std::ios::hex, std::ios::basefield );
    myStream.str(string( theIt, theIt + 4 ));
    theIt += 4;
    unsigned myNumber;
    myStream >>  myNumber;
    return myNumber;
}


unsigned
TransportLayer::getBytesPerStatusLine() {
    if (_myFirmwareVersion < 0) {
        return MAX_STATUS_LINE_LENGTH;
    } else if ( _myFirmwareVersion < 260 ) {
        // one token: <version>
        return BYTES_PER_STATUS_TOKEN + 2;
    } else if (_myFirmwareVersion == 260) {
        // nine tokens: <version> <status> <id> <mode> <framerate>
        //              <width> <height> <framenumber> <checksum>
        return 9 * BYTES_PER_STATUS_TOKEN + 2;
    } else if (_myFirmwareVersion == 261) {
        // ten tokens: <version> <status> <id> <mode> <framerate>
        //             <width> <height> <gridspacing> <framenumber> <checksum>
        return 10 * BYTES_PER_STATUS_TOKEN + 2;
    }
    throw ASSException(string("Unknown firmware version: ") + as_string(_myFirmwareVersion),
            PLUS_FILE_LINE );
    return MAX_STATUS_LINE_LENGTH; // never get here ... just avoid the warning
}

size_t
TransportLayer::getBytesPerFrame() {
    // line format: <start> <lineno> <databytes>
    return (1 + 1 + _myGridSize[0]) * _myGridSize[1] + MAX_STATUS_LINE_LENGTH;
}

void
TransportLayer::parseStatusLine(/*RasterPtr & theTargetRaster*/) {
    //AC_PRINT << "==== parseStatusLine()";
    //dumpBuffer(_myTmpBuffer);
    try {
        if ( _myTmpBuffer.size() >= getBytesPerStatusLine() ) {
            ASSURE( _myTmpBuffer[0] == MAGIC_TOKEN );
            ASSURE( _myTmpBuffer[1] == _myExpectedLine );
            std::vector<unsigned char>::iterator myIt = _myTmpBuffer.begin() + 2;
            _myFirmwareVersion = readStatusToken( myIt, 'V' );
            Vector2i myGridSize;
            unsigned myChecksum;
            bool myHasChecksumFlag = true;
            if ( _myFirmwareVersion < 260 ) {
                myGridSize[0] = 20;
                myGridSize[1] = 10;
                myHasChecksumFlag = false;
            } else if (_myFirmwareVersion == 260 ) {
                _myFirmwareStatus = readStatusToken( myIt, 'S' );
                _myControllerId = readStatusToken( myIt, 'I' );
                _myFirmwareMode = readStatusToken( myIt, 'M' );
                _myFramerate = readStatusToken( myIt, 'F' );
                myGridSize[0] = readStatusToken( myIt, 'W' );
                myGridSize[1] = readStatusToken( myIt, 'H' );
                _myFrameNo = readStatusToken( myIt, 'N' );
                myChecksum = readStatusToken( myIt, 'C' );
            } else if ( _myFirmwareVersion == 261 ) {
                _myFirmwareStatus = readStatusToken( myIt, 'S' );
                _myControllerId = readStatusToken( myIt, 'I' );
                _myFirmwareMode = readStatusToken( myIt, 'M' );
                _myFramerate = readStatusToken( myIt, 'F' );
                myGridSize[0] = readStatusToken( myIt, 'W' );
                myGridSize[1] = readStatusToken( myIt, 'H' );
                _myGridSpacing = readStatusToken( myIt, 'G' );
                _myFrameNo = readStatusToken( myIt, 'N' );
                myChecksum = readStatusToken( myIt, 'C' );
            } else {
                throw ASSException(string("Unknown firmware version: ") + as_string(_myFirmwareVersion),
                        PLUS_FILE_LINE );
            }

            if ( _myGridSize != myGridSize ) {
                _myGridSize = myGridSize;
                ASSURE(_myTmpBuffer.front() == MAGIC_TOKEN);
                _myReceiveBuffer.resize( getBytesPerFrame() );
            }

            //AC_PRINT << "local sum: " << _myChecksum << " packet checksum: " << myChecksum;
            if (_myFirstFrameFlag ) {
                dumpControllerStatus();
                _myFirstFrameFlag = false;
            } else {
                if ( myHasChecksumFlag ) {
                    if (_myChecksum == myChecksum) {
                        //AC_PRINT << "Checksum OK.";
			MAKE_SCOPE_TIMER(TransportLayer_parseStatusLine);
                        _myFrameQueueLock.lock();
                        _myFrameQueue.push( ASSEvent( _myGridSize, _myFrameBuffer ));
                        AC_TRACE << "TransportLayer: event queue size = " << _myFrameQueue.size();
                        _myFrameQueueLock.unlock();
                        // TODO use smart pointers 
                        _myFrameBuffer = 0;
                    } else {
                        AC_WARNING << "Checksum error. Dropping frame No. " << _myFrameNo << ".";
                        _myChecksumErrorCounter += 1;
                    }
                }
            }
            _myChecksum = 0;

            _myTmpBuffer.erase( _myTmpBuffer.begin(), myIt);
            _myExpectedLine = 1;
        }
    } catch ( const ASSStatusTokenException & ex) {
        // XXX Workaround Bug 595
        AC_WARNING << ex;
        AC_WARNING << "Workaround for Bug 595. Resync.";
        _mySyncLostCounter += 1;
        setState( SYNCHRONIZING );
    }
}

void
TransportLayer::addLineToChecksum(std::vector<unsigned char>::const_iterator theLineIt,
                                  std::vector<unsigned char>::const_iterator theEnd )
{
    while ( theLineIt != theEnd) {
        _myChecksum +=  * theLineIt;
        theLineIt++;
    }
}

void
TransportLayer::readSensorValues(/* RasterPtr theTargetRaster */) {
    while ( _myTmpBuffer.size() >= ( _myExpectedLine == 0 ? 
                getBytesPerStatusLine() : _myGridSize[0] + 2 ))
    {
        //dumpBuffer( _myTmpBuffer );
        if ( _myExpectedLine == 0 ) {
            parseStatusLine(/*theTargetRaster*/);
        } else {
            if ( _myTmpBuffer[0] != MAGIC_TOKEN ||
                 _myTmpBuffer[1] != _myExpectedLine)
            {
                AC_WARNING << "Sync error.";
                _mySyncLostCounter++;
                _myFrameQueueLock.lock();
                _myFrameQueue.push( ASSEvent( ASS_LOST_SYNC ));
                _myFrameQueueLock.unlock();
                setState( SYNCHRONIZING );
                return;
            }
            int myRowIdx = _myTmpBuffer[1] - 1;
            //AC_PRINT << "Got row: " << myRowIdx;

            size_t byteCount = (_myTmpBuffer.begin() + 2 + _myGridSize[0]) - (_myTmpBuffer.begin() + 2);
            if ( ! _myFrameBuffer ) {
                // TODO use smart pointers 
                _myFrameBuffer = new unsigned char[ _myGridSize[0] * _myGridSize[1] ];
                
            }
            unsigned char * myRowPtr = _myFrameBuffer + myRowIdx * _myGridSize[0];
            std::copy(_myTmpBuffer.begin() + 2, _myTmpBuffer.begin() + 2 + _myGridSize[0], myRowPtr);

            addLineToChecksum(_myTmpBuffer.begin() + 2, _myTmpBuffer.begin() + 2 + _myGridSize[0]);

            _myTmpBuffer.erase( _myTmpBuffer.begin(), _myTmpBuffer.begin() + _myGridSize[0] + 2);

            if ( myRowIdx == _myGridSize[1] - 1) {
                _myExpectedLine = 0;
                _myReceivedFrames += 1;
            } else {
                _myExpectedLine += 1;
            }
        }
    }
    _myCommandQueueLock.lock();
    if ( ! _myCommandQueue.empty() ) {
        if (_myFirmwareVersion >= 0) {
            if (_myFirmwareVersion >= 260) {
                setState( CONFIGURING );
            } else {
                AC_WARNING << "Hardware commands not supported in this firmware version ("
                           << _myFirmwareVersion << ")";
                while ( ! _myCommandQueue.empty() ) {
                    _myCommandQueue.pop();
                }
            }
        }
    }
    _myCommandQueueLock.unlock();
    //AC_PRINT << "post buf size: " << _myTmpBuffer.size();
}


void
TransportLayer::synchronize() {
    if ( ! _myMagicTokenFlag ) {
        for (unsigned i = 0; i < _myTmpBuffer.size(); ++i) {
            if (_myTmpBuffer[i] == MAGIC_TOKEN) {
                //AC_PRINT << "Found MAGIC_TOKEN.";
                _myMagicTokenFlag = true;
                if ( i > 0) {
                    _myTmpBuffer.erase( _myTmpBuffer.begin(), _myTmpBuffer.begin() + i);
                    if ( ! _myTmpBuffer.empty() ) {
                        ASSURE( _myTmpBuffer[0] == MAGIC_TOKEN); 
                    }
                }
                break;
            }
        }
    }
    if ( _myMagicTokenFlag && _myTmpBuffer.size() > 1) {
        ASSURE( _myTmpBuffer[0] == MAGIC_TOKEN );
        if (_myTmpBuffer[1] == 0) {
            //AC_PRINT << "Found status line.";
            setState(RUNNING);
        } else {
            //AC_PRINT << "Found data line: " << int(_myTmpBuffer[1]);
            _myTmpBuffer.erase( _myTmpBuffer.begin(), _myTmpBuffer.begin() + 1);
            _myMagicTokenFlag = false;
        }
    }
}


void
TransportLayer::setState( DriverState theState ) {
    //AC_PRINT << "TransportLayer::setState() " << _myState << " -> " << theState << " ...";
    _myState = theState;
    switch ( _myState ) {
        case NOT_CONNECTED:
            closeConnection();
            break;
        case SYNCHRONIZING:
            sendCommand( CMD_LEAVE_CONFIG_MODE );
            _myMagicTokenFlag = false;
            _myGridSize = Vector2i(-1, -1);
            _myTmpBuffer.clear();
            _myFirstFrameFlag = true;
            _myChecksum = 0;
            break;
        case RUNNING:
            _myExpectedLine = 0;
            break;
        case CONFIGURING:
            _myConfigureState = SEND_CONFIG_COMMANDS;
            sendCommand( CMD_ENTER_CONFIG_MODE );
            break;
        default:
            break;
    }
}

void
TransportLayer::queueCommand( const char * theCommand ) {
    _myCommandQueueLock.lock();
    _myCommandQueue.push( string( theCommand ));
    _myCommandQueueLock.unlock();
}

void
TransportLayer::handleConfigurationCommand() {
    //dumpBuffer( _myTmpBuffer );
    CommandResponse myResponse = getCommandResponse();
    switch (_myConfigureState) {
        case SEND_CONFIG_COMMANDS:
            //AC_PRINT << "SEND_CONFIG_COMMANDS";
            if (myResponse == RESPONSE_OK) {
                MAKE_SCOPE_TIMER(TransportLayer_handleConfigurationCommand);
                _myCommandQueueLock.lock();
                if ( ! _myCommandQueue.empty()) {
                    sendCommand( _myCommandQueue.front() );
                    _myCommandQueue.pop();
                    if (_myCommandQueue.empty()) {
                        _myConfigureState = WAIT_FOR_RESPONSE;
                    }
                } else {
                    sendCommand( CMD_LEAVE_CONFIG_MODE );
                    _myConfigureState = WAIT_FOR_EXIT;
                }
                _myCommandQueueLock.unlock();
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to enter config mode: " 
                         << (myResponse == RESPONSE_TIMEOUT ? "timeout" : "error");

                MAKE_SCOPE_TIMER(TransportLayer_handleConfigurationCommand2);
                _myCommandQueueLock.lock();
                while ( ! _myCommandQueue.empty() ) {
                    _myCommandQueue.pop();
                }
                _myCommandQueueLock.unlock();
                setState( SYNCHRONIZING );
            }
            break;
        case WAIT_FOR_RESPONSE:
            //AC_PRINT << "WAIT_FOR_RESPONSE";
            if (myResponse == RESPONSE_OK ) {
                sendCommand( CMD_LEAVE_CONFIG_MODE );
                _myConfigureState = WAIT_FOR_EXIT;
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to configure sensor hardware: "
                         << (myResponse == RESPONSE_TIMEOUT ? "timeout" : "error");
                sendCommand( CMD_LEAVE_CONFIG_MODE );
                _myConfigureState = WAIT_FOR_EXIT;
                //setState( SYNCHRONIZING );
            }
            break;
        case WAIT_FOR_EXIT:
            //AC_PRINT << "WAIT_FOR_EXIT";
            if (myResponse == RESPONSE_OK) {
                setState( SYNCHRONIZING );
            } else if (myResponse == RESPONSE_ERROR || myResponse == RESPONSE_TIMEOUT) {
                AC_ERROR << "Failed to leave config mode.";
                setState( SYNCHRONIZING );
            }
            break;
    }
}

CommandResponse 
TransportLayer::getCommandResponse() {
    string myString( (char*)& ( * _myTmpBuffer.begin()), _myTmpBuffer.size());
    transform( myString.begin(), myString.end(), myString.begin(), ::toupper);
    string::size_type myPos = myString.find(OK_STRING);
    //AC_PRINT << "getCommandResponse(): '" << myString << "'";
    if (myPos != string::npos ) {
        string::size_type myEnd = myPos + OK_STRING.size() - 2;
        _myTmpBuffer.erase( _myTmpBuffer.begin() , _myTmpBuffer.begin() + myEnd );
        AC_PRINT << "Got 'OK'";
        return RESPONSE_OK;
    }

    myPos = myString.find(ERROR_STRING);
    if (myPos != string::npos && myPos + ERROR_STRING.size() + 2 + 2 <= myString.size() &&
        myString[ myPos + ERROR_STRING.size() + 2 ] == '\r' && 
        myString[ myPos + ERROR_STRING.size() + 2 + 1 ] == '\n') {
        unsigned myErrorCode = as<unsigned>( myString.substr( myPos + ERROR_STRING.size(), 2 ));
        AC_ERROR << "Got 'ERR': errorcode: " << myErrorCode;
        string::size_type myEnd = myPos + ERROR_STRING.size() + 2;
        _myTmpBuffer.erase( _myTmpBuffer.begin() , _myTmpBuffer.begin() + myEnd );
        return RESPONSE_ERROR;
    }

    if ( double(asl::Time()) - _myLastCommandTime > COMMAND_TIMEOUT) {
        AC_PRINT << "Got timeout";
        return RESPONSE_TIMEOUT;
    }
    return RESPONSE_NONE;
}

void
TransportLayer::sendCommand( const std::string & theCommand ) {
    try {
        AC_PRINT << "Sending command: '" << theCommand << "'";
        std::string myCommand( theCommand );
        myCommand.append("\r");
        writeData( myCommand.c_str(), myCommand.size() );
        _myLastCommandTime = double( asl::Time() );
    } catch (const SerialPortException & ex) {
        AC_WARNING << ex;
	MAKE_SCOPE_TIMER(TransportLayer_sendCommand);
        _myFrameQueueLock.lock();
        _myFrameQueue.push( ASSEvent( ASS_LOST_COM ));
        _myFrameQueueLock.unlock();
        _myDeviceLostCounter++;
        setState(NOT_CONNECTED);
    }
}



DriverState
TransportLayer::getState() const {
    return _myState;
}


const asl::Vector2i & 
TransportLayer::getGridSize() const {
    return _myGridSize;
}

int 
TransportLayer::getGridSpacing() const {
    return _myGridSpacing;
}

int 
TransportLayer::getFirmwareVersion() const {
    return _myFirmwareVersion;
}

int 
TransportLayer::getFirmwareStatus() const {
    return _myFirmwareStatus;
}

int 
TransportLayer::getControllerId() const {
    return _myControllerId;
}

int 
TransportLayer::getFirmwareMode() const {
    return _myFirmwareMode;
}

int 
TransportLayer::getFramerate() const {
    return _myFramerate;
}

int 
TransportLayer::getLastFrameNumber() const {
    return _myFrameNo;
}

int 
TransportLayer::getLastChecksum() const {
    return _myChecksum;
}

void 
TransportLayer::dumpControllerStatus() const {
    AC_PRINT << "=== ASS Controller Status =============" << endl
             << "Firmware version : " << _myFirmwareVersion << endl
             << "Controller Id    : " << _myControllerId << endl
             << "Firmware mode    : " << _myFirmwareMode 
                    << " (" << getFirmwareModeName(_myFirmwareMode) << ")" << endl
             << "Framerate        : " << _myFramerate << endl
             << "Grid size        : " << _myGridSize << endl
             << "Grid spacing     : " << _myGridSpacing << endl
             //<< "Last frame number: " << _myFrameNo << endl
             //<< "Last Checksum    : " << _myChecksum << endl
             << "=======================================";
}

const char *
TransportLayer::getFirmwareModeName(unsigned theId) const {
    switch (theId) {
        case 1:
            return "absolute";
        case 2:
            return "relative";
        default:
            break;
    }
    AC_WARNING << "Unknown firmware mode " << theId;
    return "unknown";
}

void 
TransportLayer::threadMain( asl::PosixThread & theThread ) {
    TransportLayer & mySelf = dynamic_cast<TransportLayer&>( theThread );

    try {
        AC_PRINT << "IO thread launched";
        while (mySelf._myRunningFlag) {
            mySelf.poll();

        }
        AC_PRINT << "IO thread done";
    } catch (const asl::Exception & ex) {
        AC_ERROR << "asl::Exception in thread: " << ex;
        throw;
    } catch (const std::exception & ex ) {
        AC_ERROR << "std::exception in thread: " << ex.what();
        throw;
    } catch (...) {
        AC_ERROR << "Unknown exception in thread: ";
        throw;
        
    }
}

} // end of namespace
