//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: SpatialMixer.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.14 $
//
//
// Description:
//
//
//=============================================================================

#include "SpatialMixer.h"

#include "AudioSyncModule.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "AudioBuffer.h"
#include "NullSource.h"
#include "AudioScheduler.h"

#include <asl/string_functions.h>
#include <asl/Logger.h>

#include "asl/string_functions.h"


using namespace std;
using namespace asl;

namespace AudioBase {

SpatialMixer::SpatialMixer
        (int numChannels, int mySampleRate, const double myVolume,
             const asl::Vector2d & myExtents)
    : AudioSyncModule ("SpatialMixer", 0, mySampleRate, myVolume),
      _myExtents(myExtents)
{
    for (int i=0; i<numChannels; i++) {
        _myChannels.push_back(new ChannelInfo(newOutput(getChannelType(i, numChannels))));
    }
    _myNull = new NullSource(mySampleRate);

}

SpatialMixer::~SpatialMixer() {
    for (int i=0; i<_myChannels.size(); i++) {
        delete _myChannels[i];
    }
}

bool
SpatialMixer::init() {
    for (int i=0; i<_myChannels.size(); i++) {
        _myChannels[i]->_myBuffer=new AudioBuffer();
    }

    bool myRetVal = _myNull->init();
    if (myRetVal) {
        AudioInput * nullInput = newInput(Mono);
        AudioScheduler::get().connect(_myNull->getOutput(), nullInput);
        setInputPos(nullInput, asl::Vector2d(0.0,0.0));
    }
    return myRetVal;
}

void
SpatialMixer::setOutputPos(int myChannel, const asl::Vector2d & myPos) {
    _myChannels[myChannel]->_myPos = asl::Vector2d(myPos);
}

void
SpatialMixer::processChannel
        (AudioInput * curInput,
         int theOutputChannel,
         double theChannelVolume)
{
    AudioBuffer * myInputBuffer = curInput->get();
    if (myInputBuffer) {
        SAMPLE* myInputData = myInputBuffer->getData();
        SAMPLE* myOutputData =
                _myChannels[theOutputChannel]->_myBuffer->getData();

        unsigned myBufferSize = AudioBuffer::getNumSamples();
        for(int j = 0; j < myBufferSize; j++) {
            myOutputData[j] += myInputData[j]*theChannelVolume;
        }
    }
}

vector<double>
SpatialMixer::getChannelVolumes(const Vector2d & myPos) {
//    AC_DB("getChannelVolumes: myPos " << myPos[0] << ", " << myPos[1]);
    vector<double> myVolumes;
    for (int i=0; i<_myChannels.size(); i++) {
        myVolumes.push_back(0);
    }

    // Calculate 1/distance for each speaker. Store them in a map so they're
    // sorted by volume.
    multimap<double, int, greater<double> > myVolumeMap;
//    AC_DB("getChannelVolumes: 1/dist");
    for (int i=0; i<_myChannels.size(); i++) {
        double myDist = asl::length(myPos - _myChannels[i]->_myPos);
        double myAtten;
        if (myDist > 0.0001) {
            myAtten = 1./myDist;
        } else {
            myAtten = 10;
        }
        myVolumeMap.insert(std::make_pair(myAtten, i));
//        AC_DB("  " << i << ": " << myAtten);
    }

    // Only the four closest speakers are relevant.
    double myVolumeSum = 0;
    multimap<double, int, greater<double> >::iterator it = myVolumeMap.begin();
    for (int j=0; j<2; j++) {
        myVolumes[it->second] = it->first;
        myVolumeSum += it->first;
        it++;
    }
//    AC_DB("myVolumeSum: " << myVolumeSum);
//    AC_DB("Volumes: ");
    for (int i=0; i<_myChannels.size(); i++) {
        // Normalize and turn distance into volume setting by taking
        // sqrt.
        if (myVolumes[i] > 0.001) {
//            AC_DB("       " << i << ": " << myVolumes[i] << " -> ");
            myVolumes[i] = sqrt(myVolumes[i]/myVolumeSum);
//            AC_DB(myVolumes[i]);
        }
    }
    return myVolumes;
}

bool
SpatialMixer::isCorrectChannelType
        (AudioInput * theInput, int theChannelIndex, const asl::Vector2d & theInputPos)
{
    // Left/right positioning is done via the y coordinate. Everything with a
    // lesser y coordinate than the sound is assumed to be left, everything else
    // is right.
    const Vector2d & myOutputPos = _myChannels[theChannelIndex]->_myPos;
    ChannelType typeWanted;
    if (myOutputPos[1] < theInputPos[1]) {
        typeWanted = Left;
    } else {
        typeWanted = Right;
    }
    return theInput->getType() == typeWanted || theInput->getType() == Mono;
}

void
SpatialMixer::process() {
    for (int i=0; i<_myChannels.size(); i++) {
        _myChannels[i]->_myBuffer->clear();
    }

    for (int i=0; i<getNumInputs(); i++) {
        AudioInput * curInput = getInput(i);
        if (curInput->isConnected()) {
            vector<double> myDistVolumes;
            myDistVolumes = getChannelVolumes(_myInputPosMap[curInput]);
            for (int j=0; j<_myChannels.size(); j++) {
                if (isCorrectChannelType
                        (curInput, j, _myInputPosMap[curInput])) {
                    processChannel(curInput, j, myDistVolumes[j]);
                }
            }
        }
    }

    for (int i=0; i<_myChannels.size(); i++) {
        _myChannels[i]->_myOutput->put(*_myChannels[i]->_myBuffer);
    }

    resetDeliveredInputs();
}

void
SpatialMixer::setInputPos(AudioInput* theInput, const asl::Vector2d & myPos){
    // Incoming coordinates are normed to 0..1, internal coordinates
    // are in meters.
    Vector2d meterPos (myPos[0]*_myExtents[0], myPos[1]*_myExtents[1]);
    _myInputPosMap[theInput] = meterPos;
}

const std::string
SpatialMixer::getName() {
    return string("SpatialMixer (#inputs:")+as_string(getNumInputs())+")";
}

AudioInput *
SpatialMixer::getInputById(const string& Id) {
    // XXX
    if (Id == "Left") {
        return newInput(Left);
    } else if (Id == "Right") {
        return newInput(Right);
    } else {
        return newInput(Mono);
    }
}

const asl::Vector2d &
SpatialMixer::getExtents() {
    return _myExtents;
}

void
SpatialMixer::dump() {
    AC_PRINT << getName() << " --- " << _myChannels.size()
         << " output channels, extent: " << _myExtents[0] << ", "
         << _myExtents[1];
    for (int j=0; j<_myChannels.size(); j++) {
        AC_PRINT << "  Channel " << j << ": " << _myChannels[j]->_myPos[0]
             << ", " << _myChannels[j]->_myPos[1];
    }
}

void
SpatialMixer::inputDisconnected (AudioInput* theInput) {
    deleteInput (theInput);
    _myInputPosMap.erase (theInput);
}

SpatialMixer::ChannelInfo::ChannelInfo(AudioOutput * myOutput)
    : _myOutput(myOutput),
      _myBuffer(0),
      _myPos(0.0,0.0)
{
}

SpatialMixer::ChannelInfo::~ChannelInfo() {
    delete _myBuffer;
}


}

