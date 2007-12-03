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

#include "ExternalTimeSource.h"
#include "Pump.h"
namespace asl {
    
ExternalTimeSource::ExternalTimeSource() 
    : _myTimeOffset(0), _myStartTime(0), _myPauseFlag(false), _myPausedTime(0)
{
}

ExternalTimeSource::~ExternalTimeSource() {
}

Time ExternalTimeSource::getCurrentTime() const {
    return Pump::get().getCurrentTime() - _myStartTime - _myTimeOffset;     
}

void ExternalTimeSource::stop() {
    _myTimeOffset = 0;
    _myStartTime = 0;
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
