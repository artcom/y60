//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundOut.cpp,v $
//     $Author: pavel $
//   $Revision: 1.9 $
//
//=============================================================================

#include "SoundOut.h"
#include "AudioBuffer.h"
#include "SoundException.h"
#include "AudioInput.h"
#include "SoundCard.h"

#include <asl/Logger.h>

#include <iostream>
#include <iomanip>
#include <math.h>

namespace AudioBase {
    
using namespace std;
using namespace asl;

SoundOut::SoundOut (SoundCard* theSoundCard,  
                    int numChannels, int mySampleRate, double myVolume)
    : AudioSyncModule("SoundOut", numChannels, mySampleRate, myVolume),
      _mySoundCard(theSoundCard),
      _myMaxVolume(0),
      _myNumClicks(0)
{
    initDevice ();
}

void 
SoundOut::initDevice () {
    int myNumChannels = getNumInputs();
    _mySoundCard->openOutput();
}

SoundOut::~SoundOut () {
    _mySoundCard->closeOutput();

    if (_myNumClicks > 0) {
        AC_DEBUG << "~SoundOut: Number of potential clicks: " << _myNumClicks;
    }
    if (_myMaxVolume > 1) {
        AC_DEBUG << "~SoundOut: WARNING! Volume was too high. Maximum was " << _myMaxVolume; 
    }
/*    AC_DB("SoundOut timing: Avg time per frame: " << _myAvgFrameTime << 
             ", max. time per frame: " << _myMaxFrameTime);
*/             
}

void 
SoundOut::process () {
    vector <AudioBuffer *> myBuffers;

    // int myRetVal;
    AudioBuffer * myBuffer;
    for (int i=0; i < getNumInputs(); i++) {
        AudioInput * myInput = getInput(i);
        if (myInput && myInput->isConnected()) {
            myBuffer = myInput->get();
            getFader().applyVolume(getCurSample(), *myBuffer);
            myBuffers.push_back(myBuffer);
            if (myBuffer->hasDistortion()) {
                AC_WARNING << "Distortion!";
                double newMax = myBuffer->getMax();
                if (newMax > _myMaxVolume) {
                    _myMaxVolume = newMax;
                }
            }
            if (myBuffer->hasClicks()) {
                _myNumClicks++;
            }
        }
    }

    // Sound cards expect all their output channels to be delivered. 
    // If some inputs have been closed and others are still open (e.g. 
    // mono files where the NullSource still lives), this
    // would crash.
    if (myBuffers.size() == _mySoundCard->getNumOutChannels()) {
        _mySoundCard->write(myBuffers);
    }

    resetDeliveredInputs();
}

bool 
SoundOut::hasClicks() {
    return (_myNumClicks > 0);
}

void 
SoundOut::outputDeviceStats () const {
    // int myRetVal;

    if (_mySoundCard) {
        cerr << *_mySoundCard;
    }
}

void 
SoundOut::inputDisconnected (AudioInput* theInput) {
    _mySoundCard->getCurrentTime();
}

}
