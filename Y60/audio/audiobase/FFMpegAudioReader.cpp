//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: FFMpegAudioReader.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.30 $
//       $Date: 2005/04/29 14:08:26 $
//
//  FFMpeg audio reader.
//
//=============================================================================

#include "FFMpegAudioReader.h"
#include "AudioScheduler.h"

#include <asl/string_functions.h>
#include <asl/Auto.h>
#include <asl/Logger.h>

using namespace std;

#define DB(x) // x
#define DB2(x) //x

namespace AudioBase {

static const double FADE_TIME = 0.2;

FFMpegAudioReader::FFMpegAudioReader(const std::string & theUrl, double theVolume, unsigned theLoopCount) : 
    BufferedSource(std::string("FFMpegAudioReader=") + theUrl),
    _myLoopCount(theLoopCount),
    _myFormatContext(0), 
    _myCodecContext(0),
    _myAStreamIndex(-1), 
    _myAStream(0), 
    _mySamples(0), 
    _myLastTimestamp(0),
    _mySeekOffset(0.0), 
    _mySeekFadeEndTime(0.0), 
    _myLastVolume(0.0)
{
    setVolume(theVolume);
    load(theUrl);
}

FFMpegAudioReader::~FFMpegAudioReader() {
    AC_DEBUG << "FFMpegAudioReader::~FFMpegAudioReader";

    // delete sample buffer
    if (_mySamples) {
        delete[] _mySamples;
        _mySamples = 0;
    }

    // close codec & input file
    if (_myCodecContext) {
        avcodec_close(_myCodecContext);
        _myCodecContext = 0;
    }

    if (_myFormatContext) {
        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
    }

    _myAStreamIndex  = -1;
    _myAStream       = 0;
}

void FFMpegAudioReader::load(const std::string & theUrl) {
    AC_DEBUG << "FFMpegAudioReader::load " << theUrl;

    // register all formats and codecs
    static bool avRegistered = false;
    if (!avRegistered) {
        cerr << "FFMpegAudioReader::load " << LIBAVCODEC_IDENT << endl;
        //av_log_set_level(AV_LOG_ERROR);
        av_register_all();
        avRegistered = true;
    }

    int err;
    if ((err = av_open_input_file(&_myFormatContext, theUrl.c_str(), 0, 0, 0)) < 0) {
        throw FFMpegAudioReaderException(std::string("Unable to open input file, err=") + asl::as_string(err) + ": " + theUrl, PLUS_FILE_LINE);
    }
    if ((err = av_find_stream_info(_myFormatContext)) < 0) {
        throw FFMpegAudioReaderException(std::string("Unable to find stream info, err=") + asl::as_string(err) + ": " + theUrl, PLUS_FILE_LINE);
    }

    // find first audio stream
    _myAStreamIndex = -1;
    for (unsigned int i = 0; i < _myFormatContext->nb_streams; ++i) {
#if (LIBAVCODEC_BUILD >= 0x4910)
        if (_myFormatContext->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
#else
        if (_myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_AUDIO) {
#endif
            _myAStreamIndex = i;
            break;
        }
    }
    if (_myAStreamIndex < 0) {
        throw FFMpegAudioReaderException(std::string("No audio stream found: ") + theUrl, PLUS_FILE_LINE);
    }

    // open codec
#if (LIBAVCODEC_BUILD >= 0x4910)
    _myCodecContext = _myFormatContext->streams[_myAStreamIndex]->codec;
#else
    _myCodecContext = &_myFormatContext->streams[_myAStreamIndex]->codec;
#endif
    AVCodec * myCodec = avcodec_find_decoder(_myCodecContext->codec_id);
    if (!myCodec) {
        throw FFMpegAudioReaderException(std::string("Unable to find decoder: ") + theUrl, PLUS_FILE_LINE);
    }
    if (avcodec_open(_myCodecContext, myCodec) < 0 ) {
        throw FFMpegAudioReaderException(std::string("Unable to open codec: ") + theUrl, PLUS_FILE_LINE);
    }
    _myAStream = _myFormatContext->streams[_myAStreamIndex];

    unsigned mySeekTimestamp = 0;
#if (LIBAVCODEC_BUILD < 4738)
    err = av_seek_frame(_myFormatContext, -1, mySeekTimestamp);
#else
    err = av_seek_frame(_myFormatContext, -1, mySeekTimestamp, AVSEEK_FLAG_BACKWARD);
#endif
    if (err < 0) {
        AC_WARNING << "Unable to seek to timestamp=" << mySeekTimestamp << " url=" << theUrl;
    }

    _mySamples = new unsigned char[AVCODEC_MAX_AUDIO_FRAME_SIZE];
    _myLastTimestamp = 0;
    _mySeekOffset = 0.0;
    _myLastVolume = 0.0;

    setup(_myCodecContext->channels, _myCodecContext->sample_rate);
    go();
    setRunning(true);
}

void
FFMpegAudioReader::setSeekOffset(double theSeekOffset) {
    asl::AutoLocker<BufferedSource> myLocker(*this);
    if (_mySeekOffset == 0.0) {
        _myLastVolume = getVolume();
        fadeToVolume(0, FADE_TIME);
        _mySeekFadeEndTime = getCurrentTimestamp() + FADE_TIME;
    }
    _mySeekOffset = theSeekOffset;
}

void
FFMpegAudioReader::pause() {
    asl::AutoLocker<BufferedSource> myLocker(*this);
    if (isRunning()) {
        _myLastVolume = getVolume();
        fadeToVolume(0, FADE_TIME);
    }
    else {
        fadeToVolume(_myLastVolume, FADE_TIME);
    }
    BufferedSource::pause();
}

void
FFMpegAudioReader::process() {
    asl::AutoLocker<BufferedSource> myLocker(*this);
    DB(AC_TRACE << "FFMpegAudioReader::process " << (void*)this);

    // seek
    DB2(AC_TRACE << "seekOff=" << _mySeekOffset << " ts=" << getCurrentTimestamp());
    if (_mySeekOffset != 0.0 && getCurrentTimestamp() >= _mySeekFadeEndTime) {
        int64_t mySeekTimestamp = (int64_t)(_myLastTimestamp + _mySeekOffset * AV_TIME_BASE);
        if (mySeekTimestamp < 0) {
            mySeekTimestamp = 0;
        } else if (mySeekTimestamp >= _myAStream->duration) {
            mySeekTimestamp = 0;
        }
        DB(AC_TRACE << "seek to timestamp=" << mySeekTimestamp);
#if (LIBAVCODEC_BUILD < 4738)
        int ret = av_seek_frame(_myFormatContext, -1, mySeekTimestamp);
#else
        int ret = av_seek_frame(_myFormatContext, -1, mySeekTimestamp, AVSEEK_FLAG_BACKWARD);
#endif
        if (ret < 0) {
            AC_WARNING << "Unable to seek to timestamp=" << mySeekTimestamp;
        }

        // clear buffer after seek
        clear();

        // reset seek timestamp
        DB(AC_TRACE << "fade from " << getVolume() << " to " << _myLastVolume);
        fadeToVolume(_myLastVolume, FADE_TIME);
        _mySeekOffset = 0.0;
    }

    // fill queue
    if (isRunning() && empty()) {
        AVPacket myPacket;
        memset(&myPacket, 0, sizeof(myPacket));

        bool eof = true;
        while (empty() && (av_read_frame(_myFormatContext, &myPacket) >= 0)) {

            eof = false;
            if (myPacket.stream_index == _myAStreamIndex) {

                int mySampleSize = 0; // decompressed sample size in BYTES
                uint8_t * myData = myPacket.data;
                int myDataLen = myPacket.size;
				int myLen = 0;
                while (myDataLen > 0) {
#if (LIBAVCODEC_BUILD >= 0x4910)
                    myLen = avcodec_decode_audio(_myAStream->codec,
                        (int16_t*)_mySamples, &mySampleSize,
                        myData, myDataLen);
#else
                    myLen = avcodec_decode_audio(&(_myAStream->codec),
                        (int16_t*)_mySamples, &mySampleSize,
                        myData, myDataLen);
#endif

                    DB2(AC_TRACE << "myLen=" << myLen << " mySampleSize=" << mySampleSize << " myDataLen=" << myDataLen);
                    if (myLen > 0 && mySampleSize > 0) {
                        _myLastTimestamp = myPacket.dts;
                        addBuffer(myPacket.dts / (double)AV_TIME_BASE, _mySamples, mySampleSize);
                        myData += myLen;
                        myDataLen -= myLen;
                    } else {
                        AC_WARNING << "Unable to avcodec_decode_audio timestamp=" << _myLastTimestamp;
                        break;
                    }
                }
            }
            av_free_packet(&myPacket);
        }

        if (eof) {
            DB(AC_TRACE << "FFMpegAudioReader EOF found " << _myLastTimestamp);
            if (_myLoopCount == 0 || _myLoopCount > 1) {
                if (_myLoopCount > 1) {
                    --_myLoopCount;
                }
                DB(AC_TRACE << "looping " << _myLoopCount);
                unsigned mySeekTimestamp = 0;
#if (LIBAVCODEC_BUILD < 4738)
                int ret = av_seek_frame(_myFormatContext, -1, mySeekTimestamp);
#else
                int ret = av_seek_frame(_myFormatContext, -1, mySeekTimestamp, AVSEEK_FLAG_BACKWARD);
#endif
                if (ret < 0) {
                    AC_WARNING << "Unable to seek to timestamp=" << mySeekTimestamp;
                }
                clear();
            } else if (empty()) {
#if 1
                clear();
                stop();
#else
                setRunning(false);
                AudioScheduler::get().sendStop(this);
#endif
            }
        }
    }

    BufferedSource::process();
}
}
