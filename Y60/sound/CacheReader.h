//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _CacheReader_H_
#define _CacheReader_H_

#include "IAudioDecoder.h"
#include "SoundCacheItem.h"

#include <asl/audio/ISampleSink.h>

namespace y60 {

class CacheReader: public IAudioDecoder
{
    public:
        CacheReader (SoundCacheItemPtr myCacheItem);
        virtual ~CacheReader();

        virtual bool isSyncDecoder() const;
        virtual bool decode();
        virtual unsigned getSampleRate();
        virtual unsigned getNumChannels();
        virtual void seek (asl::Time thePosition);
        virtual asl::Time getDuration() const;
        std::string getName() const;
        virtual void setSampleSink(asl::ISampleSink* mySampleSink);
        virtual unsigned getCurFrame() const; 
        virtual void decodeEverything();

    private:
        std::string _myURI;

        SoundCacheItemPtr _myCacheItem;
        int _myCurFrame;
        asl::ISampleSink* _mySampleSink;
};

} // namespace

#endif 
