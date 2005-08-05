//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioSyncModule.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.7 $
//
//
// Description: 
//
//
//=============================================================================

#include "AudioSyncModule.h"
#include "AudioScheduler.h"
#include "AudioInput.h"
#include "IAudioBufferListener.h"

#include <asl/Assure.h>
#include <asl/string_functions.h>

#include <algorithm>

#include <asl/Logger.h>


using namespace std;



namespace AudioBase {

AudioSyncModule::AudioSyncModule(const char * myName, int myNumChannels, 
        int mySampleRate, double myVolume) 
    : AudioModule (myName, mySampleRate, myVolume),
      _myDeliveredInputs(0)
{
    switch (myNumChannels) {
    case 1:
        newInput(Mono);
        break;
    case 2:
        newInput(Left);
        newInput(Right);
        break;
    case 6:
        newInput(Left);
        newInput(Right);
        newInput(Mono);
        newInput(Sub);
        newInput(RearLeft);
        newInput(RearRight);
        break;
    default:
        newInput(Mono);
        break;
    }
}

AudioSyncModule::~AudioSyncModule() {    
    //AC_DEBUG << "~AudioSyncModule";
    vector<AudioInput*>::iterator it;
    for (it=_myInputs.begin(); it != _myInputs.end(); ++it) {
        delete *it;
    }
    _myInputs.clear();
}
    
void 
AudioSyncModule::nextInputDelivered() {
    _myDeliveredInputs += 1;
    AC_TRACE << "Delivered: " << _myDeliveredInputs << 
            ", waiting for: " << getNumConnectedInputs();
    if(_myDeliveredInputs == getNumConnectedInputs()) {
        // module has all neccessary data, place on scheduler list
        
        AudioScheduler::get().makeRunnable(this);
    }
    AC_TRACE <<  "_myInputs.size: " << _myInputs.size();
    if (_myDeliveredInputs > _myInputs.size()) {
        // If this happens, there's an internal mixup in the filter graph.
        // Since I don't trust gdb, I'll dump everything here.
        AudioScheduler::get().dump(true);
        ASSURE(false);
    }
}

int 
AudioSyncModule::getNumInputs() {
    return _myInputs.size();
}

AudioInput * 
AudioSyncModule::getInput(int i) {
    //AC_DB(getName() << "::getInput(" << i << ")");
    //AC_DB("  number of inputs: " << _myInputs.size());
    //AC_DB("  input type: " << _myInputs[i]->getType());
    ASSURE(i < _myInputs.size());
    return _myInputs[i];
}

AudioInput * 
AudioSyncModule::getInputById(const string& Id) {
    int i = asl::as_int(Id);
    return getInput(i);
}

void 
AudioSyncModule::dump (char * theIndent) {
    AudioModule::dump(theIndent);
    AC_PRINT <<  "_myDeliveredInputs = " << _myDeliveredInputs << endl;;
    AC_PRINT <<  "_myInputs.size() = " << _myInputs.size() << endl;;
}

AudioInput *
AudioSyncModule::newInput(ChannelType theType) {
    AudioInput * theInput = new AudioInput(*this, theType);
    _myInputs.push_back(theInput);
    return theInput;    
}

void
AudioSyncModule::deleteInput (AudioInput * theInput) {
    vector<AudioInput*>::iterator it;
    it = find(_myInputs.begin(), _myInputs.end(), theInput);
    if (it != _myInputs.end()) {
        _myInputs.erase(it);
    }
    delete theInput;
}

void AudioSyncModule::resetDeliveredInputs() {
    _myDeliveredInputs = 0;
}

int AudioSyncModule::getNumConnectedInputs() {
    vector<AudioInput *>::iterator it;
    int myNumConnectedInputs = 0;
    for (it = _myInputs.begin(); it != _myInputs.end(); it++) {
        if ((*it)->isConnected()) {
            myNumConnectedInputs++;
        }
    }
    return myNumConnectedInputs;
}

void 
AudioSyncModule::registerListener(IAudioBufferListener* myListener) {
    _myListener.push_back(myListener);
}

void
AudioSyncModule::callListeners() {
    if ( ! _myListener.empty()) {
        for (int i = 0; i < getNumInputs(); i++) {
            AudioBuffer * myBuffer = getInput(i)->get();
            for (unsigned int j = 0; j < _myListener.size(); ++j) {
                _myListener[j]->onNewData(this, i, myBuffer);
            }
        }
    }
}


}
