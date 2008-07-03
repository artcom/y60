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
#include "FFMpegAudioDecoder.h"
#include "CacheReader.h"

#include <asl/Logger.h>
#include <asl/Pump.h>
#include <asl/Auto.h>
#include <asl/proc_functions.h>

using namespace std;
using namespace asl;

namespace y60 {

const double myTimePerSlice = 0.05;
    
SoundManager::SoundManager()
    : _myMaxCacheSize (128*1024*1024), // 128 MB Cache
      _myMaxCacheItemSize (60*2*sizeof(float)*48000) 
                // One minute stereo float samples = 22 MB per sound.
{
    AC_DEBUG << "SoundManager::SoundManager";
    
    // Initialize ffmpeg 
    AC_DEBUG << "Soundmanager: using " << LIBAVCODEC_IDENT << endl;
    //av_log_set_level(AV_LOG_ERROR);
    av_register_all();
    
    _myFFMpegAudioDecoderFactory = new FFMpegAudioDecoderFactory;
    registerDecoderFactory(_myFFMpegAudioDecoderFactory);
    fork();
}

SoundManager::~SoundManager() {
    AC_DEBUG << "SoundManager::~SoundManager";
    msleep(50);
    if (_mySounds.size() != 0) {
        AC_DEBUG << "Deleting SoundManager, but " << _mySounds.size() << 
                " sounds are still active.";
        stopAll();
    }
    join();
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
    IAudioDecoder * myDecoder;
    SoundCacheItemPtr myCacheItem = SoundCacheItemPtr(0);
    if (theUseCache) {
        checkCacheSize();
        myCacheItem = getCacheItem(theURI);
        if (myCacheItem == SoundCacheItemPtr(0)) {
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
                myCacheItem = SoundCacheItemPtr(0);
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
    myDecoder->setSampleSink(&(*myCacheItem));
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
    {
        AC_TRACE << "SoundManager::stopAll";
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
            AC_TRACE << "stopAll: " << myNumSoundsStopped << " sounds stopped.";
        }
    }
    msleep(100);

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
    for (int i=0; i<_myDecoderFactories.size(); ++i) {
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
        return SoundCacheItemPtr(0);
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
