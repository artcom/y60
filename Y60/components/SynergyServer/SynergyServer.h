//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _SynergyServer_h_
#define _SynergyServer_h_

#include <asl/base/PosixThread.h>
#include <asl/net/TCPServer.h>
#include <asl/base/Logger.h>
#include <asl/base/ThreadLock.h>
#include <asl/math/Vector234.h>
#include <asl/base/Auto.h>
#include <queue>

// debug helper
void printVectorAsHex( const std::vector<unsigned char> & theMessage );

class SynergyPacket {

    public: 
        
        SynergyPacket() {
            _myPacketIt = _myPacket.begin();
        };

        SynergyPacket( std::vector<unsigned char> thePacket ) :
            _myPacket( thePacket ) {
            _myPacketIt = _myPacket.begin();
        }
        ~SynergyPacket() {}

        bool empty() {
            return _myPacketIt == _myPacket.end();
        }

        void assign( const std::vector<unsigned char> & thePacket ) {
            _myPacket.assign(thePacket.begin(), thePacket.end());
            _myPacketIt = _myPacket.begin();
        }

        bool getNextMessage( std::vector<unsigned char> & theMessage ) {

            if ( _myPacketIt == _myPacket.end() ) {
                return false;
            }

            AC_TRACE << "rest bytes: " << (_myPacket.end() - _myPacketIt);
            if (_myPacket.end() - _myPacketIt < 4) {
//                AC_ERROR << "borken packet!";
//                AC_ERROR << "Rest:" << std::string( _myPacketIt, _myPacket.end() );
                return false;
            }

            unsigned myLen = ((*_myPacketIt++) << 24) 
                             + ((*_myPacketIt++) << 16) 
                             + ((*_myPacketIt++) << 8) 
                             + (*_myPacketIt++); 

            AC_TRACE << "message length: " << myLen;
            if (myLen > _myPacket.end() - _myPacketIt) {
//                AC_ERROR << "borken packet!";
//                AC_ERROR << "length: " << myLen;
//                AC_ERROR << "num bytes: " << _myPacket.end() - _myPacketIt;
//                AC_ERROR << "Rest:" << std::string( _myPacketIt, _myPacket.end() );
                _myPacketIt = _myPacket.end();
                return false;
            }

            theMessage.assign(_myPacketIt, _myPacketIt + myLen);
            _myPacketIt += myLen;
            AC_TRACE << "end?: " << (_myPacketIt == _myPacket.end());

            return true;
        }

    private:

        std::vector<unsigned char> _myPacket;
        std::vector<unsigned char>::iterator _myPacketIt;

};

class SynergyServer : public asl::PosixThread {
    
    public:

        SynergyServer( asl::Unsigned32 theHost, 
                  asl::Unsigned16 thePort );
        ~SynergyServer();

        void onMouseMotion( unsigned theX, unsigned theY );
        void onMouseButton( unsigned char theButton, bool theState );
        void onMouseWheel( int theDeltaX, int theDeltaY );
        void onRelMouseMotion( unsigned theDeltaX, unsigned theDeltaY );

        bool isConnected() {
            return _myIsConnected;
        }
        asl::Vector2i getScreenSize() {
            asl::AutoLocker<asl::ThreadLock> myLocker(_myLock);
            return _myClientScreenSize;
        }

    private:

        void run();
        void stop();
        void processMessages();
        void receive();
        void send( const std::string & theMsgType, 
                   const std::vector<unsigned char> theData = std::vector<unsigned char>());
        void resetKeepalive();
        void sendHeartBeat();
        bool timedOut();
        bool canInteract();
        void parseClientInfo( const std::vector<unsigned char> & theMessage );

        inet::TCPServer  _myTCPServer;
        inet::TCPSocket* _mySocket;
        asl::Time _myKeepAliveTime;
        asl::Time _myHeartBeatTime;
        bool _myIsConnected;
        bool _myNextOp;

        asl::ThreadLock _myLock;

        std::queue<std::vector<unsigned char> > _myMessageQueue;
        std::queue<std::vector<unsigned char> > _mySendMsgQueue;

        asl::Vector2i _myClientScreenSize;

};


#endif //_SynergyServer_h_
