//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#include "AudioTimeSource.h"

#include <asl/Auto.h>

namespace asl {
    
AudioTimeSource::AudioTimeSource(unsigned myInitialDelay, unsigned mySampleRate) 
    : _mySentFrames(0),
      _mySysTimeAtLastBuffer(0),
      _myInitialDelay(myInitialDelay),
      _myRunning(false),
      _mySampleRate(mySampleRate),
      _myGlobalTimeOffset(0)
{
}

AudioTimeSource::~AudioTimeSource() {
}

void AudioTimeSource::setCurrentTime(asl::Time theTime) {
    AutoLocker<ThreadLock> myLocker(_myTimeLock);
    _myGlobalTimeOffset = theTime;
    _mySentFrames = 0;
    _mySysTimeAtLastBuffer = Time();
}

Time AudioTimeSource::getCurrentTime() {
    if (_myRunning) {
        // If the time source was just constructed, we might need to wait until 
        // some data has been sent to the sound card to get sensible values.
        while (_mySentFrames < _myInitialDelay) {
            msleep(10);
        }
        AutoLocker<ThreadLock> myLocker(_myTimeLock);
        double myTimeOffset = Time()-_mySysTimeAtLastBuffer;
        return double(_mySentFrames)/_mySampleRate+myTimeOffset+_myGlobalTimeOffset;
    } else {
        AutoLocker<ThreadLock> myLocker(_myTimeLock);
        return double(_mySentFrames)/_mySampleRate+_myGlobalTimeOffset;
    }
        
}

void AudioTimeSource::stop() {
    AutoLocker<ThreadLock> myLocker(_myTimeLock);
    _mySentFrames = 0;
    _myGlobalTimeOffset = 0;
    _myRunning = false;
}

void AudioTimeSource::pause() {
    _myRunning = false;
}

void AudioTimeSource::run() {
    if (!_myRunning) {
        _mySysTimeAtLastBuffer = Time();
        _myRunning = true;
    }
}

void AudioTimeSource::addFramesToTime(unsigned numFrames) {
    AutoLocker<ThreadLock> myLocker(_myTimeLock);
    _mySentFrames += numFrames;
    _mySysTimeAtLastBuffer = Time();
}

}
