//=============================================================================
// Copyright (C) 2004,2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: FFMpegDecoder.cpp,v $
//     $Author: ulrich $
//   $Revision: 1.6 $
//       $Date: 2005/04/01 17:03:27 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#include "FFMpegDecoder1.h"

#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <iostream>

#define DB(x) //x

using namespace std;

extern "C"
EXPORT asl::PlugInBase * y60FFMpegDecoder1_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::FFMpegDecoder(myDLHandle);
}

namespace y60 {

    FFMpegDecoder::VideoFrame::VideoFrame(unsigned theBufferSize) {
        _myBuffer = new unsigned char[theBufferSize];
        DB(AC_DEBUG << "VideoFrame buffer=" << (void*)_myBuffer);
    }

    FFMpegDecoder::VideoFrame::~VideoFrame() {
        DB(AC_DEBUG << "~VideoFrame buffer=" << (void*)_myBuffer);
        delete[] _myBuffer;
        _myBuffer = 0;
    }

    // FFMpegDecoder
    FFMpegDecoder::FFMpegDecoder(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle),
        _myFormatContext(0), _myFrame(0),
        _myVStreamIndex(-1), _myVStream(0),
        _myAStreamIndex(-1), _myAStream(0),
        _myLowerCacheTimestamp(INT_MAX), _myUpperCacheTimestamp(INT_MIN),
        _myFirstTimestamp(0), _myStartTimestamp(0),
        _myLastVideoTimestamp(0), _myEOFVideoTimestamp(INT_MIN)
    {}

    FFMpegDecoder::~FFMpegDecoder() {
        closeMovie();
    }

    asl::Ptr<MovieDecoderBase> FFMpegDecoder::instance() const {
        return asl::Ptr<MovieDecoderBase>(new FFMpegDecoder(getDLHandle()));
    }

    std::string
    FFMpegDecoder::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
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
    FFMpegDecoder::closeMovie() {
        AC_DEBUG << "FFMpegDecoder::closeMovie";

        if (_myVStream) {
            avcodec_close(&_myVStream->codec);
            _myVStreamIndex = -1;
            _myVStream = 0;
        }
        if (_myAStream) {
            avcodec_close(&_myAStream->codec);
            _myAStreamIndex = -1;
            _myAStream = 0;
        }

        if (_myFrame) {
            av_free(_myFrame);
            _myFrame = 0;
        }

        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
    }

    void
    FFMpegDecoder::load(const std::string & theFilename) {
        asl::Time myLoadStartTime;

        // register all formats and codecs
        static bool avRegistered = false;
        if (!avRegistered) {
            AC_PRINT << "FFMpegDecoder avcodec version " << LIBAVCODEC_IDENT;
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
            if (_myVStreamIndex == -1 && _myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_VIDEO) {
                _myVStreamIndex = i;
                _myVStream = _myFormatContext->streams[i];
            }
            else if (_myAStreamIndex == -1 && _myFormatContext->streams[i]->codec.codec_type == CODEC_TYPE_AUDIO) {
                _myAStreamIndex = i;
                _myAStream = _myFormatContext->streams[i];
            }
        }

        if (_myVStreamIndex < 0) {
            throw FFMpegDecoderException(std::string("No video stream found: ") + theFilename, PLUS_FILE_LINE);
        }
        if (_myAStreamIndex < 0) {
            AC_WARNING << "url='" << theFilename << "' no audio stream found";
        }

        // open codec
        AVCodec * myCodec = avcodec_find_decoder(_myVStream->codec.codec_id);
        if (!myCodec) {
            throw FFMpegDecoderException(std::string("Unable to find decoder: ") + theFilename, PLUS_FILE_LINE);
        }
        if (avcodec_open(&_myVStream->codec, myCodec) < 0 ) {
            throw FFMpegDecoderException(std::string("Unable to open codec: ") + theFilename, PLUS_FILE_LINE);
        }

        // allocate frame for YUV data
        _myFrame = avcodec_alloc_frame();

        Movie * myMovie = getMovie();
        myMovie->setPixelEncoding(y60::BGR);
        myMovie->set<ImageWidthTag>(_myVStream->codec.width);
        myMovie->set<ImageHeightTag>(_myVStream->codec.height);

        // Setup size and image matrix
        float myXResize = float(_myVStream->codec.width) / asl::nextPowerOfTwo(_myVStream->codec.width);
        float myYResize = float(_myVStream->codec.height) / asl::nextPowerOfTwo(_myVStream->codec.height);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
        myMovie->set<ImageMatrixTag>(myMatrix);

        /*
         * hack to correct wrong frame rates that seem to be generated
         * by some codecs
         */
        if (_myVStream->codec.frame_rate > 1000 && _myVStream->codec.frame_rate_base == 1) {
            _myVStream->codec.frame_rate_base = 1000;
        }
        float myFPS = _myVStream->codec.frame_rate / (float) _myVStream->codec.frame_rate_base;
        if (myFPS > 1000.0f) {
            myFPS /= 1000.0f;
        }
        myMovie->set<FrameRateTag>(myFPS);
        if (_myVStream->duration == AV_NOPTS_VALUE || 
            int(myFPS * (_myVStream->duration / (double) AV_TIME_BASE)) <= 0) {
            AC_WARNING << "url='" << theFilename << "' contains no valid duration";
            myMovie->set<FrameCountTag>(INT_MAX);
        } else {
            myMovie->set<FrameCountTag>(int(myFPS * (_myVStream->duration / (float) AV_TIME_BASE)));
        }
        AC_DEBUG << "url='" << theFilename << "' fps=" << myFPS << " framecount=" << getFrameCount();

        _myLastVideoTimestamp = 0;
        _myEOFVideoTimestamp = INT_MIN;

        // Get first timestamp
        AVPacket myPacket;
        bool myEndOfFileFlag = (av_read_frame(_myFormatContext, &myPacket) < 0);
        _myFirstTimestamp = myPacket.dts;
        av_free_packet(&myPacket);
        DB(AC_TRACE << "firstTimestamp=" << _myFirstTimestamp);

        // Get Starttime
        if (_myVStream->start_time != AV_NOPTS_VALUE) {
            _myStartTimestamp = _myVStream->start_time;
            DB(AC_TRACE << "startTimestamp=" << _myStartTimestamp);
        } else {
            _myStartTimestamp = 0;
        }

        // seek to start
#if (LIBAVCODEC_BUILD < 4738)
        int myResult = av_seek_frame(_myFormatContext, -1, _myStartTimestamp);
#else
        int myResult = av_seek_frame(_myFormatContext, -1, _myStartTimestamp, AVSEEK_FLAG_BACKWARD);
#endif
        if (myResult < 0) {
            AC_ERROR << "Could not seek to start timestamp " << _myStartTimestamp;
        }

        // create cache
        _myFrameCache.resize(FRAME_CACHE_SIZE);
        unsigned myBufferSize = getFrameWidth() * getFrameHeight() * 3;
        for (unsigned i = 0; i < FRAME_CACHE_SIZE; ++i) {
            _myFrameCache[i] = VideoFramePtr(new VideoFrame(myBufferSize));
        }
        _myCacheWriteIndex = 0;

        asl::Time myLoadEndTime;
        AC_INFO << "Load time " << (myLoadEndTime - myLoadStartTime) << "s";
    }

    double
    FFMpegDecoder::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {

        //asl::Time myDecodeStartTime;

        int64_t myFrameTimestamp = (int64_t)(theTime * AV_TIME_BASE);
        int64_t myTimePerFrame = (int64_t)(AV_TIME_BASE / float(getFrameRate()));

        if (_myFirstTimestamp < 0) {
            myFrameTimestamp += myTimePerFrame;
        }
        DB(AC_TRACE << "time=" << theTime << " timestamp=" << myFrameTimestamp);

        // search for suitable frame in cache
        VideoFramePtr myVideoFrame(0);
        int64_t myMinTimeDiff = INT_MAX;
        _myLowerCacheTimestamp = INT_MAX;
        _myUpperCacheTimestamp = INT_MIN;

        for (VideoFrameCache::iterator it = _myFrameCache.begin(); myMinTimeDiff != 0 && it != _myFrameCache.end(); ++it) {
            int64_t myCacheTimestamp = (*it)->getTimestamp();
            if (myCacheTimestamp < _myLowerCacheTimestamp) {
                _myLowerCacheTimestamp = myCacheTimestamp;
            }
            if (myCacheTimestamp > _myUpperCacheTimestamp) {
                _myUpperCacheTimestamp = myCacheTimestamp;
            }

            int64_t myTimeDiff = asl::abs(myCacheTimestamp - myFrameTimestamp);
            if (myTimeDiff < myMinTimeDiff) {
                myVideoFrame = *it;
                myMinTimeDiff = myTimeDiff;
            }
            DB(AC_TRACE << "cached=" << myCacheTimestamp << " best=" << (myVideoFrame ? myVideoFrame->getTimestamp() : 0) << " diff=" << myMinTimeDiff);
        }
        DB(AC_TRACE << "cache lower=" << _myLowerCacheTimestamp << " upper=" << _myUpperCacheTimestamp);

        if (myVideoFrame && myMinTimeDiff < (myTimePerFrame / 2)) {
            myFrameTimestamp = myVideoFrame->getTimestamp();
            DB(AC_TRACE << "FFMpegDecoder::readFrame found cached timestamp=" << myFrameTimestamp);
            copyFrame(myVideoFrame, theTargetRaster);
        }
        else if (!decodeFrame(myFrameTimestamp, theTargetRaster)) {
            AC_DEBUG << "EOF reached for timestamp=" << myFrameTimestamp;
            if (getFrameCount() == INT_MAX) {
                unsigned myLastFrame = asl::round((_myEOFVideoTimestamp - _myStartTimestamp) * getFrameRate() / AV_TIME_BASE);
                getMovie()->set<FrameCountTag>(myLastFrame + 1);
                AC_DEBUG << "Set framecount=" << getFrameCount();
            }
            setEOF(true);
        } else {
            DB(AC_TRACE << "FFMpegDecoder::readFrame decoded timestamp=" << myFrameTimestamp);
        }

        //asl::Time myDecodeEndTime; AC_TRACE << "Decoding time " << (myDecodeEndTime - myDecodeStartTime) << "s";

        return theTime; //(myFrameTimestamp / (double)AV_TIME_BASE);
    }

    bool
    FFMpegDecoder::decodeFrame(int64_t & theTimestamp, dom::ResizeableRasterPtr theTargetRaster) {

        int64_t myTimePerFrame = (int64_t)(AV_TIME_BASE / getFrameRate());

        // seek if timestamp is outside these boundaries
        if (theTimestamp < _myLowerCacheTimestamp || theTimestamp > (_myUpperCacheTimestamp + myTimePerFrame*2)) {

            if (theTimestamp < _myLowerCacheTimestamp) {
                AC_DEBUG << "Seeking because timestamp below lower cache bound " << theTimestamp << "<" << _myLowerCacheTimestamp;
            } else if (theTimestamp > (_myUpperCacheTimestamp + myTimePerFrame*2)) {
                AC_DEBUG << "Seeking because timestamp beyond upper cache bound " << theTimestamp << ">" << _myUpperCacheTimestamp;
            }

            int64_t mySeekTimestamp;
            if (theTimestamp < _myLowerCacheTimestamp) {
                mySeekTimestamp = theTimestamp - (myTimePerFrame*2);
                if (mySeekTimestamp < _myStartTimestamp) {
                    mySeekTimestamp = 0;
                }
                avcodec_flush_buffers(&_myVStream->codec);
            } else {
                mySeekTimestamp = theTimestamp;
            }
            DB(AC_TRACE << "timestamp=" << theTimestamp << " seeking to=" << mySeekTimestamp);

#if (LIBAVCODEC_BUILD < 4738)
            int myResult = av_seek_frame(_myFormatContext, -1, mySeekTimestamp);
#else
            int myResult = av_seek_frame(_myFormatContext, -1, mySeekTimestamp, AVSEEK_FLAG_BACKWARD);
#endif
            if (myResult < 0) {
                AC_ERROR << "Could not seek to timestamp " << mySeekTimestamp;
            }
        }

        VideoFramePtr myVideoFrame(0);

        AVPacket myPacket;
        memset(&myPacket, 0, sizeof(myPacket));

        // until a frame is found or eof
        while (true) {

            if (av_read_frame(_myFormatContext, &myPacket) < 0) {
                av_free_packet(&myPacket);

                // Remember the end of file timestamp
                if (_myEOFVideoTimestamp == INT_MIN) {
                    _myEOFVideoTimestamp = _myLastVideoTimestamp;
                }

                /* some codecs, such as MPEG, transmit the I and P frame with a
                 * latency of one frame. You must do the following to have a
                 * chance to get the last frame of the video
                 */
                int frameFinished = 0;
                int myLen = avcodec_decode_video(&_myVStream->codec, _myFrame, &frameFinished, NULL, 0);
                if (frameFinished) {
                    _myLastVideoTimestamp += myTimePerFrame;

                    // YUV to RGB
                    myVideoFrame = _myFrameCache[_myCacheWriteIndex++ % _myFrameCache.size()];
                    myVideoFrame->setTimestamp(_myLastVideoTimestamp);
                    convertFrame(_myFrame, myVideoFrame->getBuffer());

                    // last frame found
                    if (_myLastVideoTimestamp >= theTimestamp) {
                        break;
                    } else {
                        continue;
                    }
                }
                return false;
            }

            if (myPacket.stream_index == _myVStreamIndex) {

                int frameFinished = 0;
                unsigned char* myData = myPacket.data;
                int myDataLen = myPacket.size;

                while (frameFinished == 0 && myDataLen > 0) {
                    int myLen = avcodec_decode_video(&_myVStream->codec,
                                                     _myFrame, &frameFinished,
                                                     myData, myDataLen);
                    if (myLen < 0) {
                        AC_ERROR << "av_decode_video error";
                        break;
                    }
                    myData += myLen;
                    myDataLen -= myLen;
                }
                DB(AC_TRACE << "decoded dts=" << myPacket.dts << " finished=" << frameFinished);
                if (frameFinished == 0) {
                    continue;
                }
                _myLastVideoTimestamp = myPacket.dts;

                // YUV to RGB
                myVideoFrame = _myFrameCache[_myCacheWriteIndex++ % _myFrameCache.size()];
                myVideoFrame->setTimestamp(myPacket.dts);
                convertFrame(_myFrame, myVideoFrame->getBuffer());

                // suitable framestamp?
                if (_myLastVideoTimestamp >= theTimestamp) {
                    DB(AC_TRACE << "found " << _myLastVideoTimestamp << " for " << theTimestamp);
                    break;
                }
            } else if (myPacket.stream_index == _myAStreamIndex) {
                //AC_WARNING << "Got audio timestamp=" << myPacket.dts;
            }
        }

        // store actual timestamp
        theTimestamp = myPacket.dts;
        av_free_packet(&myPacket);
        copyFrame(myVideoFrame, theTargetRaster);

        return true;
    }

    void FFMpegDecoder::convertFrame(AVFrame* theFrame, unsigned char* theBuffer) {

        if (!theFrame) {
            AC_ERROR << "FFMpegDecoder::decodeVideoFrame invalid AVFrame";
            return;
        }
        unsigned int myLineSizeBytes = getBytesRequired(getFrameWidth(), getPixelFormat());

        AVPicture myDestPict;
        myDestPict.data[0] = theBuffer;
        myDestPict.data[1] = theBuffer+1;
        myDestPict.data[2] = theBuffer+2;

        myDestPict.linesize[0] = myLineSizeBytes;
        myDestPict.linesize[1] = myLineSizeBytes;
        myDestPict.linesize[2] = myLineSizeBytes;

        // convert to RGB
        int myDestFmt;
        if (getPixelFormat() == y60::RGB) {
            myDestFmt = PIX_FMT_RGB24;
        } else {
            myDestFmt = PIX_FMT_BGR24;
        }
        img_convert(&myDestPict, myDestFmt,
                    (AVPicture*)theFrame, _myVStream->codec.pix_fmt,
                    _myVStream->codec.width, _myVStream->codec.height);
    }

    void FFMpegDecoder::copyFrame(VideoFramePtr theVideoFrame, dom::ResizeableRasterPtr theTargetRaster) {
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
        memcpy(theTargetRaster->pixels().begin(), theVideoFrame->getBuffer(), theTargetRaster->pixels().size());
    }
}
