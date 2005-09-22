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

#include <asl/HWSampleSink.h>
#include <asl/Stream.h>
#include <asl/ISampleSink.h>

#include "IAudioDecoder.h"

namespace y60 {

class Sound;

typedef asl::Ptr<Sound> SoundPtr;
typedef asl::WeakPtr<Sound> SoundWeakPtr;

DEFINE_EXCEPTION(SoundException, asl::Exception);

class Sound :     
    public asl::TimeSource, public asl::ISampleSink
{
    public:
        Sound (std::string myURI,  IAudioDecoder * myDecoder, bool theLoop = false);
//        Sound (const std::string & myURI, asl::Ptr < asl::ReadableStream > myStream, 
//                bool theLoop = false);
        virtual ~Sound();
        void setSelf(const SoundPtr& mySelf);
        virtual void play ();
        virtual void pause ();
        virtual void stop ();
        virtual void setVolume (float theVolume);
        virtual void fadeToVolume (float Volume, asl::Time theTime);
        virtual double getVolume () const;
        virtual std::string getName() const;
        virtual asl::Time getDuration() const;
        virtual asl::Time getCurrentTime() const;
        virtual bool isLooping() const;
        virtual void seek (asl::Time thePosition);
        virtual void seekRelative (double theAmount);
        virtual asl::Time getBufferedTime() const;
        virtual bool canSeek() const;
        virtual bool isPlaying() const;
        virtual unsigned getNumUnderruns() const;

        // Interface to Media.
        void update(double theTimeSlice);
    
        // ISampleSink interface (to Decoder)
        asl::AudioBufferPtr createBuffer(unsigned theNumFrames);
        void queueSamples(asl::AudioBufferPtr& theBuffer);
        
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
};

} // namespace

#endif 
