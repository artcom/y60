//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_WII_EVENT_INCLUDED
#define Y60_WII_EVENT_INCLUDED

#include <string>
#include <asl/Vector234.h>

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
    WII_STATUS
};

class WiiEvent {
    public:
    // Ctor for connection events
    WiiEvent(unsigned theID, WiiEventType theEvent) :
        id( theID ),
        type( theEvent ) {}

    // Ctor for button events
    WiiEvent(unsigned theID, const std::string & theName, bool thePressedFlag) :
        id( theID ),
        type( WII_BUTTON ),
        buttonname( theName ),
        pressed( thePressedFlag ) {}

    // Ctor for motion events
    WiiEvent(unsigned theID, const asl::Vector3f & theAcceleration) :
        id( theID ),
        type( WII_MOTION ),
        acceleration( theAcceleration ) {}

    // Ctor for ir data
    WiiEvent(unsigned theID, const asl::Vector2i theIRData[4], const asl::Vector2f & theScreenPosition ) :
        id( theID ),
        type( WII_INFRARED ),
        screenPosition( theScreenPosition )
    {
        for (unsigned i = 0; i < 4; ++i) {
            irPositions[i] = theIRData[i];
        }
    }

    // Ctor for status reports
    WiiEvent(unsigned theID, asl::Unsigned8 theBatteryLevel, asl::Unsigned8 theStatusBits ) :
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

    unsigned id;

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
