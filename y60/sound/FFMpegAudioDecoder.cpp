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

// own header
#include "FFMpegAudioDecoder.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

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

#include <asl/base/Auto.h>
#include <asl/audio/Pump.h>

using namespace std;
using namespace asl;

namespace y60 {

asl::Block FFMpegAudioDecoder::_myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE<< 1);

FFMpegAudioDecoder::FFMpegAudioDecoder (const string& myURI)
    : _myURI (myURI),
      _myFormatContext(0),
      _myStreamIndex(-1),
      _mySampleRate(0),
      _myNumChannels(0),
      _myResampleContext(0),
      _mySampleSink(0),
      _myCurFrame(0)
{
    AC_DEBUG << "FFMpegAudioDecoder::FFMpegAudioDecoder";
    open();
}

FFMpegAudioDecoder::~FFMpegAudioDecoder() {
    AC_DEBUG << "FFMpegAudioDecoder::~FFMpegAudioDecoder (" << _myURI << ")";
    close();
}

bool FFMpegAudioDecoder::isSyncDecoder() const {
    // Note that if this is ever changed - i.e. if several decoders run in different
    // threads, _myResampledSamples can't be static variables anymore!
    return true;
}

std::string FFMpegAudioDecoder::getName() const {
    return _myURI;
}

void FFMpegAudioDecoder::setSampleSink(asl::ISampleSink* mySampleSink) {
    _mySampleSink = mySampleSink;
}

unsigned FFMpegAudioDecoder::getCurFrame() const {
    return _myCurFrame;
}

void FFMpegAudioDecoder::decodeEverything() {
    bool isDone = false;
    while (!isDone) {
        isDone = decode();
    }
}

Time FFMpegAudioDecoder::getDuration() const {
    if (_myFormatContext) {
        AVStream * myStream = _myFormatContext->streams[_myStreamIndex];
#if LIBAVCODEC_BUILD >= 0x5100
        return (double(myStream->duration)*av_q2d(myStream->time_base));
#else
        return (double(myStream->duration)/double(AV_TIME_BASE));
#endif
    } else {
        return 0;
    }
}

void FFMpegAudioDecoder::seek (Time thePosition)
{
    AC_TRACE << "FFMpegAudioDecoder::seek(" << thePosition << ")";
#if (LIBAVCODEC_BUILD < 4738)
    int ret = av_seek_frame(_myFormatContext, -1, (long long)(thePosition*AV_TIME_BASE));
#else
    int ret = av_seek_frame(_myFormatContext, -1, (long long)(thePosition*AV_TIME_BASE),
            AVSEEK_FLAG_BACKWARD);
#endif
    if (ret < 0) {
        AC_WARNING << "Unable to seek to timestamp=" << thePosition;
        return;
    }
}

void FFMpegAudioDecoder::open() {
    AC_DEBUG << "FFMpegAudioDecoder::open (" << _myURI << ")" << _myURI;

    try {
        int err;
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 2, 0)
        err = avformat_open_input(&_myFormatContext, _myURI.c_str(), NULL, NULL);
#else
        err = av_open_input_file(&_myFormatContext, _myURI.c_str(), 0, 0, 0);
#endif
        if (err < 0) {
            if (err == -6) {
                throw DecoderException(std::string("Can't decode ")+_myURI, PLUS_FILE_LINE);
            } else {
                throw FileNotFoundException(std::string("Unable to open input file, errno=") +
                    asl::as_string(err) + "("+strerror(AVERROR(err))+"): " + _myURI, PLUS_FILE_LINE);
            }
        }
        if ((err = av_find_stream_info(_myFormatContext)) < 0) {
            throw DecoderException(std::string("Unable to find stream info, err=") +
                    asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
        }
        // find first audio stream
        _myStreamIndex = -1;
        for (unsigned int i = 0; i < _myFormatContext->nb_streams; ++i) {
#if (LIBAVCODEC_BUILD >= 0x4910)
        #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
            if (_myFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
        #else
            if (_myFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
        #endif
#else
            if (_myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_AUDIO) {
#endif
                _myStreamIndex = i;
                break;
            }
        }
        if (_myStreamIndex < 0) {
            throw DecoderException(std::string("No audio stream found: ") + _myURI,
                    PLUS_FILE_LINE);
        }

        // open codec
#if (LIBAVCODEC_BUILD >= 0x4910)
        AVCodecContext * myCodecContext = _myFormatContext->streams[_myStreamIndex]->codec;
#else
        AVCodecContext * myCodecContext = &_myFormatContext->streams[_myStreamIndex]->codec;
#endif
        AVCodec * myCodec = avcodec_find_decoder(myCodecContext->codec_id);
        if (!myCodec) {
            throw DecoderException(std::string("Unable to find decoder: ") + _myURI,
                    PLUS_FILE_LINE);
        }

        {
            AutoLocker<ThreadLock> myLocker(_myAVCodecLock);
#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53,6,0)
            if (avcodec_open2(myCodecContext, myCodec, NULL) < 0 ) {
#else
            if (avcodec_open(myCodecContext, myCodec) < 0 ) {
#endif
                throw DecoderException(std::string("Unable to open codec: ") + _myURI,
                        PLUS_FILE_LINE);
            }
        }

        _mySampleRate = myCodecContext->sample_rate;
        _myNumChannels = myCodecContext->channels;
        AC_DEBUG << "Number of channels: " << _myNumChannels << endl;
        AC_DEBUG << "Sample rate: " << _mySampleRate << endl;
        AC_DEBUG << "Sample format: " << myCodecContext->sample_fmt << endl;

        if (_mySampleRate != Pump::get().getNativeSampleRate() || 
            myCodecContext->sample_fmt != SAMPLE_FMT_S16) 
        {
#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,15,0)
            _myResampleContext = av_audio_resample_init(
                    _myNumChannels, _myNumChannels,
                    Pump::get().getNativeSampleRate(), _mySampleRate,
                    SAMPLE_FMT_S16, myCodecContext->sample_fmt,
                    16, 10, 0, 0.8);
#else
            _myResampleContext = audio_resample_init(_myNumChannels, _myNumChannels,
                    Pump::get().getNativeSampleRate(), _mySampleRate);
#endif
        }
    } catch (const DecoderException &) {
        close();
        throw;
    }
}

void FFMpegAudioDecoder::close() {
    AC_DEBUG << "FFMpegAudioDecoder::close() (" << _myURI << ")";

    if (_myFormatContext) {
        if (_myResampleContext) {
            audio_resample_close(_myResampleContext);
            _myResampleContext = 0;
        }
        AVCodecContext * myCodecContext = NULL;
        if(_myStreamIndex >= 0) {
#           if (LIBAVCODEC_BUILD >= 0x4910)
                myCodecContext = _myFormatContext->streams[_myStreamIndex]->codec;
#           else
                myCodecContext = &_myFormatContext->streams[_myStreamIndex]->codec;
#           endif
        }
        if (_mySampleRate && _myNumChannels) {
            AutoLocker<ThreadLock> myLocker(_myAVCodecLock);
            avcodec_close(myCodecContext);
        }
        if (_myFormatContext) {
            av_close_input_file(_myFormatContext);
            _myFormatContext = 0;
        }
        _myStreamIndex = -1;
    }
}

bool FFMpegAudioDecoder::decode() {
    ASSURE(_myFormatContext);
    AVPacket myPacket;

#if (LIBAVCODEC_BUILD >= 0x4910)
    AVCodecContext * myCodec = _myFormatContext->streams[_myStreamIndex]->codec;
#else
    AVCodecContext * myCodec = &(_myFormatContext->streams[_myStreamIndex]->codec);
#endif

    int err = av_read_frame(_myFormatContext, &myPacket);
    if (err < 0) {
        return true;
    }
    if (myPacket.stream_index == _myStreamIndex) {
        int myBytesDecoded = 0;

        // we need an aligned buffer
        int16_t * myAlignedBuf;
        myAlignedBuf = (int16_t *)av_malloc( AVCODEC_MAX_AUDIO_FRAME_SIZE<<1 );


#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,27,0)
        AVPacket myTempPacket;
        av_init_packet(&myTempPacket);
        myTempPacket.data = myPacket.data;
        myTempPacket.size = myPacket.size;
        while (myTempPacket.size > 0) {
            myBytesDecoded = AVCODEC_MAX_AUDIO_FRAME_SIZE<<1;
            int myLen = avcodec_decode_audio3(myCodec,
                myAlignedBuf, &myBytesDecoded, &myTempPacket);
            if (myLen < 0) {
                AC_WARNING << "av_decode_audio error";
                myTempPacket.size = 0;
                break;
            }
            myTempPacket.data += myLen;
            myTempPacket.size -= myLen;
            AC_TRACE << "data left " << myTempPacket.size << " read " << myLen;

#else
        const uint8_t* myData = myPacket.data;
        int myDataLen = myPacket.size;
        while (myDataLen > 0) {
#   if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(51,28,0)
            myBytesDecoded = AVCODEC_MAX_AUDIO_FRAME_SIZE<<1;
            int myLen = avcodec_decode_audio2(myCodec,
                myAlignedBuf, &myBytesDecoded, myData, myDataLen);
#   else
            int myLen = avcodec_decode_audio(myCodec,
                myAlignedBuf, &myBytesDecoded, myData, myDataLen);
#   endif
            if (myLen < 0) {
                AC_WARNING << "av_decode_audio error";
                myDataLen = 0;
                break;
            }
            myData += myLen;
            myDataLen -= myLen;
            AC_TRACE << "data left " << myDataLen << " read " << myLen;
#endif

            if ( myBytesDecoded <= 0 ) {
                continue;
            }
#if  LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(51,4,0)
            int numFrames = myBytesDecoded/(av_get_bytes_per_sample(myCodec->sample_fmt)*_myNumChannels);
#else
            int numFrames = myBytesDecoded/(av_get_bits_per_sample_format(myCodec->sample_fmt)/8*_myNumChannels);
#endif
            AC_TRACE << "FFMpegAudioDecoder::decode(): Frames per buffer= " << numFrames;
            AudioBufferPtr myBuffer;
            if (_myResampleContext) {
                numFrames = audio_resample(_myResampleContext,
                        (int16_t*)(_myResampledSamples.begin()),
                        myAlignedBuf, numFrames);
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(_myResampledSamples.begin(), SF_S16, _myNumChannels);
            } else {
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(myAlignedBuf, SF_S16, _myNumChannels);
            }
            myBuffer->setStartFrame(_myCurFrame);
            _myCurFrame += myBuffer->getNumFrames();
            _mySampleSink->queueSamples(myBuffer);
        }
        av_free( myAlignedBuf );
    }
    av_free_packet(&myPacket);
    return false;
}

unsigned FFMpegAudioDecoder::getSampleRate() {
    return _mySampleRate;
}

unsigned FFMpegAudioDecoder::getNumChannels() {
    return _myNumChannels;
}

FFMpegAudioDecoderFactory::FFMpegAudioDecoderFactory() {
}

IAudioDecoder* FFMpegAudioDecoderFactory::tryCreateDecoder(const std::string& myURI)
{
    AC_DEBUG << "FFMpegAudioDecoderFactory::tryCreateDecoder (" << myURI << ")";
    return new FFMpegAudioDecoder(myURI);
}

int FFMpegAudioDecoderFactory::getPriority() const {
    return 1;
}

}
