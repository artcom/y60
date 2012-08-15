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

#include "FFMpegDecoder1.h"

#include <y60/video/Movie.h>
#include <asl/base/Logger.h>
#include <asl/base/os_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>

// remove ffmpeg macros
#ifdef START_TIMER
#   undef START_TIMER
#endif
#ifdef STOP_TIMER
#   undef STOP_TIMER
#endif

#include <asl/base/Dashboard.h>
#include <iostream>

#define DB(x) // x

using namespace std;
using namespace asl;

extern "C"
EXPORT asl::PlugInBase * FFMpegDecoder1_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::FFMpegDecoder1(myDLHandle);
}

namespace y60 {

    FFMpegDecoder1::VideoFrame::VideoFrame(unsigned theBufferSize) {
        _myBuffer = new unsigned char[theBufferSize];
        DB(AC_DEBUG << "VideoFrame buffer=" << (void*)_myBuffer);
    }

    FFMpegDecoder1::VideoFrame::~VideoFrame() {
        DB(AC_DEBUG << "~VideoFrame buffer=" << (void*)_myBuffer);
        delete[] _myBuffer;
        _myBuffer = 0;
    }

    // FFMpegDecoder1
    FFMpegDecoder1::FFMpegDecoder1(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle),
        _myFormatContext(0), _myFrame(0),
        _myVStream(0),
        _myLastVStreamIndex(-1),
        _myStartTimestamp(0),
        _myLastVideoTimestamp(0),
        _myDestinationPixelFormat(PIX_FMT_BGR24),
        _myBytesPerPixel(0),
        _hasShutDown(false),
        _myFirstFrameDecodedFlag(false)
    {
        DB(AC_DEBUG << "instantiated an FFMpegDecoder1...");
    }

    FFMpegDecoder1::~FFMpegDecoder1() {
        shutdown();
        DB(AC_DEBUG << "destroyed an FFMpegDecoder1...");
    }

    void FFMpegDecoder1::shutdown() {
        if (!_hasShutDown) {
            closeMovie();
        }
        _hasShutDown = true;
   }

    asl::Ptr<MovieDecoderBase> FFMpegDecoder1::instance() const {
        return asl::Ptr<MovieDecoderBase>(new FFMpegDecoder1(getDLHandle()));
    }

    std::string
    FFMpegDecoder1::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
        // assume that ffmpeg can decode everything, except y60 formats
        if (asl::toLowerCase(asl::getExtension(theUrl)) != "m60" &&
            asl::toLowerCase(asl::getExtension(theUrl)) != "x60" &&
            asl::toLowerCase(asl::getExtension(theUrl)) != "d60" &&
            asl::toLowerCase(asl::getExtension(theUrl)) != "i60" )
        {
            AC_INFO << "FFMpegDecoder1 can decode :" << theUrl << endl;
            return MIME_TYPE_MPG;
        } else {
            AC_INFO << "FFMpegDecoder1 can not decode :" << theUrl << endl;
            return "";
        }
    }

    void
    FFMpegDecoder1::closeMovie() {
        AC_DEBUG << "FFMpegDecoder1::closeMovie";
        if (_myVStream) {
            avcodec_close(_myVStream->codec);
            _myLastVStreamIndex = -1;
            _myVStream = 0;
        }

        if (_myFrame) {
            av_free(_myFrame);
            _myFrame = 0;
        }
        if (_myFormatContext) {
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 17, 0)
            avformat_close_input(&_myFormatContext);
#else
            av_close_input_file(_myFormatContext);
#endif
        }
        _myFormatContext = 0;
    }

    void
    FFMpegDecoder1::setMovieParameters(int myIndex) {
        AVStream * myVStream = _myFormatContext->streams[myIndex];

        if (myVStream == 0) {
            return;
        }

        Movie * myMovie = getMovie();

        int myWidth, myHeight;
        myWidth = myVStream->codec->width;
        myHeight = myVStream->codec->height;
        myMovie->set<ImageWidthTag>(myWidth);
        myMovie->set<ImageHeightTag>(myHeight);
    }

    void
    FFMpegDecoder1::load(const std::string & theFilename) {
        asl::Time myLoadStartTime;
        AC_INFO << "FFMpegDecoder1::load(" << theFilename << ")";
        // register all formats and codecs
        static bool avRegistered = false;
        if (!avRegistered) {
            AC_INFO << "FFMpegDecoder1::load";
            AC_INFO << "\tlibavcodec:\t" << LIBAVCODEC_IDENT;
            AC_INFO << "\tlibavformat:\t" << LIBAVFORMAT_IDENT;
            av_log_set_level(AV_LOG_ERROR);
            av_register_all();
            avRegistered = true;
        }

        // open file
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 2, 0)
        if (avformat_open_input(&_myFormatContext, theFilename.c_str(), NULL, NULL) < 0) {
#else
        if (av_open_input_file(&_myFormatContext, theFilename.c_str(), 0, 0, 0) < 0) {
#endif
            throw FFMpegDecoderException(std::string("Unable to open input file: ") + theFilename, PLUS_FILE_LINE);
        }

        av_read_play(_myFormatContext);
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 6, 0)
        if (avformat_find_stream_info(_myFormatContext, NULL) < 0) {
#else
        if (av_find_stream_info(_myFormatContext) < 0) {
#endif
            throw FFMpegDecoderException(std::string("Unable to find stream info: ") + theFilename, PLUS_FILE_LINE);
        }

        // find video/audio streams
        for (unsigned i = 0; i < static_cast<unsigned>(_myFormatContext->nb_streams); ++i) {
            int myCodecType;
            myCodecType = _myFormatContext->streams[i]->codec->codec_type;
        #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
            if (myCodecType == AVMEDIA_TYPE_VIDEO) {
        #else
            if (myCodecType == CODEC_TYPE_VIDEO) {
        #endif
                _myVStream = _myFormatContext->streams[i];
                break;
            }
        }

        if (_myVStream == 0) {
            throw FFMpegDecoderException(std::string("No video stream found: ") + theFilename, PLUS_FILE_LINE);
        }

        // open codec
        AVCodec * myCodec = avcodec_find_decoder(_myVStream->codec->codec_id);
        if (!myCodec) {
            throw FFMpegDecoderException(std::string("Unable to find decoder: ") + theFilename, PLUS_FILE_LINE);
        }

#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53,8,0)
        std::string thread_count = "auto";
        asl::get_environment_var("Y60_FFMPEG_DECODER_THREADS", thread_count);
        AVDictionary *opts = NULL;
        if (!av_dict_get(opts, "threads", NULL, 0)) {
            av_dict_set(&opts, "threads", thread_count.c_str(), 0);
        }
        if (avcodec_open2(_myVStream->codec, myCodec, &opts) < 0 ) {
#else
        AC_INFO << "multithreaded decoding is not available - update your ffmpeg libs to libavcodec >= " << AV_STRINGIFY(AV_VERSION(53,8,0));
        if (avcodec_open(_myVStream->codec, myCodec) < 0 ) {
#endif
            throw FFMpegDecoderException(std::string("Unable to open codec: ") + theFilename, PLUS_FILE_LINE);
        }
#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53,8,0)
        av_dict_free(&opts);
#endif
        // allocate frame for YUV data
        _myFrame = avcodec_alloc_frame();

        AVCodecContext * myVCodec = _myVStream->codec;
        int myWidth = myVCodec->width;
        int myHeight = myVCodec->height;

AC_PRINT<<"1";
        // pixelformat stuff
        Movie * myMovie = getMovie();
        PixelEncoding myRasterEncoding = PixelEncoding(getEnumFromString(myMovie->get<RasterPixelFormatTag>(), PixelEncodingString));

        // TargetPixelFormatTag is the format the incoming movieframe will be converted in
        if (myMovie->get<TargetPixelFormatTag>() != "") {
            TextureInternalFormat myTargetPixelFormat = TextureInternalFormat(getEnumFromString(myMovie->get<TargetPixelFormatTag>(), TextureInternalFormatStrings));
            switch(myTargetPixelFormat) {
                case TEXTURE_IFMT_YUVA420:
                    myRasterEncoding = YUVA420;
                    break;
                case TEXTURE_IFMT_YUV420:
                    myRasterEncoding = YUV420;
                    break;
                case TEXTURE_IFMT_YUV422:
                    myRasterEncoding = YUV422;
                    break;
                case TEXTURE_IFMT_YUV444:
                    myRasterEncoding = YUV444;
                    break;
                case TEXTURE_IFMT_RGBA8:
                    myRasterEncoding = RGBA;
                    break;
                case TEXTURE_IFMT_ALPHA:
                    myRasterEncoding = ALPHA;
                    break;
                case TEXTURE_IFMT_LUMINANCE:
                case TEXTURE_IFMT_LUMINANCE8:
                case TEXTURE_IFMT_LUMINANCE16:
                case TEXTURE_IFMT_INTENSITY:
                    myRasterEncoding = GRAY;
                    break;
                case TEXTURE_IFMT_RGB:
                    myRasterEncoding = RGB;
                    break;
                default:
                    AC_FATAL << "Unsupported pixel format " << myMovie->get<TargetPixelFormatTag>() << " in FFMpegDecoder1";
                    break;
            }
        }

AC_PRINT<<"2";
        switch (myRasterEncoding) {
            case RGBA:
                {AC_TRACE << "Using TEXTURE_IFMT_RGBA8 pixels";}
                _myDestinationPixelFormat = PIX_FMT_BGRA;
                _myBytesPerPixel = 4;
                myMovie->createRaster(myWidth, myHeight, 1, y60::BGRA);
                break;
            case ALPHA:
                {AC_TRACE << "Using GRAY pixels";}
                _myDestinationPixelFormat = PIX_FMT_GRAY8;
                _myBytesPerPixel = 1;
                myMovie->createRaster(myWidth, myHeight, 1, y60::ALPHA);
                break;
            case GRAY:
                {AC_TRACE << "Using GRAY pixels";}
                _myDestinationPixelFormat = PIX_FMT_GRAY8;
                _myBytesPerPixel = 1;
                myMovie->createRaster(myWidth, myHeight, 1, y60::GRAY);
                break;
            case YUV420:
                {AC_DEBUG << "Using YUV420 pixels";}
                _myDestinationPixelFormat = PIX_FMT_YUV420P;
                if (myVCodec->pix_fmt != PIX_FMT_YUV420P) {
                    AC_WARNING<<"you're trying to use YUV2RGB shader but the source video pixel format is not YUV420p, src: " + theFilename;
                }
                myMovie->createRaster(myWidth, getFrameHeight(), 1, y60::GRAY);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth/2, myHeight/2), y60::GRAY, 1);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth/2, myHeight/2), y60::GRAY, 1);
                break;
            case YUVA420:
                {AC_DEBUG << "Using YUVA420 pixels";}
                _myDestinationPixelFormat = PIX_FMT_YUVA420P;
                if (myVCodec->pix_fmt != PIX_FMT_YUVA420P) {
                    AC_WARNING<<"you're trying to use YUV2RGB shader but the source video pixel format is not YUVA420p, src: " + theFilename;
                }
                myMovie->createRaster(myWidth, myHeight, 1, y60::GRAY);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth/2, myHeight/2), y60::GRAY, 1);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth/2, myHeight/2), y60::GRAY, 1);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth, myHeight), y60::GRAY, 1);
                break;
            case YUV422:
                {AC_DEBUG << "Using YUV422 pixels";}
                _myDestinationPixelFormat = PIX_FMT_YUV422P;
                if (myVCodec->pix_fmt != PIX_FMT_YUV422P) {
                    AC_WARNING<<"you're trying to use YUV2RGB shader but the source video pixel format is not YUV422p, src: " + theFilename;
                }
                myMovie->createRaster(myWidth, myHeight, 1, y60::GRAY);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth, myHeight/2), y60::GRAY, 1);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth, myHeight/2), y60::GRAY, 1);
                break;
            case YUV444:
                {AC_DEBUG << "Using YUV444 pixels";}
                _myDestinationPixelFormat = PIX_FMT_YUV444P;
                if (myVCodec->pix_fmt != PIX_FMT_YUV444P) {
                    AC_WARNING<<"you're trying to use YUV2RGB shader but the source video pixel format is not YUV444p, src: " + theFilename;
                }
                myMovie->createRaster(myWidth, myHeight, 1, y60::GRAY);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth, myHeight), y60::GRAY, 1);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, myWidth, myHeight), y60::GRAY, 1);
                break;
            case RGB:
            default:
                {AC_TRACE << "Using BGR pixels";}
                _myDestinationPixelFormat = PIX_FMT_BGR24;
                _myBytesPerPixel = 3;
                myMovie->createRaster(myWidth, myHeight, 1, y60::BGR);
                break;
        }
        unsigned myRasterCount = myMovie->getNode().childNodesLength();
        if (_myDestinationPixelFormat == PIX_FMT_YUV420P
            || _myDestinationPixelFormat == PIX_FMT_YUV422P
            || _myDestinationPixelFormat == PIX_FMT_YUV444P)
        {
            memset(myMovie->getRasterPtr(0)->pixels().begin(), 16, myMovie->getRasterPtr(0)->pixels().size());
            memset(myMovie->getRasterPtr(1)->pixels().begin(), 127, myMovie->getRasterPtr(1)->pixels().size());
            memset(myMovie->getRasterPtr(2)->pixels().begin(), 127, myMovie->getRasterPtr(2)->pixels().size());
        } else if (_myDestinationPixelFormat == PIX_FMT_YUVA420P ) {
            memset(myMovie->getRasterPtr(0)->pixels().begin(), 16, myMovie->getRasterPtr(0)->pixels().size());
            memset(myMovie->getRasterPtr(1)->pixels().begin(), 127, myMovie->getRasterPtr(1)->pixels().size());
            memset(myMovie->getRasterPtr(2)->pixels().begin(), 127, myMovie->getRasterPtr(2)->pixels().size());
            memset(myMovie->getRasterPtr(3)->pixels().begin(), 0, myMovie->getRasterPtr(3)->pixels().size());
        } else {
            for (unsigned i = 0; i < myRasterCount; i++) {
                myMovie->getRasterPtr(i)->clear();
            }
        }

        _myFirstFrameDecodedFlag = false; // we use the dts of the first decoded frame

        // framerate
        double myFPS;
        myFPS = av_q2d(_myVStream->r_frame_rate);
        myMovie->set<FrameRateTag>(myFPS);
AC_PRINT<<"3";

        // duration
        if (myVCodec->codec_id == CODEC_ID_MPEG1VIDEO || myVCodec->codec_id == CODEC_ID_MPEG2VIDEO )
        {
            // For some codecs, the duration value is not set. For MPEG1 and MPEG2,
            // ffmpeg gives often a wrong value.
            myMovie->set<FrameCountTag>(-1);

        } else if (myVCodec->codec_id == CODEC_ID_WMV1 || myVCodec->codec_id == CODEC_ID_WMV2 ||
                   myVCodec->codec_id == CODEC_ID_WMV3)
        {
            myMovie->set<FrameCountTag>(int(_myVStream->duration * myFPS / 1000));
        } else {
	        double myDuration = 0.0;
            if(_myFormatContext->start_time == static_cast<int64_t>(AV_NOPTS_VALUE)) {
                myDuration = (_myFormatContext->duration )*myFPS/(double)AV_TIME_BASE;
            } else {
                myDuration = (_myFormatContext->duration - _myFormatContext->start_time )*myFPS/(double)AV_TIME_BASE;
            }
            myMovie->set<FrameCountTag>(int(myDuration));
        }
        AC_INFO << "FFMpegDecoder1::load() " << theFilename << " fps="
                << myFPS << " framecount=" << getFrameCount();
        _myLastVideoTimestamp = 0;

        // Get Starttime
        if (_myVStream->start_time != static_cast<int64_t>(AV_NOPTS_VALUE)) {
            _myStartTimestamp = _myVStream->start_time;
        } else {
            _myStartTimestamp = 0;
        }
        AC_DEBUG << "startTimestamp=" << _myStartTimestamp;

        asl::Time myLoadEndTime;
        AC_INFO << "Load time " << (myLoadEndTime - myLoadStartTime) << "s";

        double myAspectRatio = 1.0;
        // calc aspect ratio
#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(52,21,0)
        if (_myVStream->sample_aspect_ratio.num) {
           myAspectRatio = av_q2d(_myVStream->sample_aspect_ratio);
        } else if (_myVStream->codec->sample_aspect_ratio.num) {
#else            
        if (_myVStream->codec->sample_aspect_ratio.num) {
#endif            
           myAspectRatio = av_q2d(_myVStream->codec->sample_aspect_ratio);
        } else {
           myAspectRatio = 0;
        }
        if (myAspectRatio <= 0.0) {
            myAspectRatio = 1.0;
        }
        myAspectRatio *= (float)_myVStream->codec->width / _myVStream->codec->height; 
        myMovie->set<AspectRatioTag>((float)myAspectRatio);
AC_PRINT<<"4";

    }

    double
    FFMpegDecoder1::readFrame(double theTime, unsigned theFrame, RasterVector theTargetRaster) {
        // theTime is in seconds,
        double myTimeUnitsPerSecond = 1/ av_q2d(_myVStream->time_base);
        double myFrameTimestamp = theTime * myTimeUnitsPerSecond + _myStartTimestamp;

        AC_DEBUG << "time=" << theTime << " timestamp=" << myFrameTimestamp <<
                " myTimeUnitsPerSecond=" << myTimeUnitsPerSecond;
        AC_DEBUG << "_myLastVideoTimestamp=" << _myLastVideoTimestamp;
        if ((_myLastVideoTimestamp != 0) && (myFrameTimestamp == _myLastVideoTimestamp)) {
            return theTime;
        }
        if (!decodeFrame(myFrameTimestamp, theTargetRaster[0])) {
            AC_DEBUG << "EOF reached for timestamp=" << myFrameTimestamp;
            setEOF(true);
        } else {
            DB(AC_TRACE << "FFMpegDecoder1::readFrame decoded timestamp=" << myFrameTimestamp);
        }
        return theTime;
    }

    // TODO: Make theTimestamp an input-only parameter.
    bool
    FFMpegDecoder1::decodeFrame(double & theTimestamp, dom::ResizeableRasterPtr theTargetRaster) {
        AC_DEBUG << "--- decodeFrame ---" << endl;
        double myTimeUnitsPerSecond = 1/ av_q2d(_myVStream->time_base);
        double myTimePerFrame = myTimeUnitsPerSecond / getFrameRate();
        if (_myVStream->index_entries != NULL) {
            if (_myLastVideoTimestamp != AV_NOPTS_VALUE &&
                (theTimestamp < _myLastVideoTimestamp || theTimestamp >
                 _myLastVideoTimestamp + (2*myTimePerFrame)))
            {
//            cout <<"seek, theTimeStanp: " << theTimestamp << " last timestamp : "
//                  << _myLastVideoTimestamp << " Frametime : " << myTimePerFrame <<endl;

                // [ch] If we seek directly to the seek timestamp the packet sometimes cannot be decoded afterwards.
                // Therefore this workaround seeks a bit (0.01 Frames) before the requested timestamp
                int64_t mySeekTimestamp = int64_t(theTimestamp-myTimePerFrame/100);
                if (mySeekTimestamp < 0) {
                    mySeekTimestamp = 0;
                }
//            int64_t mySeekTimestamp = theTimestamp;
                AC_DEBUG << "SEEK timestamp=" << theTimestamp << " lastVideoTimestamp="
                         << _myLastVideoTimestamp << " seek=" << mySeekTimestamp;

                int myResult = av_seek_frame(_myFormatContext, _myLastVStreamIndex, mySeekTimestamp,
                                             AVSEEK_FLAG_BACKWARD);
                if (myResult < 0) {
                    AC_ERROR << "Could not seek to timestamp " << mySeekTimestamp;
                }
            }
        }

        VideoFramePtr myVideoFrame;

        AVPacket myPacket;
        memset(&myPacket, 0, sizeof(myPacket));

        // until a frame is found or eof
        for(;;) {
            // EOF handling
            int myError;
            if ((myError = av_read_frame(_myFormatContext, &myPacket)) < 0) {
                av_free_packet(&myPacket);
                /*
                 * Some codecs, such as MPEG, transmit the I and P frame with a
                 * latency of one frame. You must do the following to have a
                 * chance to get the last frame of the video
                 */
                int frameFinished = 0;
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,27,0)
                AVPacket myTempPacket;
                av_init_packet(&myTempPacket);
                myTempPacket.data = NULL;
                myTempPacket.size = 0;
                /*int myLen =*/ avcodec_decode_video2(_myVStream->codec, _myFrame,
                        &frameFinished, &myTempPacket);
#else
                /*int myLen =*/ avcodec_decode_video(_myVStream->codec, _myFrame,
                        &frameFinished, NULL, 0);
#endif
                if (frameFinished) {
                    _myLastVideoTimestamp += myTimePerFrame;

                    // last frame found
                    if (_myLastVideoTimestamp >= theTimestamp) {
                        break;
                    } else {
                        continue;
                    }
                } else {
                    AC_DEBUG << "eof reached : error: " << myError << " " << (myError==-11);
                    return false;
                }
            }

            //       if (myPacket.stream_index == _myVStreamIndex) {
            int myCodecType;
            myCodecType = _myFormatContext->streams[myPacket.stream_index]->codec->codec_type;
            #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
                if (myCodecType == AVMEDIA_TYPE_VIDEO) {
            #else
                if (myCodecType == CODEC_TYPE_VIDEO) {
            #endif
                if (_myLastVStreamIndex != myPacket.stream_index) {
                    setMovieParameters(myPacket.stream_index);
                }

                int frameFinished = 0;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,27,0)
                AVPacket myTempPacket;
                av_init_packet(&myTempPacket);
                myTempPacket.data = myPacket.data;
                myTempPacket.size = myPacket.size;
                while (frameFinished == 0 && myTempPacket.size > 0) {
                    int myLen = avcodec_decode_video2(_myVStream->codec, _myFrame,
                            &frameFinished, &myTempPacket);
                    if (myLen < 0) {
                        AC_ERROR << "av_decode_video error";
                        break;
                    }
                    myTempPacket.data += myLen;
                    myTempPacket.size -= myLen;
#else
                unsigned char* myData = myPacket.data;
                int myDataLen = myPacket.size;
                while (frameFinished == 0 && myDataLen > 0) {
                    int myLen = avcodec_decode_video(_myVStream->codec,
                                                 _myFrame, &frameFinished,
                                                 myData, myDataLen);

                    if (myLen < 0) {
                        AC_ERROR << "av_decode_video error";
                        break;
                    }
                    myData += myLen;
                    myDataLen -= myLen;
#endif
                }

                AC_DEBUG << "decoded dts=" << myPacket.dts << " pts=" << myPacket.pts
                        << " finished=" << frameFinished;
                if (frameFinished == 0) {
                    continue;
                }
                _myLastVideoTimestamp = double(myPacket.dts);
                if (!_myFirstFrameDecodedFlag) {
                    _myFirstFrameDecodedFlag = true;
                    _myStartTimestamp = myPacket.dts;
                }
                
                // suitable framestamp?
                if (_myLastVideoTimestamp >= theTimestamp) {
                    DB(AC_TRACE << "found " << _myLastVideoTimestamp << " for " << theTimestamp);
                    break;
                }
            }
            av_free_packet(&myPacket);
        }

        Movie * myMovie = getMovie();
        AVCodecContext * myVCodec = _myVStream->codec;
        if (myVCodec->pix_fmt == PIX_FMT_PAL8 && _myDestinationPixelFormat == PIX_FMT_GRAY8) {
            myMovie->getRasterPtr(0)->resize(getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(0)->pixels().begin(), _myFrame->data[0], _myFrame->linesize[0], getFrameWidth(), getFrameHeight());
        } else if (myVCodec->pix_fmt == PIX_FMT_BGRA && _myDestinationPixelFormat == PIX_FMT_BGRA) {
            myMovie->getRasterPtr(0)->resize(getFrameWidth()*4, getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(0)->pixels().begin(), _myFrame->data[0], _myFrame->linesize[0], getFrameWidth()*4, getFrameHeight());
        } else if (myVCodec->pix_fmt == PIX_FMT_YUV420P && _myDestinationPixelFormat == PIX_FMT_YUV420P) {
            myMovie->getRasterPtr(0)->resize(getFrameWidth(), getFrameHeight());
            myMovie->getRasterPtr(1)->resize(getFrameWidth()/2, getFrameHeight()/2);
            myMovie->getRasterPtr(2)->resize(getFrameWidth()/2, getFrameHeight()/2);
            copyPlaneToRaster(myMovie->getRasterPtr(0)->pixels().begin(), _myFrame->data[0], _myFrame->linesize[0], getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(1)->pixels().begin(), _myFrame->data[1], _myFrame->linesize[1], getFrameWidth()/2, getFrameHeight()/2);
            copyPlaneToRaster(myMovie->getRasterPtr(2)->pixels().begin(), _myFrame->data[2], _myFrame->linesize[2], getFrameWidth()/2, getFrameHeight()/2);
        } else if (myVCodec->pix_fmt == PIX_FMT_YUV422P && _myDestinationPixelFormat == PIX_FMT_YUV422P) {
            myMovie->getRasterPtr(0)->resize(getFrameWidth(), getFrameHeight());
            myMovie->getRasterPtr(1)->resize(getFrameWidth(), getFrameHeight()/2);
            myMovie->getRasterPtr(2)->resize(getFrameWidth(), getFrameHeight()/2);
            copyPlaneToRaster(myMovie->getRasterPtr(0)->pixels().begin(), _myFrame->data[0], _myFrame->linesize[0], getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(1)->pixels().begin(), _myFrame->data[1], _myFrame->linesize[1], getFrameWidth(), getFrameHeight()/2);
            copyPlaneToRaster(myMovie->getRasterPtr(2)->pixels().begin(), _myFrame->data[2], _myFrame->linesize[2], getFrameWidth(), getFrameHeight()/2);
        } else if (myVCodec->pix_fmt == PIX_FMT_YUV444P && _myDestinationPixelFormat == PIX_FMT_YUV444P) {
            myMovie->getRasterPtr(0)->resize(getFrameWidth(), getFrameHeight());
            myMovie->getRasterPtr(1)->resize(getFrameWidth(), getFrameHeight());
            myMovie->getRasterPtr(2)->resize(getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(0)->pixels().begin(), _myFrame->data[0], _myFrame->linesize[0], getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(1)->pixels().begin(), _myFrame->data[1], _myFrame->linesize[1], getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(2)->pixels().begin(), _myFrame->data[2], _myFrame->linesize[2], getFrameWidth(), getFrameHeight());
        } else if (myVCodec->pix_fmt == PIX_FMT_YUVA420P && _myDestinationPixelFormat == PIX_FMT_YUVA420P) {
            myMovie->getRasterPtr(0)->resize(getFrameWidth(), getFrameHeight());
            myMovie->getRasterPtr(1)->resize(getFrameWidth()/2, getFrameHeight()/2);
            myMovie->getRasterPtr(2)->resize(getFrameWidth()/2, getFrameHeight()/2);
            myMovie->getRasterPtr(3)->resize(getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(0)->pixels().begin(), _myFrame->data[0], _myFrame->linesize[0], getFrameWidth(), getFrameHeight());
            copyPlaneToRaster(myMovie->getRasterPtr(1)->pixels().begin(), _myFrame->data[1], _myFrame->linesize[1], getFrameWidth()/2, getFrameHeight()/2);
            copyPlaneToRaster(myMovie->getRasterPtr(2)->pixels().begin(), _myFrame->data[2], _myFrame->linesize[2], getFrameWidth()/2, getFrameHeight()/2);
            copyPlaneToRaster(myMovie->getRasterPtr(3)->pixels().begin(), _myFrame->data[3], _myFrame->linesize[3], getFrameWidth(), getFrameHeight());
        } else {
            convertFrame(_myFrame, theTargetRaster);
        }

        // store actual timestamp
        theTimestamp = double(myPacket.dts);
        AC_DEBUG << "Packet dts: " << myPacket.dts << ", pts: " << myPacket.pts;

        _myLastVStreamIndex = myPacket.stream_index;
        av_free_packet(&myPacket);

        return true;
    }

    void FFMpegDecoder1::convertFrame(AVFrame* theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
        if (!theFrame) {
            AC_ERROR << "FFMpegDecoder1::decodeVideoFrame invalid AVFrame";
            return;
        }
        unsigned int myLineSizeBytes = getFrameWidth() * _myBytesPerPixel;

        AVPicture myDestPict;
        myDestPict.data[0] = theTargetRaster->pixels().begin();
        myDestPict.data[1] = theTargetRaster->pixels().begin()+1;
        myDestPict.data[2] = theTargetRaster->pixels().begin()+2;

        myDestPict.linesize[0] = myLineSizeBytes;
        myDestPict.linesize[1] = myLineSizeBytes;
        myDestPict.linesize[2] = myLineSizeBytes;


        AVCodecContext * myVCodec = _myVStream->codec;

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(51,38,0)
        START_TIMER(decodeFrame_img_convert);
        img_convert(&myDestPict, _myDestinationPixelFormat,
                    (AVPicture*)theFrame, myVCodec->pix_fmt,
                    myVCodec->width, myVCodec->height);
        STOP_TIMER(decodeFrame_img_convert);

#else
    START_TIMER(decodeFrame_sws_scale);

        int mySWSFlags = SWS_FAST_BILINEAR;//SWS_BICUBIC;
        SwsContext * img_convert_ctx = sws_getContext(myVCodec->width, myVCodec->height,
            myVCodec->pix_fmt,
            myVCodec->width, myVCodec->height,
            _myDestinationPixelFormat,
            mySWSFlags, NULL, NULL, NULL);
        sws_scale(img_convert_ctx, ((AVPicture*)theFrame)->data,
            ((AVPicture*)theFrame)->linesize, 0, myVCodec->height,
            myDestPict.data, myDestPict.linesize);

        sws_freeContext(img_convert_ctx);
    STOP_TIMER(decodeFrame_sws_scale);

#endif
    }
}
