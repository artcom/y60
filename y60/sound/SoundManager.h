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

#ifndef _ac_y60_SoundManager_h_
#define _ac_y60_SoundManager_h_

#include "y60_sound_settings.h"

#include "Sound.h"
#include "SoundCacheItem.h"
#include "IAudioDecoderFactory.h"

#include <asl/base/Singleton.h>
#include <asl/base/Stream.h>
#include <asl/base/ThreadLock.h>
#include <asl/base/PosixThread.h>

#include <string>
#include <vector>

namespace y60 {

    class Y60_SOUND_DECL SoundManager :
        public asl::Singleton<SoundManager>, private asl::PosixThread
    {
        friend class asl::SingletonManager;
    public:
        virtual ~SoundManager();

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
        virtual void fadeToVolume(float theVolume, float theTime);
        virtual float getVolume() const;
        virtual void getVolumes(std::vector<float> & theVolumes) const;
        virtual void setVolume(float theVolume);
        virtual void setVolumes(std::vector<float> theVolumes);
        virtual unsigned int getChannelCount() const;
        virtual unsigned getNumSounds() const;
        virtual unsigned getNumOpenSounds() const {
            return _mySounds.size();
        }
        virtual unsigned getMaxOpenSounds() const {
            return Y60_MAX_OPEN_SOUNDS;
        }
        virtual void setMaxOpenSounds(unsigned theLimit) {
            Y60_MAX_OPEN_SOUNDS = theLimit;
        }
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
        IAudioDecoderFactory* _myFFMpegAudioDecoderFactory;

        // Cache stuff.
        unsigned _myMaxCacheSize; // In bytes.
        unsigned _myMaxCacheItemSize;
        typedef std::map<std::string, SoundCacheItemPtr> CacheMap; // Cached sounds indexed by uri.
        CacheMap _myCache;
        unsigned Y60_MAX_OPEN_SOUNDS;
    };
}

#endif
