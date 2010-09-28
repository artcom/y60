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

#include <numeric>

namespace asl {

VolumeFader::VolumeFader(SampleFormat theSampleFormat, unsigned int theChannelCount)
    : Effect(createEffectFunctor<VolumeFaderFunctor>(theSampleFormat))
{
    setChannelCount(theChannelCount);
    // _myCurrentVolume = _myBeginVolume = _myEndVolume = 0.0;
    _myCurrentFrame = _myFadeBeginFrame = _myFadeEndFrame = 0;
}

void
VolumeFader::setChannelCount(unsigned int theCount) {
    _myCurrentVolumes.resize(theCount);
    _myBeginVolumes.resize(theCount);
    _myEndVolumes.resize(theCount);
}

void VolumeFader::setVolumes(std::vector<float> theTargetVolumes, unsigned theFadeDurationFrames) {
    _myBeginVolumes = _myCurrentVolumes;
    if (theTargetVolumes.size() != _myCurrentVolumes.size()) {
        throw Exception("VolumeFader::setVolumes() - number of volumes doesn't match number of channels", PLUS_FILE_LINE);
    }
    _myEndVolumes = theTargetVolumes; 
    _myFadeBeginFrame = _myCurrentFrame;
    _myFadeEndFrame = _myCurrentFrame + theFadeDurationFrames;
}
void VolumeFader::setVolume(float theTargetVolume, unsigned theFadeDurationFrames) {
    std::vector<float> targetVolumes(_myCurrentVolumes.size(), theTargetVolume);
    setVolumes(targetVolumes, theFadeDurationFrames);
}

float VolumeFader::getVolume() const {
    std::vector<float> myVolumes;
    getVolumes(myVolumes);
    return accumulate(myVolumes.begin(), myVolumes.end(), 0.0f) / myVolumes.size();
}
void VolumeFader::getVolumes(std::vector<float> & theVolumes) const {
    // Hack: In a real fade, we want to return the actual volume. In a fade that's
    // just there to prevent clicks, this returns the destination volume.
    if (_myCurrentFrame + DEFAULT_FADE_FRAMES < _myFadeEndFrame) {
        getVolumes(_myCurrentFrame, theVolumes);
    } else {
        theVolumes = _myEndVolumes;
    }
}

void VolumeFader::getVolumes(Unsigned64 theFrame, std::vector<float> & theVolumes) const {
    if(_myCurrentFrame >= _myFadeBeginFrame) {
        if (theFrame < _myFadeEndFrame) {
            theVolumes.resize(_myBeginVolumes.size());
            float myFadePercent = float(theFrame - _myFadeBeginFrame)/
                    (_myFadeEndFrame - _myFadeBeginFrame);
            for (AC_SIZE_TYPE i = 0; i < theVolumes.size(); ++i) {
                float myVolumeDiff = _myEndVolumes[i]-_myBeginVolumes[i];
                theVolumes[i] = _myBeginVolumes[i]+myFadePercent*myVolumeDiff;
            }
            return;
        } else {
            theVolumes = _myEndVolumes;
            return;
        }
    } else {
        AC_WARNING << "VolumeFader::getVolume: _myCurrentFrame < _myFadeBeginFrame";
        theVolumes = _myBeginVolumes;
        return;
    }
}

float VolumeFader::getVolume(Unsigned64 theFrame) const {
    std::vector<float> myVolumes;
    getVolumes(theFrame, myVolumes);
    return accumulate(myVolumes.begin(), myVolumes.end(),0.0f) / myVolumes.size();
}

} // namespace
