/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "TransportLayer.h"
#include "ASSDriver.h"
#include "ASSUtils.h"

#include <asl/base/Assure.h>
#include <y60/base/SettingsParser.h>

#ifdef TL_LATENCY_TEST
#   include <asl/SerialDeviceFactory.h>
#endif

using namespace std;
using namespace asl;
using namespace y60;

static const char * DriverStateStrings[] = {
    "not_connected",
    "synchronizing",
    "running",
    "configuring",
    ""
};


IMPLEMENT_ENUM( y60::DriverState, DriverStateStrings);


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
const unsigned int MAX_NUM_STATUS_TOKENS(12);
const unsigned int BYTES_PER_STATUS_TOKEN(5);
const unsigned int MAX_STATUS_LINE_LENGTH(MAX_NUM_STATUS_TOKENS * BYTES_PER_STATUS_TOKEN + 2);

DEFINE_EXCEPTION(ASSStatusLineException, asl::Exception);

TransportLayer::TransportLayer(const char * theTransportName, const dom::NodePtr & theSettings) :
    asl::PosixThread(TransportLayer::threadMain),

    _myTransportName(theTransportName),

    _myState(NOT_CONNECTED),
    _myConfigureState(),
    _myLastCommandTime(double(asl::Time())),
    _myFrameBuffer(0),
    _myChecksum(0),
    _myFirstFrameFlag(true),
    _myMagicTokenFlag(false),
    _myExpectedLine(0),

    _myTmpBuffer(0),

    _myFirmwareVersion(-1),
    _myFirmwareStatus(-1),
    _myGridSize(-1,-1),
    _myGridSpacing(-1),
    _myControllerId(-1),
    _myFirmwareMode(-1),
    _myFramerate(-1),
    _myFrameNo(-1),
    _myCurrentMultiplex(0),
    _myMultiplexMax(0),

    _mySyncLostCounter(0),
    _myDeviceLostCounter(0),
    _myReceivedFrames(0),
    _myChecksumErrorCounter(0)
{
    getConfigSetting( theSettings, "EventQueueSize", _myEventQueueSize,
                      static_cast<std::queue<ASSEvent>::size_type>(100u));

#ifdef TL_LATENCY_TEST
    _myLatencyTestPort = getSerialDevice(0);
    _myLatencyTestPort->open(9600, 8, SerialDevice::NO_PARITY, 1, false);
#endif
}

TransportLayer::~TransportLayer() {

#ifdef TL_LATENCY_TEST
    _myLatencyTestPort->close();
    delete _myLatencyTestPort;
#endif

    dumpStatistics();
}


void
TransportLayer::stopThread() {
    AC_INFO << "Shutting down proximatrix thread.";
    join();
}

static const double NO_DATA_TIMEOUT = 3.0;

void
TransportLayer::poll() {
    // check for data timeout
    if(_myState != NOT_CONNECTED) {
        asl::Time myNow;
        double myTimeSinceLastData = myNow - _myLastDataTime;
        if(myTimeSinceLastData > NO_DATA_TIMEOUT) {
            AC_WARNING << "No data received for " << myTimeSinceLastData << " seconds.";
            connectionLost();
            return;
        }
    }

    _myFrameQueueLock.lock();
    if (_myFrameQueue.size() > _myEventQueueSize) {
        //AC_WARNING << "Event queue has more than " << _myEventQueueSize << " items. Flushing.";
        while ( ! _myFrameQueue.empty() ) {
            _myFrameQueue.pop();
        }
    }
    _myFrameQueueLock.unlock();

    switch (_myState) {
        case NOT_CONNECTED:
            establishConnection();
            if (_myState == NOT_CONNECTED) {
                // XXX: this keeps us from thrashing when we are not connected but is ugly.
                msleep(1000);
            }
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
        case DriverStateEnum_MAX: break; // avoid warning
    }
}

unsigned
TransportLayer::readStatusToken(std::deque<unsigned char>::iterator & theIt, const char theToken) {
    if (*theIt != theToken) {
        throw ASSStatusLineException(string("Failed to parse status token ") + theToken, PLUS_FILE_LINE);
    }
    theIt += 1;
    istringstream myStream;
    myStream.setf(std::ios::hex, std::ios::basefield);
    myStream.str(string(theIt, theIt + 4));
    theIt += 4;
    unsigned myNumber;
    myStream >>  myNumber;
    return myNumber;
}


unsigned
TransportLayer::getBytesPerStatusLine() {
    if (_myFirmwareVersion < 0) {
        return MAX_STATUS_LINE_LENGTH;
    } else if (_myFirmwareVersion < 260) {
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
    } else if (_myFirmwareVersion == 262) {
        // twelve tokens: <version> <status> <id> <mode> <framerate>
        //             <width> <height> <gridspacing> <framenumber>
        //             <multiplex> <current_multiplex>
        //             <checksum>
        return 12 * BYTES_PER_STATUS_TOKEN + 2;
    }
    throw ASSStatusLineException(string("Unknown firmware version: ") + as_string(_myFirmwareVersion),
            PLUS_FILE_LINE );
}

size_t
TransportLayer::getBytesPerFrame() {
    // line format: <start> <lineno> <databytes>
    return (1 + 1 + valuesPerLine() ) * _myGridSize[1] + MAX_STATUS_LINE_LENGTH;
}

void
TransportLayer::parseStatusLine() {
    try {
        if (_myTmpBuffer.size() >= getBytesPerStatusLine()) {
            ASSURE( _myTmpBuffer[0] == MAGIC_TOKEN);
            ASSURE( _myTmpBuffer[1] == _myExpectedLine);
            std::deque<unsigned char>::iterator myIt = _myTmpBuffer.begin() + 2;
            _myFirmwareVersion = readStatusToken( myIt, 'V' );
            Vector2i myGridSize;
            unsigned myChecksum = 0;
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
            } else if ( _myFirmwareVersion == 262 ) {
                _myFirmwareStatus = readStatusToken( myIt, 'S' );
                _myControllerId = readStatusToken( myIt, 'I' );
                _myFirmwareMode = readStatusToken( myIt, 'M' );
                _myFramerate = readStatusToken( myIt, 'F' );
                myGridSize[0] = readStatusToken( myIt, 'W' );
                myGridSize[1] = readStatusToken( myIt, 'H' );
                _myGridSpacing = readStatusToken( myIt, 'G' );
                _myFrameNo = readStatusToken( myIt, 'N' );
                _myMultiplexMax = readStatusToken( myIt, 'X' );
                _myCurrentMultiplex = readStatusToken( myIt, 'x' );
                myChecksum = readStatusToken( myIt, 'C' );
            } else {
                throw ASSStatusLineException(string("Unknown firmware version: ") + as_string(_myFirmwareVersion),
                        PLUS_FILE_LINE );
            }

#ifdef TL_LATENCY_TEST
            if (_myFrameNo % 16 == 0) {
                static bool state = false;
                _myLatencyTestPort->setStatusLine( state ? SerialDevice::DTR : 0 );
                state = ! state;
            } else {
                //_myLatencyTestPort->setStatusLine( 0 );
            }
#endif
            if ( _myGridSize != myGridSize ) {
                _myGridSize = myGridSize;
                ASSURE(_myTmpBuffer.front() == MAGIC_TOKEN);
            }

            AC_TRACE << "local sum: " << _myChecksum << " packet checksum: " << myChecksum;
            if (_myFirstFrameFlag ) {
                dumpConfiguration();
                _myFirstFrameFlag = false;
            } else {
                //if (_myMultiplexMax == _myCurrentMultiplex -1 ) {
                if ( _myCurrentMultiplex == 0 ) {
                    if ( myHasChecksumFlag ) {
                        // TODO check checksums for multiplexed frames
                        if (_myChecksum == myChecksum ) {
                            _myFrameQueueLock.lock();
                            _myFrameQueue.push( ASSEvent( _myGridSize, _myFrameNo,
                                        _myFrameBuffer ));
                            AC_TRACE << "TransportLayer: event queue size = " << _myFrameQueue.size();
                            // TODO use smart pointers
                            _myFrameBuffer = 0;
                            _myFrameQueueLock.unlock();
                        } else {
                            AC_WARNING << "Checksum error. Dropping frame No. " << _myFrameNo << ".";
                            _myChecksumErrorCounter += 1;
                        }
                    }
                }
            }
            _myChecksum = 0;

            _myTmpBuffer.erase( _myTmpBuffer.begin(), myIt);
            _myExpectedLine = 1;
        }
    } catch (const ASSStatusLineException & ex) {
        AC_WARNING << "Failed to parse status line: " << ex;
        syncLost();
    }
}

void
TransportLayer::addLineToChecksum(std::deque<unsigned char>::const_iterator theLineIt,
                                  std::deque<unsigned char>::const_iterator theEnd)
{
    while ( theLineIt != theEnd) {
        _myChecksum +=  * theLineIt;
        theLineIt++;
    }
}

unsigned
TransportLayer::valuesPerLine() const {
    if (_myMultiplexMax) {
        return _myGridSize[0] / _myMultiplexMax;
    } else {
        return _myGridSize[0];
    }
}

void
TransportLayer::readSensorValues() {
    while ( _myTmpBuffer.size() >= ( _myExpectedLine == 0 ?
                getBytesPerStatusLine() : valuesPerLine() + 2 ))
    {
        //std::string hexDump;
        //asl::binToString(&_myTmpBuffer[0], _myTmpBuffer.size(), hexDump);
        //AC_TRACE << "Buffered bytes: " << hexDump;

        //std::string ascDump;
        //for(unsigned i = 0; i < _myTmpBuffer.size(); i++) {
        //    unsigned char c = _myTmpBuffer[i];
        //    ascDump += " ";
        //    if(c >= 32 && c < 127) {
        //        ascDump += c;
        //    } else {
        //        ascDump += ".";
        //    }
        //}
        //AC_TRACE << "Buffered chars: " << ascDump;

        if ( _myExpectedLine == 0 ) {
            parseStatusLine();
        } else {
            if ( _myTmpBuffer[0] != MAGIC_TOKEN || _myTmpBuffer[1] != _myExpectedLine) {
                AC_WARNING << "Framing error in sensor data.";
                syncLost();
                return;
            }
            int myRowIdx = _myTmpBuffer[1] - 1;
            //AC_PRINT << "Got row: " << myRowIdx;

            //size_t byteCount = (_myTmpBuffer.begin() + 2 + valuesPerLine() ) - (_myTmpBuffer.begin() + 2);
            if ( ! _myFrameBuffer ) {
                // TODO use smart pointers
                _myFrameBuffer = new unsigned char[ _myGridSize[0] * _myGridSize[1] ];

            }
            unsigned char * myRowPtr = _myFrameBuffer + myRowIdx * _myGridSize[0];
            if (_myMultiplexMax == 0) {
                std::copy(_myTmpBuffer.begin() + 2, _myTmpBuffer.begin() + 2 + _myGridSize[0], myRowPtr);
            } else {
                std::deque<unsigned char>::iterator myInput = _myTmpBuffer.begin() + 2;
                myRowPtr += _myCurrentMultiplex;
                for (unsigned i = 0; i < valuesPerLine(); ++i) {
                    * myRowPtr = * myInput;
                    myRowPtr += _myMultiplexMax;
                    ++myInput;
                }
            }

            addLineToChecksum(_myTmpBuffer.begin() + 2, _myTmpBuffer.begin() + 2 + valuesPerLine());

            _myTmpBuffer.erase( _myTmpBuffer.begin(), _myTmpBuffer.begin() + valuesPerLine() + 2);

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
            setState(RUNNING);
        } else {
            _myTmpBuffer.erase( _myTmpBuffer.begin(), _myTmpBuffer.begin() + 1);
            _myMagicTokenFlag = false;
        }
    }
}


void
TransportLayer::setState( DriverState theState ) {
    //AC_DEBUG << "TransportLayer::setState() " << _myState << " -> " << theState << " ...";
    _myState = theState;
    switch ( _myState ) {
        case NOT_CONNECTED:
            closeConnection();
            break;
        case SYNCHRONIZING:
            _myLastDataTime.setNow();
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
        AC_INFO << "Sending command: " << theCommand;
        std::string myCommand(theCommand);
        myCommand.append("\r");
        writeData(myCommand.c_str(), myCommand.size());
        _myLastCommandTime = double(asl::Time());
    } catch (const SerialPortException & ex) {
        AC_WARNING << "Error while sending command: " << ex;
        connectionLost();
    }
}

void
TransportLayer::receivedData(char* theData, size_t theSize) {
    _myTmpBuffer.insert(_myTmpBuffer.end(), theData, theData + theSize);
    _myLastDataTime.setNow();
}

void
TransportLayer::syncLost() {
    AC_WARNING << "Sync lost. Resynchronizing.";
    _mySyncLostCounter++;
    _myTmpBuffer.clear();
    _myFrameQueueLock.lock();
    _myFrameQueue.push(ASSEvent(ASS_LOST_SYNC));
    _myFrameQueueLock.unlock();
    setState(SYNCHRONIZING);
}

void
TransportLayer::connectionLost() {
    AC_WARNING << "Connection lost. Reconnecting.";
    _myDeviceLostCounter++;
    _myTmpBuffer.clear();
    _myFrameQueueLock.lock();
    _myFrameQueue.push(ASSEvent(ASS_LOST_COM));
    _myFrameQueueLock.unlock();
    setState(NOT_CONNECTED);
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
TransportLayer::dumpConfiguration() const {
    AC_PRINT << "=== ASS Controller Status =============" << endl
             << "Firmware version : " << _myFirmwareVersion << endl
             << "Controller Id    : " << _myControllerId << endl
             << "Firmware mode    : " << _myFirmwareMode
                    << " (" << getFirmwareModeName(_myFirmwareMode) << ")" << endl
             << "Framerate        : " << _myFramerate << endl
             << "Grid size        : " << _myGridSize << endl
             << "Grid spacing     : " << _myGridSpacing << endl
             << "=======================================";
}

void
TransportLayer::dumpStatistics() const {
    AC_PRINT << "=== ASS Transport Layer Stats =========" << endl
             << "Frames received       : " << _myReceivedFrames << endl
             << "Synchronization errors: " << _mySyncLostCounter << endl
             << "Device lost           : " << _myDeviceLostCounter << endl
             << "Checksum errors       : " << _myChecksumErrorCounter << endl
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
    AC_FATAL << "Unknown firmware mode " << theId;
    return "unknown";
}

void
TransportLayer::threadMain(asl::PosixThread & theThread) {
    TransportLayer & mySelf = dynamic_cast<TransportLayer&>(theThread);

    AC_DEBUG << "Proximatrix thread running";
    while (!mySelf.shouldTerminate()) {
        mySelf.poll();
    }
    AC_DEBUG << "Proximatrix thread terminating";
}

} // end of namespace
