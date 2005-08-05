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
//   $Id: PdF1Projector.cpp,v 1.3 2004/09/24 12:56:10 valentin Exp $
//   $Author: valentin $
//   $Revision: 1.3 $
//   $Date: 2004/09/24 12:56:10 $
//
// projectiondesign F1 Projector controller.
//
//=============================================================================

#include "PdF1Projector.h"
#include "CRC16.h"

#include <asl/SerialDevice.h>
#include <asl/string_functions.h>
#include <asl/Exception.h>

#include <iostream>


// Packet size
const unsigned int PACKET_SIZE = 32;

/*
 * STATE OP_VALUE
 * ON    0x0001
 * OFF   0x0000
 */
const unsigned char F1_POWER_ON_OFF[] = {
    0xBE, 0xEF, 0x03, 0x19, 0x00, // header
    0x82, 0x14,                   // CRC lo,hi
    0x01,                         // SET
    0x9C, 0x02                    // operation id
};

/*
 * INPUT           OP_VALUE
 * VGA1            0x0000
 * VGA2            0x0001
 * DVI             0x0002
 * Component       0x0003
 * S-Video         0x0004
 * Composite Video 0x0005
 * Composite HD    0x0006
 */
const unsigned char F1_SELECT_INPUT[] = {
    0xBE, 0xEF, 0x03, 0x19, 0x00,
    0xEA, 0xE9,
    0x01,
    0x01, 0x44
};


PdF1Projector::PdF1Projector(int thePortNum) : Projector(thePortNum)
{
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice) {
        throw asl::Exception("Failed to get serial device!", PLUS_FILE_LINE);
    }
    myDevice->open(19200, 8, asl::SerialDevice::NO_PARITY, 1);
    _myDescription = "Projection Design F1 on port : " + asl::as_string(thePortNum); 
}


void
PdF1Projector::power(bool thePowerFlag)
{
    std::cerr << "PdF1Projector::power " << (thePowerFlag ? "on" : "off") << std::endl;

    unsigned char packet[PACKET_SIZE];
    memset(packet, 0, sizeof(packet));
    memcpy(packet, F1_POWER_ON_OFF, sizeof(F1_POWER_ON_OFF));

    setOpValue(packet, (unsigned short) thePowerFlag);
    sendPacket(packet, sizeof(packet));
}

void
PdF1Projector::selectInput(VideoSource theVideoSource)
{
    std::cerr << "PdF1Projector::selectInput " << theVideoSource << std::endl;

    unsigned char packet[PACKET_SIZE];
    memset(packet, 0, sizeof(packet));
    memcpy(packet, F1_SELECT_INPUT, sizeof(F1_SELECT_INPUT));

    unsigned short value = 0;
    switch (theVideoSource) {
    case RGB_1:
        value = 0x0000;
        break;
    case RGB_2:
        value = 0x0001;
        break;
    case DVI:
        value = 0x0002;
        break;
    case SVIDEO:
        value = 0x0004;
        break;
    case VIDEO:
        value = 0x0005;
        break;
    case VIEWER:
    default:
        throw asl::Exception("Unknown projector input source.", PLUS_FILE_LINE);
    }

    setOpValue(packet, value);
    sendPacket(packet, sizeof(packet));
}


/**********************************************************************
 *
 * Private
 *
 **********************************************************************/

bool PdF1Projector::checkHeader(const unsigned char* packet) const
{
    if (packet[0] != (unsigned char) 0xBE || packet[1] != (unsigned char) 0xEF) {
        std::cerr << "### Invalid packet header" << std::endl;
        return false;
    }

    return true;
}

void PdF1Projector::setOpValue(unsigned char* packet, unsigned short value)
{
    packet[16] = value & 0xff;
    packet[17] = (value >> 8);
}

void PdF1Projector::sendPacket(unsigned char* packet, unsigned int packetLen)
{
    asl::SerialDevice * myDevice = getDevice();
    if (!myDevice) {
        throw asl::Exception("I´m not even supposed to be here today.", PLUS_FILE_LINE);
    }

#if 1 
    // sanity checks
    if (packetLen != PACKET_SIZE) {
        std::cerr << "### Unexpected packet size" << std::endl;
    }
    if (!checkHeader(packet)) {
        std::cerr << "### Unexpected packet header" << std::endl;
    }
#endif

    // pre-computed CRC
    unsigned short givenCrc = (packet[6] << 8) | packet[5];
    packet[5] = packet[6] = 0x00;

    // calculate CRC, check against pre-computed
    unsigned short crc = asl::CRC16(packet, packetLen);
#if 0
    if (givenCrc != 0 && crc != givenCrc) {
        std::cerr << "### CRC mismatch: given=0x" << std::hex << givenCrc << " calc=0x" << crc << std::dec << std::endl;
    }
#endif
    packet[5] = crc & 0xFF;
    packet[6] = (crc >> 8);

    myDevice->write((const char*) packet, packetLen);
}
