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

#include "FFMpegDecoder2.h"
#include "FFMpegURLProtocol.h"
#include "Demux.h"

#include <y60/video/Movie.h>
#include <y60/sound/SoundManager.h>
#include <asl/base/Ptr.h>
#include <asl/base/Auto.h>
#include <asl/audio/Pump.h>
#include <asl/base/Logger.h>
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
#include <stdlib.h>

#define DB(x) //x
#define DBV(x) //x
#define DBA(x) //x

using namespace std;
using namespace asl;

extern "C"
EXPORT asl::PlugInBase * FFMpegDecoder2_instantiatePlugIn(asl::DLHandle myDLHandle) {
    return new y60::FFMpegDecoder2(myDLHandle);
}

namespace y60 {


    const double FFMpegDecoder2::AUDIO_BUFFER_SIZE = 0.5;

    asl::Block FFMpegDecoder2::_myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE*10);

    FFMpegDecoder2::FFMpegDecoder2(asl::DLHandle theDLHandle) :
        AsyncDecoder(),
        PosixThread(),
        PlugInBase(theDLHandle),
        _myLastVideoFrame(),
        _myAVCodecLock(),
        _myFormatContext(0),
        _myVStreamIndex(-1),
        _myVStream(0),
        _myAStreamIndex(-1),
        _myAStreamIndexDom(-1),
        _myAStream(0),
        _myMsgQueue(),
        _myFrame(0),
        _myDemux(),
        _myVideoStartTimestamp(0),
        _myDestinationPixelFormat(PIX_FMT_BGR24),
        _myResampleContext(0),
        _myNumFramesDecoded(0),
        _myNumIFramesDecoded(0),
        _myVideoStreamTimeBase(-1),
        _myFrameRate(25),
        _myMaxCacheSize(8),
        _myFrameWidth(0),
        _myFrameHeight(0),
        _myBytesPerPixel(1),
        _myLastFrameTime(0),
        _myAdjustAudioOffsetFlag(false),
        _hasShutDown(false)
    {}

    FFMpegDecoder2::~FFMpegDecoder2() {
        DB(AC_DEBUG << "FFMpegDecoder2::~FFMpegDecoder2()");
        shutdown();
    }
    void FFMpegDecoder2::shutdown() {
        if (!_hasShutDown) {
           DB(AC_DEBUG << "FFMpegDecoder2::shutdown()");
           closeMovie();
            if (_myResampleContext) {
                audio_resample_close(_myResampleContext);
                _myResampleContext = 0;
            }
        }
        _hasShutDown = true;
    }
    asl::Ptr<MovieDecoderBase> FFMpegDecoder2::instance() const {
        return asl::Ptr<MovieDecoderBase>(new FFMpegDecoder2(getDLHandle()));
    }

    std::string
    FFMpegDecoder2::canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream) {
        // assume that ffmpeg can decode everything, except y60 formats
        if (asl::toLowerCase(asl::getExtension(theUrl)) != "m60" &&
            asl::toLowerCase(asl::getExtension(theUrl)) != "x60" &&
            asl::toLowerCase(asl::getExtension(theUrl)) != "d60" &&
            asl::toLowerCase(asl::getExtension(theUrl)) != "i60" )
        {
            AC_INFO << "FFMpegDecoder2 can decode :" << theUrl << endl;
            return MIME_TYPE_MPG;
        } else {
            AC_INFO << "FFMpegDecoder2 can not decode :" << theUrl << endl;
            return "";
        }
    }

    void
    FFMpegDecoder2::load(asl::Ptr<ReadableStream> theSource, const string & theFilename) {
        string theStreamID = string("acstream://") + theFilename;
        registerStream(theStreamID, theSource);
        load(theStreamID);
    }

    void
    FFMpegDecoder2::load(const std::string & theFilename) {
        AC_INFO << "FFMpegDecoder2::load(" << theFilename << ")";
        // register all formats and codecs
        static bool avRegistered = false;
        if (!avRegistered) {
            AC_INFO << "FFMpegDecoder2::load " << LIBAVCODEC_IDENT;
            av_log_set_level(AV_LOG_ERROR);
            av_register_all();
            avRegistered = true;
        }

#if LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(53, 2, 0)
        if (avformat_open_input(&_myFormatContext, theFilename.c_str(), NULL, NULL) < 0) {
#else
        if (av_open_input_file(&_myFormatContext, theFilename.c_str(), 0, 0, 0) < 0) {
#endif
            throw FFMpegDecoder2Exception(std::string("Unable to open input file: ")
                    + theFilename, PLUS_FILE_LINE);
        }

        if (av_find_stream_info(_myFormatContext) < 0) {
            throw FFMpegDecoder2Exception(std::string("Unable to find stream info: ")
                    + theFilename, PLUS_FILE_LINE);
        }
        // for debugging you can let ffmpeg tell detail about the movie
        //char myString[200];
        //dump_format(_myFormatContext, 0, myString, 0);
        // find video/audio streams
        _myDemux = asl::Ptr<Demux>(new Demux(_myFormatContext));
        unsigned myAudioStreamIndex = 0;
        _myAllAudioStreamIndicies.clear();
        for (unsigned i = 0; i < static_cast<unsigned>(_myFormatContext->nb_streams); ++i) {
            int myCodecType =  _myFormatContext->streams[i]->codec->codec_type;
        #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
            if (_myVStreamIndex == -1 && myCodecType == AVMEDIA_TYPE_VIDEO) {
        #else
            if (_myVStreamIndex == -1 && myCodecType == CODEC_TYPE_VIDEO) {
        #endif
                _myVStreamIndex = i;
                _myVStream = _myFormatContext->streams[i];
        #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
            } else if (myCodecType == AVMEDIA_TYPE_AUDIO) {
        #else
            } else if (myCodecType == CODEC_TYPE_AUDIO) {
        #endif
                if (_myAStreamIndex == -1 || myAudioStreamIndex == getMovie()->get<AudioStreamTag>()) {
                    _myAStreamIndex = i;
                    _myAStream = _myFormatContext->streams[i];
                    getMovie()->set<AudioStreamTag>(myAudioStreamIndex);
                    _myAStreamIndexDom = myAudioStreamIndex;
                }
                _myAllAudioStreamIndicies.push_back(i);
                _myDemux->enableStream(i);
                AVCodecContext * myACodec = _myFormatContext->streams[i]->codec;
                // open codec
                AVCodec * myCodec = avcodec_find_decoder(myACodec->codec_id);
                if (avcodec_open(myACodec, myCodec) < 0 ) {
                    throw FFMpegDecoder2Exception(std::string("Unable to open audio codec: "), PLUS_FILE_LINE);
                }

                myAudioStreamIndex++;
            }
        }

        if (_myVStream) {
            setupVideo(theFilename);
        } else {
            AC_INFO << "FFMpegDecoder2::load " << theFilename << " no video stream found";
            _myVStream = 0;
            _myVStreamIndex = -1;
        }
        if (_myAStream && getAudioFlag()) {
            setupAudio(theFilename);
        } else {
            AC_INFO << "FFMpegDecoder2::load " << theFilename
                    << " no audio stream found or disabled";
            _myAudioSink = HWSampleSinkPtr();
            _myAStream = 0;
            _myAStreamIndex = -1;
        }
        if (_myVStreamIndex != -1) {
            _myDemux->enableStream(_myVStreamIndex);
        }
        for (int i = 0; i < _myAllAudioStreamIndicies.size(); i++) {
            if (_myAllAudioStreamIndicies[i] != -1) {
                _myDemux->enableStream(_myAllAudioStreamIndicies[i]);
            }
        }

        getVideoProperties(theFilename);
        decodeFrame();
        // reload video properties after first decode
        getVideoProperties(theFilename);
    }

    void FFMpegDecoder2::startOverAgain() {
        AC_DEBUG <<"FFMpegDecoder2::startOverAgain";

        if (isUnjoined()) {
            DB(AC_DEBUG << "Joining FFMpegDecoder Thread");
            join();
        }
        doSeek(0, false);

        if (!isUnjoined()) {
            DB(AC_DEBUG << "Forking FFMpegDecoder Thread");
            PosixThread::fork();
        } else {
            DB(AC_DEBUG << "Thread already running. No forking.");
        }
    }

    void FFMpegDecoder2::startMovie(double theStartTime, bool theStartAudioFlag) {
        AC_INFO << "FFMpegDecoder2::startMovie "<< getMovie()->get<ImageSourceTag>() << ", time: " << theStartTime << " frames in queue: "<<_myMsgQueue.size();
        double myCurrentTime = 0.0;
        if (_myLastVideoFrame) {
            myCurrentTime = _myLastVideoFrame->getTime();    
        }
        if (theStartTime > 0) {
            if (isUnjoined()) {
                DB(AC_DEBUG << "Joining FFMpegDecoder Thread");
                join();
            }
        }
        if (theStartAudioFlag && hasAudio() && getDecodeAudioFlag()) {
            if (isUnjoined()) {
                DB(AC_DEBUG << "Joining FFMpegDecoder Thread");
                join();
            }
            _myAudioTimeOffset = theStartTime;
            _myAudioSink->stop();
            // ensure that the audio buffer is stopped and cleared
            while (_myAudioSink->getState() != HWSampleSink::STOPPED) {
                asl::msleep(10);
            }
        }
        _myLastVideoFrame = VideoMsgPtr();

        if (!isActive()) {
            if (shouldSeek(myCurrentTime, theStartTime) || _myMsgQueue.hasEOF()) {
                _myDemux->clearPacketCache();
                _myMsgQueue.clear();
                _myMsgQueue.reset();
                doSeek(theStartTime);
                if (theStartAudioFlag && hasAudio() && getDecodeAudioFlag()) {
                    _myAdjustAudioOffsetFlag = true;
                }
            } else if (theStartAudioFlag && hasAudio() && getDecodeAudioFlag()) {
                _myAdjustAudioOffsetFlag = true;
                for (int i = 0; i < _myAllAudioStreamIndicies.size(); i++) {
                    _myDemux->clearPacketCache(_myAllAudioStreamIndicies[i]);
                    av_seek_frame(_myFormatContext, _myAllAudioStreamIndicies[i],
                                  (int64_t)(theStartTime*(1/ av_q2d(_myAStream->time_base))) +
                                  _myAStream->start_time, AVSEEK_FLAG_BACKWARD);
                    avcodec_flush_buffers(_myAStream->codec);
                }
                _myVideoStartTimestamp = 0;
            }
            bool myAudioEOFFlag = false;
            if (theStartAudioFlag && hasAudio() && getDecodeAudioFlag()) {
                myAudioEOFFlag = !readAudio();
            }
            if (_myMsgQueue.size() == 0) {
                if(!decodeFrame() || myAudioEOFFlag) {
                    _myDemux->clearPacketCache();
                    _myMsgQueue.clear();
                    _myMsgQueue.reset();
                    doSeek(theStartTime);
                    if (theStartAudioFlag && hasAudio() && getDecodeAudioFlag()) {
                        _myAdjustAudioOffsetFlag = true;
                    }
                }
            }
        }
        setState(RUN);

        if (!isUnjoined()) {
            DB(AC_DEBUG << "Forking FFMpegDecoder Thread");
            PosixThread::fork();
        } else {
            DB(AC_DEBUG << "Thread already running. No forking.");
        }
        AsyncDecoder::startMovie(theStartTime, theStartAudioFlag);
    }

    void FFMpegDecoder2::resumeMovie(double theStartTime, bool theResumeAudioFlag) {
        AC_INFO << "FFMpegDecoder2::resumeMovie, time: " << theStartTime;
        _myLastVideoFrame = VideoMsgPtr();
        setState(RUN);
        if (!isUnjoined()) {
            DB(AC_DEBUG << "Forking FFMpegDecoder Thread");
            PosixThread::fork();
        } else {
            DB(AC_DEBUG << "Thread already running. No forking.");
        }
        AsyncDecoder::resumeMovie(theStartTime, theResumeAudioFlag);
    }

    void FFMpegDecoder2::stopMovie(bool theStopAudioFlag) {
        AC_INFO << "FFMpegDecoder2::stopMovie";

        if (isUnjoined()) {
            DB(AC_DEBUG << "Joining FFMpegDecoder Thread");
            join();
        }
        if (getState() != STOP) {
            AC_DEBUG << "Stopping Movie";
            _myLastVideoFrame = VideoMsgPtr();

            doSeek(0);
            Movie * myMovie = getMovie();
            myMovie->set<CurrentFrameTag>(0);

            _myDemux->clearPacketCache();
            _myMsgQueue.clear();
            _myMsgQueue.reset();
            dumpCache();
            AsyncDecoder::stopMovie(theStopAudioFlag);
        }
    }

    void
    FFMpegDecoder2::closeMovie() {
        AC_INFO << "FFMpegDecoder2::closeMovie";
        // stop thread
        stopMovie();

        AutoLocker<ThreadLock> myLocker(_myAVCodecLock);
        // codecs
        if (_myVStream) {
            avcodec_close(_myVStream->codec);
            _myVStreamIndex = -1;
            _myVStream = 0;
        }
        if (_myFrame) {
            av_free(_myFrame);
            _myFrame = 0;
        }
        if (_myAStream) {
            for (int i = 0; i < _myAllAudioStreamIndicies.size(); i++) {
                avcodec_close(_myFormatContext->streams[_myAllAudioStreamIndicies[i]]->codec);
            }
            _myAStreamIndex = -1;
            _myAStream = 0;
        }
        if (_myFormatContext) {
            av_close_input_file(_myFormatContext);
            _myFormatContext = 0;
        }
        AsyncDecoder::closeMovie();
    }

    bool FFMpegDecoder2::readAudio() {
        DBA(AC_TRACE << "---- FFMpegDecoder2::readAudio: "<<_myAudioSink->getPumpTime();)
        //XXX: think about replacing BufferedTime with PumpTime here, as PumpTime is used anywhere else
        double myDestBufferedTime = double(_myAudioSink->getBufferedTime())+8/_myFrameRate;
        if (myDestBufferedTime > AUDIO_BUFFER_SIZE) {
            myDestBufferedTime = AUDIO_BUFFER_SIZE;
        }
        while (double(_myAudioSink->getBufferedTime()) < myDestBufferedTime) {
            DBA(AC_DEBUG << "---- FFMpegDecoder2::readAudio: getBufferedTime="
                     << _myAudioSink->getBufferedTime();)
             AVPacket * myPacket;
             for (int i = 0; i < _myAllAudioStreamIndicies.size(); i++) {
                 if (_myAllAudioStreamIndicies[i] == _myAStreamIndex) {
                     myPacket = _myDemux->getPacket(_myAllAudioStreamIndicies[i]);
                 } else {
                     _myDemux->getPacket(_myAllAudioStreamIndicies[i]);
                 }
             }
            //AVPacket * myPacket = _myDemux->getPacket(_myAStreamIndex);
            if (!myPacket) {
                _myAudioSink->stop(true);
                DBA(AC_DEBUG << "---- FFMpegDecoder::readAudio(): eof");
                DBA(AC_DEBUG << "---- FFMpegDecoder::readAudio(): play until audio buffer is empty");
                return false;
            }
            addAudioPacket(*myPacket);
            av_free_packet(myPacket);
            delete myPacket;
        }
        return true;
    }

    void FFMpegDecoder2::addAudioPacket(const AVPacket & thePacket) {
        DBA(AC_TRACE << "FFMpegDecoder2::addAudioPacket()");
        int64_t pts = thePacket.dts;
        if (thePacket.pts != static_cast<int64_t>(AV_NOPTS_VALUE)) {
            pts = thePacket.pts;
        }
        double myTime = pts / (1/av_q2d(_myAStream->time_base));
        if (_myAdjustAudioOffsetFlag) {
            _myAdjustAudioOffsetFlag = false;
            DBA(AC_DEBUG<<"adjusting audioTimeOffset old: "<<_myAudioTimeOffset<<", new: "<<myTime);
            _myAudioTimeOffset = myTime;
        }
        // we need an aligned buffer
        int16_t * myAlignedBuf;
        myAlignedBuf = (int16_t *)av_malloc( AVCODEC_MAX_AUDIO_FRAME_SIZE *10 );

        int myBytesDecoded = 0; // decompressed sample size in bytes

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,27,0)
        AVPacket myTempPacket;
        av_init_packet(&myTempPacket);
        myTempPacket.data = thePacket.data;
        myTempPacket.size = thePacket.size;
        while (myTempPacket.size > 0) {
            myBytesDecoded = AVCODEC_MAX_AUDIO_FRAME_SIZE *10;
            int myLen = avcodec_decode_audio3(_myAStream->codec,
                myAlignedBuf, &myBytesDecoded, &myTempPacket);
            if (myLen < 0) {
                AC_WARNING << "av_decode_audio error";
                myTempPacket.size = 0;
                break;
            }
            myTempPacket.data += myLen;
            myTempPacket.size -= myLen;
            DBA(AC_TRACE << "data left " << myTempPacket.size << " read " << myLen);

#else
        const uint8_t* myData = thePacket.data;
        int myDataLen = thePacket.size;
        while (myDataLen > 0) {
#   if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(51,28,0)
            myBytesDecoded = AVCODEC_MAX_AUDIO_FRAME_SIZE *10;
            int myLen = avcodec_decode_audio2(_myAStream->codec,
                myAlignedBuf, &myBytesDecoded, myData, myDataLen);
#   else
            int myLen = avcodec_decode_audio(_myAStream->codec,
                myAlignedBuf, &myBytesDecoded, myData, myDataLen);
#   endif
            if (myLen < 0) {
                AC_WARNING << "av_decode_audio error";
                myDataLen = 0;
                break;
            }
            myData += myLen;
            myDataLen -= myLen;
            DBA(AC_TRACE << "data left " << myDataLen << " read " << myLen);

#endif
            if ( myBytesDecoded <= 0 ) {
                continue;
            }
            int myNumChannels = _myAStream->codec->channels;
#if  LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(51,4,0)
            int myBytesPerSample = av_get_bytes_per_sample(_myAStream->codec->sample_fmt);
#else
            int myBytesPerSample = av_get_bits_per_sample_format(_myAStream->codec->sample_fmt)>>3;
#endif
            if (myNumChannels == 6) {
                if (myBytesPerSample == 2) {
                    myBytesDecoded = downmix5p1ToStereo(myAlignedBuf, myBytesDecoded);
                } else if (myBytesPerSample == 4) {
                    myBytesDecoded = downmix5p1ToStereo(reinterpret_cast<int32_t *>(myAlignedBuf), myBytesDecoded);
                } else {
                    throw FFMpegDecoder2Exception(std::string("unsupported sample format for 5.1 downmix: "), PLUS_FILE_LINE);
                }
                myNumChannels = 2;
            }
            int numFrames = myBytesDecoded/(myBytesPerSample*myNumChannels);
            DBA(AC_TRACE << "FFMpegDecoder2::decode(): Frames per buffer= " << numFrames);
            // queue audio sample
            AudioBufferPtr myBuffer;
            if (_myResampleContext) {
                numFrames = audio_resample(_myResampleContext,
                        (int16_t*)(_myResampledSamples.begin()),
                        myAlignedBuf, numFrames);
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(_myResampledSamples.begin(), SF_S16, myNumChannels);
            } else {
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(myAlignedBuf, SF_S16, myNumChannels);
            }
            _myAudioSink->queueSamples(myBuffer);
            DBA(AC_DEBUG << "decoded audio time=" << myTime);
        } // while

        av_free( myAlignedBuf );
    }

    bool FFMpegDecoder2::decodeFrame() {
        DBV(AC_DEBUG << "---- FFMpegDecoder2::decodeFrame");
        AVPacket * myPacket = 0;
        unsigned int myDecodedPacketsPerFrame = 0;
        START_TIMER(decodeFrame_ffmpegdecode);
        // until a frame is found or eof
        bool myEndOfFileFlag = false;
        while (!myEndOfFileFlag) {
            DBV(AC_TRACE << "---- decodeFrame: getPacket");
            myPacket = _myDemux->getPacket(_myVStreamIndex);
            if (myPacket == 0) {
                myEndOfFileFlag = true;

                // Usually, we're done at this point. mpeg1 and mpeg2, however,
                // deliver another frame...
                DBV(AC_DEBUG << "---- decodeFrame: eof");
                int myFrameCompleteFlag = 0;
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,27,0)
                AVPacket myTempPacket;
                av_init_packet(&myTempPacket);
                myTempPacket.data = NULL;
                myTempPacket.size = 0;
                /*int myLen =*/ avcodec_decode_video2(_myVStream->codec, _myFrame,
                        &myFrameCompleteFlag, &myTempPacket);
#else
                /*int myLen =*/ avcodec_decode_video(_myVStream->codec, _myFrame,
                        &myFrameCompleteFlag, NULL, 0);
#endif
                if (myFrameCompleteFlag) {
                    DBV(AC_DEBUG << "---- decodeFrame: Last frame.");
                    // The only way to get the timestamp of this frame is to take
                    // the timestamp of the previous frame and add an appropriate
                    // amount.
                    // (_myFrame->coded_picture_number contains garbage on the second
                    // and successive loops.)
                    // Yuck.
                    addCacheFrame(_myFrame, _myLastFrameTime+1.0/_myFrameRate);
                    _myNumFramesDecoded++;
                    if (_myFrame->pict_type == FF_I_TYPE) {
                        DBV(AC_DEBUG << "***** I_FRAME *****");
                        _myNumIFramesDecoded++;
                    }
                    break;
                }
            } else {
                DBV(AC_DEBUG << "---- myPacket->dts=" << myPacket->dts);
                int myFrameCompleteFlag = 0;
                START_TIMER(decodeFrame_avcodec_decode);
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,27,0)
                AVPacket myTempPacket;
                av_init_packet(&myTempPacket);
                myTempPacket.data = myPacket->data;
                myTempPacket.size = myPacket->size;
                int myLen = avcodec_decode_video2(_myVStream->codec, _myFrame,
                                &myFrameCompleteFlag, &myTempPacket);
#else
                // try to decode the frame
                int myLen = avcodec_decode_video(_myVStream->codec, _myFrame,
                                &myFrameCompleteFlag, myPacket->data, myPacket->size);
#endif
                STOP_TIMER(decodeFrame_avcodec_decode);
                DBV(AC_DEBUG <<"FFMpegDecoder2::decodeFrame frame doneflag :  "<< myFrameCompleteFlag<<" len: "<<myLen);
                DBV(AC_DEBUG << "dts=" << myPacket->dts << ", position=" <<
                        myPacket->pos << ", duration=" <<
                        myPacket->duration << ", packet pts=" <<
                        myPacket->pts<<", frame pts: "<<_myFrame->pts<<", size: "<<myPacket->size);
                if (myLen < 0) {
                    AC_ERROR << "---- av_decode_video error";
                } else if (myLen < myPacket->size) {
                    AC_ERROR << "---- av_decode_video: Could not decode video in one step";
                }

                if(!myFrameCompleteFlag) {
                    // count dropped frames
                    myDecodedPacketsPerFrame++;
                    DBV(AC_DEBUG << "### needed packets to decode frame: "<<myDecodedPacketsPerFrame);
                }
                // start_time indicates the begin of the video
                if (myFrameCompleteFlag  && (myPacket->dts >= _myVStream->start_time)) {
                    STOP_TIMER(decodeFrame_ffmpegdecode);
                    int64_t myNextPacketTimestamp = myPacket->dts;
                    double myFrameTime = (double)myNextPacketTimestamp/_myVideoStreamTimeBase;
                    DBV(AC_DEBUG << "---- add decoded frame"<< " time_base:"<<_myVideoStreamTimeBase
                                <<" FrameTime: "<<myFrameTime;)
                    addCacheFrame(_myFrame, myFrameTime);
                    _myLastFrameTime = myFrameTime;
                    _myNumFramesDecoded++;
                    if (_myFrame->pict_type == FF_I_TYPE) {
                        DBV(AC_DEBUG << "***** I_FRAME *****");
                        _myNumIFramesDecoded++;
                    }

                    av_free_packet(myPacket);
                    delete myPacket;
                    break;
                }
                av_free_packet(myPacket);
                delete myPacket;
            }
        }
        return !myEndOfFileFlag;
    }

    void FFMpegDecoder2::convertFrame(AVFrame* theFrame, unsigned char* theBuffer) {
        if (!theFrame) {
            AC_ERROR << "FFMpegDecoder::convertFrame invalid AVFrame";
            return;
        }

        AVPicture myDestPict;
        myDestPict.data[0] = theBuffer;
        myDestPict.data[1] = theBuffer+1;
        myDestPict.data[2] = theBuffer+2;

        unsigned myLineSizeBytes = _myFrameWidth * _myBytesPerPixel;
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

    VideoMsgPtr FFMpegDecoder2::createFrame(double theTimestamp) {
        DBV(AC_DEBUG << "FFMpegDecoder2::createFrame");

        vector<unsigned> myBufferSizes;
        switch (_myDestinationPixelFormat) {
            case PIX_FMT_BGRA:
                myBufferSizes.push_back(_myFrameWidth * _myFrameHeight * 4);
                break;
            case PIX_FMT_GRAY8:
                myBufferSizes.push_back(_myFrameWidth * _myFrameHeight * 1);
                break;
            case PIX_FMT_YUV420P:
                myBufferSizes.push_back(_myFrameWidth * _myFrameHeight);
                myBufferSizes.push_back(_myFrameWidth * _myFrameHeight / 4);
                myBufferSizes.push_back(_myFrameWidth * _myFrameHeight / 4);
                break;
            default:
                myBufferSizes.push_back(_myFrameWidth * _myFrameHeight * 3);
                break;
        }
        return VideoMsgPtr(new VideoMsg(VideoMsg::MSG_FRAME, theTimestamp, myBufferSizes));
    }

    void
    FFMpegDecoder2::dumpCache() {
        AC_DEBUG << "VideoMsgQueue size: " << _myMsgQueue.size();
        _myDemux->dump();
    }

    void
    FFMpegDecoder2::checkAudioStream() {
        if (_myAStreamIndexDom == getMovie()->get<AudioStreamTag>()) {
            return;
        }
        unsigned myAudioStreamIndex = 0;
        for (unsigned i = 0; i < static_cast<unsigned>(_myFormatContext->nb_streams); ++i) {
            int myCodecType =  _myFormatContext->streams[i]->codec->codec_type;        
        #if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 64, 0)
            if (myCodecType == AVMEDIA_TYPE_AUDIO) {
        #else
            if (myCodecType == CODEC_TYPE_AUDIO) {
        #endif
                if (_myAStreamIndex == -1 || myAudioStreamIndex == getMovie()->get<AudioStreamTag>()) {
                    if (isUnjoined()) {
                        DB(AC_DEBUG << "Joining FFMpegDecoder Thread");
                        join();
                    }
                    _myAStreamIndex = i;
                    _myAStream = _myFormatContext->streams[i];
                    _myAStreamIndexDom = myAudioStreamIndex;
                    getMovie()->set<AudioStreamTag>(_myAStreamIndexDom);
                    if (!isUnjoined()) {
                        DB(AC_DEBUG << "seek: Forking FFMpegDecoder Thread");
                        PosixThread::fork();
                    } else {
                        DB(AC_DEBUG << "Thread already running. No forking.");
                    }
                    break;
                }
                myAudioStreamIndex++;
            }
        }
        getMovie()->set<AudioStreamTag>(_myAStreamIndexDom);
    }
    double
    FFMpegDecoder2::readFrame(double theTime, unsigned /*theFrame*/, RasterVector theTargetRaster)
    {
        AC_TRACE << "readFrame, Time wanted=" << theTime;
        //ASSURE(!getEOF());
        if (getEOF()) {
            return theTime;
        }
        if (getPlayMode() == y60::PLAY_MODE_STOP) {
            AC_DEBUG << "readFrame: not playing.";
            return theTime;
        }
        // check audio index change
        if (hasAudio() && getDecodeAudioFlag()) {
            checkAudioStream();
        }
        VideoMsgPtr myVideoMsg;
        double myStreamTime = theTime;
        myStreamTime += _myVideoStartTimestamp/_myVideoStreamTimeBase;
        AC_TRACE << "_myVideoStreamTimeBase: "<< _myVideoStreamTimeBase <<" _myVideoStartTimestamp: "
                <<_myVideoStartTimestamp<<" Time=" << theTime;
        AC_TRACE <<" - Reading frame timestamp: " << myStreamTime<< " from queue.";

        try {
            bool useLastVideoFrame = false;
            if (_myLastVideoFrame) {
                if(shouldSeek(_myLastVideoFrame->getTime(), myStreamTime)) {
                    seek(theTime);
                    _myMovieTime = theTime;
                    return -1;
                }
                double myFrameTime = _myLastVideoFrame->getTime();
                double myFrameDiff = (myStreamTime-myFrameTime)*_myFrameRate;
                if ( myFrameDiff > -0.9 && myFrameDiff < 0.5) {
                    AC_DEBUG << "readFrame: Reusing last frame. myFrameTime=" << myFrameTime
                        << ", myStreamTime=" << myStreamTime;
                    myVideoMsg = _myLastVideoFrame;
                    useLastVideoFrame = true;
                }
            }
            bool myFrameDroppedFlag = false;

            if (!useLastVideoFrame) {
                for(;;) {
                    myVideoMsg = _myMsgQueue.pop_front();
                    bool myEOFFoundFlag = _myMsgQueue.hasEOF();
                    if (hasAudio() && getDecodeAudioFlag() && myEOFFoundFlag && !isActive()) {
                        startOverAgain();
                    }
                    if (myVideoMsg->getType() == VideoMsg::MSG_EOF) {
                        setEOF(true);
                        return theTime;
                    }
                    double myTimestamp = myVideoMsg->getTime();
                    AC_TRACE << "readFrame: FrameTime=" << myTimestamp << ", Calculated frame #="
                        << (myTimestamp - (_myVideoStartTimestamp/_myVideoStreamTimeBase))*_myFrameRate
                        << ", Cache size=" << _myMsgQueue.size();
                    double myFrameDiff = (myStreamTime - myTimestamp)*_myFrameRate;
                    AC_TRACE << "           myFrameDiff=" << myFrameDiff;
                    if (myFrameDiff < 0.5 && myFrameDiff > -1.0) {
                        break;
                    } else if (myFrameDiff <= -1.0){
                        // no video frame for audio time in the queue -> wait until the times match again
                        AC_DEBUG << "can not find frame: " << myStreamTime << " -> decoder must have dropped it!!";
                        myFrameDroppedFlag = true;
                        _myMsgQueue.push_front(myVideoMsg); // don't loose the current video frame
                        theTime = -1;
                        break;
                    }
                }
            }
            // Current frame is in myVideoMsg now. Convert to a format that Y60 can use.
            // we think this is for version bumping
            for (unsigned i = 0 ; i < theTargetRaster.size(); i++) {
                theTargetRaster[i]->resize( theTargetRaster[i]->width(), theTargetRaster[i]->height());
            }
            if (myVideoMsg) {
                if(!myFrameDroppedFlag){
                    AC_DEBUG << "readFrame: Frame delivered. wanted=" << theTime
                        << ", got=" << (myVideoMsg->getTime()- _myVideoStartTimestamp/_myVideoStreamTimeBase);
                    theTime = myVideoMsg->getTime() - _myVideoStartTimestamp/_myVideoStreamTimeBase;
                    _myLastVideoFrame = myVideoMsg;
                }
                if (getPlayMode() == y60::PLAY_MODE_PAUSE && !_myLastVideoFrame) {
                    _myLastVideoFrame = myVideoMsg;
                }
                for (unsigned i = 0 ; i < theTargetRaster.size(); i++) {
                    memcpy(theTargetRaster[i]->pixels().begin(), myVideoMsg->getBuffer(i),
                            theTargetRaster[i]->pixels().size());
                }
            } else {
                // TODO: Figure out if/why this happens. Delete?
                AC_WARNING << "readFrame, empty frame.";
                if (_myDestinationPixelFormat == PIX_FMT_YUV420P ) {
                    memset(theTargetRaster[0]->pixels().begin(), 16, theTargetRaster[0]->pixels().size());
                    memset(theTargetRaster[1]->pixels().begin(), 127, theTargetRaster[1]->pixels().size());
                    memset(theTargetRaster[2]->pixels().begin(), 127, theTargetRaster[2]->pixels().size());
                } else {
                    for (unsigned i = 0 ; i < theTargetRaster.size(); i++) {
                        memset(theTargetRaster[i]->pixels().begin(), 0, theTargetRaster[i]->pixels().size());
                    }
                }
            }
            getMovie()->set<CacheSizeTag>(_myMsgQueue.size());
        } catch (asl::ThreadSemaphore::ClosedException &) {
            AC_WARNING << "Semaphore Destroyed while in readframe";
        }
        return theTime;
    }

    void FFMpegDecoder2::run() {
        AC_DEBUG << "---- run starting";
        if (_myVStream == 0 && _myAStream == 0) {
            AC_WARNING << "---- Neither audio nor video stream in FFMpegDecoder2::run";
            return;
        }

        // decoder loop
        bool isDone = false;
        while (!shouldTerminate() && !isDone) {
            DBV(AC_TRACE << "---- FFMpegDecoder2::loop");
            if (_myMsgQueue.size() >= _myMaxCacheSize) {
                // decode the audio...but let the video thread sleep
                if (hasAudio() && getDecodeAudioFlag()) {
                    DBA(AC_DEBUG<<"---sleeping ---still decode audio");
                    /*bool myAudioEofFlag = !*/readAudio();
                }
                //XXX: forces the main thread to wait when joining decode thread
                asl::msleep(10);
                yield();
                continue;
            }
            if (hasAudio()&& getDecodeAudioFlag()) {
                //XXX: disabled audio eof flag beacause it doesn't make sense for the current decoder structure
                // eof only means the demuxer demuxed all packets,
                // but there can be a lot of packets in his queue, which the video decoder hasn't decoded yet
                // If we had a seperate video & audio decoder, we could stop the appropriate thread
                /*bool myAudioEofFlag = !*/readAudio();
            }
            DBV(AC_TRACE << "---- Updating cache");
            try {
                if (!decodeFrame()) {
                    std::vector<unsigned> myFrameSize;
                    myFrameSize.push_back(0);
                    _myMsgQueue.push_back(VideoMsgPtr(new VideoMsg(VideoMsg::MSG_EOF, 0, myFrameSize)));
                    if (hasAudio() && getDecodeAudioFlag()) {
                        isDone = true;
                        AC_DEBUG << "---- EOF Yielding Thread.";
                        continue;
                    } else {
                        doSeek(0, false);
                    }
                }
            } catch (asl::ThreadSemaphore::ClosedException &) {
                // This should never happen.
                AC_WARNING << "---- Semaphore destroyed while in run. Terminating Thread.";
                return;
            }

            DBV(AC_TRACE << "---- end of loop");
        }
        AC_DEBUG << "---- FFMpegDecoder2::run terminating";
    }

    void FFMpegDecoder2::setupVideo(const std::string & theFilename) {
        AC_INFO << "FFMpegDecoder2::setupVideo";
        AVCodecContext * myVCodec = _myVStream->codec;
        // open codec
        AVCodec * myCodec = avcodec_find_decoder(myVCodec->codec_id);
        if (!myCodec) {
            throw FFMpegDecoder2Exception(std::string("Unable to find video codec: ")
                    + theFilename, PLUS_FILE_LINE);
        }

        {
            AutoLocker<ThreadLock> myLocker(_myAVCodecLock);
            if (avcodec_open(myVCodec, myCodec) < 0 ) {
                throw FFMpegDecoder2Exception(std::string("Unable to open video codec: ")
                                              + theFilename, PLUS_FILE_LINE);
            }
        }

        Movie * myMovie = getMovie();
        AC_DEBUG << "PF=" << myMovie->get<RasterPixelFormatTag>();

        PixelEncoding myRasterEncoding = PixelEncoding(getEnumFromString(myMovie->get<RasterPixelFormatTag>(),
                                                        PixelEncodingString));

        // TargetPixelFormatTag is the format the incoming movieframe will be converted in
        if (myMovie->get<TargetPixelFormatTag>() != "") {
            TextureInternalFormat myTargetPixelFormat = TextureInternalFormat(getEnumFromString(myMovie->get<TargetPixelFormatTag>(), TextureInternalFormatStrings));
            switch(myTargetPixelFormat) {
                case TEXTURE_IFMT_YUV420:
                    myRasterEncoding = YUV420;
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
                    AC_FATAL << "Unsupported pixel format " << myMovie->get<TargetPixelFormatTag>() << " in FFMpegDecoder2";
                    break;
            }
        }

        // Setup size and image matrix
        _myFrameWidth = myVCodec->width;
        _myFrameHeight = myVCodec->height;


        switch (myRasterEncoding) {
            case RGBA:
                {AC_DEBUG << "Using TEXTURE_IFMT_RGBA8 pixels";}
                _myDestinationPixelFormat = PIX_FMT_BGRA;
                _myBytesPerPixel = 4;
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::BGRA);
                break;
            case ALPHA:
                {AC_DEBUG << "Using Alpha pixels";}
                _myDestinationPixelFormat = PIX_FMT_GRAY8;
                _myBytesPerPixel = 1;
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::ALPHA);
                break;
            case GRAY:
                {AC_DEBUG << "Using GRAY pixels";}
                _myDestinationPixelFormat = PIX_FMT_GRAY8;
                _myBytesPerPixel = 1;
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::GRAY);
                break;
            case YUV420:
                {AC_DEBUG << "Using YUV420 pixels";}
                _myDestinationPixelFormat = PIX_FMT_YUV420P;
                if (myVCodec->pix_fmt != PIX_FMT_YUV420P) {
                    AC_WARNING<<"you're trying to use YUV2RGB shader but the source video pixel format is not YUV420p, src: " + theFilename;
                }
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::GRAY);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, _myFrameWidth/2, _myFrameHeight/2), y60::GRAY, 1);
                myMovie->addRasterValue(createRasterValue( y60::GRAY, _myFrameWidth/2, _myFrameHeight/2), y60::GRAY, 1);
                break;
            case RGB:
            default:
                {AC_DEBUG << "Using BGR pixels";}
                _myDestinationPixelFormat = PIX_FMT_BGR24;
                _myBytesPerPixel = 3;
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::BGR);
                break;
        }
        unsigned myRasterCount = myMovie->getNode().childNodesLength();
        if (_myDestinationPixelFormat == PIX_FMT_YUV420P ) {
            memset(myMovie->getRasterPtr(0)->pixels().begin(), 16, myMovie->getRasterPtr(0)->pixels().size());
            memset(myMovie->getRasterPtr(1)->pixels().begin(), 127, myMovie->getRasterPtr(1)->pixels().size());
            memset(myMovie->getRasterPtr(2)->pixels().begin(), 127, myMovie->getRasterPtr(2)->pixels().size());
        } else {
            for (unsigned i = 0; i < myRasterCount; i++) {
                myMovie->getRasterPtr(i)->clear();
            }
        }

        // allocate frame for YUV data
        _myFrame = avcodec_alloc_frame();
    }

    void
    FFMpegDecoder2::getVideoProperties(const std::string & theFilename) {
        AVCodecContext * myVCodec = _myVStream->codec;
        Movie * myMovie = getMovie();

        _myFrameRate = av_q2d(_myVStream->r_frame_rate);
        
        myMovie->set<FrameRateTag>(_myFrameRate);
        if (myVCodec->codec_id == CODEC_ID_MPEG1VIDEO || myVCodec->codec_id == CODEC_ID_MPEG2VIDEO )
        {
            // For some codecs, the duration value is not set. For MPEG1 and MPEG2,
            // ffmpeg gives often a wrong value.
            _myVideoStreamTimeBase = 1/ av_q2d(_myVStream->time_base);
            //unsigned myFrameCount = unsigned(_myVStream->duration*_myFrameRate/_myVideoStreamTimeBase);

        } else if (myVCodec->codec_id == CODEC_ID_WMV1 || myVCodec->codec_id == CODEC_ID_WMV2 ||
                   myVCodec->codec_id == CODEC_ID_WMV3)
        {
            myMovie->set<FrameCountTag>(int(_myVStream->duration * _myFrameRate / 1000));
            myMovie->set<MaxCacheSizeTag>(std::min(int(myMovie->get<MaxCacheSizeTag>()), int(myMovie->get<FrameCountTag>())));
            _myVideoStreamTimeBase = 1/ av_q2d(_myVStream->time_base);
        } else {
            double myDuration = 0.0;
            if(_myFormatContext->start_time == static_cast<int64_t>(AV_NOPTS_VALUE)) {
                myDuration = (_myFormatContext->duration )*_myFrameRate/(double)AV_TIME_BASE;
            } else {
                myDuration = (_myFormatContext->duration - _myFormatContext->start_time )*_myFrameRate/(double)AV_TIME_BASE;
            }
            myMovie->set<FrameCountTag>(int(asl::round(myDuration)));
            myMovie->set<MaxCacheSizeTag>(std::min(int(myMovie->get<MaxCacheSizeTag>()), int(myMovie->get<FrameCountTag>())));
            _myVideoStreamTimeBase = 1/ av_q2d(_myVStream->time_base);
        }
        _myMaxCacheSize = myMovie->get<MaxCacheSizeTag>();

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

        _myVideoStartTimestamp = _myVStream->start_time;//myPacket->dts;
        AC_INFO << "FFMpegDecoder2::getVideoProperties() " << theFilename << " fps="
                << _myFrameRate << " framecount=" << getFrameCount()<< " time_base: "
                <<_myVideoStreamTimeBase;
        AC_INFO << "r_framerate den: " <<_myVStream->r_frame_rate.den<< " r_framerate num: "<< _myVStream->r_frame_rate.num;
        AC_INFO << "stream time_base: " << _myVStream->time_base.den << ","<<_myVStream->time_base.num;
        AC_INFO << "codec time_base: " << _myVStream->codec->time_base.den << ","<<_myVStream->codec->time_base.num;
        AC_INFO << "formatcontex start_time: " << _myFormatContext->start_time<<" stream start_time: "<<_myVStream->start_time;
        AC_INFO << "aspect ratio= " << myAspectRatio;

    }

    void
    FFMpegDecoder2::setupAudio(const std::string & theFilename) {
        AVCodecContext * myACodec = _myAStream->codec;

        _myAudioSink = Pump::get().createSampleSink(theFilename);

        unsigned myChannels = (myACodec->channels > 2) ? 2 : myACodec->channels;
        if (myACodec->sample_rate != static_cast<int>(Pump::get().getNativeSampleRate()) ||
            myACodec->sample_fmt != SAMPLE_FMT_S16)
        {
#if  LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,15,0)
            _myResampleContext = av_audio_resample_init(
                    myChannels, myChannels,
                    Pump::get().getNativeSampleRate(), myACodec->sample_rate,
                    SAMPLE_FMT_S16, myACodec->sample_fmt,
                    16, 10, 0, 0.8);
#else
            _myResampleContext = audio_resample_init(myChannels,
                    myChannels, Pump::get().getNativeSampleRate(),
                    myACodec->sample_rate);
#endif
        }
        AC_DEBUG << "FFMpegDecoder2::setupAudio() done. resampling "
            << (_myResampleContext != 0);
    }

    void FFMpegDecoder2::addCacheFrame(AVFrame* theFrame, double theTime) {
        MAKE_SCOPE_TIMER(FFMpegDecoder2_addCacheFrame);
        DBV(AC_DEBUG << "---- try to add frame at " << theTime);
        VideoMsgPtr myVideoFrame = createFrame(theTime);
        AVCodecContext * myVCodec = _myVStream->codec;
        if (myVCodec->pix_fmt == PIX_FMT_PAL8 && _myDestinationPixelFormat == PIX_FMT_GRAY8) {
            copyPlaneToRaster(myVideoFrame->getBuffer(0), theFrame->data[0], theFrame->linesize[0], _myFrameWidth, _myFrameHeight);
        } else if (myVCodec->pix_fmt == PIX_FMT_BGRA && _myDestinationPixelFormat == PIX_FMT_BGRA) {
            copyPlaneToRaster(myVideoFrame->getBuffer(0), theFrame->data[0], theFrame->linesize[0], _myFrameWidth*4, _myFrameHeight);
        } else {
            if (_myDestinationPixelFormat == PIX_FMT_YUV420P ) {
                copyPlaneToRaster(myVideoFrame->getBuffer(0), theFrame->data[0], theFrame->linesize[0], _myFrameWidth, _myFrameHeight);
                copyPlaneToRaster(myVideoFrame->getBuffer(1), theFrame->data[1], theFrame->linesize[1], _myFrameWidth/2, _myFrameHeight/2);
                copyPlaneToRaster(myVideoFrame->getBuffer(2), theFrame->data[2], theFrame->linesize[2], _myFrameWidth/2, _myFrameHeight/2);
            } else {
                convertFrame(theFrame, myVideoFrame->getBuffer());
            }
        }
        _myMsgQueue.push_back(myVideoFrame);

        DBV(AC_DEBUG << "---- Added Frame to cache, Frame # : "
            << double(theTime - _myVideoStartTimestamp/_myVideoStreamTimeBase)*_myFrameRate
            << " cache size=" << _myMsgQueue.size());
    }

    bool FFMpegDecoder2::shouldSeek(double theCurrentTime, double theDestTime) {
        double myDistance = (theDestTime-theCurrentTime)*_myFrameRate;
        AC_DEBUG << "FFMpegDecoder2::shouldSeek: Dest=" << theDestTime << ", Curr=" << theCurrentTime<<" --> distance: "<< myDistance;
        return (myDistance > _myMaxCacheSize || myDistance < 0);
    }

    void FFMpegDecoder2::seek(double theDestTime) {
        AC_DEBUG << "FFMpegDecoder2::seek() desttime: "<<theDestTime;
        if (isUnjoined()) {
            DB(AC_DEBUG << "Joining FFMpegDecoder Thread");
            join();
        }
        if (hasAudio() && getDecodeAudioFlag()) {
            _myAudioSink->stop();
            // ensure that the audio buffer is stopped and cleared
            while (_myAudioSink->getState() != HWSampleSink::STOPPED) {
                asl::msleep(10);
            }
        }

        _myDemux->clearPacketCache();
        _myMsgQueue.clear();
        _myMsgQueue.reset();

        doSeek(theDestTime);
        _myLastVideoFrame = VideoMsgPtr();
        if (hasAudio() && getDecodeAudioFlag()) {
            if (getState() == RUN) {
                _myAudioSink->play();
            }
            _myAudioTimeOffset = theDestTime;
            _myAdjustAudioOffsetFlag = true;
        }
        if (!isActive()) {
            if (hasAudio() && getDecodeAudioFlag()) {
                readAudio();
            }
            decodeFrame();
        }


        if (!isUnjoined()) {
            DB(AC_DEBUG << "seek: Forking FFMpegDecoder Thread");
            PosixThread::fork();
        } else {
            DB(AC_DEBUG << "Thread already running. No forking.");
        }
    }

    // Calls ffmpeg seek, flushes buffers etc.
    void FFMpegDecoder2::doSeek(double theDestTime, bool theSeekAudioFlag) {
        AC_DEBUG << "FFMpegDecoder2::doSeek destTime: " << theDestTime << " destFrame: " << theDestTime * _myFrameRate;

        // AVSEEK_FLAG_ANY: seek to any frame, even non keyframes -> results in artifacts until the next iframe is decoded
        // AVSEEK_FLAG_BACKWARD: seek backwards -> ffmpeg seek better when this flag is enabled, even when seeking forward

        unsigned char mySeekFlags = AVSEEK_FLAG_BACKWARD;// | AVSEEK_FLAG_ANY;
        int64_t mySeekTimeInTimeBaseUnits = int64_t(theDestTime*_myVideoStreamTimeBase);
        /*int myResult =*/ av_seek_frame(_myFormatContext, _myVStreamIndex,
                                mySeekTimeInTimeBaseUnits, mySeekFlags);
        if (theSeekAudioFlag && hasAudio() && getDecodeAudioFlag()) {
            mySeekTimeInTimeBaseUnits = int64_t(theDestTime*(1/ av_q2d(_myAStream->time_base)));
             for (int i = 0; i < _myAllAudioStreamIndicies.size(); i++) {
                /*int myResult =*/ av_seek_frame(_myFormatContext, _myAllAudioStreamIndicies[i],
                                         mySeekTimeInTimeBaseUnits, mySeekFlags);
             }
        }
        avcodec_flush_buffers(_myVStream->codec);
        if (theSeekAudioFlag && hasAudio() && getDecodeAudioFlag()) {
            avcodec_flush_buffers(_myAStream->codec);
        }
    }
}
