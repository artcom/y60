//=============================================================================
// Copyright (C) 2004-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
#include <asl/base/Logger.h>
#include <sstream>
#include <memory>

using namespace std;

SensorServer::SensorServer(unsigned theComPort, unsigned theBaudRate) {
    _myComPort = asl::Ptr<asl::SerialDevice>(asl::getSerialDevice(theComPort));
    _myComPort->open(theBaudRate, 8, asl::SerialDevice::NO_PARITY, 1);
}


void
SensorServer::parseLine(const string & theLine, unsigned & theController, unsigned & theBitMask) {
    istringstream myStream(theLine);

    char  c;
    if (theLine[0] == 7) {
        myStream >> c; // skip bell
    }
    myStream >> theController;
    myStream >> c; // skip comma
    myStream >> theBitMask;
}


void
SensorServer::handleLines(string & theBuffer, SensorData & theData) {
    std::string::size_type i = theBuffer.find_first_of("\r\n");
    while(i != string::npos) {
        string myLine = theBuffer.substr(0 ,i);
        if (!myLine.empty()) {
            unsigned myController, myBitmask;
            parseLine(myLine, myController, myBitmask);
            theData.push_back(make_pair(myController, myBitmask));
        }
        theBuffer = theBuffer.substr(i+1);
        std::string::size_type nextline_start = theBuffer.find_first_not_of("\r\n");
        if ( nextline_start != string::npos) {
            theBuffer = theBuffer.substr(nextline_start);
        }
    }
}

void 
SensorServer::poll(SensorData & theData) {
    size_t myNumBytes = _myComPort->peek();
    if (myNumBytes) {
        vector<char> myBuffer(myNumBytes+1);
        _myComPort->read(&myBuffer[0], myNumBytes);
        myBuffer[myNumBytes] = 0;
        _myFifo += string(&myBuffer[0]);
        handleLines(_myFifo, theData);
    }
}
