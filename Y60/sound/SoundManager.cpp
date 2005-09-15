//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "SoundManager.h"
#include "FFMpegDecoder.h"

#include <asl/Logger.h>
#include <asl/Pump.h>
#include <asl/Auto.h>

using namespace std;
using namespace asl;

namespace y60 {

const double myTimePerSlice = 0.05;
    
SoundManager::SoundManager() {
    AC_DEBUG << "SoundManager::SoundManager";
    _myFFMpegDecoderFactory = new FFMpegDecoderFactory;
    registerDecoderFactory(_myFFMpegDecoderFactory);
    fork();
}

SoundManager::~SoundManager() {
    AC_DEBUG << "SoundManager::~SoundManager";
    msleep(50);
    if (_mySounds.size() != 0) {
        AC_DEBUG << "Deleting SoundManager, but " << _mySounds.size() << " sounds are still active.";
        stopAll();
    }
    join();
    unregisterDecoderFactory(_myFFMpegDecoderFactory);
    delete _myFFMpegDecoderFactory;
    if (_myDecoderFactories.size()) {
        AC_WARNING << _myDecoderFactories.size() << " decoder factories still registered.";
    }
}

void SoundManager::setSysConfig(const Time& myLatency, const string& myDeviceName) {
    Pump::setSysConfig(myLatency, myDeviceName);
}

void SoundManager::setAppConfig(unsigned mySampleRate, unsigned numOutputChannels, 
        bool useDummy)
{
    Pump::setAppConfig(mySampleRate, numOutputChannels, useDummy);
}

void SoundManager::registerDecoderFactory(IAudioDecoderFactory* theFactory) {
    AC_DEBUG << "Registering decoder factory " << theFactory;
    _myDecoderFactories.insert(
            lower_bound(_myDecoderFactories.begin(), _myDecoderFactories.end(), theFactory),
            theFactory);
    
}

void SoundManager::unregisterDecoderFactory(IAudioDecoderFactory* theFactory) {
    std::vector <IAudioDecoderFactory*>::iterator it;
    
    for (it=_myDecoderFactories.begin(); it != _myDecoderFactories.end(); ++it) {
        if (*it == theFactory) {
            _myDecoderFactories.erase(it);
            return;
        }
    }
    AC_WARNING << "unregisterDecoderFactory: Factory at " << (void *)theFactory << 
            " not registered.";
}

SoundPtr SoundManager::createSound(const string & theURI) {
    // Workaround function since the JS binding doesn't support default parameters.
    return createSound(theURI, false, "");
}

SoundPtr SoundManager::createSound(const string & theURI, bool theLoop) {
    // Workaround function since the JS binding doesn't support default parameters.
    return createSound(theURI, theLoop, "");
}

SoundPtr SoundManager::createSound(const string & theURI, bool theLoop,
        const std::string & theName)
{
    // We need a factory function so we can set the Sound's mySelf pointer and so
    // we can do some decoder creation magic.
    AutoLocker<ThreadLock> myLocker(_myLock);
    string myName = theName;
    if (myName.empty()) {
        myName = theURI;
    }
    IAudioDecoder * myDecoder = createDecoder(theURI);
    SoundPtr mySound = SoundPtr(new Sound(theURI, myDecoder, theLoop));
    mySound->setSelf(mySound);
    _mySounds.push_back(mySound);
    return mySound;
}
/*
SoundPtr SoundManager::createSound(const string & theURI, Ptr<ReadableStream> theStream, 
        bool theLoop)
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    return SoundPtr(0);
}
*/
void SoundManager::setVolume(float theVolume) {
    Pump::get().setVolume(theVolume);
}

void SoundManager::fadeToVolume(float theVolume, float theTime) {
    Pump::get().fadeToVolume(theVolume, theTime);
}


float SoundManager::getVolume() const {
    return Pump::get().getVolume();
}

unsigned SoundManager::getNumSounds() const {
    return Pump::get().getNumSinks();
}

bool SoundManager::isRunning() const {
    return Pump::get().isRunning();
}

void SoundManager::stopAll() {
    {
        AC_DEBUG << "SoundManager::stopAll";
        std::vector < SoundWeakPtr >::iterator it;
        unsigned myNumSoundsStopped = 0;
        // Here, we'd like to just lock the Sounds, iterate through the sounds and
        // call stop on all of them. This takes too long, though, so we can't lock 
        // the whole time. That in turn means that the list of sounds might change 
        // while we're iterating through it.
        for (int i = _mySounds.size()-1; i >= 0; --i) {
            AutoLocker<ThreadLock> myLocker(_myLock);
            cerr << "0";
            if (i >= _mySounds.size()) {
                // This can happen if sounds have been deleted in the meantime.
                i = _mySounds.size()-1;
                if (i<0) {
                    break;
                }
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
    AC_DEBUG << "SoundManager::stopAll end";
}

void SoundManager::update() {
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

void SoundManager::run() {
    AC_DEBUG << "SoundManager::run started";
    while (!shouldTerminate()) {
        update();
        msleep(unsigned(myTimePerSlice*1000));
    }
    AC_DEBUG << "SoundManager::run ended";
}

IAudioDecoder * SoundManager::createDecoder(const std::string & theURI) {
    IAudioDecoder * myDecoder = 0;
    for (int i=_myDecoderFactories.size()-1; i>=0; --i) {
        IAudioDecoderFactory* myCurrentFactory = _myDecoderFactories[i];
        try {
            myDecoder = myCurrentFactory->tryCreateDecoder(theURI);
            break;
        } catch (const DecoderException& e) {
            AC_DEBUG << e;
        }
    }
    if (myDecoder == 0) {
        throw DecoderException(std::string("No suitable decoder for ")+theURI+" found.", 
                PLUS_FILE_LINE);
    }
    return myDecoder;
}

} // namespace
