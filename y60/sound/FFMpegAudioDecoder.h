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

#ifndef _FFMpegAudioDecoder_H_
#define _FFMpegAudioDecoder_H_

#include "y60_sound_settings.h"

#include "IAudioDecoder.h"
#include "SoundManager.h"

#include <asl/audio/ISampleSink.h>
#include <asl/base/Block.h>

#ifdef _WIN32
#define EMULATE_INTTYPES
#endif

extern "C" {
#ifdef OSX
#   include <libavformat/avformat.h>
#else
#   if defined(_MSC_VER)
#       pragma warning (push, 1)
#   endif //defined(_MSC_VER)
#   include <avformat.h>
#   if defined(_MSC_VER)
#       pragma warning (pop)
#   endif //defined(_MSC_VER)
#endif
}

#ifndef AV_VERSION_INT
#define AV_VERSION_INT(a,b,c) (a<<16 | b<<8 | c)
#endif

// XXX: ffmpeg defines this
#ifdef SampleFormat
#   ifdef _WIN32
#       pragma message("undefing polluting SampleFormat from ffmpeg")
#   else
#       warning undefing polluting SampleFormat from ffmpeg
#   endif
#undef SampleFormat
#endif

namespace y60 {

class Y60_SOUND_DECL FFMpegAudioDecoder: public IAudioDecoder
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

         // avcodec open/close must be protected by locks
        asl::ThreadLock _myAVCodecLock;

        std::string _myURI;
        AVFormatContext * _myFormatContext;

        int _myStreamIndex;
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
        virtual ~FFMpegAudioDecoderFactory() {};
        virtual IAudioDecoder* tryCreateDecoder(const std::string& myURI);
        virtual int getPriority() const;
};

} // namespace

#endif
