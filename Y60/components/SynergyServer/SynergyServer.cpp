//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.  
//=============================================================================

#include "SynergyServer.h"
#include <asl/base/Time.h>
#include <y60/base/iostream_functions.h>
#include <asl/net/net_functions.h>
#include <sstream>

const unsigned int READ_BUFFER_SIZE = 2000;
const unsigned char SYNERGY_VERSION[4] = {0,1,0,3};
const unsigned int BITMASK = 0x000000FF;
const long long KEEP_ALIVE_TIMEOUT = 3000;
const long long HEART_BEAT_TIMEOUT = 500;
        
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
    join();
    _myTCPServer.close();
    if (_mySocket) {
        _mySocket->close();
        delete _mySocket;
    }
}


void SynergyServer::run() {

    while (!shouldTerminate()) {

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
                    _mySocket->send( reinterpret_cast<char*>(&(*myMsg.begin())), 
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

    AC_TRACE << "SynergyServer::send(" << theMessage << ")";

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


void SynergyServer::processMessages() {

    AC_TRACE << "SynergyServer::processMessages";
    
    while (!_myMessageQueue.empty()) {
    
        AC_TRACE << "have " << _myMessageQueue.size() << " messages in my queue!";

        std::vector<unsigned char> myMsg = _myMessageQueue.front();
        _myMessageQueue.pop();

        if (std::string(myMsg.begin(), myMsg.begin() + 7) == "Synergy") {
            AC_TRACE << "Got hello message";
            send( "QINF" );
            resetKeepalive();
        }
        else if (std::string(myMsg.begin(), myMsg.begin() + 4) == "CALV") {
            AC_TRACE << "Got keep alive message";
            send( "CALV" );
            resetKeepalive();
        }
        else if (std::string(myMsg.begin(), myMsg.begin() + 4) == "DINF") {
            AC_TRACE << "Got DInfo message";
            parseClientInfo(myMsg);
            send( "CIAK" );            
            send( "CROP" );
            std::vector<unsigned char> myOptions(4,0);
            send( "DSOP", myOptions ); 
            std::vector<unsigned char> myScreenPos(10,0);
            send( "CINN", myScreenPos ); 
            send( "CALV" );
            resetKeepalive();
            _myIsConnected = true;
        }
        else if (std::string(myMsg.begin(), myMsg.begin() + 4) == "CNOP") {
            AC_TRACE << "Got NOP message";
            continue;
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
        return;
    }
    
    _myLock.lock();
    _myClientScreenSize[0] = (theMsg[8] << 8) + theMsg[9];
    _myClientScreenSize[1] = (theMsg[10] << 8) + theMsg[11];
    _myLock.unlock();

}
