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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "SynergyServer.h"
#include <asl/base/Time.h>
#include <y60/base/iostream_functions.h>
#include <asl/net/net_functions.h>
#include <asl/base/begin_end.h>
#include <sstream>

const unsigned int READ_BUFFER_SIZE = 2000;
const unsigned char SYNERGY_VERSION[4] = {0,1,0,3};
const unsigned int BITMASK = 0x000000FF;
const unsigned long long KEEP_ALIVE_TIMEOUT = 3000;
const unsigned long long HEART_BEAT_TIMEOUT = 500;

unsigned char MOUSE_WHEEL_DOWN[4] = {0x00, 0x00, 0xFF, 0x88};
unsigned char MOUSE_WHEEL_UP[4] = {0x00, 0x00, 0x00, 0x78};


void printVectorAsHex( const std::vector<unsigned char> & theMessage ) {
    AC_TRACE << "printVectorAsHex";
    std::ostringstream myStream;
    myStream.setf( std::ios::hex, std::ios::basefield );
    for (unsigned i = 0; i < theMessage.size(); i++) {
        myStream << std::setw(2) <<  std::setfill('0') << int(theMessage[i]) << " ";
    }
    AC_TRACE << "Message: " << myStream.str();
}



SynergyServer::SynergyServer( asl::Unsigned32 theHost, asl::Unsigned16 thePort ) :
    _myTCPServer( theHost, thePort ),
    _mySocket(NULL),
    _myIsConnected(false),
    _myClientScreenSize(0,0)
{
    AC_TRACE << "ASS_Mouse::SynergyServer";

    // XXX: never start threads from CTORs. they might run into uninitialized virtuals.
    fork();
}


SynergyServer::~SynergyServer() {

    AC_TRACE << "SynergyServer::~SynergyServer";
    stop();

}


void SynergyServer::onMouseMotion( unsigned theX, unsigned theY ) {
    AC_TRACE << "SynergyServer::onMouseMotion(" << theX << "," << theY << ")";

    if (!_myIsConnected) {
        AC_TRACE << "failed sending mouse motion";
        return;
    }

    std::vector<unsigned char> myMouseData;
    myMouseData.push_back((theX >> 8) & BITMASK);
    myMouseData.push_back(theX & BITMASK);
    myMouseData.push_back((theY >> 8) & BITMASK);
    myMouseData.push_back(theY & BITMASK);

    send( "DMMV", myMouseData );

}


void SynergyServer::onRelMouseMotion( int theDeltaX, int theDeltaY ) {
    AC_TRACE << "SynergyServer::onRelMouseMotion(" << theDeltaX << "," << theDeltaY << ")";

    if (!_myIsConnected) {
        AC_TRACE << "failed sending mouse motion";
        return;
    }

    std::vector<unsigned char> myMouseData;
    myMouseData.push_back((theDeltaX >> 8) & BITMASK);
    myMouseData.push_back(theDeltaX & BITMASK);
    myMouseData.push_back((theDeltaY >> 8) & BITMASK);
    myMouseData.push_back(theDeltaY & BITMASK);

    send( "DMRM", myMouseData );

}

void SynergyServer::onMouseButton( unsigned char theButton, bool theState ) {

    AC_TRACE << "SynergyServer::onMouseButton";

    if (!_myIsConnected) {
        return;
    }

    std::vector<unsigned char> myButton(1,1);
    // synergy button indices are slightly different...
    if (theButton == 2) {
        myButton[0] = 3;
    } else if (theButton == 3) {
        myButton[0] = 2;
    }

    if (theState) {
        send( "DMDN", myButton );
    } else {
        send( "DMUP", myButton );
    }

}


void SynergyServer::onMouseWheel( int theDeltaX, int theDeltaY ) {

    if (!_myIsConnected) {
        return;
    }

    if (theDeltaY == 1) {
        send( "DMWM", std::vector<unsigned char>( MOUSE_WHEEL_DOWN,
                                                  MOUSE_WHEEL_DOWN
                                                  + sizeof(MOUSE_WHEEL_DOWN) ) );
    } else if (theDeltaY == -1) {
        send( "DMWM", std::vector<unsigned char>( MOUSE_WHEEL_UP,
                                                  MOUSE_WHEEL_UP
                                                  + sizeof(MOUSE_WHEEL_UP) ) );
    }

}


void SynergyServer::stop() {
    AC_TRACE << "SynergyServer::stop";
    send( "CBYE" );
    join();
    _myTCPServer.close();
    if (_mySocket) {
        _mySocket->close();
        delete _mySocket;
    }
}


void SynergyServer::run() {

    while (!shouldTerminate()) {

        // XXX: replace this with blocking io.
        //      receive should be simple blocking socket io.
        //      sending can be done without going through a queue.
        asl::msleep(10);

        try {

            if (NULL == _mySocket) {
                AC_PRINT << "Waiting for synergy connection on "
                         << asl::as_dotted_address(_myTCPServer.getHost())
                         << " at port " <<  _myTCPServer.getPort() << ".";
                _mySocket = _myTCPServer.waitForConnection();
                _mySocket->setBlockingMode(false);
                std::vector<unsigned char> myVersion;
                myVersion.insert( myVersion.end(), SYNERGY_VERSION, SYNERGY_VERSION +
                                  sizeof( SYNERGY_VERSION ) );
                send( "Synergy", myVersion );
                resetKeepalive();
                AC_PRINT << "...connected!";
            }


            if (timedOut()) {
                _myIsConnected = false;
                AC_WARNING << "Connection to synergy client timed out!";
                if (_mySocket) {
                    _mySocket->close();
                    delete _mySocket;
                    _mySocket = NULL;
                }
                resetKeepalive();
                continue;
            }

            receive();
            processMessages();

            if (_myIsConnected) {
                sendHeartBeat();
            }

            while (!_mySendMsgQueue.empty()) {
                if (_mySocket != NULL && _mySocket->isValid()) {
                    _myLock.lock();
                    std::vector<unsigned char> myMsg =
                        std::vector<unsigned char>(_mySendMsgQueue.front());
                    _mySendMsgQueue.pop();
                    _myLock.unlock();
                    _mySocket->send( reinterpret_cast<char*>(asl::begin_ptr(myMsg)),
                                     myMsg.size() );
                }
            }

        } catch (inet::SocketException & ex) {
            AC_ERROR << ex;
        } catch ( ... ) {
            AC_ERROR << "got other exception...";
        }
    }

}


void SynergyServer::send( const std::string & theMessage,
                     const std::vector<unsigned char> theData )
{

    AC_DEBUG << "SynergyServer::send(" << theMessage << ")";

    size_t myMsgLen = theMessage.size() + theData.size();
    std::vector<unsigned char> myData;
    myData.push_back( myMsgLen >> 24 & BITMASK );
    myData.push_back( myMsgLen >> 16 & BITMASK );
    myData.push_back( myMsgLen >> 8 & BITMASK );
    myData.push_back( myMsgLen & BITMASK );
    myData.insert( myData.end(), theMessage.begin(), theMessage.end() );
    myData.insert( myData.end(), theData.begin(), theData.end() );

    _myLock.lock();
    _mySendMsgQueue.push(myData);
    _myLock.unlock();

}

void SynergyServer::receive() {

    AC_TRACE << "SynergyServer::receive()";

    try {

        std::vector<unsigned char> myData;
        while (_mySocket->isValid() && _mySocket->peek(1)) {
            char myInputBuffer[READ_BUFFER_SIZE];
            memset( myInputBuffer, 0, sizeof( myInputBuffer ) );
            unsigned myNumBytes = _mySocket->receive( myInputBuffer,
                                                      sizeof( myInputBuffer ) );
            myData.insert( myData.end(), myInputBuffer, myInputBuffer + myNumBytes );
        }
        if (!myData.empty()) {
            //printVectorAsHex( myData );
            SynergyPacket myPacket( myData );
            std::vector<unsigned char> myMsg;
            while (myPacket.getNextMessage( myMsg )) {
                _myMessageQueue.push( myMsg );
            }
            resetKeepalive();
        }

    } catch (inet::SocketException & ex) {
        AC_ERROR << ex;
    }

}


void SynergyServer::resetKeepalive() {
    AC_TRACE << "SynergyServer::resetKeepalive";
    _myKeepAliveTime = _myKeepAliveTime.setNow();;
}


void SynergyServer::sendHeartBeat() {
    asl::Time myCurrentTime;
    bool myBeat = myCurrentTime.millis() - _myHeartBeatTime.millis() >= HEART_BEAT_TIMEOUT;
    if (myBeat) {
        send("CALV");
        _myHeartBeatTime.setNow();
    }
}


bool SynergyServer::timedOut() {
    AC_TRACE << "SynergyServer::timedOut";
    asl::Time myCurrentTime;
    bool myAlarm = myCurrentTime.millis() - _myKeepAliveTime.millis() >= KEEP_ALIVE_TIMEOUT;
    return myAlarm;
}

static bool messageStartsWith(std::vector<unsigned char> theMessage, std::string thePrefix) {
    if(theMessage.size() >= thePrefix.size()) {
        return std::equal(thePrefix.begin(), thePrefix.end(), theMessage.begin());
    } else {
        return false;
    }
}

void SynergyServer::processMessages() {

    AC_TRACE << "SynergyServer::processMessages";

    while (!_myMessageQueue.empty()) {

        AC_TRACE << "have " << _myMessageQueue.size() << " messages in my queue!";

        std::vector<unsigned char> myMsg = _myMessageQueue.front();
        _myMessageQueue.pop();

        if (messageStartsWith(myMsg, "Synergy")) {
            AC_DEBUG << "Got hello message";
            send( "QINF" );
            resetKeepalive();
        }
        else if (messageStartsWith(myMsg, "CALV")) {
            AC_DEBUG << "Got keep alive message";
            resetKeepalive();
        }
        else if (messageStartsWith(myMsg, "DINF")) {
            AC_DEBUG << "Got DInfo message";
            parseClientInfo(myMsg);
            send( "CIAK" );
            if (!_myIsConnected) {
                send( "CROP" );
                std::vector<unsigned char> myOptions(4,0);
                send( "DSOP", myOptions );
                std::vector<unsigned char> myScreenPos(10,0);
                send( "CINN", myScreenPos );
                send( "CALV" );
            }
            resetKeepalive();
            _myIsConnected = true;
        }
        else if (messageStartsWith(myMsg, "CNOP")) {
            AC_DEBUG << "Got NOP message";
            // do nothing
        }
        else if (messageStartsWith(myMsg, "CCLP")) {
            AC_DEBUG << "Got Clipboard message";
            // do nothing (yet)
        }
        else {
            AC_PRINT << "Got strange message: " << std::string( myMsg.begin(),
                                                                myMsg.begin() + 4 );
        }
    }

}

void SynergyServer::parseClientInfo( const std::vector<unsigned char> & theMsg ) {

    if( std::string( theMsg.begin(), theMsg.begin() + 4 ) != "DINF" ) {
        AC_ERROR << "No client-info message!";
        return;
    }

    if (theMsg.size() != 18) {
        AC_ERROR << "Client info incomplete! Got only " << theMsg.size() << "bytes!";
        // reconnect...
        _mySocket->close();
        delete _mySocket;
        _mySocket = NULL;
        return;
    }

    _myLock.lock();
    _myClientScreenSize[0] = (theMsg[8] << 8) + theMsg[9];
    _myClientScreenSize[1] = (theMsg[10] << 8) + theMsg[11];
    _myLock.unlock();

}
