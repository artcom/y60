/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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

#include "DummyPump.h"

#include <asl/base/Logger.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Assure.h>

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

