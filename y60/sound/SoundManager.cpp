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

// own header
#include "SoundManager.h"

#include "FFMpegAudioDecoder.h"
#include "CacheReader.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

extern "C" {
#ifdef OSX
#   include <libavformat/avformat.h>
#else
#   if defined(_MSC_VER)
#       pragma warning (push, 1)
#   endif //defined(_MSC_VER)
#   include <avformat.h>
#   if defined(_MSC_VER)
#       pragma warning (pop)
#   endif //defined(_MSC_VER)
#endif
}


#include <asl/base/Logger.h>
#include <asl/audio/Pump.h>
#include <asl/base/Auto.h>
#include <asl/base/proc_functions.h>
#include <asl/base/os_functions.h>

using namespace std;
using namespace asl;

namespace y60 {

const double myTimePerSlice = 0.05;
const unsigned DEFAULT_Y60_MAX_OPEN_SOUNDS = 200;

SoundManager::SoundManager()
    : _myMaxCacheSize (128*1024*1024), // 128 MB Cache
      _myMaxCacheItemSize (60*2*sizeof(float)*48000)
                // One minute stereo float samples = 22 MB per sound.
{
    AC_DEBUG << "SoundManager::SoundManager";

    // Initialize ffmpeg
    AC_INFO << "Soundmanager: using " << LIBAVCODEC_IDENT << endl;
    av_log_set_level(AV_LOG_ERROR);
    av_register_all();

    _myFFMpegAudioDecoderFactory = new FFMpegAudioDecoderFactory;
    registerDecoderFactory(_myFFMpegAudioDecoderFactory);

    Y60_MAX_OPEN_SOUNDS = asl::getenv("Y60_MAX_OPEN_SOUNDS", DEFAULT_Y60_MAX_OPEN_SOUNDS);

    // XXX: forking here is dangerous because virtuals are not yet
    //      initialized. this can lead to hard-to-catch bugs and
    //      can only be prevented by letting the client call fork(). [IA]
    fork();
}

SoundManager::~SoundManager() {
    AC_DEBUG << "SoundManager::~SoundManager";
    //msleep(50); //XXX: why?!
    if (_mySounds.size() != 0) {
        AC_DEBUG << "Deleting SoundManager, but " << _mySounds.size() <<
                " sounds are still active.";
        stopAll();
    }
    if (isUnjoined()) {
        join();
    }
    unregisterDecoderFactory(_myFFMpegAudioDecoderFactory);
    delete _myFFMpegAudioDecoderFactory;
    _myCache.clear();
    if (_myDecoderFactories.size()) {
        AC_WARNING << _myDecoderFactories.size() << " decoder factories still registered.";
    }
    if (AudioBufferBase::getNumBuffersAllocated() > 1) {
        AC_WARNING << "SoundManager being deleted, but " <<
                AudioBufferBase::getNumBuffersAllocated()-1 <<
                " buffers are still allocated.";
    }
    if (Sound::getNumSoundsAllocated() > 0) {
        AC_WARNING << "SoundManager being deleted, but " <<
                Sound::getNumSoundsAllocated() <<
                " Sounds are still allocated.";
    }
}

void 
SoundManager::stop() {
    if (_mySounds.size() != 0) {
        AC_DEBUG << "stopping SoundManager, but " << _mySounds.size() <<
                " sounds are still active.";
        stopAll();
    }
    if (isUnjoined()) {
        join();
    }
}

void SoundManager::setCacheSize(unsigned myTotalSize, unsigned myItemSize) {
    _myMaxCacheSize = myTotalSize;
    _myMaxCacheItemSize = myItemSize;
    checkCacheSize();
}

unsigned SoundManager::getMaxCacheSize() const {
    return _myMaxCacheSize;
}

unsigned SoundManager::getMaxCacheItemSize() const {
    return _myMaxCacheItemSize;
}

bool lessFactory(const IAudioDecoderFactory* a, const IAudioDecoderFactory* b) {
    return a->getPriority() < b->getPriority();
}

void SoundManager::registerDecoderFactory(IAudioDecoderFactory* theFactory) {
    AC_DEBUG << "Registering decoder factory " << theFactory;
    _myDecoderFactories.insert(
            lower_bound(_myDecoderFactories.begin(), _myDecoderFactories.end(),
                    theFactory, lessFactory),
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
    return createSound(theURI, false, true);
}

SoundPtr SoundManager::createSound(const string & theURI, bool theLoop) {
    // Workaround function since the JS binding doesn't support default parameters.
    return createSound(theURI, theLoop, true);
}

SoundPtr SoundManager::createSound(const string & theURI, bool theLoop,
        bool theUseCache)
{
    // We need a factory function so we can set the Sound's mySelf pointer and
    // do some decoder creation and cache management magic.
    AC_TRACE << "SoundManager::createSound " << theURI << ", loop: " << theLoop
            << ", use cache: " << theUseCache;
    AutoLocker<ThreadLock> myLocker(_myLock);
    if (getNumOpenSounds() >= getMaxOpenSounds()) {
        AC_ERROR << "Maximum number of concurrent sounds ("<<getMaxOpenSounds()<<") reached,"
                 << "fix your application or adjust the Y60_MAX_OPEN_SOUNDS environment variable"
                 <<  "and the limits for maximum number of open files";
        return SoundPtr();
    }
    IAudioDecoder * myDecoder;
    SoundCacheItemPtr myCacheItem = SoundCacheItemPtr();
    if (theUseCache) {
        checkCacheSize();
        myCacheItem = getCacheItem(theURI);
        if (myCacheItem == SoundCacheItemPtr()) {
            AC_TRACE << "    --> Sound not cached yet.";
            // This sound hasn't been played before.
            myCacheItem = SoundCacheItemPtr(new SoundCacheItem(theURI));
            addCacheItem(myCacheItem);
            myDecoder = createDecoder(theURI);
        } else {
            if (myCacheItem->isFull()) {
                // The sound is cached completely.
                AC_TRACE << "    --> Usable sound cache item exists.";
                myDecoder = new CacheReader(myCacheItem);
            } else {
                // The sound is currently being cached. Ignore the cache.
                AC_TRACE << "    --> Sound cache item exists, but isn't complete.";
                myDecoder = createDecoder(theURI);
                myCacheItem = SoundCacheItemPtr();
            }
        }
    } else {
        myDecoder = createDecoder(theURI);
    }
    SoundPtr mySound = SoundPtr(new Sound(theURI, myDecoder, myCacheItem, theLoop));
    mySound->setSelf(mySound);
    _mySounds.push_back(mySound);
    return mySound;
}

void SoundManager::fadeToVolume(float theVolume, float theTime) {
    Pump::get().fadeToVolume(theVolume, theTime);
}


float SoundManager::getVolume() const {
    return Pump::get().getVolume();
}

void SoundManager::getVolumes(std::vector<float> & theVolumes) const {
    Pump::get().getVolumes(theVolumes);
}
void SoundManager::setVolume(float theVolume) {
    Pump::get().setVolume(theVolume);
}

void SoundManager::setVolumes(std::vector<float> theVolumes) {
    Pump::get().setVolumes(theVolumes);
}

unsigned SoundManager::getNumSounds() const {
    return Pump::get().getNumSinks();
}

unsigned int SoundManager::getChannelCount() const {
    return Pump::get().getNumOutputChannels();
}

bool SoundManager::isRunning() const {
    return Pump::get().isRunning();
}

void SoundManager::preloadSound(const std::string& theURI) {
    AC_TRACE << "SoundManager::preloadSound(" << theURI << ")";
    if (_myCache.find(theURI) != _myCache.end()) {
        AC_TRACE << "    --> Already in cache. Ignoring.";
        return;
    }
    checkCacheSize();
    SoundCacheItemPtr myCacheItem = SoundCacheItemPtr(new SoundCacheItem(theURI));
    addCacheItem(myCacheItem);
    IAudioDecoder * myDecoder = createDecoder(theURI);
    myDecoder->setSampleSink(myCacheItem.get());
    myDecoder->decodeEverything();
    myCacheItem->doneCaching(myDecoder->getCurFrame());
    delete myDecoder;
}

void SoundManager::deleteCacheItem(const std::string& theURI) {
    CacheMap::iterator it;
    it = _myCache.find(theURI);
    if (it != _myCache.end()) {
        _myCache.erase(it);
    }
}

unsigned SoundManager::getCacheMemUsed() const {
    CacheMap::const_iterator it;
    unsigned myMemUsed = 0;
    for (it=_myCache.begin(); it != _myCache.end(); ++it) {
        myMemUsed += it->second->getMemUsed();
    }
    return myMemUsed;
}

unsigned SoundManager::getNumItemsInCache() const {
    return _myCache.size();
}

void SoundManager::stopAll() {
    AC_TRACE << "SoundManager::stopAll";
    unsigned myNumSoundsStopped = 0;

    std::vector<SoundPtr> mySounds;

    {
        AutoLocker<ThreadLock> myLocker(_myLock);
        std::vector<SoundWeakPtr>::iterator wi;
        for (wi = _mySounds.begin(); wi != _mySounds.end(); wi++) {
            SoundPtr p = wi->lock();
            if(p) {
                mySounds.push_back(p);
            }
        }
    }

    std::vector<SoundPtr>::iterator si;
    for(si = mySounds.begin(); si != mySounds.end(); si++) {
        AC_TRACE << "stopAll: Stopping " << (*si)->getName();
        (*si)->stop();
        myNumSoundsStopped++;
    }

    if (myNumSoundsStopped) {
        AC_TRACE << "stopAll: " << myNumSoundsStopped << " sounds stopped.";
    }

    update();

    AC_TRACE << "SoundManager::stopAll end";
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
    AC_DEBUG << "createDecoder: " << _myDecoderFactories.size() << " factories registered.";
    for (std::vector<IAudioDecoderFactory*>::size_type i=0; i<_myDecoderFactories.size(); ++i) {
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

void SoundManager::addCacheItem(SoundCacheItemPtr theItem) {
    string myURI = theItem->getURI();
    CacheMap::iterator it;
    it = _myCache.find(myURI);
    if (it != _myCache.end()) {
        AC_WARNING << "Ignoring attempt to add cache item with duplicate key " << myURI;
        return;
    }
    _myCache[myURI] = theItem;
}

SoundCacheItemPtr SoundManager::getCacheItem(const std::string & theURI) const {
    CacheMap::const_iterator it;
    it = _myCache.find(theURI);
    if (it == _myCache.end()) {
        return SoundCacheItemPtr();
    } else {
        return it->second;
    }
}

void SoundManager::checkCacheSize() {
    AC_TRACE << "SoundManager::checkCacheSize: " << getCacheMemUsed();
    if (getCacheMemUsed() > _myMaxCacheSize) {
        AC_TRACE << "SoundManager: removing excess items from cache.";
    }

    // TODO: Check if searching linearly is a performance hit and change if needed.
    // -> Maybe also move the cache to a separate class when that's nessesary.
    int myIteration = 0;
    while (getCacheMemUsed() > _myMaxCacheSize) {
        AC_TRACE << "    " << myIteration << ": " << getCacheMemUsed();
        CacheMap::iterator it;
        CacheMap::iterator myOldestItem = _myCache.begin();
        for (it=_myCache.begin(); it != _myCache.end(); ++it) {
            SoundCacheItemPtr myItem = it->second;
            if (!myItem->isInUse() &&
                myItem->isFull() &&
                myItem->getLastUsedTime() < myOldestItem->second->getLastUsedTime())
            {
                myOldestItem = it;
            }
        }
        if (myOldestItem == _myCache.end()) {
            AC_WARNING << "SoundManager: Can't find item to remove from cache since all are in use.";
            break;
        }
        _myCache.erase(myOldestItem);
        myIteration++;
    }
}

} // namespace
