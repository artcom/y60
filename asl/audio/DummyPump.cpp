//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#include "DummyPump.h"

#include <asl/Logger.h>
//#include <asl/string_functions.h>
//#include <asl/numeric_functions.h>
//#include <asl/Assure.h>

//#include <exception>
//#include <sstream>
//#include <string.h>

using namespace std;

namespace asl {

DummyPump::~DummyPump () {
    AC_INFO << "DummyPump::~DummyPump";
    Pump::stop();
}

Time DummyPump::getCurrentTime () {
    return Time();
}

DummyPump::DummyPump () 
    : Pump(SF_F32, 0)
{
    AC_INFO << "DummyPump::DummyPump";
   
    setDeviceName("Dummy Sound Device");
    
    setCardName("Dummy Sound Card");

    _curFrame = 0;
    _myOutputBuffer.init(2048, getNumOutputChannels(), getNativeSampleRate());
    _myTempBuffer.init(2048, getNumOutputChannels(), getNativeSampleRate());
    
    dumpState();
    start();
}

void DummyPump::pump()
{
    static Time lastTime;
    
    msleep(unsigned(1000*getLatency()));
    Time curTime;
    double TimeSinceLastPump = curTime-lastTime;
    unsigned numFramesToDeliver = unsigned(TimeSinceLastPump*getNativeSampleRate());
    lastTime = curTime;
    AC_TRACE << "DummyPump::pump: numFramesToDeliver=" << numFramesToDeliver;

    mix(_myOutputBuffer, numFramesToDeliver);
}

}

