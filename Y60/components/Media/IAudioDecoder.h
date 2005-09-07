//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _IAudioDecoder_H_
#define _IAudioDecoder_H_

#include <asl/ISampleSink.h>
#include <asl/Time.h>

namespace y60 {

DEFINE_EXCEPTION(DecoderException, asl::Exception);

class IAudioDecoder
{
    public:
        virtual ~IAudioDecoder() {};

        virtual bool decode() = 0;
        virtual unsigned getSampleRate() = 0;
        virtual unsigned getNumChannels() = 0;
        virtual void seek (asl::Time thePosition) {};
        virtual bool canSeek() const { return false;};
        virtual asl::Time getDuration() const = 0;
        virtual std::string getName() const = 0;
};

} // namespace

#endif 
