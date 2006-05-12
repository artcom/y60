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
// Panasonic PT-D5500 projector
//
//=============================================================================

#ifndef _ac_watchdog_Panasonic_h_
#define _ac_watchdog_Panasonic_h_

#include "Projector.h"

#include <vector>
#include <string>

class PanasonicProjector : public Projector
{
public:
    explicit PanasonicProjector(int thePortNumber, int theBaud);

    virtual void configure(const dom::NodePtr & theConfigNode);

    virtual void power(bool thePowerFlag);
    virtual void selectInput(VideoSource theSource);
    virtual void lamps(unsigned theLampsMask);
    virtual void lampPower(bool thePowerHighFlag);
    virtual void shutter(bool theShutterFlag);
    virtual void update();
    
private:
    unsigned _myNumProjectors;
    unsigned _myFirstID;
    unsigned _myPowerDelay;
    unsigned _myReadTimeout;

    std::vector<std::string> _myResponses;

    void sendCommand(const std::string & theCommand, const std::string & theParams = "", unsigned theTimeout = 0);
    void sendCommandSingle(const std::string & theCommand, const std::string & theParams, unsigned theAddress);
    void readFromDevice(unsigned theTimeout = 300);
    
    void parseResponse(const std::string & thePattern, const std::string & theParamName, const std::string & theUnit = "");
};

#endif
