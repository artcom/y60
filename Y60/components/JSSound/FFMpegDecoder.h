//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _FFMpegDecoder_H_
#define _FFMpegDecoder_H_

#include "IAudioDecoder.h"
#include "Media.h"

#include <asl/ISampleSink.h>
#include <asl/Block.h>

#ifdef WIN32
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avformat.h>

namespace y60 {

class FFMpegDecoder: public IAudioDecoder
{
    public:
        FFMpegDecoder (std::string myURI);
//        FFMpegDecoder (asl::Ptr < asl::ReadableStream > myStream, asl::HWSampleSinkPtr mySink);
        virtual ~FFMpegDecoder();

        virtual bool decode(asl::ISampleSink* mySampleSink);
        virtual unsigned getSampleRate();
        virtual unsigned getNumChannels();
        virtual void seek (asl::Time thePosition);
        virtual bool canSeek() const;
        virtual asl::Time getDuration() const;
        std::string getName() const;

    private:
        void open();
        void close();

        std::string _myURI;
        AVFormatContext * _myFormatContext;

        int _myStreamIndex;
        asl::Block _mySamples;
        asl::Block _myResampledSamples;
        unsigned _mySampleRate;
        unsigned _myNumChannels;
        ReSampleContext * _myResampleContext;
};

class FFMpegDecoderFactory: public IAudioDecoderFactory
{
    public:
        FFMpegDecoderFactory();
        virtual IAudioDecoder* tryCreateDecoder(std::string myURI);
};

} // namespace

#endif 
