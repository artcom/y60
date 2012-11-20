/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "Sound.h"

#include "CacheReader.h"

#include <asl/base/Auto.h>
#include <asl/audio/Pump.h>

using namespace std;
using namespace asl;

namespace y60 {

asl::ThreadLock Sound::_mySoundsAllocatedLock;

int Sound::_myNumSoundsAllocated = 0;

Sound::Sound (string myURI, IAudioDecoder * myDecoder, SoundCacheItemPtr myCacheItem,
        bool theLoop)
    : _myURI (myURI),
      _myDecoder(myDecoder),
      _myDecodingComplete(false),
      _myIsLooping(theLoop),
      _myTargetBufferedTime(1.0),
      _myMaxUpdateTime(0.2),
      _myCacheItem(myCacheItem)
{
    AC_DEBUG << "Sound::Sound(" << myURI << ", loop: " << theLoop << ")";
    _myLockedSelf = SoundPtr();
    _mySampleSink = Pump::get().createSampleSink(myURI);
    _myDecoder->setSampleSink(this);
    AutoLocker<ThreadLock> myLocker(_mySoundsAllocatedLock);
    _myNumSoundsAllocated++;
}

Sound::~Sound()
{
    AC_DEBUG << "Sound::~Sound (" << _myURI << ")";
    delete _myDecoder;
    _myDecoder = 0;
    AutoLocker<ThreadLock> myLocker(_mySoundsAllocatedLock);
    _myNumSoundsAllocated--;
}

void Sound::setSelf(const SoundPtr& mySelf)
{
    _mySelf = mySelf;
    _myLockedSelf = SoundPtr();
}

void Sound::play() {
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_TRACE << "Sound::play (" << _myURI << ")";
    _myDecodingComplete = false;
    _myLockedSelf = _mySelf.lock();
    _myDecoder->play();
    update(0.1);
    _mySampleSink->play();
}

void Sound::pause() {
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_TRACE << "Sound::pause (" << _myURI << ")";
    _myDecoder->pause();
    _mySampleSink->pause();
    _myLockedSelf = SoundPtr();
}

void Sound::stop() {
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_TRACE << "Sound::stop (" << _myURI << ")";
    _myDecoder->stop();
    _mySampleSink->stop();
    if (_myCacheItem) {
        _myCacheItem->doneCaching();
        if (!_myCacheItem->isFull()) {
            _myCacheItem = SoundCacheItemPtr();
        }
    }
    close();
    _myLockedSelf = SoundPtr();
}

void Sound::setVolume(float theVolume) {
    _mySampleSink->setVolume(theVolume);
}

void Sound::setVolumes(float theLeftVolume, float theRightVolume) {
    std::vector<float> myVolumes;
    myVolumes.push_back(theLeftVolume);
    myVolumes.push_back(theRightVolume);
    _mySampleSink->setVolumes(myVolumes);
}

void Sound::fadeToVolume (float theVolume, Time theTime) {
    _mySampleSink->fadeToVolume(theVolume, float(theTime));
}

double Sound::getVolume() const {
    return _mySampleSink->getVolume();
}

bool Sound::isLooping() const {
    return _myIsLooping;
}

void Sound::setLooping(bool theLoop) {
    _myIsLooping = theLoop;
}

std::string Sound::getName() const {
    return _myURI;
}

Time Sound::getDuration() const {
    AutoLocker<ThreadLock> myLocker(_myLock);
    return _myDecoder->getDuration();
}

Time Sound::getCurrentTime() {
    return _mySampleSink->getCurrentTime();
}

void Sound::seek (Time thePosition)
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_TRACE << "Sound::seek()";
    bool myIsPlaying = isPlaying();
    std::vector<float> myOldVolumes = _mySampleSink->getVolumes();
    _mySampleSink->stop();
    // Forget the old sample sink. It'll fade out and then destroy itself.
    _mySampleSink = Pump::get().createSampleSink(_myURI);

    // Throw away cache.
    if (_myCacheItem) {
        _myCacheItem->doneCaching();
        _myCacheItem = SoundCacheItemPtr();
    }

    _myDecoder->seek(thePosition);

    _mySampleSink->setCurrentTime(thePosition);
    _mySampleSink->setVolumes(myOldVolumes);
    _myLockedSelf = _mySelf.lock();
    if (myIsPlaying) {
        update(0.1);
        _mySampleSink->play();
    }
}

void Sound::seekRelative (double theAmount)
{
    AutoLocker<ThreadLock> myLocker(_myLock);
    double myDestPosition = double(_mySampleSink->getCurrentTime())+theAmount;
    if (myDestPosition < 0.0) {
        myDestPosition = 0;
    }
    seek (myDestPosition);
}

Time Sound::getBufferedTime () const
{
    return _mySampleSink->getBufferedTime();
}

bool Sound::isPlaying() const {
    return (_mySampleSink->getState() == HWSampleSink::RUNNING);
}

unsigned Sound::getNumUnderruns() const {
    return _mySampleSink->getNumUnderruns();
}

Sound::CacheState Sound::getCacheState() const {
    if (dynamic_cast<CacheReader*>(_myDecoder)) {
        return CACHED;
    } else {
        if (_myCacheItem) {
            return CACHING;
        } else {
            return NOCACHE;
        }
    }
}

void Sound::update(double theTimeSlice) {
    AutoLocker<ThreadLock> myLocker(_myLock);
    double myBuffersFilledRatio = double(_mySampleSink->getBufferedTime())/
            _myTargetBufferedTime;
    double myTimeToBuffer = double(_mySampleSink->getBufferedTime())+
            myBuffersFilledRatio*theTimeSlice+
            (1-myBuffersFilledRatio)*_myMaxUpdateTime;
    _myDecoder->setTime(getCurrentTime());
    AC_TRACE << "Sound::update: " << hex << (void*)this << dec << " playing=" <<
            isPlaying() << " decodingComplete=" << _myDecodingComplete;
    if (_myDecodingComplete && !isPlaying()) {
        AC_TRACE << "Sound::update: Playback complete";
        _myDecodingComplete = false;
        _myLockedSelf = SoundPtr();
    }
    if (!_myDecodingComplete) {
        bool myEOF = false;
        if (_myDecoder->isSyncDecoder()) {
            while (double(_mySampleSink->getBufferedTime()) < myTimeToBuffer && !myEOF) {
                myEOF = _myDecoder->decode();
            }
        } else {
            if (_myDecoder->isEOF()) {
                myEOF = true;
            }
        }
        if (myEOF) {
            if (_myCacheItem && !_myCacheItem->isFull()) {
                _myCacheItem->doneCaching(_myDecoder->getCurFrame());
            }
            if (_mySampleSink->getState() != HWSampleSink::STOPPED) {
                if (_myCacheItem && _myCacheItem->isFull() &&
                        !dynamic_cast<CacheReader *>(_myDecoder))
                {
                    AC_TRACE << " ----> Reading from cache.";
                    delete _myDecoder;
                    _myDecoder = new CacheReader(_myCacheItem);
                    _myDecoder->setSampleSink(this);
                }
                if (_myIsLooping) {
                    AC_TRACE << "Sound::update: Loop";
                    _myDecoder->seek(0);
                    update(0.1);
                } else {
                    AC_TRACE << "Sound::update: DecodingComplete";
                    _myDecodingComplete = true;
                    _mySampleSink->stop(true);
                    close();
                }
            }
        }
    }
}

bool Sound::queueSamples(AudioBufferPtr& theBuffer) {
    // TODO: Code review: Is locking ok here?
    if (_myCacheItem && !_myCacheItem->isFull()) {
        bool myCacheItemIsFull = !_myCacheItem->queueSamples(theBuffer);
        if (myCacheItemIsFull) {
            _myCacheItem = SoundCacheItemPtr();
        }
    }
    _mySampleSink->queueSamples(theBuffer);
    return true;
}

void Sound::close() {
    AutoLocker<ThreadLock> myLocker(_myLock);
    AC_DEBUG << "Sound::close() (" << _myURI << ")";
    _myDecoder->seek(0);
}

int Sound::getNumSoundsAllocated() {
    return _myNumSoundsAllocated;
}

}

