//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "DecoderContext.h"
#include "FFMpegDecoder.h"
#include "AudioFrame.h"
#include "FrameAnalyser.h"

#include <asl/Logger.h>
#include <asl/os_functions.h>
#include <asl/console_functions.h>

#ifdef WIN32
#pragma warning( disable : 4244 ) // Disable ffmpeg warning
#define EMULATE_INTTYPES
#endif
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#ifdef WIN32
#pragma warning( default : 4244 ) // Renable ffmpeg warning
#endif

#include <float.h>

using namespace std;
using namespace asl;

#define DB(x) // x

namespace y60 {

    DecoderContext::DecoderContext(const std::string & theFilename) : 
        _myVideoStream(0),
        _myAudioStream(0),
        _myVideoStreamIndex(-1),
        _myAudioStreamIndex(-1),
        _myEndOfFileTimestamp(DBL_MAX),
        _myFilename(theFilename),
        _myCurrentAudioPacket(0),
        _myCurPosInAudioPacket(0),
        _myAudioEnabled(true),
        _myDestinationFormat(PIX_FMT_BGR24)

    {
        AC_DEBUG << "DecoderContext::DecoderContext";
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
            if (_myVideoStreamIndex == -1 
                    && _myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_VIDEO)
            {
                _myVideoStreamIndex = i;
                AC_DEBUG << "found video stream, index " << _myVideoStreamIndex;
                _myVideoStream = _myFormatContext->streams[i];
            } else if (_myAudioStreamIndex == -1 
                    && _myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_AUDIO)
            {
                _myAudioStreamIndex = i;
                AC_DEBUG << "found audio stream, index " << _myAudioStreamIndex;
                _myAudioStream = _myFormatContext->streams[i];
            }
        }    

        // open codec
        AVCodec * myCodec = avcodec_find_decoder(_myVideoStream->codec.codec_id);
        if (!myCodec) {
            throw FFMpegDecoderException(std::string("Unable to find decoder: ") + theFilename, PLUS_FILE_LINE);
        }
        if (avcodec_open(&_myVideoStream->codec, myCodec) < 0) {
            throw FFMpegDecoderException(std::string("Unable to open codec: ") + theFilename, PLUS_FILE_LINE);
        }

        // Hack to correct wrong frame rates that seem to be generated
        // by some codecs
        if (_myVideoStream->codec.frame_rate > 1000 && _myVideoStream->codec.frame_rate_base == 1) {
            _myVideoStream->codec.frame_rate_base = 1000;
        }

        _myAdvanceFrame = avcodec_alloc_frame();

        string myFrameAnalyserString;
        if (asl::get_environment_var("Y60_FRAME_ANALYSER", myFrameAnalyserString)) {            
            FrameAnalyser myFrameAnalyser(_myFormatContext, _myVideoStream, _myVideoStreamIndex);
            myFrameAnalyser.run(asl::as<unsigned>(myFrameAnalyserString));
        }
    }

    DecoderContext::~DecoderContext() {
        if (_myAudioStream && _myAudioEnabled) {
            AC_DEBUG << "  Closing audio";
            avcodec_close(&_myAudioStream->codec);
            _myAudioStreamIndex = -1;
            _myAudioStream = 0;
        }
        if (_myVideoStream) {
            AC_DEBUG << "  Closing video";
            avcodec_close(&_myVideoStream->codec);
            _myVideoStreamIndex = -1;
            _myVideoStream = 0;
        }
        av_free(_myAdvanceFrame);
        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
    }

    void
    DecoderContext::setTargetPixelEncoding(PixelEncoding thePixelEncoding) {
        switch (thePixelEncoding) {
            case y60::RGBA:
            case y60::BGRA:
                _myDestinationFormat = PIX_FMT_RGBA32;
                break;
            case y60::ALPHA:
            case y60::GRAY:
                _myDestinationFormat = PIX_FMT_GRAY8;
                break;
            case y60::RGB:
            case y60::BGR:
            default:
                _myDestinationFormat = PIX_FMT_BGR24;
                break;
        }
    }

    unsigned
    DecoderContext::getBytesPerPixel() const {
        switch (_myDestinationFormat) {
            case PIX_FMT_GRAY8:
                return 1;
            case PIX_FMT_BGR24:
                return 3;
            case PIX_FMT_RGBA32:
                return 4;
            default:
                throw FFMpegDecoderException(std::string("Unknown bytes per pixel for destination format: ") + 
                     as_string(_myDestinationFormat), PLUS_FILE_LINE);
        }
    }

    unsigned DecoderContext::getWidth() const {
        return _myVideoStream->codec.width;
    }
    unsigned DecoderContext::getHeight() const {
        return _myVideoStream->codec.height;
    }

    bool
    DecoderContext::decodeVideo(AVFrame * theVideoFrame) {
        DB(cerr << "decodeVideo" << endl;)
        AVPacket * myPacket;
 
        int64_t myStartTime = 0;
        if (_myVideoStream->start_time != AV_NOPTS_VALUE) {
            myStartTime = _myVideoStream->start_time;
        } 

        // Read until complete video frame read or end of file reached
        bool myEndOfFileFlag = false;
        while (!myEndOfFileFlag) {
            myPacket = getPacket(true);
            if (myPacket == 0) {
                myEndOfFileFlag = true;
                DB(cerr << "Video: eof" << endl;)
            } else {
                int myFrameCompleteFlag = 0;
                int myLen = avcodec_decode_video(&_myVideoStream->codec, theVideoFrame, 
                        &myFrameCompleteFlag, myPacket->data, myPacket->size);

                if (myLen < 0) {
                    AC_ERROR << "av_decode_video error";
                } else if (myLen < myPacket->size) {
                    AC_ERROR << "av_decode_video: Could not decode video in one step";
                }                

                if (myFrameCompleteFlag) {
                    // The presenation timestamp is not always set by ffmpeg, so we calculate
                    // it ourself. ffplay uses the same technique.
                    theVideoFrame->pts = myPacket->dts - myStartTime;
                    break;
                }                 
            }
        }
        if (myPacket) {
            av_free_packet(myPacket);
            delete myPacket;
        }
        return myEndOfFileFlag;
    }

    bool 
    DecoderContext::decodeAudio(AudioFrame * theAudioFrame) {
        DB(cerr << "decodeAudio" << endl;)
        int64_t myStartTime = 0;
        if (_myVideoStream->start_time != AV_NOPTS_VALUE) {
            myStartTime = _myVideoStream->start_time;
        } 
        if (!_myCurrentAudioPacket) {
            _myCurrentAudioPacket = getPacket(false);
        }
        if (!_myCurrentAudioPacket) {
            DB(cerr << "Audio: eof" << endl;)
            return true;
        }
        bool myFrameDecoded = false;
        int myFrameSize = -1;
        int64_t myPacketTime;
        while (myFrameSize < 0) {
            uint8_t* myCurReadPtr = _myCurrentAudioPacket->data+_myCurPosInAudioPacket;
            int myDataLeftInPacket = _myCurrentAudioPacket->size-_myCurPosInAudioPacket;
            int myBytesDecoded = avcodec_decode_audio(&_myAudioStream->codec,
                    (int16_t*)(theAudioFrame->getSamples()), &myFrameSize,
                    myCurReadPtr, myDataLeftInPacket);
            myPacketTime = _myCurrentAudioPacket->dts;
            if (myBytesDecoded < 0) {
                AC_WARNING << "av_decode_audio error";
            }
            _myCurPosInAudioPacket += myBytesDecoded;
            if (myBytesDecoded < 0 || _myCurPosInAudioPacket >= _myCurrentAudioPacket->size) {
                av_free_packet(_myCurrentAudioPacket);
                delete _myCurrentAudioPacket;
                _myCurrentAudioPacket = getPacket(false);
                if (!_myCurrentAudioPacket) {
                    DB(cerr << "Audio: eof" << endl;)
                        return true;
                }
                _myCurPosInAudioPacket = 0;
            }
        }
        theAudioFrame->setSizeInBytes(myFrameSize);
        theAudioFrame->setTimestamp((myPacketTime - myStartTime) / (double)AV_TIME_BASE);
        return false;
    }

    void
    DecoderContext::seekToTime(double theTimestamp) {
        clearPacketCache();
        int myFrameIndex = int(theTimestamp * _myVideoStream->r_frame_rate / _myVideoStream->r_frame_rate_base);        
        double myFrameTime = double(_myVideoStream->r_frame_rate_base) / _myVideoStream->r_frame_rate;

        DB(cerr << TTYYELLOW << "seekToTime: " << theTimestamp << " index: " << myFrameIndex << ENDCOLOR << endl;)
        if (theTimestamp > myFrameTime * 1.5) {
            // We only want to decode to the timestamp just before the searched frame
            theTimestamp -= myFrameTime * 1.5;
        } else {
            theTimestamp = 0;
        }
        
        // newer releases of ffmpeg may require a 4th argument to av_seek_frame
        #if LIBAVFORMAT_BUILD <= 4616
            int myResult = av_seek_frame(_myFormatContext, _myVideoStreamIndex, myFrameIndex);
        #else
            int myResult = av_seek_frame(_myFormatContext, _myVideoStreamIndex, myFrameIndex, AVSEEK_FLAG_BACKWARD);
        #endif

        if (myResult < 0) {
            AC_ERROR << "Seek error.";
            return;
        }

        if (theTimestamp == 0) {
            return;
        }

        // We got to a key frame. Forward until we get to the frame we want.
        while (true) {
            long long myLastDTS = advance();
            if (myLastDTS == UINT_MAX) {
                AC_WARNING << "Seek reached end of file";
                return;
            }

            double myTimestamp = (myLastDTS - _myVideoStream->start_time) / (double)AV_TIME_BASE;
            if (myTimestamp >= theTimestamp) {
                if (myTimestamp >= theTimestamp + myFrameTime) {
                    AC_ERROR << "Seek went into the future. Last dts: " << myTimestamp << endl;
                }
                return;
            }
        }
    }

    long long
    DecoderContext::advance() const {
        int64_t   myLastDTS;
        AVPacket  myPacket;        
        int myFoundPictureFlag = 0;

        while (myFoundPictureFlag == 0) {
            if (av_read_frame(_myFormatContext, &myPacket) < 0) {
                break;
            }
            myLastDTS = myPacket.dts;
    
            // Make sure that the packet is from the actual video stream.
            if (myPacket.stream_index == _myVideoStreamIndex) {
                avcodec_decode_video(&_myVideoStream->codec,
                    _myAdvanceFrame, &myFoundPictureFlag,
                    myPacket.data, myPacket.size);
            }
            av_free_packet(&myPacket);
        }

        // From ffmpeg apiexample.c: some codecs, such as MPEG, transmit the
        // I and P frame with a latency of one frame. You must do the
        // following to have a chance to get the last frame of the video.
        if (!myFoundPictureFlag) {
            avcodec_decode_video(&_myVideoStream->codec,
                                 _myAdvanceFrame, &myFoundPictureFlag,
                                 0, 0);
            myLastDTS += AV_TIME_BASE * _myVideoStream->r_frame_rate_base / _myVideoStream->r_frame_rate;
        }
        if (!myFoundPictureFlag) {
            myLastDTS = UINT_MAX;
        }        
        return myLastDTS;
    }

    double
    DecoderContext::getFrameRate() const {
        double myFPS = _myVideoStream->codec.frame_rate / (double) _myVideoStream->codec.frame_rate_base;
        if (myFPS > 1000.0f) {
            myFPS /= 1000.0f;
        }

        return myFPS;
    }

    unsigned 
    DecoderContext::getAudioCannelCount() const {
        if (_myAudioStream) {
            return _myAudioStream->codec.channels;
        } else {
            return 0;
        }
    }

    void 
    DecoderContext::disableAudio() {
        _myAudioEnabled = false;
    }

    AVPacket * 
    DecoderContext::getPacket(bool theGetVideo) {
        int myStreamIndex;
        int myOtherStreamIndex;
        PacketList* myPacketList;
        PacketList* myOtherPacketList;
        if (theGetVideo) {
            myStreamIndex = _myVideoStreamIndex;
            myOtherStreamIndex = _myAudioStreamIndex;
            myPacketList = &_myVideoPackets;
            myOtherPacketList = &_myAudioPackets;
        } else {
            myStreamIndex = _myAudioStreamIndex;
            myOtherStreamIndex = _myVideoStreamIndex;
            myPacketList = &_myAudioPackets;
            myOtherPacketList = &_myVideoPackets;
        }

        if (!myPacketList->empty()) {
            AVPacket * myPacket = myPacketList->front();
            myPacketList->pop_front();
            return myPacket;
        } else {
            AVPacket * myPacket;
            bool myEndOfFileFlag;
            do {
                myPacket = new AVPacket;
                myEndOfFileFlag = (av_read_frame(_myFormatContext, myPacket) < 0);
                if (myEndOfFileFlag) {
                    delete myPacket;
                    myPacket = 0;
                    break;
                }
                // Without av_dup_packet, ffmpeg reuses myPacket->data at first opportunity 
                // and trashes our memory.
                av_dup_packet(myPacket);
                if (_myAudioEnabled && myPacket->stream_index == myOtherStreamIndex) {
                    myOtherPacketList->push_back(myPacket);
                }
            } while (myPacket->stream_index != myStreamIndex);
            return myPacket; 
        }
    }
    
    void 
    DecoderContext::clearPacketCache() {
        list<AVPacket*>::iterator it;
        for (it=_myVideoPackets.begin(); it != _myVideoPackets.end(); ++it) {
            av_free_packet(*it);
            delete *it;
        }
        _myVideoPackets.clear();
        for (it=_myAudioPackets.begin(); it != _myAudioPackets.end(); ++it) {
            av_free_packet(*it);
            delete *it;
        }
        _myAudioPackets.clear();
        if (_myCurrentAudioPacket) {
            av_free_packet(_myCurrentAudioPacket);
            delete _myCurrentAudioPacket;
            _myCurrentAudioPacket = 0;
            _myCurPosInAudioPacket = 0;
        }
    }

    void 
    DecoderContext::convertFrame(AVFrame * theFrame, unsigned char * theTargetBuffer) {
        if (!theFrame) {
            AC_ERROR << "DecoderContext::convertFrame() - Invalid frame";
            return;
        }

        unsigned int myLineSizeBytes = _myVideoStream->codec.width * getBytesPerPixel();
        AVPicture myDestPict;
        myDestPict.data[0] = theTargetBuffer;
        myDestPict.data[1] = theTargetBuffer + 1;
        myDestPict.data[2] = theTargetBuffer + 2;

        myDestPict.linesize[0] = myLineSizeBytes;
        myDestPict.linesize[1] = myLineSizeBytes;
        myDestPict.linesize[2] = myLineSizeBytes;

        img_convert(&myDestPict, _myDestinationFormat, (AVPicture*)theFrame, _myVideoStream->codec.pix_fmt,
                    _myVideoStream->codec.width, _myVideoStream->codec.height);
    }
}

