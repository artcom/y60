//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Sound_H_
#define _Sound_H_

#include "IAudioDecoder.h"
#include "SoundCacheItem.h"

#include <asl/HWSampleSink.h>
#include <asl/Stream.h>
#include <asl/ISampleSink.h>
#include <asl/ThreadLock.h>

namespace y60 {

class Sound;

typedef asl::Ptr<Sound> SoundPtr;
typedef asl::WeakPtr<Sound> SoundWeakPtr;

DEFINE_EXCEPTION(SoundException, asl::Exception);

class Sound :     
    public asl::TimeSource, public asl::ISampleSink
{
    public:
        Sound (std::string myURI,  IAudioDecoder * myDecoder, SoundCacheItemPtr myCacheItem, 
                bool myLoop = false);
        virtual ~Sound();
        void setSelf(const SoundPtr& mySelf);
        void play ();
        void pause ();
        void stop ();
        void setVolume (float theVolume);
        void fadeToVolume (float Volume, asl::Time theTime);
        double getVolume () const;
        std::string getName() const;
        asl::Time getDuration() const;
        asl::Time getCurrentTime() const;
        bool isLooping() const;
        void seek (asl::Time thePosition);
        void seekRelative (double theAmount);
        asl::Time getBufferedTime() const;
        bool isPlaying() const;
        unsigned getNumUnderruns() const;
        enum CacheState {CACHING, CACHED, NOCACHE};
        CacheState getCacheState() const;

        // Interface to Media.
        void update(double theTimeSlice);
    
        // ISampleSink interface (to Decoder)
        bool queueSamples(asl::AudioBufferPtr& theBuffer);
        
        static int getNumSoundsAllocated();
        
    private:
        void open();
        void close();
        bool decode();
        
        mutable asl::ThreadLock _myLock;

        std::string _myURI;
        asl::HWSampleSinkPtr _mySampleSink;
        IAudioDecoder * _myDecoder;

        SoundWeakPtr _mySelf;
        SoundPtr _myLockedSelf;

        bool _myDecodingComplete;
        bool _myIsLooping;

        asl::Time _myTargetBufferedTime;
        asl::Time _myMaxUpdateTime; // Max. time to prefetch per update.

        SoundCacheItemPtr _myCacheItem; // This is 0 if we're not caching.

        static asl::ThreadLock _mySoundsAllocatedLock;
        static int _myNumSoundsAllocated;
};

} // namespace

#endif 
