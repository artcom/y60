//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioEater.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.3 $
//
//
// Description: 
//
//
//=============================================================================

#include "AudioEater.h"
#include "AudioSyncModule.h"
#include "AudioInput.h"
#include "AudioBuffer.h"

#include <asl/Time.h>
#include <math.h>


using namespace std;


namespace AudioBase {

AudioEater::AudioEater(int numChannels, int mySampleRate)
        : AudioSyncModule("AudioEater", numChannels, mySampleRate, 1.0), 
          _numBuffersEaten(0),
          _myMaxVolume(0)
{ 
}

AudioEater::~AudioEater() { 
}

void AudioEater::process () {
    _myMaxVolume = max (_myMaxVolume, getInput(0)->get()->getMax());
    resetDeliveredInputs();
    _numBuffersEaten++;
}

double AudioEater::getMaxVolume() {
    return _myMaxVolume;
}

int AudioEater::getBuffersEaten() {
    return _numBuffersEaten; 
}

}





