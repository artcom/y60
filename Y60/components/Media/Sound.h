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

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Reenable warning
#endif

#include <ffmpeg/avformat.h>

namespace y60 {

class Sound;

typedef asl::Ptr<Sound> SoundPtr;
typedef asl::WeakPtr<Sound> SoundWeakPtr;

DEFINE_EXCEPTION(SoundException, asl::Exception);

class Sound :     
    public asl::TimeSource
{
    public:
        Sound (std::string myURI, asl::HWSampleSinkPtr mySampleSink, bool theLoop = false);
        Sound (const std::string & myURI, asl::Ptr < asl::ReadableStream > myStream, 
                asl::HWSampleSinkPtr mySink, bool theLoop = false);
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
    
    private:
        void open();
        void close();
        void checkForClose();
        bool isOpen() const;
        bool decode();
        void queueSamples(asl::AudioBufferPtr theBuffer);
        
        mutable asl::ThreadLock _myLock;

        asl::HWSampleSinkPtr _mySampleSink;
//        AudioDecoder * _myAudioDecoder;
        std::string _myURI;
        
        AVFormatContext * _myFormatContext;
        int _myStreamIndex;
        asl::Block _mySamples;
        asl::Block _myResampledSamples;
        unsigned _mySampleRate;
        unsigned _myNumChannels;

        SoundWeakPtr _mySelf;
        SoundPtr _myLockedSelf;
        bool _myDecodingComplete;

        bool _myIsLooping;
        std::list<asl::AudioBufferPtr> _myBufferCache; // For looping sounds.

        asl::Time _myTargetBufferedTime;
        asl::Time _myMaxUpdateTime; // Max. time to prefetch per update.

		ReSampleContext * _myResampleContext;
        
};

} // namespace

#endif 
