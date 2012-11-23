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
//
//    $RCSfile: SensorServer.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.8 $
//       $Date: 2005/02/03 16:59:51 $
//
// Dieters Sensor Array...
//
//=============================================================================

#include "SensorServer.h"

#include <asl/serial/SerialDeviceFactory.h>
#include <asl/base/Time.h>
#include <asl/base/Logger.h>
#include <asl/base/string_functions.h>

#include <boost/regex.hpp> 

using namespace std;

SensorServer::SensorServer() : _myStatusInterval(5) {
}

void
SensorServer::openDevice(const std::string & theComPort, unsigned theBaudRate) {
    unsigned port = -1;
    asl::SerialDevice * myComPort = 0;
    if (asl::is_decimal_number(theComPort, port)) {
        myComPort = asl::getSerialDevice(port);
    } else {
        myComPort = asl::getSerialDeviceByName(theComPort);
    }
    _myComPort = asl::Ptr<asl::SerialDevice>(myComPort);
    _myComPort->open(theBaudRate, 8, asl::SerialDevice::NO_PARITY, 1);
}

bool
SensorServer::parseLine(const string & theLine, unsigned & theController, unsigned & theBitMask) {
    boost::regex expr("(\\d+)\\s*,\\s*(\\d+)");
    boost::smatch what;
    if (boost::regex_search(theLine, what, expr)) {
        theController = asl::as<unsigned>(what[1]);
        theBitMask = asl::as<unsigned>(what[2]);
        return true;
    }
    return false;
}

void
SensorServer::handleLines(string & theBuffer, SensorData & theData) {
    std::string::size_type i = theBuffer.find("\r\n");
    while(i != string::npos) {
        string myLine = theBuffer.substr(0 ,i);
        if (!myLine.empty()) {
            unsigned myController, myBitmask;
            if (parseLine(myLine, myController, myBitmask)) {
                theData.push_back(make_pair(myController, myBitmask));
                _mySensorStatus = "OK";
                _myLastStatusTime.setNow();
            }
        }
        theBuffer = theBuffer.substr(i+2);
        i = theBuffer.find("\r\n");
    }
}

void
SensorServer::poll(SensorData & theData) {
    char myBuffer[1024];
    size_t myBytesRead = sizeof(myBuffer);

    _myComPort->read(myBuffer, myBytesRead);
    if(myBytesRead) {
        string myData(myBuffer, myBytesRead);
        if (myData.find("OK") != string::npos) {
            _mySensorStatus = "OK";
            _myLastStatusTime.setNow();
        } else {
            _myFifo += myData;
            handleLines(_myFifo, theData);
        }
    }
}

std::string
SensorServer::getStatus() {
    std::string myCurrentStatus = _mySensorStatus;
    if (isStatusOutDated() || myCurrentStatus.empty()) {
        _myComPort->write("C", 1);
        _mySensorStatus = "outdated";
    }
    return myCurrentStatus;
}

bool
SensorServer::isStatusOutDated() {
    asl::Time now;
    return now - _myLastStatusTime > _myStatusInterval;
}

void
SensorServer::calibrate(const std::string & theString) {
    for (unsigned int i = 0; i < theString.size(); ++i) {
        _myComPort->write(&theString[i], 1);
        asl::msleep(10);
    }
}
