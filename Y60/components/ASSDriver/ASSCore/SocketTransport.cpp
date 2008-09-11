//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

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
        _mySocket->setRemoteAddr(inHostAddress, _myPort);
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
            size_t myMaxBytes = _myReceiveBuffer.size();

            // [DS] If the serial port is removed a non-blocking read returns EAGAIN
            // for some reason. Peek throws an exception which is just what we want.
            //_mySerialPort->peek();

            AC_DEBUG << "bytes received within the last " << NO_DATA_TIMEOUT 
                     << " seconds: " << _myNumReceivedBytes;
            size_t myCurrentReceivedBytes = _mySocket->receive(reinterpret_cast<char*>(& ( * _myReceiveBuffer.begin())),
                                                               myMaxBytes );

            _myNumReceivedBytes += myCurrentReceivedBytes;
            AC_TRACE << "SocketTransport::readData() received bytes: " << myCurrentReceivedBytes
                     << " total received: " << _myNumReceivedBytes 
                     << " receivebuffer size : " << _myReceiveBuffer.size();

            _myTmpBuffer.insert( _myTmpBuffer.end(),
                                 _myReceiveBuffer.begin(), _myReceiveBuffer.begin() + myCurrentReceivedBytes );
            //dumpBuffer( _myReceiveBuffer );
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
        _mySocket = TCPClientSocketPtr(0);
    }


} // end of namespace
