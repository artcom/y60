//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _SoundCacheItem_H_
#define _SoundCacheItem_H_

#include <asl/Ptr.h>
#include <asl/Time.h>
#include <asl/AudioBufferBase.h>
#include <asl/ISampleSink.h>

#include <string>
#include <map>

namespace y60 {

class SoundCacheItem: public asl::ISampleSink
{
    public:
        SoundCacheItem(const std::string& myURI);
        virtual ~SoundCacheItem();
        std::string getURI() const;
        unsigned getMemUsed() const;
        bool isFull() const;
        void doneCaching(int theTotalFrames = -1);

        asl::AudioBufferPtr getBuffer(int theStartFrame) const;
        int getNumFrames() const;
        unsigned getSampleRate() const;
        unsigned getNumChannels() const;
        asl::Time getLastUsedTime() const;
        void incInUseCount();
        void decInUseCount();
        bool isInUse() const;

        // ISampleSink interface
        bool queueSamples(asl::AudioBufferPtr& theBuffer);
        
    private:
        std::string _myURI;
        // An int has room for 12 hours of samples at 48000 samples/sec.
        int _myFramesDecoded;
        int _myTotalFrames;
        typedef std::map<int, asl::AudioBufferPtr> BufferMap;
        BufferMap _myBuffers;

        asl::Time _myLastUseTime;
        int _myInUseCount;
        
};

typedef asl::Ptr<SoundCacheItem> SoundCacheItemPtr;

} // namespace

#endif 
