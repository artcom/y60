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

#include "SocketTransport.h"
#include "ASSUtils.h"
#include "ASSDriver.h"

#include <asl/base/file_functions.h>
#include <y60/base/SettingsParser.h>

using namespace std;
using namespace asl;
using namespace y60;
using namespace inet;

namespace y60 {

    static const unsigned NO_DATA_TIMEOUT = 30;

    class ASSDriver;

    SocketTransport::SocketTransport(const dom::NodePtr & theSettings) :
        TransportLayer( "socket", theSettings ),
        _myNumReceivedBytes( 0 )

    {
        init( theSettings );
    }

    SocketTransport::~SocketTransport() {
        stopThread();
        closeConnection();
    }

    bool 
    SocketTransport::settingsChanged(dom::NodePtr theSettings) {
        bool myChangedFlag = false;

        myChangedFlag |= settingChanged( theSettings, "TransportLayer", _myTransportName);
        myChangedFlag |= settingChanged( theSettings, "IpAddress", _myIpAddressString);
        myChangedFlag |= settingChanged( theSettings, "Port", _myPort );

        return myChangedFlag;
    }

    void 
    SocketTransport::init(dom::NodePtr theSettings) {
        //AC_PRINT << "SocketTransport::init()";
        getConfigSetting( theSettings, "IpAddress", _myIpAddressString, std::string("127.0.0.1" ));
        getConfigSetting<unsigned>( theSettings, "Port", _myPort, 5000 );
    }


    void 
    SocketTransport::establishConnection() {
        AC_PRINT << "SocketTransport::establishConnection()";
        _mySocket = TCPClientSocketPtr(new TCPClientSocket());

        Unsigned32 inHostAddress = getHostAddress(_myIpAddressString.c_str());
        _mySocket->setConnectionTimeout(2);
        _mySocket->setRemoteAddr(inHostAddress, static_cast<asl::Unsigned16>(_myPort));
        _mySocket->connect();
        if (!_mySocket->isValid()) {
            _mySocket->retry(10);
        }

        if (_mySocket->isValid()) {
            setState( SYNCHRONIZING );
            std::vector<unsigned char> myTrashBuffer;
            myTrashBuffer.resize(1024 * 1024);
            _mySocket->receive(reinterpret_cast<char*>(& ( * myTrashBuffer.begin())),
                               myTrashBuffer.size() );        
        }
    }

    void 
    SocketTransport::readData() {
        try {
            AC_DEBUG << "bytes received within the last " << NO_DATA_TIMEOUT 
                     << " seconds: " << _myNumReceivedBytes;

            char myReceiveBuffer[1024];
            size_t myMaxBytes = sizeof(myReceiveBuffer);
            size_t myCurrentReceivedBytes = _mySocket->receive(myReceiveBuffer, myMaxBytes);

            _myNumReceivedBytes += myCurrentReceivedBytes;
            AC_TRACE << "SocketTransport::readData() received bytes: " << myCurrentReceivedBytes
                     << " total received: " << _myNumReceivedBytes;

            _myTmpBuffer.insert(_myTmpBuffer.end(), myReceiveBuffer, myReceiveBuffer + myCurrentReceivedBytes);
            //dumpBuffer( myReceiveBuffer );
        } catch (const SocketException & ex) {
            /*MAKE_SCOPE_TIMER(SocketTransport_serialPortException);
              _myFrameQueueLock.lock();
              _myFrameQueue.push( ASSEvent( ASS_LOST_COM ));
              _myFrameQueueLock.unlock();*/
            AC_ERROR <<  ex;
            setState(NOT_CONNECTED);
        }
    }

    void 
    SocketTransport::writeData(const char * theData, size_t theSize) {
        if ( _mySocket ) {
            _mySocket->send(theData, theSize);
        } else {
            AC_WARNING << "Can not write data. No socket.";
            setState( NOT_CONNECTED );
        }
    }

    void 
    SocketTransport::closeConnection() {
        _mySocket->close();
        _mySocket = TCPClientSocketPtr();
    }


} // end of namespace
