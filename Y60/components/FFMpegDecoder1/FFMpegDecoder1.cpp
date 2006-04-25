//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "FFMpegDecoder1.h"

#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <iostream>

#define DB(x) // x

using namespace std;

extern "C"
EXPORT asl::PlugInBase * y60FFMpegDecoder1_instantiatePlugIn(asl::DLHandle myDLHandle) {
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
        _myVStreamIndex(-1), _myVStream(0),
        _myStartTimestamp(0),
        _myLastVideoTimestamp(0), _myEOFVideoTimestamp(INT_MIN)
    {
        DB(AC_DEBUG << "instantiated an FFMpegDecoder1...");
    }

    FFMpegDecoder1::~FFMpegDecoder1() {
        closeMovie();
    }

    asl::Ptr<MovieDecoderBase> FFMpegDecoder1::instance() const {
        return asl::Ptr<MovieDecoderBase>(new FFMpegDecoder1(getDLHandle()));
    }

    std::string
    FFMpegDecoder1::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        if (asl::toLowerCase(asl::getExtension(theUrl)) == "mpg" ||
            asl::toLowerCase(asl::getExtension(theUrl)) == "m2v") {
            AC_INFO << "FFMpegDecoder1 can decode :" << theUrl << endl;
            return MIME_TYPE_MPG;
        } else {
            AC_INFO << "FFMpegDecoder1 can not decode :" << theUrl << " responsible for extensions mpg and m2v" << endl;
            return "";
        }
    }

    void
    FFMpegDecoder1::closeMovie() {
        AC_DEBUG << "FFMpegDecoder1::closeMovie";
        if (_myVStream) {
#if LIBAVCODEC_BUILD >= 0x5100
            avcodec_close(_myVStream->codec);
#else
            avcodec_close(&_myVStream->codec);
#endif
            _myVStreamIndex = -1;
            _myVStream = 0;
        }

        if (_myFrame) {
            av_free(_myFrame);
            _myFrame = 0;
        }

        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
    }

    void
    FFMpegDecoder1::load(const std::string & theFilename) {
        asl::Time myLoadStartTime;

        // register all formats and codecs
        static bool avRegistered = false;
        if (!avRegistered) {
            AC_INFO << "FFMpegDecoder1 avcodec version " << LIBAVCODEC_IDENT;
            av_log_set_level(AV_LOG_ERROR);
            av_register_all();
            avRegistered = true;
        }

        // open file
        if (av_open_input_file(&_myFormatContext, theFilename.c_str(), 0, FFM_PACKET_SIZE, 0) < 0) {
            throw FFMpegDecoderException(std::string("Unable to open input file: ") + theFilename, PLUS_FILE_LINE);
        }

#if LIBAVCODEC_BUILD >= 0x5100
        av_read_play(_myFormatContext);
#endif

        if (av_find_stream_info(_myFormatContext) < 0) {
            throw FFMpegDecoderException(std::string("Unable to find stream info: ") + theFilename, PLUS_FILE_LINE);
        }

        // find video/audio streams
        for (unsigned i = 0; i < _myFormatContext->nb_streams; ++i) {
            int myCodecType;
#if LIBAVCODEC_BUILD >= 0x5100
            myCodecType = _myFormatContext->streams[i]->codec->codec_type;
#else
            myCodecType = _myFormatContext->streams[i]->codec.codec_type;
#endif
            if (_myVStreamIndex == -1 && myCodecType == CODEC_TYPE_VIDEO) {
                _myVStreamIndex = i;
                _myVStream = _myFormatContext->streams[i];
            }
        }

        if (_myVStreamIndex < 0) {
            throw FFMpegDecoderException(std::string("No video stream found: ") + theFilename, PLUS_FILE_LINE);
        }

        // open codec
#if LIBAVCODEC_BUILD >= 0x5100
        AVCodec * myCodec = avcodec_find_decoder(_myVStream->codec->codec_id);
#else
        AVCodec * myCodec = avcodec_find_decoder(_myVStream->codec.codec_id);
#endif
        if (!myCodec) {
            throw FFMpegDecoderException(std::string("Unable to find decoder: ") + theFilename, PLUS_FILE_LINE);
        }
#if LIBAVCODEC_BUILD >= 0x5100
        if (avcodec_open(_myVStream->codec, myCodec) < 0 ) {
#else
        if (avcodec_open(&_myVStream->codec, myCodec) < 0 ) {
#endif
            throw FFMpegDecoderException(std::string("Unable to open codec: ") + theFilename, PLUS_FILE_LINE);
        }

        // allocate frame for YUV data
        _myFrame = avcodec_alloc_frame();

        Movie * myMovie = getMovie();

        int myWidth, myHeight;
#if LIBAVCODEC_BUILD >= 0x5100
        myWidth = _myVStream->codec->width;
        myHeight = _myVStream->codec->height;
#else
        myWidth = _myVStream->codec.width;
        myHeight = _myVStream->codec.height;
#endif
        myMovie->set<ImageWidthTag>(myWidth);
        myMovie->set<ImageHeightTag>(myHeight);

        // Setup size and image matrix
        float myXResize = float(myWidth) / asl::nextPowerOfTwo(myWidth);
        float myYResize = float(myHeight) / asl::nextPowerOfTwo(myHeight);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
        myMovie->set<ImageMatrixTag>(myMatrix);

        // framerate
        float myFPS;
#if LIBAVCODEC_BUILD >= 0x5100
        myFPS = (1.0f / av_q2d(_myVStream->codec->time_base));
#else
        /*
         * hack to correct wrong frame rates that seem to be generated
         * by some codecs
         */
        if (_myVStream->codec.frame_rate > 1000 && _myVStream->codec.frame_rate_base == 1) {
            _myVStream->codec.frame_rate_base = 1000;
        }
        myFPS = _myVStream->codec.frame_rate / (float) _myVStream->codec.frame_rate_base;
        if (myFPS > 1000.0f) {
            myFPS /= 1000.0f;
        }
#endif
        myMovie->set<FrameRateTag>(myFPS);

        // duration
        if (_myVStream->duration == AV_NOPTS_VALUE ||
            int(myFPS * (_myVStream->duration / (double) AV_TIME_BASE)) <= 0) {
            AC_WARNING << "url='" << theFilename << "' contains no valid duration";
            myMovie->set<FrameCountTag>(INT_MAX);
        } else {
            myMovie->set<FrameCountTag>(asl::round(myFPS * (_myVStream->duration / (float) AV_TIME_BASE)));
        }

        AC_DEBUG << "url='" << theFilename << "' fps=" << myFPS << " framecount=" << getFrameCount();

        _myLastVideoTimestamp = 0;
        _myEOFVideoTimestamp = INT_MIN;

        // Get Starttime
        if (_myVStream->start_time != AV_NOPTS_VALUE) {
            _myStartTimestamp = _myVStream->start_time;
        } else {
            _myStartTimestamp = 0;
        }
        AC_DEBUG << "startTimestamp=" << _myStartTimestamp;

        asl::Time myLoadEndTime;
        AC_INFO << "Load time " << (myLoadEndTime - myLoadStartTime) << "s";
    }

    double
    FFMpegDecoder1::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        // theTime is in seconds,
        int64_t myTimeUnitsPerSecond = (int64_t)(1/ av_q2d(_myVStream->time_base));
#if LIBAVCODEC_BUILD >= 0x5100
        int64_t myFrameTimestamp = (int64_t)(theTime * myTimeUnitsPerSecond) + _myStartTimestamp;
#else
        int64_t myFrameTimestamp = (int64_t)(theTime * AV_TIME_BASE) + _myStartTimestamp;
#endif

        AC_DEBUG << "time=" << theTime << " timestamp=" << myFrameTimestamp <<
                " myTimeUnitsPerSecond=" << myTimeUnitsPerSecond;
        AC_DEBUG << "_myLastVideoTimestamp=" << _myLastVideoTimestamp;

        if (!decodeFrame(myFrameTimestamp, theTargetRaster)) {
            AC_DEBUG << "EOF reached for timestamp=" << myFrameTimestamp;
            if (getFrameCount() == INT_MAX) {
                unsigned myLastFrame = asl::round((_myEOFVideoTimestamp - _myStartTimestamp) * getFrameRate() / AV_TIME_BASE);
                getMovie()->set<FrameCountTag>(myLastFrame + 1);
                AC_DEBUG << "Set framecount=" << getFrameCount();
            }
            setEOF(true);
        } else {
            DB(AC_TRACE << "FFMpegDecoder1::readFrame decoded timestamp=" << myFrameTimestamp);
        }

        return theTime;
    }

    // TODO: Make theTimestamp an input-only parameter.
    bool
    FFMpegDecoder1::decodeFrame(int64_t & theTimestamp, dom::ResizeableRasterPtr theTargetRaster) {
        AC_DEBUG << "--- decodeFrame ---" << endl;
        int64_t myTimeUnitsPerSecond = (int64_t)(1/ av_q2d(_myVStream->time_base));
#if LIBAVCODEC_BUILD >= 0x5100
        int64_t myTimePerFrame = (int64_t)(myTimeUnitsPerSecond / getFrameRate());
#else
        int64_t myTimePerFrame = (int64_t)(AV_TIME_BASE / getFrameRate());
#endif

        if (_myLastVideoTimestamp != AV_NOPTS_VALUE &&
            (theTimestamp < _myLastVideoTimestamp || theTimestamp > 
             _myLastVideoTimestamp + (2*myTimePerFrame)))
        {
//            cout <<"seek, theTimeStanp: " << theTimestamp << " last timestamp : " 
//                  << _myLastVideoTimestamp << " Frametime : " << myTimePerFrame <<endl;

            int64_t mySeekTimestamp = theTimestamp;
            AC_DEBUG << "SEEK timestamp=" << theTimestamp << " lastVideoTimestamp=" 
                    << _myLastVideoTimestamp << " seek=" << mySeekTimestamp;

#if (LIBAVCODEC_BUILD < 4738)
            int myResult = av_seek_frame(_myFormatContext, -1, mySeekTimestamp);
#else
            int myResult = av_seek_frame(_myFormatContext, -1, mySeekTimestamp, 
                    AVSEEK_FLAG_BACKWARD);
#endif
            if (myResult < 0) {
                AC_ERROR << "Could not seek to timestamp " << mySeekTimestamp;
            }
        }

        VideoFramePtr myVideoFrame(0);

        AVPacket myPacket;
        memset(&myPacket, 0, sizeof(myPacket));

#if LIBAVCODEC_BUILD >= 0x5100
        AVCodecContext * myVCodec = _myVStream->codec;
#else
        AVCodecContext * myVCodec = &_myVStream->codec;
#endif

        // until a frame is found or eof
        while (true) {

            // EOF handling
            if (av_read_frame(_myFormatContext, &myPacket) < 0) {
                av_free_packet(&myPacket);
#if 0
                // Remember the end of file timestamp
                if (_myEOFVideoTimestamp == INT_MIN) {
                    _myEOFVideoTimestamp = _myLastVideoTimestamp;
                }

                /*
                 * Some codecs, such as MPEG, transmit the I and P frame with a
                 * latency of one frame. You must do the following to have a
                 * chance to get the last frame of the video
                 */
                int frameFinished = 0;
                int myLen = avcodec_decode_video(myVCodec, _myFrame, &frameFinished, NULL, 0);
                if (frameFinished) {
                    _myLastVideoTimestamp += myTimePerFrame;
//                    _myEOFVideoTimestamp = _myLastVideoTimestamp;

                    // last frame found
                    if (_myLastVideoTimestamp >= theTimestamp) {
                        break;
                    } else {
                        continue;
                    }
                }
                AC_DEBUG << "eof reached";
#endif
                return false;
            }

            if (myPacket.stream_index == _myVStreamIndex) {

                int frameFinished = 0;
                unsigned char* myData = myPacket.data;
                int myDataLen = myPacket.size;

                while (frameFinished == 0 && myDataLen > 0) {
                    int myLen = avcodec_decode_video(myVCodec,
                                                     _myFrame, &frameFinished,
                                                     myData, myDataLen);
                    if (myLen < 0) {
                        AC_ERROR << "av_decode_video error";
                        break;
                    }
                    myData += myLen;
                    myDataLen -= myLen;
                }

                AC_DEBUG << "decoded dts=" << myPacket.dts << " pts=" << myPacket.pts
                        << " finished=" << frameFinished;
                if (frameFinished == 0) {
                    continue;
                }
                _myLastVideoTimestamp = myPacket.dts;

                // suitable framestamp?
                if (_myLastVideoTimestamp >= theTimestamp) {
                    DB(AC_TRACE << "found " << _myLastVideoTimestamp << " for " << theTimestamp);
                    break;
                }
            }
            av_free_packet(&myPacket);
        }

        convertFrame(_myFrame, theTargetRaster);
        // store actual timestamp

        theTimestamp = myPacket.dts;
        AC_DEBUG << "Packet dts: " << myPacket.dts << ", pts: " << myPacket.pts;
        av_free_packet(&myPacket);

        return true;
    }

    void FFMpegDecoder1::convertFrame(AVFrame* theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
        if (!theFrame) {
            AC_ERROR << "FFMpegDecoder1::decodeVideoFrame invalid AVFrame";
            return;
        }
        unsigned int myLineSizeBytes = getBytesRequired(getFrameWidth(), getPixelFormat());

        AVPicture myDestPict;
        myDestPict.data[0] = theTargetRaster->pixels().begin();
        myDestPict.data[1] = theTargetRaster->pixels().begin()+1;
        myDestPict.data[2] = theTargetRaster->pixels().begin()+2;

        myDestPict.linesize[0] = myLineSizeBytes;
        myDestPict.linesize[1] = myLineSizeBytes;
        myDestPict.linesize[2] = myLineSizeBytes;

        // pixelformat stuff
        int myDestFmt;
        switch (getMovie()->getPixelEncoding()) {
        case y60::RGBA:
        case y60::BGRA:
            AC_TRACE << "Using RGBA pixels";
            getMovie()->set<ImagePixelFormatTag>(asl::getStringFromEnum(y60::RGBA, PixelEncodingString));
            myDestFmt = PIX_FMT_RGBA32;
            break;
        case y60::ALPHA:
        case y60::GRAY:
            AC_TRACE << "Using GRAY pixels";
            myDestFmt = PIX_FMT_GRAY8;
            break;
        case y60::RGB:
        case y60::BGR:
        default:
            AC_TRACE << "Using BGR pixels";
            myDestFmt = PIX_FMT_BGR24;
            getMovie()->set<ImagePixelFormatTag>(asl::getStringFromEnum(y60::BGR, PixelEncodingString));
            break;
        }

#if LIBAVCODEC_BUILD >= 0x5100
        AVCodecContext * myVCodec = _myVStream->codec;
#else
        AVCodecContext * myVCodec = &_myVStream->codec;
#endif
        img_convert(&myDestPict, myDestFmt, (AVPicture*)theFrame,
                    myVCodec->pix_fmt, myVCodec->width, myVCodec->height);
    }
}
