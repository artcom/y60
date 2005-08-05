
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: AudioModule.cpp,v $
//     $Author: pavel $
//   $Revision: 1.12 $
//
//=============================================================================

#include "AudioModule.h"
#include "AudioOutput.h"
#include "AudioDefs.h"
#include "VolumeFader.h"
#include "AudioBuffer.h"

#include <asl/Logger.h>
#include <asl/Assure.h>

#include <algorithm>
#include <list>

namespace AudioBase {

//#define ASSURE_POLICY AssurePolicy::Throw
#define DB(x) //x

using namespace std;

AudioModule::AudioModule(const std::string & theName, int theSampleRate,
                         double theVolume, bool theFadeIn)
   : _myName(theName),
     _myVolume(theVolume), _myFader(theVolume, theFadeIn),
     _curSample(0),
     _mySampleRate(theSampleRate)
{
    AC_DEBUG << "AudioModule " << (void*)this << " " << _myName;
}

AudioModule::~AudioModule() {
    AC_DEBUG << "~AudioModule " << (void*)this << " " << _myName;
    for (vector<AudioOutput*>::iterator i = _myOutputs.begin(); i != _myOutputs.end(); ++i) {
        AudioOutput* output = (*i);
        if (output) {
            delete output;
        }
    }
}


AudioOutput *
AudioModule::newOutput (ChannelType theChannel) {
    AudioOutput * theOutput = new AudioOutput(theChannel, _myVolume, &_myFader);
    _myOutputs.push_back(theOutput);
    return theOutput;
}

AudioOutput *
AudioModule::findOutput(ChannelType theChannel) const {
    for (std::vector<AudioOutput*>::const_iterator it = _myOutputs.begin();
        it != _myOutputs.end(); ++it) {
        if ((*it)->getType() == theChannel) {
            return *it;
        }
    }
    return NULL;
}

void
AudioModule::deleteOutput (AudioOutput * theOutput) {
    std::vector<AudioOutput*>::iterator it =
            find (_myOutputs.begin(), _myOutputs.end(), theOutput);
    if (it != _myOutputs.end()) {
        _myOutputs.erase (it);
        delete theOutput;
    }
}

void
AudioModule::postProcess () {
    _curSample += AudioBuffer::getNumSamples();
}

void
AudioModule::setVolume (double myVolume) {
    DB(AC_TRACE << "AudioModule::setVolume() volume=" << myVolume
            << " _curSample= " << _curSample);
    _myVolume = myVolume;
    _myFader.setVolume(_curSample, _myVolume);
}

void
AudioModule::fadeToVolume (double theVolume, double theDuration) {
    DB(AC_TRACE << "AudioModule::fadeToVolume(): " << theVolume
            << ", _curSample= " << _curSample << ", theDuration= "
            << theDuration << ", sample rate: " << getSampleRate());
    if ((theDuration*getSampleRate()) < FADE_DURATION) {
        setVolume(theVolume);
    } else {
        _myVolume = theVolume;
        _myFader.fadeTo(_curSample, theVolume, int(theDuration*getSampleRate()));
    }
}

void
AudioModule::setSampleRate(int theSampleRate) {
    _mySampleRate = theSampleRate;
}

const
VolumeFader&
AudioModule::getFader() const {
    return _myFader;
}

unsigned
AudioModule::getCurSample() const {
    return _curSample;
}

double
AudioModule::getVolume () {
    return _myFader.getVolume(_curSample, false /* do debug */);
}

ModuleList
AudioModule::getConnectedModules() {
    ModuleList myModules;
    vector<AudioOutput *>::iterator i;
    for (i=_myOutputs.begin(); i != _myOutputs.end(); i++) {
        if ((*i)->getInputModule()) {
            myModules.push_back((*i)->getInputModule());
        }
    }
    return myModules;
}

int
AudioModule::getNumOutputs() const {
    return _myOutputs.size();
}


AudioOutput*
AudioModule::getOutput(int i) {
    ASSURE(i < _myOutputs.size());
    return _myOutputs[i];
}

const std::string
AudioModule::getName() {
    if (_myName.length() < 200) {
        return _myName;
    } else {
        // Bogus name => entry is probably trashed.
        return "Invalid entry";
    }
}

int
AudioModule::getSampleRate() const {
    return _mySampleRate;
}


void
AudioModule::dump(char * theIndent) {
    AC_TRACE << theIndent << "Module: " << _myName << " (" << this << ")";
    ModuleList myModules = getConnectedModules();
    AC_TRACE << theIndent << "  Number of connected modules: " << myModules.size();
    for (ModuleList::const_iterator it = myModules.begin(); it != myModules.end(); ++it) {
        AC_TRACE << theIndent << "    "  << *it << ": " << (*it)->getName();
    }
    AC_TRACE << theIndent << "  _myVolume: " << _myVolume;
}

}
