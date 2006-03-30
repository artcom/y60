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
//   $Id: HitachiProjector.cpp,v 1.2 2004/09/24 12:56:10 valentin Exp $
//   $Author: valentin $
//   $Revision: 1.2 $
//   $Date: 2004/09/24 12:56:10 $
//
// Hitachi Projector controller.
//
//=============================================================================

#include "HitachiProjector.h"

#include <asl/SerialDevice.h>
#include <asl/Exception.h>
#include <asl/Time.h>
#include <asl/Logger.h>
#include <asl/string_functions.h>

#include <iostream>

const char LIES_POWER_UP[]      = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0xBA, 0xD2, 0x01, 0x00, 0x00, 0x60, 0x01, 0x00 };
const char LIES_POWER_DOWN[]    = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0x2A, 0xD3, 0x01, 0x00, 0x00, 0x60, 0x00, 0x00 };

const char LIES_INPUT_RGB_1[]   = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0xFE, 0xD2, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00 };
const char LIES_INPUT_VIDEO[]   = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0x6E, 0xD3, 0x01, 0x00, 0x00, 0x20, 0x01, 0x00 };
const char LIES_INPUT_SVIDEO[]  = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0x9E, 0xD3, 0x01, 0x00, 0x00, 0x20, 0x02, 0x00 };
const char LIES_INPUT_M1[]      = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0x0E, 0xD2, 0x01, 0x00, 0x00, 0x20, 0x03, 0x00 };
//const char LIES_INPUT_BNC[]
//const char LIES_INPUT_COMPONENT[]

const char LIES_SHUTTER_OPEN[]  = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0xFB, 0xD8, 0x01, 0x00, 0x20, 0x30, 0x00, 0x00 };
const char LIES_SHUTTER_CLOSE[] = { 0xBE, 0xEF, 0x03, 0x06, 0x00, 0x6B, 0xD9, 0x01, 0x00, 0x20, 0x30, 0x01, 0x00 };

using namespace std;

HitachiProjector::HitachiProjector(int thePortNum) : Projector(thePortNum)
{
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice) {
        throw asl::Exception("Failed to get serial device!", PLUS_FILE_LINE);
    }
    myDevice->open(19200, 8, asl::SerialDevice::NO_PARITY, 1);
    _myDescription = "Hitachi on port : " + asl::as_string(thePortNum); 
}


void
HitachiProjector::power(bool thePowerFlag) {
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice)
        return;

    if (thePowerFlag) {
        AC_DEBUG << "HitachiProjector::powerUp - 1" ;
        myDevice->write(LIES_POWER_UP, 13);
        asl::msleep(1000);
        myDevice->close();
        myDevice->open(19200, 8, asl::SerialDevice::NO_PARITY, 1);
        asl::msleep(1000);
        AC_DEBUG << "HitachiProjector::powerUp - 2" ;
        myDevice->write(LIES_POWER_UP, 13);
    }
    else {
        myDevice->write(LIES_POWER_DOWN, 13);
        AC_DEBUG << "HitachiProjector::powerDown" ;
    }
}


void
HitachiProjector::selectInput(VideoSource theVideoSource) {
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice)
        return;

    std::cerr << "HitachiProjector::selectInput " << getStringFromEnum(theVideoSource) << std::endl;

    switch (theVideoSource) {
    case RGB_1 :
        myDevice->write(LIES_INPUT_RGB_1, 13);
        break;
    case RGB_2 :
        std::cerr << "Input not supported." << std::endl;
        break;
    case VIDEO :
        myDevice->write(LIES_INPUT_VIDEO, 13);
        break;
    case SVIDEO :
        myDevice->write(LIES_INPUT_SVIDEO, 13);
        break;
    case DVI :
    case M1 :
        myDevice->write(LIES_INPUT_M1, 13);
        break;
    case VIEWER :
        std::cerr << "Input not supported." << std::endl;
        break;
    default:
        throw asl::Exception("Unknown projector input source.", PLUS_FILE_LINE);
    };
    AC_DEBUG << "HitachiProjector::selectInput" ;
}


void
HitachiProjector::shutter(bool theShutterOpenFlag) {
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice)
        return;

    if (theShutterOpenFlag) {
        myDevice->write(LIES_SHUTTER_OPEN, 13);
    } else {
        myDevice->write(LIES_SHUTTER_CLOSE, 13);
    }
}
