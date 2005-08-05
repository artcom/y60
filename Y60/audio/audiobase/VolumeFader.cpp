//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: VolumeFader.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.13 $
//
//
// Description:
//
//
//=============================================================================

#include "VolumeFader.h"
#include "AudioBuffer.h"
#include "AudioDefs.h"

#include <stdio.h>
#include <math.h>
#include <iostream>

#include <asl/Logger.h>

using namespace std;

namespace AudioBase {

VolumeFader::VolumeFader(double myVolume, bool myFadeIn)
    : _myLastVolume (myVolume),
      _myFadeStartSample (-10000),
      _myFadeDuration(0)
{
    if (myVolume > 50.0 || myVolume < 0.0) {
        AC_WARNING << "VolumeFader(): Volume " << myVolume
             << " has strange value. 1 is neutral (no amplification).";
    }
    if (myFadeIn) {
        _myVolume = 0;
        setVolume (0, myVolume);
    } else {
        _myVolume = myVolume;
    }
}

void
VolumeFader::applyVolume(unsigned curSample, AudioBuffer & theBuffer) const {
//    cerr << "VolumeFader::applyVolume" << endl;
    // This if is an optimization: if the volume is 1 and it isn't changing,
    // then we don't need to touch the buffer at all.
    if (_myFadeStartSample > curSample - AudioBuffer::getNumSamples() ||
        fabs (_myVolume - 1) > 0.0001)
    {
        SAMPLE * myData = theBuffer.getData();
        for (unsigned i = 0; i < AudioBuffer::getNumSamples(); ++i) {
            myData[i] *= getVolume (curSample+i);
        }
    }
/*
    if (theBuffer.hasDistortion()) {
        AC_WARNING("VolumeFader::put(): distortion. Volume= "
            << _myVolume << ", max: " << theBuffer.getMax());
    }
*/
}

void
VolumeFader::setVolume(unsigned curSample, double myVolume) {
    AC_DEBUG << "setVolume: " << curSample << " " << myVolume << endl;
    fadeTo(curSample, myVolume, FADE_DURATION);
}

void
VolumeFader::fadeTo(unsigned curSample, double myVolume, unsigned numSamples) {
    _myLastVolume      = getVolume(curSample);
    _myFadeDuration    = numSamples;
    _myFadeStartSample = curSample;
    _myVolume          = myVolume;

    AC_TRACE << 
        "VolumeFader::fadeTo(): _myFadeStartSample= "
         << _myFadeStartSample << ", curSample= " << curSample
         << ", _myLastVolume: " << _myLastVolume
         << ", _myVolume: " << _myVolume;
}

double
VolumeFader::getVolume (unsigned curSample, bool doDebug) const {
    if (doDebug) {
        cerr << "VolumeFader::getVolume: curSample = " << curSample
                << ", _myFadeStartSample = " << _myFadeStartSample
                << ", _myVolume = " << _myVolume << endl;
    }
    if (curSample - _myFadeStartSample < _myFadeDuration) {
        if (curSample - _myFadeStartSample < 0) {
            cerr << "VolumeFader::getVolume: curSample (" << curSample
                     << ") < _myFadeStartSample (" << _myFadeStartSample << "). " << endl;
            return _myLastVolume;
        } else {
            double myFractionDone = ((double(curSample)-_myFadeStartSample)/_myFadeDuration);
            double curVolume =
                myFractionDone * (_myVolume-_myLastVolume)+_myLastVolume;
            if (myFractionDone < 0 || myFractionDone > 1) {
                cerr << "VolumeFader::getVolume: kaputt." << endl;
            }
            if (doDebug) {
                cerr << "         Fading, volume=" << curVolume << ", _myFadeDuration = " << _myFadeDuration
                        << ", myFractionDone = " << myFractionDone << ", _myLastVolume = " << _myLastVolume << endl;
            }
            return curVolume;
        }
    } else {
        return _myVolume;
    }
}

}
