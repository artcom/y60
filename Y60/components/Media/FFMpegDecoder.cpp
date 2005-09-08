//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "FFMpegDecoder.h"

#include <asl/Auto.h>
#include <asl/Pump.h>

using namespace std;
using namespace asl;

namespace y60 {

FFMpegDecoder::FFMpegDecoder (string myURI)
    : _myURI (myURI),
      _mySamples(AVCODEC_MAX_AUDIO_FRAME_SIZE),
      _myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE),
      _myFormatContext(0),
      _myStreamIndex(-1),
      _myResampleContext(0)
{
    AC_DEBUG << "FFMpegDecoder::FFMpegDecoder";
    open();
}

FFMpegDecoder::~FFMpegDecoder()
{
    AC_DEBUG << "FFMpegDecoder::~FFMpegDecoder (" << _myURI << ")";
    close();
}

std::string FFMpegDecoder::getName() const {
    return _myURI;
}

Time FFMpegDecoder::getDuration() const {
    if (_myFormatContext) {
        return (_myFormatContext->streams[_myStreamIndex]->duration/double(AV_TIME_BASE));
    } else {
        return 0;
    }
}

void FFMpegDecoder::seek (Time thePosition)
{
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

bool FFMpegDecoder::canSeek() const {
    return true;
}

void FFMpegDecoder::open() {
    AC_DEBUG << "FFMpegDecoder::open (" << _myURI << ")" << _myURI;
    
    // register all formats and codecs
    static bool avRegistered = false;
    if (!avRegistered) {
        AC_DEBUG << "FFMpegDecoder::open: " << LIBAVCODEC_IDENT << endl;
        //av_log_set_level(AV_LOG_ERROR);
        av_register_all();
        avRegistered = true;
    }
    
    try {
        int err;
        if ((err = av_open_input_file(&_myFormatContext, _myURI.c_str(), 0, 0, 0)) < 0) {
            throw FileNotFoundException(std::string("Unable to open input file, err=") + 
                    asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
        }
        if ((err = av_find_stream_info(_myFormatContext)) < 0) {
            throw DecoderException(std::string("Unable to find stream info, err=") + 
                    asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
        }
        // find first audio stream
        _myStreamIndex = -1;
        for (unsigned int i = 0; i < _myFormatContext->nb_streams; ++i) {
            if (_myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_AUDIO) {
                _myStreamIndex = i;
                break;
            }
        }
        if (_myStreamIndex < 0) {
            throw DecoderException(std::string("No audio stream found: ") + _myURI, 
                    PLUS_FILE_LINE);
        }

        // open codec
        AVCodecContext * myCodecContext = &_myFormatContext->streams[_myStreamIndex]->codec;
        AVCodec * myCodec = avcodec_find_decoder(myCodecContext->codec_id);
        if (!myCodec) {
            throw DecoderException(std::string("Unable to find decoder: ") + _myURI, PLUS_FILE_LINE);
        }
        if (avcodec_open(myCodecContext, myCodec) < 0 ) {
            throw DecoderException(std::string("Unable to open codec: ") + _myURI, PLUS_FILE_LINE);
        }

        _mySampleRate = myCodecContext->sample_rate;
        _myNumChannels = myCodecContext->channels;
        AC_INFO << "Number of channels: " << _myNumChannels << endl;
        AC_INFO << "Sample rate: " << _mySampleRate << endl;

        if (_mySampleRate != Pump::get().getNativeSampleRate()) {
            _myResampleContext = audio_resample_init(_myNumChannels, _myNumChannels,    
                    Pump::get().getNativeSampleRate(), _mySampleRate);
        }
    } catch (const DecoderException &) {
        close();
        throw;
    }
}

void FFMpegDecoder::close() {
    AC_DEBUG << "FFMpegDecoder::close() (" << _myURI << ")";

    if (_myFormatContext) {
        if (_myResampleContext) {
            audio_resample_close(_myResampleContext);
            _myResampleContext = 0;
        }
        AVCodecContext * myCodecContext = &_myFormatContext->streams[_myStreamIndex]->codec;
        if (myCodecContext) {
            avcodec_close(myCodecContext);
        }
        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
        _myStreamIndex = -1;
    }
}

bool FFMpegDecoder::decode(asl::ISampleSink* mySampleSink) {
    ASSURE(_myFormatContext);
    AVPacket myPacket;

    AVCodecContext * myCodec = &(_myFormatContext->streams[_myStreamIndex]->codec);
    
    int err = av_read_frame(_myFormatContext, &myPacket);
    if (err < 0) {
        return true;
    }
    if (myPacket.stream_index == _myStreamIndex) {
        int myBytesDecoded = 0;
        unsigned char* myData = myPacket.data;
        int myDataLen = myPacket.size;
        int myLen = 0;
        while (myDataLen > 0) {
            myLen = avcodec_decode_audio(myCodec, (int16_t*)_mySamples.begin(), 
                    &myBytesDecoded, myData, myDataLen);
            if (myLen > 0 && myBytesDecoded > 0) {
                int numFrames = myBytesDecoded/(getBytesPerSample(SF_S16)*_myNumChannels);
                AC_TRACE << "FFMpegDecoder::decode(): Frames per buffer= " << numFrames;
                AudioBufferPtr myBuffer;
                if (_myResampleContext) {
                    numFrames = audio_resample(_myResampleContext, 
                            (int16_t*)(_myResampledSamples.begin()),
                            (int16_t*)(_mySamples.begin()), 
                            numFrames);
                    myBuffer = mySampleSink->createBuffer(numFrames);
                    myBuffer->convert(_myResampledSamples.begin(), SF_S16, _myNumChannels);
                } else {
                    myBuffer = mySampleSink->createBuffer(numFrames);
                    myBuffer->convert(_mySamples.begin(), SF_S16, _myNumChannels);
                }
                mySampleSink->queueSamples(myBuffer);
                myData += myLen;
                myDataLen -= myLen;
            } else {
                if (myLen <= 0)  {
                    AC_WARNING << "Unable to avcodec_decode_audio: myLen=" << myLen 
                            << ", myBytesDecoded=" << myBytesDecoded;
                } else {
                    AC_DEBUG << "Unable to avcodec_decode_audio: myLen=" << myLen 
                            << ", myBytesDecoded=" << myBytesDecoded;
                }
                break;
            }
        }
    }
    av_free_packet(&myPacket);
    return false;
}

unsigned FFMpegDecoder::getSampleRate() {
    return _mySampleRate;
}

unsigned FFMpegDecoder::getNumChannels() {
    return _myNumChannels;
}

FFMpegDecoderFactory::FFMpegDecoderFactory() {
}

IAudioDecoder* FFMpegDecoderFactory::tryCreateDecoder(std::string myURI) 
{
    return new FFMpegDecoder(myURI);
}

}

