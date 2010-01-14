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

#ifndef Y60_WII_EVENT_INCLUDED
#define Y60_WII_EVENT_INCLUDED

#include <string>
#include <asl/math/Vector234.h>

enum WiiReportMode {
    BUTTON,
    MOTION,
    INFRARED
};

enum WiiEventType {
    WII_BUTTON,
    WII_MOTION,
    WII_INFRARED,
    WII_LOST_CONNECTION,
    WII_FOUND,
    WII_STATUS,
    WII_FOUND_IR_CURSOR,
    WII_LOST_IR_CURSOR
};

class WiiEvent {
    public:
    // Ctor for connection events
    WiiEvent(const std::string & theID, WiiEventType theEvent) :
        id( theID ),
        type( theEvent ) {}

    // Ctor for button events
    WiiEvent(const std::string & theID, const std::string & theName, bool thePressedFlag) :
        id( theID ),
        type( WII_BUTTON ),
        buttonname( theName ),
        pressed( thePressedFlag ) {}

    // Ctor for motion events
    WiiEvent(const std::string & theID, const asl::Vector3f & theAcceleration) :
        id( theID ),
        type( WII_MOTION ),
        acceleration( theAcceleration ) {}

    // Ctor for ir data
    WiiEvent(const std::string & theID, const asl::Vector2i theIRData[4], const asl::Vector2f & theScreenPosition ) :
        id( theID ),
        type( WII_INFRARED ),
        screenPosition( theScreenPosition )
    {
        for (unsigned i = 0; i < 4; ++i) {
            irPositions[i] = theIRData[i];
        }
    }

    // Ctor for status reports
    WiiEvent(const std::string & theID, asl::Unsigned8 theBatteryLevel, asl::Unsigned8 theStatusBits ) :
        id( theID ),
        type(WII_STATUS)
    {
        batteryLevel = float(theBatteryLevel) / float(0xC0);
        extensionConnected = theStatusBits & 0x02;
        speakerEnabled = theStatusBits & 0x04;
        continousReports = theStatusBits & 0x08;
        leds[0] = theStatusBits & 0x10;
        leds[1] = theStatusBits & 0x20;
        leds[2] = theStatusBits & 0x40;
        leds[3] = theStatusBits & 0x80;
    }

    std::string id;

    WiiEventType type;
    // Button data
    std::string buttonname;
    bool        pressed;

    // motion data
    asl::Vector3f acceleration;

    // infrared
    asl::Vector2i irPositions[4];
    asl::Vector2f screenPosition;

    // status report
    float batteryLevel;
    bool  extensionConnected;
    bool  speakerEnabled;
    bool  continousReports;
    bool  leds[4];
};

#endif // Y60_WII_EVENT_INCLUDED
