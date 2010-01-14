/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "AudioTimeSource.h"


#include <asl/base/Auto.h>
#include <asl/base/Logger.h>

namespace asl {

AudioTimeSource::AudioTimeSource(unsigned myInitialDelay, unsigned mySampleRate)
    : _mySentFrames(0),
      _mySysTimeAtLastBuffer(0),
      _myInitialDelay(myInitialDelay),
      _mySampleRate(mySampleRate),
      _myGlobalTimeOffset(0),
      _myRunning(false)
{}

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
