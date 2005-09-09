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
//         $Id: Projector.cpp,v 1.6 2004/11/27 11:19:07 ulrich Exp $
//     $Author: ulrich $
//   $Revision: 1.6 $
//       $Date: 2004/11/27 11:19:07 $
//
// Projector factory
//
//=============================================================================

#include "Projector.h"

#include "NecProjector.h"
#include "PdF1Projector.h"
#include "PanasonicProjector.h"

#include <asl/SerialDeviceFactory.h>
#include <asl/SerialDevice.h>
#include <asl/Exception.h>
#include <dom/Nodes.h>

Projector* Projector::getProjector(const std::string& theType, int thePortNum)
{
    Projector* projector = 0;

    if (theType.size() == 0 || theType == "nec") {
        projector = new NecProjector(thePortNum);
    }
    else if (theType == "pdf1") {
        projector = new PdF1Projector(thePortNum);
    }
    else if (theType == "panasonic" || theType == "pt-d5500") {
        projector = new PanasonicProjector(thePortNum);
    }
    else {
        throw asl::Exception(std::string("Unknown projector: ") + theType);
    }

    return projector;
}

Projector* Projector::getProjector(const dom::NodePtr & theProjectorNode, Logger* theLogger)
{
    Projector*  projector = 0;
    std::string myType  = "";
    int         myPort = -1;

    if (theProjectorNode->nodeType() == dom::Node::ELEMENT_NODE) {
        if (theProjectorNode->getAttribute("type")) {
            myType = theProjectorNode->getAttribute("type")->nodeValue();
        }

        if (theProjectorNode->getAttribute("port")) {
            myPort = asl::as<int>(theProjectorNode->getAttribute("port")->nodeValue());
        }

        if (myPort != -1) {
            if (myType.size() == 0 || myType == "nec") {
                projector = new NecProjector(myPort);
            }
            else if (myType == "pdf1") {
                projector = new PdF1Projector(myPort);
            }
            else if (myType == "panasonic" || myType == "pt-d5500") {
                projector = new PanasonicProjector(myPort);
            }
            else {
                throw asl::Exception(std::string("Unknown projector type: ") + myType);
            }
            projector->setLogger(theLogger);
            projector->configure(theProjectorNode);
        }
    }

    return projector;
}

void
Projector::configure(const dom::NodePtr & theConfigNode)
{
    std::cerr << "Projector::configure " << *theConfigNode << std::endl;
    
    if (theConfigNode->getAttribute("input")) {
        _myInitialInputSource = getEnumFromString(theConfigNode->getAttribute("input")->nodeValue());
        AC_DEBUG << "_myInitialInputSource: " << getStringFromEnum(_myInitialInputSource) << std::endl;
    }
}

void
Projector::selectInput(const std::string& theSource)
{
    VideoSource mySource = getEnumFromString(theSource);
    if (mySource != NONE) {
	    std::cerr << "Projector::selectInput " << theSource << std::endl;
        selectInput(mySource);
    }
}

bool
Projector::command(const std::string & theCommand)
{
    if (!_myCommandEnable) {
        std::cerr << "Projector::command disabled, ignoring '" << theCommand << "'" << std::endl;
        return true;
    }

    std::cerr << "Projector::command '" << theCommand << "'" << std::endl;
    if (theCommand == "projector_on") {
        power(true);
    }
    else if (theCommand == "projector_off") {
        power(false);
    }
    else if (theCommand == "projector_shutter_open") {
        shutter(true);
    }
    else if (theCommand == "projector_shutter_close") {
        shutter(false);
    }
    else if (theCommand == "projector_power_low") {
        lampPower(false);
    }
    else if (theCommand == "projector_power_high") {
        lampPower(true);
    }
    else if (theCommand.substr(0, 22) == "projector_input_select") {
        int myIndex = theCommand.find("=", 0);
        if (myIndex != std::string::npos) {
            std::string mySource = theCommand.substr(myIndex+1);
            selectInput(mySource);
        }
    }
    else {
        std::cerr << "Projector::command unknown '" << theCommand << "'" << std::endl;
        return false;
    }
    return true;
}


/*
 * Private
 */
Projector::Projector(int thePortNum) :
    _mySerialDevice(0), _myLogger(0), _myCommandEnable(true),
    _myInitialInputSource(NONE)
{
    if (thePortNum != -1) {
        _mySerialDevice = asl::getSerialDevice(thePortNum);
        if (!_mySerialDevice) {
            throw asl::Exception("Failed to get serial device", PLUS_FILE_LINE);
        }
	    //_mySerialDevice->setNoisy(true);
    }
}

Projector::~Projector()
{
    if (_mySerialDevice) {
        _mySerialDevice->close();
        delete _mySerialDevice;
        _mySerialDevice = 0;
    }
}


Projector::VideoSource
Projector::getEnumFromString(const std::string& theSource)
{
    if (theSource == "RGB_1") {
        return RGB_1;
    }
    if (theSource == "RGB_2") {
        return RGB_2;
    }
    if (theSource == "VIDEO") {
        return VIDEO;
    }
    if (theSource == "SVIDEO") {
        return SVIDEO;
    }
    if (theSource == "DVI") {
        return DVI;
    }
    if (theSource == "VIEWER") {
        return VIEWER;
    }
    return NONE;
}

std::string
Projector::getStringFromEnum(const Projector::VideoSource theSource)
{
    if (theSource == RGB_1) {
        return "RGB_1";
    }
    if (theSource == RGB_2) {
        return "RGB_2";
    }
    if (theSource == VIDEO) {
        return "VIDEO";
    }
    if (theSource == SVIDEO) {
        return "SVIDEO";
    }
    if (theSource == DVI) {
        return "DVI";
    }
    if (theSource == VIEWER) {
        return "VIEWER";
    }
    return "NONE";
}
