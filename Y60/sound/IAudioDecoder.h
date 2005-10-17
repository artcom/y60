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

class IAudioDecoder
{
    public:
        virtual ~IAudioDecoder() {};
        
        virtual void seek(asl::Time thePosition) = 0;
        virtual unsigned getSampleRate() = 0;
        virtual unsigned getNumChannels() = 0;
        virtual asl::Time getDuration() const = 0;
        virtual std::string getName() const = 0;
        virtual void setSampleSink(asl::ISampleSink* mySampleSink) = 0;
        virtual void setTime(asl::Time myTime) {};
        virtual unsigned getCurFrame() const = 0; // This is the total number of frames that 
                                 // have been decoded. It doesn't take into account seeks etc.
        virtual void decodeEverything() = 0;
       
        // There are two types of decoders. Sync decoders don't have their own
        // decoding thread. Buffering is handled externally - the Decoder client
        // calls decode() until enough data has been buffered.
        // Async decoders have an internal thread that decodes the data and do their
        // own buffering, so they need to know when playback is running.
        virtual bool isSyncDecoder() const = 0;
        
        // Sync decoder methods
        virtual bool decode() { return false; };

        // Async decoder methods
        virtual void play() {};
        virtual void stop() {};
        virtual void pause() {};
        virtual bool isEOF() const { return false; };
};

} // namespace

#endif 
