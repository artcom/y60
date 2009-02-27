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

#include "SerialTransport.h"
#include "ASSUtils.h"
#include "ASSDriver.h"

#include <asl/base/file_functions.h>
#include <asl/serial/SerialDeviceFactory.h>
#include <y60/base/SettingsParser.h>

using namespace std;
using namespace asl;
using namespace y60;

namespace y60 {

static const unsigned NO_DATA_TIMEOUT = 3;

class ASSDriver;

SerialTransport::SerialTransport(const dom::NodePtr & theSettings) :
    TransportLayer( "serial", theSettings ),
    _mySerialPort( 0 ),
    _myUseUSBFlag( false ),
    _myPortNum( -1 ),
    _myBaudRate( 57600 ),
    _myBitsPerSerialWord( 8 ),
    _myStopBits( 1 ),
    _myHandshakingFlag( false ),
    _myParity( SerialDevice::NO_PARITY ),
    _myLastComTestTime( 0 ),
    _myNumReceivedBytes( 0 )

{
    init( theSettings );
}

SerialTransport::~SerialTransport() {
    stopThread();
    closeConnection();
}

bool 
SerialTransport::settingsChanged(dom::NodePtr theSettings) {
    bool myChangedFlag = false;

    myChangedFlag |= settingChanged( theSettings, "TransportLayer", _myTransportName);

    myChangedFlag |= settingChanged( theSettings, "SerialPort", _myPortNum );
    myChangedFlag |= settingChanged( theSettings, "UseUSB", _myUseUSBFlag );
    myChangedFlag |= settingChanged( theSettings, "BaudRate", _myBaudRate );
    myChangedFlag |= settingChanged( theSettings, "BitsPerWord", _myBitsPerSerialWord );
    myChangedFlag |= settingChanged( theSettings, "Parity", _myParity );
    myChangedFlag |= settingChanged( theSettings, "StopBits", _myStopBits );
    myChangedFlag |= settingChanged( theSettings, "Handshaking", _myHandshakingFlag );

    return myChangedFlag;
}

void 
SerialTransport::init(dom::NodePtr theSettings) {
    //AC_PRINT << "SerialTransport::init()";

    getConfigSetting( theSettings, "SerialPort", _myPortNum, -1 );
    getConfigSetting( theSettings, "UseUSB", _myUseUSBFlag, false );
    getConfigSetting( theSettings, "BaudRate", _myBaudRate, 57600 );
    getConfigSetting( theSettings, "BitsPerWord", _myBitsPerSerialWord, 8 );

    getConfigSetting( theSettings, "Parity", _myParity, SerialDevice::NO_PARITY );
    getConfigSetting( theSettings, "StopBits", _myStopBits, 1 );
    getConfigSetting( theSettings, "Handshaking", _myHandshakingFlag, false );
}


void 
SerialTransport::establishConnection() {
    //AC_PRINT << "SerialTransport::establishConnection()";
    if (_myPortNum >= 0 ) {
        _myLastComTestTime = asl::Time();
        _myNumReceivedBytes = 0;

#ifdef _WIN32
        _mySerialPort = getSerialDevice( _myPortNum );
#endif
#ifdef LINUX
        if (_myUseUSBFlag) {
            string myDeviceName("/dev/ttyUSB");
            myDeviceName += as_string( _myPortNum );
            if ( fileExists( myDeviceName ) ) {
                _mySerialPort = getSerialDeviceByName( myDeviceName );
            }
        } else {
            _mySerialPort = getSerialDevice( _myPortNum );
        }
#endif
#ifdef OSX
        // [DS] IIRC the FTDI devices on Mac OS X appear as /dev/tty.usbserial-[devid]
        // where [devid] is a string that is flashed into the FTDI chip. I'll
        // check that the other day.
        AC_PRINT << "TODO: implement device name handling for FTDI USB->RS232 "
            << "virtual TTYs on Mac OS X.";
#endif

        if (_mySerialPort) {
            try {
                _mySerialPort->open( _myBaudRate, _myBitsPerSerialWord,
                        _myParity, _myStopBits, _myHandshakingFlag, 0 , 1);
                _mySerialPort->setStatusLine( SerialDevice::RTS);

                setState( SYNCHRONIZING );
            } catch (const asl::SerialPortException & /*ex*/) {
                if ( _mySerialPort ) {
                    delete _mySerialPort;
                    _mySerialPort = 0;
                }
            }
        }
    } else {
        AC_PRINT << "scanForSerialPort() No port configured.";
    }
}

void 
SerialTransport::readData() {
    try {
        //AC_PRINT << "SerialTransport::readData()";
        size_t myMaxBytes = _myReceiveBuffer.size();

        double myCurrentTimeStamp = asl::Time();
        if (myCurrentTimeStamp - _myLastComTestTime > NO_DATA_TIMEOUT) {
            _myLastComTestTime = myCurrentTimeStamp;
            if (_myNumReceivedBytes == 0 ) {
                AC_WARNING << "ASSDriver lost communication";
                throw SerialPortException("CommuncationLost", PLUS_FILE_LINE);
            } else {
                _myNumReceivedBytes = 0;
            }
        }

        // [DS] If the serial port is removed a non-blocking read returns EAGAIN
        // for some reason. Peek throws an exception which is just what we want.
        _mySerialPort->peek();

        AC_DEBUG << "bytes received within the last " << NO_DATA_TIMEOUT 
                 << " seconds: " << _myNumReceivedBytes;
        /*
        AC_PRINT << "SerialTransport::readData() pending bytes: " << myPendingBytes
                 << " max: " << myMaxBytes;
                 */
        
        _mySerialPort->read( reinterpret_cast<char*>(& ( * _myReceiveBuffer.begin())),
                myMaxBytes );
        _myNumReceivedBytes += myMaxBytes;

        _myTmpBuffer.insert( _myTmpBuffer.end(),
                _myReceiveBuffer.begin(), _myReceiveBuffer.begin() + myMaxBytes );
        //dumpBuffer( _myFrameBuffer );
    } catch (const SerialPortException & /*ex*/) {
        //AC_WARNING << ex;
        //_myDriver->createTransportLayerEvent( "lost_communication" );
        MAKE_SCOPE_TIMER(SerialTransport_serialPortException);
        _myFrameQueueLock.lock();
        _myFrameQueue.push( ASSEvent( ASS_LOST_COM ));
        _myFrameQueueLock.unlock();
        _myDeviceLostCounter++;
        setState(NOT_CONNECTED);
    }
}

void 
SerialTransport::writeData(const char * theData, size_t theSize) {
    if ( _mySerialPort ) {
        _mySerialPort->write( theData, theSize );
    } else {
        AC_WARNING << "Can not write data. No serial port.";
        setState( NOT_CONNECTED );
    }
}

void 
SerialTransport::closeConnection() {
    //AC_PRINT << "SerialTransport::closeConnection()";
    if (_mySerialPort) {
        //AC_PRINT << "Disabling RTS";
        // XXX this disables both RTS and DTR ... the current (Win32) API does not permit to
        //     retrive the current state of the outbound lines...
        try {
            _mySerialPort->setStatusLine( 0 );
        } catch ( const asl::SerialPortException & /*ex*/ ) {
            //AC_WARNING << "Failed to disable RTS line: " << ex;
        }

        /*
        if ( _mySerialPort->getStatusLine() & SerialDevice::RTS) {
            AC_PRINT << "RTS is on";
        } else {
            AC_PRINT << "RTS is off";
        }
        */

        delete _mySerialPort;
        _mySerialPort = 0;
    }
}


} // end of namespace
