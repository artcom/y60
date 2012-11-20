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

#ifndef _SynergyServer_h_
#define _SynergyServer_h_

#include "y60_synergyserver_settings.h"

#include <queue>

#include <asl/base/PosixThread.h>
#include <asl/net/TCPServer.h>
#include <asl/base/Logger.h>
#include <asl/base/ThreadLock.h>
#include <asl/math/Vector234.h>
#include <asl/base/Auto.h>

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
                return false;
            }

            unsigned int myLen =   ((*(_myPacketIt  )) << 24)
                                 + ((*(_myPacketIt+1)) << 16)
                                 + ((*(_myPacketIt+2)) <<  8)
                                 +  (*(_myPacketIt+3));
            _myPacketIt+=4;

            AC_TRACE << "message length: " << myLen;
            if (static_cast<long>(myLen) > static_cast<long>(_myPacket.end() - _myPacketIt)) {
                // broken packet
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
        void onRelMouseMotion( int theDeltaX, int theDeltaY );

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
