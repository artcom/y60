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
*/

// own header
#include "FFMpegAudioDecoder.h"

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
extern "C" {
#    include <libavformat/avformat.h>
}
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#ifndef AV_VERSION_INT
#define AV_VERSION_INT(a,b,c) (a<<16 | b<<8 | c)
#endif
#if LIBAVCODEC_VERSION_MAJOR < 54
#define AV_SAMPLE_FMT_S16 SAMPLE_FMT_S16
#endif

#include <asl/base/Auto.h>
#include <asl/audio/Pump.h>

#include <y60/base/FFMpegLockManager.h>

using namespace std;
using namespace asl;

namespace y60 {

asl::Block FFMpegAudioDecoder::_myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE);

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
        return (double(myStream->duration)*av_q2d(myStream->time_base));
    } else {
        return 0;
    }
}

void FFMpegAudioDecoder::seek (Time thePosition)
{
    AC_TRACE << "FFMpegAudioDecoder::seek(" << thePosition << ")";
    int ret = av_seek_frame(_myFormatContext, -1, (long long)(thePosition*AV_TIME_BASE),
            AVSEEK_FLAG_BACKWARD);
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
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 6, 0)
        if ((err = avformat_find_stream_info(_myFormatContext, NULL)) < 0) {
#else
        if ((err = av_find_stream_info(_myFormatContext)) < 0) {
#endif
            throw DecoderException(std::string("Unable to find stream info, err=") +
                    asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
        }
        // find first audio stream
        _myStreamIndex = -1;
        for (unsigned int i = 0; i < _myFormatContext->nb_streams; ++i) {
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
            if (_myFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
#else
            if (_myFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
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
        AVCodecContext * myCodecContext = _myFormatContext->streams[_myStreamIndex]->codec;
        AVCodec * myCodec = avcodec_find_decoder(myCodecContext->codec_id);
        if (!myCodec) {
            throw DecoderException(std::string("Unable to find decoder: ") + _myURI,
                    PLUS_FILE_LINE);
        }

#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53,6,0)
        if (avcodec_open2(myCodecContext, myCodec, NULL) < 0 ) {
#else
        if (avcodec_open(myCodecContext, myCodec) < 0 ) {
#endif
            throw DecoderException(std::string("Unable to open codec: ") + _myURI,
                    PLUS_FILE_LINE);
        }

        _mySampleRate = myCodecContext->sample_rate;
        _myNumChannels = myCodecContext->channels;
        AC_DEBUG << "Number of channels: " << _myNumChannels << endl;
        AC_DEBUG << "Sample rate: " << _mySampleRate << endl;
        AC_DEBUG << "Sample format: " << myCodecContext->sample_fmt << endl;

        if (myCodecContext->channels != Pump::get().getNumOutputChannels() ||
            _mySampleRate != Pump::get().getNativeSampleRate() ||
            myCodecContext->sample_fmt != AV_SAMPLE_FMT_S16)
        {
#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,15,0)
            _myResampleContext = av_audio_resample_init(
                    Pump::get().getNumOutputChannels(), _myNumChannels,
                    Pump::get().getNativeSampleRate(), _mySampleRate,
                    AV_SAMPLE_FMT_S16, av_get_packed_sample_fmt(myCodecContext->sample_fmt),
                    16, 10, 0, 0.8);
#else
            _myResampleContext = audio_resample_init(Pump::get().getNumOutputChannels(), _myNumChannels,
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
            myCodecContext = _myFormatContext->streams[_myStreamIndex]->codec;
        }
        if (_mySampleRate && _myNumChannels) {
            
            avcodec_close(myCodecContext);
        }
        if (_myFormatContext) {
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 17, 0)
            avformat_close_input(&_myFormatContext);
#else
            av_close_input_file(_myFormatContext);
#endif
            _myFormatContext = 0;
        }
        _myStreamIndex = -1;
    }
}

bool FFMpegAudioDecoder::decode() {
    ASSURE(_myFormatContext);
    AVPacket myPacket;

    AVCodecContext * myCodec = _myFormatContext->streams[_myStreamIndex]->codec;

    int err = av_read_frame(_myFormatContext, &myPacket);
    if (err < 0) {
        return true;
    }
    if (myPacket.stream_index == _myStreamIndex) {
        int myBytesDecoded = 0;

        // we need an aligned buffer
        char * myAlignedBuf;
        myAlignedBuf = (char *)av_malloc( AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE );


#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,27,0)
        AVPacket myTempPacket;
        av_init_packet(&myTempPacket);
        myTempPacket.data = myPacket.data;
        myTempPacket.size = myPacket.size;
        while (myTempPacket.size > 0) {
            myBytesDecoded = AVCODEC_MAX_AUDIO_FRAME_SIZE;
            int myLen = avcodec_decode_audio3(myCodec,
                (int16_t*)myAlignedBuf, &myBytesDecoded, &myTempPacket);
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
            myBytesDecoded = AVCODEC_MAX_AUDIO_FRAME_SIZE;
            int myLen = avcodec_decode_audio2(myCodec,
                (int16_t*)myAlignedBuf, &myBytesDecoded, myData, myDataLen);
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
            int myBytesPerSample = av_get_bytes_per_sample(myCodec->sample_fmt);
#else
            int myBytesPerSample = av_get_bits_per_sample_format(myCodec->sample_fmt)>>3;
#endif
            int numFrames = myBytesDecoded/(myBytesPerSample*_myNumChannels);
            AC_TRACE << "FFMpegAudioDecoder::decode(): Frames per buffer= " << numFrames;
            AudioBufferPtr myBuffer;
            bool isPlanar = false;
            bool needsResample = (_myResampleContext != NULL);
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(51, 27, 0)
            isPlanar = av_sample_fmt_is_planar(myCodec->sample_fmt);
            if (isPlanar) {
                char* packedBuffer = (char *)av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);
                planarToInterleaved(packedBuffer, myAlignedBuf, _myNumChannels, myBytesPerSample,
                                    myCodec->frame_size);
                numFrames = audio_resample(_myResampleContext,
                        (int16_t*)(_myResampledSamples.begin()),
                        (int16_t*)packedBuffer, numFrames);
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(_myResampledSamples.begin(), SF_S16, 2);
                av_free(packedBuffer);
                needsResample = false;
            }
#endif
            // queue audio sample
            if (needsResample) {
                numFrames = audio_resample(_myResampleContext,
                        (int16_t*)(_myResampledSamples.begin()),
                        (int16_t*)myAlignedBuf, numFrames);
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(_myResampledSamples.begin(), SF_S16, 2);
            } else if (!isPlanar){
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

void FFMpegAudioDecoder::planarToInterleaved(char* outputBuffer, char* inputBuffer, int numChannels,
        int bytesPerSample, int numSamples)
{
    int i, j;
    char * planes[8] = {};
    for (i=0; i<numChannels; i++) {
        planes[i] = inputBuffer + i*(numSamples*bytesPerSample);
    }
    for (i=0; i<numSamples; i++) {
        for (j=0; j<numChannels; j++) {
            memcpy(outputBuffer, planes[j], bytesPerSample);
            outputBuffer += bytesPerSample;
            planes[j] += bytesPerSample;
        }
    }
}

unsigned FFMpegAudioDecoder::getSampleRate() {
    return _mySampleRate;
}

unsigned FFMpegAudioDecoder::getNumChannels() {
    return _myNumChannels;
}

FFMpegAudioDecoderFactory::FFMpegAudioDecoderFactory() {
    AC_INFO << "Soundmanager: using " << LIBAVCODEC_IDENT << endl;
    av_log_set_level(AV_LOG_ERROR);
    av_register_all();
    FFMpegLockManager::get();
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
