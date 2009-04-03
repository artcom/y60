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

#include <asl/base/file_functions.h>

#include <asl/serial/SerialDeviceFactory.h>

#include <y60/base/SettingsParser.h>

#include "ASSUtils.h"
#include "ASSDriver.h"

using namespace std;
using namespace asl;
using namespace y60;

namespace y60 {

static const double NO_DATA_TIMEOUT = 3.0;

class ASSDriver;

SerialTransport::SerialTransport(const dom::NodePtr & theSettings) :
    TransportLayer("serial", theSettings),
    _mySerialPort(0),
    _myPortNum(-1),
    _myPortName("-"),
    _myBaudRate(57600),
    _myBitsPerSerialWord(8),
    _myStopBits(1),
    _myHandshakingFlag(false),
    _myParity(SerialDevice::NO_PARITY),
    _myLastDataTime()

{
    AC_DEBUG << "SerialTransport::SerialTransport()";

    init(theSettings);
}

SerialTransport::~SerialTransport() {
    AC_DEBUG << "SerialTransport::~SerialTransport()";

    stopThread();
    closeConnection();
}

bool 
SerialTransport::settingsChanged(dom::NodePtr theSettings) {
    bool myChangedFlag = false;

    myChangedFlag |= settingChanged( theSettings, "TransportLayer", _myTransportName);
    myChangedFlag |= settingChanged( theSettings, "SerialPort", _myPortNum );
    myChangedFlag |= settingChanged( theSettings, "SerialPortName", _myPortName );
    myChangedFlag |= settingChanged( theSettings, "BaudRate", _myBaudRate );
    myChangedFlag |= settingChanged( theSettings, "BitsPerWord", _myBitsPerSerialWord );
    myChangedFlag |= settingChanged( theSettings, "Parity", _myParity );
    myChangedFlag |= settingChanged( theSettings, "StopBits", _myStopBits );
    myChangedFlag |= settingChanged( theSettings, "Handshaking", _myHandshakingFlag );

    return myChangedFlag;
}

void 
SerialTransport::init(dom::NodePtr theSettings) {
    getConfigSetting(theSettings, "SerialPort", _myPortNum, -1);
    getConfigSettingString(theSettings, "SerialPortName", _myPortName, "-");
    getConfigSetting(theSettings, "BaudRate", _myBaudRate, 57600);
    getConfigSetting(theSettings, "BitsPerWord", _myBitsPerSerialWord, 8);
    getConfigSetting(theSettings, "Parity", _myParity, SerialDevice::NO_PARITY);
    getConfigSetting(theSettings, "StopBits", _myStopBits, 1);
    getConfigSetting(theSettings, "Handshaking", _myHandshakingFlag, false);
}


void 
SerialTransport::establishConnection() {
    AC_DEBUG << "SerialTransport::establishConnection()";

    if (_mySerialPort) {
        delete _mySerialPort;
        _mySerialPort = 0;
    }

    if (_myPortName == "-") {
        if (_myPortNum >= 0 ) {
            _mySerialPort = getSerialDevice(_myPortNum);
        }
    } else {
        _mySerialPort = getSerialDeviceByName(_myPortName);
    }

    if (!_mySerialPort) {
        AC_ERROR << "ASS does not have a serial port configured";
        return;
    }

    try {
        _mySerialPort->open(_myBaudRate, _myBitsPerSerialWord,
           _myParity, _myStopBits, _myHandshakingFlag, 0, 1);

        _mySerialPort->setStatusLine(SerialDevice::RTS);

        setState(SYNCHRONIZING);
    } catch (const asl::SerialPortException & ex) {
        AC_WARNING << "Exception while opening serial: " << ex;
        if (_mySerialPort) {
            delete _mySerialPort;
            _mySerialPort = 0;
            return;
        }
    }

    _myLastDataTime.setNow();
    _mySerialPort->flush();
}

void 
SerialTransport::readData() {
    // XXX: this looks odd and suspicious. remove, test and fix.
    if(!_mySerialPort) {
        return;
    }

    try {
        // check for timeout
        asl::Time myNow;
        double myTimeSinceLastData = myNow - _myLastDataTime;
        if(myTimeSinceLastData > NO_DATA_TIMEOUT) {
            AC_WARNING << "No data received for " << myTimeSinceLastData << " seconds.";
            connectionLost();
            return;
        }

        // [DS] If the serial port is removed a non-blocking read returns EAGAIN
        // for some reason. Peek throws an exception which is just what we want.
        _mySerialPort->peek();

        // read, blocking when there is no data
        asl::Time myBefore, myAfter;
        size_t myBytesReceived = _myReceiveBuffer.size();
        _mySerialPort->read(reinterpret_cast<char*>(& ( * _myReceiveBuffer.begin())), myBytesReceived);
        myAfter.setNow();

        AC_TRACE << "Read took " << myAfter.millis() - myBefore.millis() << " milliseconds.";

        if(myBytesReceived > 0) {
            // update data timeout reference
            _myLastDataTime.setNow();
            
            // report the read
            AC_DEBUG << "Received " << myBytesReceived << " with " << _myTmpBuffer.size() << " queued.";

            //// dump data in hex
            //std::string hexDump;
            //asl::binToString(&_myReceiveBuffer[0], myBytesReceived, hexDump);
            //AC_TRACE << "Received bytes: " << hexDump;

            //// and also in ascii
            //std::string ascDump;
            //for(unsigned i = 0; i < myBytesReceived; i++) {
            //    unsigned char c = _myReceiveBuffer[i];
            //    ascDump += " ";
            //    if(c >= 32 && c < 127) {
            //        ascDump += c;
            //    } else {
            //        ascDump += ".";
            //    }
            //}
            //AC_TRACE << "Received chars: " << ascDump;

            //// warn about overrunning buffer
            if(_myTmpBuffer.size() > 1024) {
                AC_WARNING << "Input buffer is running over. Currently at " << _myTmpBuffer.size() << " bytes.";
            }

            // queue the data into the protocol parser buffer
            _myTmpBuffer.insert(_myTmpBuffer.end(), _myReceiveBuffer.begin(), _myReceiveBuffer.begin() + myBytesReceived);
        } else {
            AC_TRACE << "Read returned nothing.";
        }

    } catch (const SerialPortException & ex) {
        AC_WARNING << "Exception while reading: " << ex;
        connectionLost();
    }
}

void 
SerialTransport::writeData(const char * theData, size_t theSize) {
    // XXX: this looks highly suspicious. remove and test.
    if (!_mySerialPort) {
        AC_WARNING << "Can not write data. No serial port.";
        setState(NOT_CONNECTED);
    }

    try {
        _mySerialPort->write( theData, theSize );
    } catch (const SerialPortException & ex) {
        AC_WARNING << "Exception while writing: " << ex;
        connectionLost();
    }
}

void 
SerialTransport::closeConnection() {
    AC_DEBUG << "SerialTransport::closeConnection()";

    if (_mySerialPort) {
        try {
            // XXX: This resets all outgoing status lines. It should only reset CTS.
            _mySerialPort->setStatusLine(0);
        } catch (const asl::SerialPortException & ex) {
            AC_WARNING << "Exception while resetting status lines: " << ex;
        }

        delete _mySerialPort;
        _mySerialPort = 0;
    }
}

} // end of namespace
