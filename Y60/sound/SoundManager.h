//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_SoundManager_h_
#define _ac_y60_SoundManager_h_

#include "Sound.h"
#include "SoundCacheItem.h"
#include "IAudioDecoderFactory.h"

#include <asl/Singleton.h>
#include <asl/Stream.h>
#include <asl/ThreadLock.h>
#include <asl/PosixThread.h>

#include <string>
#include <vector>

namespace y60 {

    class SoundManager : public asl::Singleton<SoundManager>, private PosixThread {
        friend class asl::SingletonManager;
    public:
        virtual ~SoundManager();

        virtual void setSysConfig(const asl::Time& myLatency, 
                const std::string& myDeviceName = "");
        virtual void setAppConfig(unsigned mySampleRate, unsigned numOutputChannels = 2, 
                bool useDummy = false);
        virtual void setCacheSize(unsigned myTotalSize, unsigned myItemSize);
        virtual unsigned getMaxCacheSize() const;
        virtual unsigned getMaxCacheItemSize() const;

        // These are virtual so they can be called from outside the plugin.
        virtual void registerDecoderFactory(IAudioDecoderFactory* theFactory);
        virtual void unregisterDecoderFactory(IAudioDecoderFactory* theFactory);

        virtual SoundPtr createSound(const std::string & theURI);
        virtual SoundPtr createSound(const std::string & theURI, bool theLoop);
        virtual SoundPtr createSound(const std::string & theURI, bool theLoop,
                bool theUseCache);
        virtual void setVolume(float theVolume);
        virtual void fadeToVolume(float theVolume, float theTime);
        virtual float getVolume() const; 
        virtual unsigned getNumSounds() const;
        virtual void stopAll();
        virtual bool isRunning() const;

        // Caching stuff
        virtual void preloadSound(const std::string& theURI);
        virtual void deleteCacheItem(const std::string& theURI);
        virtual unsigned getCacheMemUsed() const;
        virtual unsigned getNumItemsInCache() const;
        
        virtual void update();
        
    private:
        SoundManager();
        void run();
        IAudioDecoder * createDecoder(const std::string & theURI);

        void addCacheItem(SoundCacheItemPtr theItem);
        SoundCacheItemPtr getCacheItem(const std::string & theURI) const;
        void checkCacheSize();
        
        asl::ThreadLock _myLock;
        std::vector < SoundWeakPtr > _mySounds;

        // Sorted according to decoder priority.
        std::vector < IAudioDecoderFactory* > _myDecoderFactories; 
        IAudioDecoderFactory* _myFFMpegDecoderFactory;

        // Cache stuff.
        unsigned _myMaxCacheSize; // In bytes.
        unsigned _myMaxCacheItemSize;
        typedef map<std::string, SoundCacheItemPtr> CacheMap; // Cached sounds indexed by uri.
        CacheMap _myCache;
    };
}

#endif
