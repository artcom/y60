//============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//         $Id: PanasonicProjector.cpp,v 1.7 2004/12/01 16:28:38 ulrich Exp $
//     $Author: ulrich $
//   $Revision: 1.7 $
//       $Date: 2004/12/01 16:28:38 $
//
// Panasonic PT-D5500 projector
//
//=============================================================================

#include "PanasonicProjector.h"
#include "Logger.h"

#include <asl/SerialDevice.h>
#include <asl/string_functions.h>
#include <asl/Exception.h>
#include <asl/Time.h>
#include <dom/Nodes.h>

#include <iostream>
#include <strstream>
using namespace std;


PanasonicProjector::PanasonicProjector(int thePortNum) : Projector(thePortNum),
    _myFirstID(1),
    _myNumProjectors(2),
    _myPowerDelay(2000),
    _myReadTimeout(250)
{
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice) {
        throw asl::Exception("Failed to get serial device!", PLUS_FILE_LINE);
    }
    myDevice->open(9600, 8, asl::SerialDevice::NO_PARITY, 1);
    _myDescription = "Panasonic PT-D5500 on port : " + asl::as_string(thePortNum);
}


void
PanasonicProjector::configure(const dom::NodePtr & theConfigNode)
{
    //cerr << "PanasonicProjector::configure " << *theConfigNode << endl;

    dom::NodePtr myNode;
    if ((myNode = theConfigNode->childNode("NumProjectors"))) {
        _myNumProjectors = asl::as<int>(myNode->childNode(0)->nodeValue());
    }

    if ((myNode = theConfigNode->childNode("FirstID"))) {
        _myFirstID = asl::as<int>(myNode->childNode(0)->nodeValue());
    }
    
    if ((myNode = theConfigNode->childNode("PowerDelay"))) {
        _myPowerDelay = asl::as<int>(myNode->childNode(0)->nodeValue());
    }

    if ((myNode = theConfigNode->childNode("ReadTimeout"))) {
        _myReadTimeout = asl::as<int>(myNode->childNode(0)->nodeValue());
    }
        
    if ((myNode = theConfigNode->childNode("Input"))) {
        Projector::selectInput(myNode->childNode(0)->nodeValue());
    }

    if ((myNode = theConfigNode->childNode("Lamps"))) {
        lamps(asl::as<int>(myNode->childNode(0)->nodeValue()));
    }
    
    if ((myNode = theConfigNode->childNode("LampPower"))) {
        // OLP:0 = High, OLP:1 = Low
    }
}

void
PanasonicProjector::power(bool thePowerFlag)
{
    cerr << "PanasonicProjector::power " << (thePowerFlag ? "on" : "off") << endl;
    sendCommand((thePowerFlag ? "PON" : "POFF"), "", _myPowerDelay);
}

void
PanasonicProjector::selectInput(VideoSource theVideoSource)
{
    cerr << "PanasonicProjector::selectInput " << theVideoSource << endl;
    std::string myParams;
    switch (theVideoSource) {
    case RGB_1:
        myParams = "RG1";
        break;
    case RGB_2:
        myParams = "RG2";
        break;
    case VIDEO:
        myParams = "VID";
        break;
    case SVIDEO:
        myParams = "SVD";
        break;
    case DVI:
        myParams = "DVI";
        break;
    case VIEWER:
        default:
        throw asl::Exception("Unknown projector input source.", PLUS_FILE_LINE);
    }
    sendCommand("IIS", myParams);
}

void
PanasonicProjector::lamps(unsigned theLampsMask)
{
    cerr << "PanasonicProjector::lamps " << theLampsMask << endl;
    std::string myParams;
    switch (theLampsMask) {
    case 0:
        myParams = "1"; // SINGLE
        break;
    case 1:
        myParams = "2"; // LAMP 1
        break;
    case 2:
        myParams = "3"; // LAMP 2
        break;
    case 3:
        myParams = "0"; // DUAL
        break;
    }
    sendCommand("LPM", myParams);
}

void
PanasonicProjector::lampPower(bool thePowerHighFlag)
{
    cerr << "PanasonicProjector::lampPower power=" << thePowerHighFlag << endl;
    sendCommand("OLP", (thePowerHighFlag ? "0" : "1"));
}

void
PanasonicProjector::shutter(bool theShutterOpenFlag)
{
    cerr << "PanasonicProjector::shutter open=" << theShutterOpenFlag << endl;
    sendCommand("OSH", (theShutterOpenFlag ? "0" : "1"));
    asl::msleep(1000);
    sendCommand("OSH", (theShutterOpenFlag ? "0" : "1"));
}

void
PanasonicProjector::update()
{
    //cerr << "PanasonicProjector::update" << endl;

    // power mode
    sendCommand("QPW","");
    parseResponse("%03d", "Power");

    // runtime
    sendCommand("QST","");
    parseResponse("%05d", "Runtime", "h");

    // temp input air
    sendCommand("QTM","0");
    parseResponse("%04d", "Input Air", "C");

#if 0 // NOT AVAILABLE ON PT-D5500
    // temp output air
    sendCommand("QTM","1",0);
    parseResponse("%04d", "Output Air", "C");
#endif
    
    // temp optical module
    sendCommand("QTM","2",0);
    parseResponse("%04d", "Optical Module", "C");
}


/**********************************************************************
 *
 * Private
 *
 **********************************************************************/

#ifdef WIN32
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif

const char STX[] = "\002";
const char ETX[] = "\003";

void
PanasonicProjector::sendCommand(const string & theCommand, const string & theParams, unsigned theTimeout)
{
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice) {
        return;
    }

    _myResponses.clear();
    for (unsigned i = 0; i < _myNumProjectors; ++i) {
        sendCommandSingle(theCommand, theParams, i);
        asl::msleep(theTimeout);
    }
}

void
PanasonicProjector::sendCommandSingle(const string & theCommand, const string & theParams, unsigned theAddress)
{
    // address
    char myAddress[] = "ZZ";
    if (theAddress != ~0) {
        SNPRINTF(myAddress, sizeof(myAddress), "%02d", theAddress+_myFirstID);
    }
 
    // assemble buffer
    char myBuf[256];
    SNPRINTF(myBuf, sizeof(myBuf), "%sAD%s;%s", STX, myAddress, theCommand.c_str());
    if (!theParams.empty()) {
        strcat(myBuf, ":");
        strcat(myBuf, theParams.c_str());
    }
    strcat(myBuf, ETX);

    //cerr << "PanasonicProjector::sendCommand buf='" << myBuf << "'" << endl;
    getDevice()->write(myBuf, strlen(myBuf));
    readFromDevice(_myReadTimeout);
}

void
PanasonicProjector::readFromDevice(unsigned theTimeout)
{
    //cerr << "PanasonicProjector::readFromDevice" << endl;

    char myReadBuffer[1024];
    unsigned myReadBufferIndex = 0;

    char myBuf[256] = "";
    unsigned myTimeout = 0;
    while (myTimeout < theTimeout) {
        
        size_t myBufLen = sizeof(myBuf);
        bool myReadErr = getDevice()->read(myBuf, myBufLen);
        if (myReadErr) {
            cerr << "Error reading from device" << endl;
            break;
        }
        //cerr << "len=" << myBufLen << endl;
        if (myBufLen == 0) {
            const unsigned myDelay = 100;
            asl::msleep(myTimeout);
            myTimeout += myDelay;
            continue;
        }
        for (unsigned i = 0; i < myBufLen; ++i) {
            char myChar = myBuf[i];
            if (myChar == 0) {
                continue;
            }
            //cerr << hex << (int) myChar << dec << "=" << myChar << endl;
            if (myChar == STX[0]) {
                myReadBufferIndex = 0;
            }
            myReadBuffer[myReadBufferIndex++] = myChar;
            if (myChar == ETX[0]) {
                myReadBuffer[myReadBufferIndex++] = '\0';
                _myResponses.push_back(std::string(myReadBuffer));
                myReadBufferIndex = 0;
                continue;
            }
        }
		// XXX MAYBE RESET myTimeout=0
    }
}

void
PanasonicProjector::parseResponse(const std::string & thePattern, const std::string & theParamName, const std::string & theUnit)
{
	std::stringstream s;
    s << "PanasonicProjector " << theParamName;
    for (unsigned i = 0; i < _myResponses.size(); ++i) {

        const char* myData = _myResponses[i].c_str() + 1;
        int myValue;

        s << " ID" << (i + _myFirstID) << ":";
        if (sscanf((char*) myData, thePattern.c_str(), &myValue) == 1) {
			s << myValue << theUnit;
        }
        else {
            s << "??? (" << _myResponses[i] << ")";
        }
    }
    //cerr << s.str() << endl;
#if 1
	if (getLogger()) {
		getLogger()->logToFile(s.str());
	}
#endif
    _myResponses.clear();
}