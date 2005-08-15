//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Media.h"

#include <asl/Logger.h>
#include <asl/Pump.h>
#include <asl/Auto.h>

using namespace std;
using namespace asl;

extern "C"
EXPORT asl::PlugInBase * y60Media_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::Media(myDLHandle);
}

namespace y60 {

const double myTimePerSlice = 0.05;
    
Media::Media(asl::DLHandle theDLHandle)
    : PlugInBase(theDLHandle)
{
    AC_DEBUG << "Media::Media";
    fork();
}

Media::~Media() {
    if (_mySounds.size() != 0) {
        AC_WARNING << "Deleting Media, but " << _mySounds.size() << " sounds are still active.";
        stopAll();
    }
    join();
}

void Media::setSysConfig(const Time& myLatency, const string& myDeviceName) {
    Pump::setSysConfig(myLatency, myDeviceName);
}

void Media::setAppConfig(unsigned mySampleRate, unsigned numOutputChannels, 
        bool useDummy)
{
    Pump::setAppConfig(mySampleRate, numOutputChannels, useDummy);
}

SoundPtr Media::createSound(const string & theURI, bool theLoop,
        const std::string & theName)
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    string myName = theName;
    if (myName.empty()) {
        myName = theURI;
    }
    HWSampleSinkPtr mySampleSink = Pump::get().createSampleSink(myName); 
    SoundPtr mySound = SoundPtr(new Sound(theURI, mySampleSink, theLoop));
    mySound->setSelf(mySound);
    _mySounds.push_back(mySound);
    return mySound;
}

SoundPtr Media::createSound(const string & theURI, Ptr<ReadableStream> theStream, 
        bool theLoop)
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    return SoundPtr(0);
}

void Media::setVolume(float theVolume) {
    Pump::get().setVolume(theVolume);
}

void Media::fadeToVolume(float theVolume, float theTime) {
    Pump::get().fadeToVolume(theVolume, theTime);
}


float Media::getVolume() const {
    return Pump::get().getVolume();
}

unsigned Media::getNumSounds() const {
    return Pump::get().getNumSinks();
}

void Media::stopAll() {
    {
        AC_DEBUG << "Media::stopAll";
        std::vector < SoundWeakPtr >::iterator it;
        unsigned myNumSoundsStopped = 0;
        // Here, we'd like to just lock the Sounds, iterate through the sounds and
        // call stop on all of them. This takes too long, though, so we can't lock 
        // the whole time. That in turn means that the list of sounds might change 
        // while we're iterating through it.
        for (int i = _mySounds.size()-1; i >= 0; --i) {
            AutoLocker<ThreadLock> myLocker(_myLock);
            if (i >= _mySounds.size()) {
                // This can happen if sounds have been deleted in the meantime.
                i = _mySounds.size()-1;
            }
            SoundPtr curSound = _mySounds[i].lock();
            if (curSound) {
                AC_TRACE << "stopAll: Stopping " << curSound->getName();
                curSound->stop();
                myNumSoundsStopped++;
            }
            msleep(1);
        }
        if (myNumSoundsStopped) {
            AC_DEBUG << "stopAll: " << myNumSoundsStopped << " sounds stopped.";
        }
    }
    msleep(100);

    update();
    AC_DEBUG << "Media::stopAll end";
}

void Media::update() {
    AutoLocker<ThreadLock> myLocker(_myLock);
    std::vector < SoundWeakPtr >::iterator it;
    for (it = _mySounds.begin(); it != _mySounds.end();) {
        SoundPtr curSound = (*it).lock();
        if (!curSound) {
            it = _mySounds.erase(it);
        } else {
            curSound->update(myTimePerSlice);
            ++it;
        }
    }
}

void Media::run() {
    AC_DEBUG << "Media::run started";
    while (!shouldTerminate()) {
        update();
        msleep(unsigned(myTimePerSlice*1000));
    }
    AC_DEBUG << "Media::run ended";
}

} // namespace
