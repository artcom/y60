//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: FFMpegDecoder.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#include "DecoderContext.h"
#include "FFMpegDecoder.h"
#include "AudioFrame.h"
#include "FrameAnalyser.h"

#include <asl/Logger.h>
#include <asl/os_functions.h>

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Renable ffmpeg warning
#endif

using namespace std;
using namespace asl;

namespace y60 {

    DecoderContext::DecoderContext() : 
        _myFormatContext(0),
        _myVideoStream(0),
        _myAudioStream(0),
        _myVideoStreamIndex(-1),
        _myAudioStreamIndex(-1)
    {}

    DecoderContext::~DecoderContext() {
        if (_myVideoStream) {
            avcodec_close(&_myVideoStream->codec);
            _myVideoStreamIndex = -1;
            _myVideoStream = 0;
        }
        if (_myAudioStream) {
            avcodec_close(&_myAudioStream->codec);
            _myAudioStreamIndex = -1;
            _myAudioStream = 0;
        }

        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
    }

    bool
    DecoderContext::hasVideo() const {
        return (_myVideoStream ? true : false);
    }

    bool
    DecoderContext::hasAudio() const {
        return (_myAudioStream ? true : false);
    }

    PixelFormat 
    DecoderContext::getPixelFormat() {
        if (_myVideoStream) {
            return _myVideoStream->codec.pix_fmt;
        } else {
            return PIX_FMT_NB;
        }
    }

    void
    DecoderContext::load(const std::string & theFilename) {
        // register all formats and codecs
        static bool avRegistered = false;
        if (!avRegistered) {
            AC_INFO << "FFMpegDecoder avcodec version " << LIBAVCODEC_IDENT;
            av_log_set_level(AV_LOG_ERROR);
            av_register_all();
            avRegistered = true;
        }

        // open file
        if (av_open_input_file(&_myFormatContext, theFilename.c_str(), 0, FFM_PACKET_SIZE, 0) < 0) {
            throw FFMpegDecoderException(std::string("Unable to open input file: ") + theFilename, PLUS_FILE_LINE);
        }
        if (av_find_stream_info(_myFormatContext) < 0) {
            throw FFMpegDecoderException(std::string("Unable to find stream info: ") + theFilename, PLUS_FILE_LINE);
        }

        // find video/audio streams
        for (unsigned i = 0; i < _myFormatContext->nb_streams; ++i) {
            if (_myVideoStreamIndex == -1 && _myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_VIDEO) {
                _myVideoStreamIndex = i;
                _myVideoStream = _myFormatContext->streams[i];
            } else if (_myAudioStreamIndex == -1 && _myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_AUDIO) {
                _myAudioStreamIndex = i;
                _myAudioStream = _myFormatContext->streams[i];
            }
        }
    }

    void
    DecoderContext::runFrameAnalyser() {
        string myFrameAnalyserString;
        if (asl::get_environment_var("Y60_FRAME_ANALYSER", myFrameAnalyserString)) {            
            FrameAnalyser myFrameAnalyser(_myFormatContext, _myVideoStream, _myVideoStreamIndex);
            myFrameAnalyser.run(asl::as<unsigned>(myFrameAnalyserString));
        }
    }

    DecoderContext::FrameType
    DecoderContext::decode(AVFrame * theVideoFrame, AudioFrame * theAudioFrame) {
        DecoderContext::FrameType myFrameType = FrameTypeEOF;
        AVPacket myPacket;
        memset(&myPacket, 0, sizeof(myPacket));
        
        // Read until complete frame read or end of file reached
        while (bool myValidFilePosition = (av_read_frame(_myFormatContext, &myPacket) >= 0)) {
            if (myPacket.stream_index == _myVideoStreamIndex) {
                int myFrameCompleteFlag = 0;
                int myLen = avcodec_decode_video(&_myVideoStream->codec, theVideoFrame, &myFrameCompleteFlag,
                                                    myPacket.data, myPacket.size);

                if (myLen < 0) {
                    AC_ERROR << "av_decode_video error";
                } else if (myLen < myPacket.size) {
                    AC_ERROR << "av_decode_video: Could not decode video in one step";
                }

                if (myFrameCompleteFlag) { 
                    myFrameType = FrameTypeVideo;
                    break;
                } 
            } else if (myPacket.stream_index == _myAudioStreamIndex && theAudioFrame) {
                int myLen = avcodec_decode_audio(&_myAudioStream->codec, (int16_t*)theAudioFrame->getSamples(), (int *)&theAudioFrame->_mySampleSize,
                    myPacket.data, myPacket.size);
                theAudioFrame->_myTimestamp = myPacket.dts / (double)AV_TIME_BASE;

                if (myLen < 0) {
                    AC_ERROR << "avcodec_decode_audio error";
                } else if (myLen < myPacket.size) {
                    AC_ERROR << "avcodec_decode_audio: Could not decode video in one step";
                }

                myFrameType = FrameTypeAudio;
                break;
            } 
        }
        
        av_free_packet(&myPacket);                  
        return myFrameType;
    }
}