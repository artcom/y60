//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SoundIn.cpp,v $
//
//     $Author: thomas $
//
//   $Revision: 1.7 $
//
//
// Description:
//
//
//=============================================================================

#include "SoundIn.h"
#include "AudioBuffer.h"
#include "AudioOutput.h"
#include "SoundCard.h"

#include <iostream>

namespace AudioBase {

using namespace std;

SoundIn::SoundIn (SoundCard* theSoundCard, int myNumChannels,
        int mySampleRate, double myVolume)
    : AudioAsyncModule ("SoundIn", mySampleRate, myVolume, true),
      _myNumUnderruns(0),
      _myReadBlocks(0),
      _mySoundCard(theSoundCard)
{
    //TODO: Tell the soundcard what sample rate we're using.
    _mySoundCard->openInput(myNumChannels);

    for (int i=0; i<myNumChannels; i++) {
        newOutput (Mono);
    }
}

SoundIn::~SoundIn () {
    for (int i=0; i<getNumOutputs(); i++) {
        getOutput(i)->stop();
    }
//    cerr << "SoundIn::~SoundIn()" << endl;
    _mySoundCard->closeInput();

    for (int i=0; i<_myBuffer.size(); i++) {
        delete _myBuffer[i];
    }
}

bool SoundIn::init() {
    for (int i=0; i<getNumOutputs(); i++) {
        _myBuffer.push_back(new AudioBuffer());
    }
    go();
    return AudioAsyncModule::init();
}

void
SoundIn::process () {
    _mySoundCard->read(_myBuffer);

    for (int i=0; i<getNumOutputs(); i++) {
        getOutput(i)->put(*getBuffer(i));
    }
}

AudioBuffer *
SoundIn::getBuffer(int i) {
    if (i>=0 && i < _myBuffer.size() && _myBuffer[i] ) {
        return _myBuffer[i];
    } else {
        return 0;
    }
}

int
SoundIn::getNumOverruns() const {
    return _myNumUnderruns;
};

}
