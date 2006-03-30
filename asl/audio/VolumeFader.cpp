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

#include "VolumeFader.h"

namespace asl {

VolumeFader::VolumeFader(SampleFormat theSampleFormat) 
    : Effect(createEffectFunctor<VolumeFaderFunctor>(theSampleFormat))
{
    _myCurrentVolume = _myBeginVolume = _myEndVolume = 0.0;
    _myCurrentFrame = _myFadeBeginFrame = _myFadeEndFrame = 0;
}

void VolumeFader::setVolume(float theTargetVolume, unsigned theFadeDurationFrames) {
    _myBeginVolume = _myCurrentVolume;
    _myEndVolume = theTargetVolume;

    _myFadeBeginFrame = _myCurrentFrame;
    _myFadeEndFrame = _myCurrentFrame + theFadeDurationFrames;
}

float VolumeFader::getVolume() const {
    // Hack: In a real fade, we want to return the actual volume. In a fade that's
    // just there to prevent clicks, this returns the destination volume.
    if (_myCurrentFrame + DEFAULT_FADE_FRAMES < _myFadeEndFrame) {
        return getVolume(_myCurrentFrame);
    } else {
        return _myEndVolume;
    }
}

float VolumeFader::getVolume(Unsigned64 theFrame) const {
    if(_myCurrentFrame >= _myFadeBeginFrame) {
        if (theFrame < _myFadeEndFrame) {
            float myVolumeDiff = _myEndVolume-_myBeginVolume;
            float myFadePercent = float(theFrame - _myFadeBeginFrame)/
                    (_myFadeEndFrame - _myFadeBeginFrame);
            float myVolume = _myBeginVolume+myFadePercent*myVolumeDiff;
            return myVolume;
        } else {
            return _myEndVolume;
        }
    } else {
        AC_WARNING << "VolumeFader::getVolume: _myCurrentFrame < _myFadeBeginFrame";
        return _myBeginVolume;
    }
}

} // namespace
