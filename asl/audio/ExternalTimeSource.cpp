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

#include "ExternalTimeSource.h"
#include "Pump.h"
namespace asl {
    
ExternalTimeSource::ExternalTimeSource() 
    : _myStartTime(0), _myTimeOffset(0), _myPausedTime(0), _myPauseFlag(false)
{}

ExternalTimeSource::~ExternalTimeSource() {}

Time ExternalTimeSource::getCurrentTime() const {
    return Pump::get().getCurrentTime() - _myStartTime - _myTimeOffset;     
}

void ExternalTimeSource::stop() {
    _myTimeOffset = 0;
    _myStartTime = 0;
    _myPausedTime = 0;
    _myPauseFlag = false;
}

void ExternalTimeSource::pause() {
    if (!_myPauseFlag) {
        _myPausedTime = Pump::get().getCurrentTime();   
        _myPauseFlag = true; 
    }
}

void ExternalTimeSource::run() {
    if (_myPauseFlag) {
        _myTimeOffset = _myTimeOffset + (Pump::get().getCurrentTime() - _myPausedTime);
    } else {
        _myStartTime = Pump::get().getCurrentTime();
    }
    _myPauseFlag = false;
}

}
