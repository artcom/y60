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
