//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "FFMpegAudioDecoder.h"

#include <asl/base/Auto.h>
#include <asl/audio/Pump.h>

using namespace std;
using namespace asl;

namespace y60 {

asl::Block FFMpegAudioDecoder::_mySamples(AVCODEC_MAX_AUDIO_FRAME_SIZE);
asl::Block FFMpegAudioDecoder::_myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE);

FFMpegAudioDecoder::FFMpegAudioDecoder (const string& myURI)
    : _myURI (myURI),
      _myFormatContext(0),
      _myStreamIndex(-1),
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
    // threads, _mySamples and _myResampledSamples can't be static variables anymore!
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
        if ((err = av_open_input_file(&_myFormatContext, _myURI.c_str(), 0, 0, 0)) < 0) {
            if (err == -6) {
                throw DecoderException(std::string("Can't decode ")+_myURI, PLUS_FILE_LINE);
            } else {
                throw FileNotFoundException(std::string("Unable to open input file, err=") + 
                    asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
            }
        }
        if ((err = av_find_stream_info(_myFormatContext)) < 0) {
            throw DecoderException(std::string("Unable to find stream info, err=") + 
                    asl::as_string(err) + ": " + _myURI, PLUS_FILE_LINE);
        }
        // find first audio stream
        _myStreamIndex = -1;
        for (int i = 0; i < _myFormatContext->nb_streams; ++i) {
#if (LIBAVCODEC_BUILD >= 0x4910)
            if (_myFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
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
        if (avcodec_open(myCodecContext, myCodec) < 0 ) {
            throw DecoderException(std::string("Unable to open codec: ") + _myURI, 
                    PLUS_FILE_LINE);
        }

        _mySampleRate = myCodecContext->sample_rate;
        _myNumChannels = myCodecContext->channels;
        AC_DEBUG << "Number of channels: " << _myNumChannels << endl;
        AC_DEBUG << "Sample rate: " << _mySampleRate << endl;

        if (_mySampleRate != Pump::get().getNativeSampleRate()) {
            _myResampleContext = audio_resample_init(_myNumChannels, _myNumChannels,    
                    Pump::get().getNativeSampleRate(), _mySampleRate);
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
#if (LIBAVCODEC_BUILD >= 0x4910)
        AVCodecContext * myCodecContext = _myFormatContext->streams[_myStreamIndex]->codec;
#else
        AVCodecContext * myCodecContext = &_myFormatContext->streams[_myStreamIndex]->codec;
#endif
        if (myCodecContext) {
            avcodec_close(myCodecContext);
        }
        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
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
        unsigned char* myData = myPacket.data;
        int myDataLen = myPacket.size;
        
        int myLen = 0;
        while (myDataLen > 0) {
            myLen = avcodec_decode_audio(myCodec, (int16_t*)_mySamples.begin(), 
                    &myBytesDecoded, myData, myDataLen);
            if (myLen > 0 && myBytesDecoded > 0) {
                int numFrames = myBytesDecoded/(getBytesPerSample(SF_S16)*_myNumChannels);
                AC_TRACE << "FFMpegAudioDecoder::decode(): Frames per buffer= " << numFrames;
                AudioBufferPtr myBuffer;
                if (_myResampleContext) {
                    numFrames = audio_resample(_myResampleContext, 
                            (int16_t*)(_myResampledSamples.begin()),
                            (int16_t*)(_mySamples.begin()), 
                            numFrames);
                    myBuffer = Pump::get().createBuffer(numFrames);
                    myBuffer->convert(_myResampledSamples.begin(), SF_S16, _myNumChannels);
                } else {
                    myBuffer = Pump::get().createBuffer(numFrames);
                    myBuffer->convert(_mySamples.begin(), SF_S16, _myNumChannels);
                }
                myBuffer->setStartFrame(_myCurFrame);
                _myCurFrame += myBuffer->getNumFrames();
                _mySampleSink->queueSamples(myBuffer);
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
