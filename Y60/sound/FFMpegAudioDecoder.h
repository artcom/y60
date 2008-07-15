//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _FFMpegAudioDecoder_H_
#define _FFMpegAudioDecoder_H_

#include "IAudioDecoder.h"
#include "SoundManager.h"

#include <asl/ISampleSink.h>
#include <asl/Block.h>

#ifdef WIN32
#define EMULATE_INTTYPES
#endif

extern "C" {
#ifdef OSX
#include <libavformat/avformat.h>
#else
#include <ffmpeg/avformat.h>
#endif
}

namespace y60 {

class FFMpegAudioDecoder: public IAudioDecoder
{
    public:
        FFMpegAudioDecoder (const std::string& myURI);
        virtual ~FFMpegAudioDecoder();

        virtual unsigned getSampleRate();
        virtual unsigned getNumChannels();
        virtual void seek (asl::Time thePosition);
        virtual asl::Time getDuration() const;
        std::string getName() const;
        virtual void setSampleSink(asl::ISampleSink* mySampleSink);
        virtual unsigned getCurFrame() const; 
        virtual void decodeEverything();

        virtual bool isSyncDecoder() const;
        virtual bool decode();

    private:
        void open();
        void close();

        std::string _myURI;
        AVFormatContext * _myFormatContext;

        int _myStreamIndex;
        static asl::Block _mySamples;
        static asl::Block _myResampledSamples;
        unsigned _mySampleRate;
        unsigned _myNumChannels;
        ReSampleContext * _myResampleContext;
        asl::ISampleSink* _mySampleSink;
        
        unsigned _myCurFrame;
};

class FFMpegAudioDecoderFactory: public IAudioDecoderFactory
{
    public:
        FFMpegAudioDecoderFactory();
        virtual IAudioDecoder* tryCreateDecoder(const std::string& myURI);
        virtual int getPriority() const; 
};

} // namespace

#endif 
