//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Mixer.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.14 $
//
//
// Description: 
//
//
//=============================================================================

#include "Mixer.h"

#include "AudioSyncModule.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"
#include "NullSource.h"
#include "AudioScheduler.h"

#include <asl/string_functions.h>
#include <asl/Assure.h>

using namespace std; 
using namespace asl;

namespace AudioBase {

    // outputs are created in createReader so no need to create them in
    // the ctor of the base class
Mixer::Mixer(unsigned myNumChannels, int mySampleRate, const double myVolume) 
    : AudioSyncModule ("Mixer", 0, mySampleRate, myVolume), _myNumChannels(myNumChannels), _myNull(0)
{
    for (unsigned i = 0; i < myNumChannels; ++i)
    {
        ChannelType type = getChannelType(i, myNumChannels);
        _myMixerOutputs.push_back(newOutput(type));
    }
    _myNull = new NullSource(mySampleRate);    
}

Mixer::~Mixer() {
    ASSURE_MSG(getNumInputs() == 1, "Only the NullSource should be connected to the mixer on destruction");
    for (unsigned i = 0; i < _myNumChannels; ++i) {
        delete _myMixerOutputs[i];
        _myMixerOutputs[i] = NULL;
    }
    _myMixerOutputs.clear();
}

bool 
Mixer::init() {
    for (unsigned i = 0; i < _myNumChannels; ++i) {
        _myMixerBuffers.push_back(new AudioBuffer());
    }
    if (_myNull) {
        _myNull->init();
    }
    // We connect the nullsource to all channels that don't have an input
    AudioScheduler::get().connect (_myNull->getOutput(), newInput(Mono));
    return true;
}

void
Mixer::processChannel(unsigned index) {
    AudioBuffer * myBuffer = _myMixerBuffers[index];
    ChannelType type = getChannelType(index, _myNumChannels);

    // Initialize mixed data
    SAMPLE * myMixedData = myBuffer->getData();
    for(unsigned j = 0; j < myBuffer->getNumSamples(); ++j) {
        myMixedData[j] = 0;
    }

    // Find all inputs of the same type and mix them together
    for (unsigned i = 0; i < getNumInputs(); ++i) {
        AudioInput * myInput = getInput(i);
        if (myInput->isConnected() == false || myInput->getType() != type) {
            continue;
        }

        AudioBuffer * myChannelBuffer = myInput->get();
        if (myChannelBuffer) {
            SAMPLE* myChannelData = myChannelBuffer->getData();
            for(unsigned j = 0; j < myBuffer->getNumSamples(); ++j) {
                myMixedData[j] += myChannelData[j];
            }
        } 
    }
}

void 
Mixer::process() {
    for (unsigned i = 0; i < _myNumChannels; ++i) {
        processChannel(i);
        _myMixerOutputs[i]->put(*_myMixerBuffers[i]);
    }
    resetDeliveredInputs();
}

const std::string 
Mixer::getName() {
    return string("Mixer (#inputs:")+as_string(getNumInputs())+")";
}

AudioOutput * 
Mixer::getOutput(ChannelType theOutput) {
    return _myMixerOutputs[getChannelIndex(theOutput, _myNumChannels)];
}

AudioInput * 
Mixer::getInputById(const string& Id) {
    // XXX This is a Hack, Mixer shouldn't know which Types are available
    if (Id == "Left") {
        return newInput(Left);
    } else if (Id == "Right") {
        return newInput(Right);
    } else if (Id == "Mono") {
        return newInput(Mono);
    } else if (Id == "Center") {
        return newInput(Center);
    } else if (Id == "Sub") {
        return newInput(Sub);
    } else if (Id == "RearRight") {
        return newInput(RearRight);
    } else if (Id == "RearLeft") {
        return newInput(RearLeft);
    } else {
        return newInput(Mono);
    }
}

void 
Mixer::inputDisconnected (AudioInput* theInput) {
    deleteInput (theInput);
}

}

