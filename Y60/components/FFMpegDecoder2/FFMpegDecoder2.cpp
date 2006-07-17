//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "FFMpegDecoder2.h"
#include "FFMpegURLProtocol.h"
#include "Demux.h"

#include <y60/SoundManager.h>
#include <asl/Ptr.h>
#include <asl/Auto.h>
#include <asl/Pump.h> //must come before Assure.h
#include <asl/Block.h>
#include <asl/Logger.h>
#include <asl/Assure.h>
#include <asl/file_functions.h>

#include <iostream>
#include <stdlib.h>

#define DB(x) //x
#define DB2(x) //x

using namespace std;
using namespace asl;

#define USE_RGBA 0
#define USE_GRAY 1

const long SEMAPHORE_TIMEOUT = asl::ThreadSemaphore::WAIT_INFINITE; //1000 * 60 * 1;

extern "C"
EXPORT asl::PlugInBase * y60FFMpegDecoder2_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::FFMpegDecoder2(myDLHandle);
}

namespace y60 {

    const unsigned FFMpegDecoder2::FRAME_CACHE_SIZE = 8;
    const double FFMpegDecoder2::AUDIO_BUFFER_SIZE = 0.5;

    asl::Block FFMpegDecoder2::_myResampledSamples(AVCODEC_MAX_AUDIO_FRAME_SIZE);
    asl::Block FFMpegDecoder2::_mySamples(AVCODEC_MAX_AUDIO_FRAME_SIZE);

    FFMpegDecoder2::FFMpegDecoder2(asl::DLHandle theDLHandle) :
        PlugInBase(theDLHandle),
        AsyncDecoder(),
        PosixThread(),
        _myFormatContext(0),
        _myFrame(0),
        _myVStreamIndex(-1),
        _myVStream(0),
        _myStartTimestamp(-1),
        _myAStreamIndex(-1),
        _myAStream(0),
        _myDemux(0),
        _myDestinationPixelFormat(0),
        _myFrameCache(SEMAPHORE_TIMEOUT),
        _myFrameRecycler(SEMAPHORE_TIMEOUT),
        _myResampleContext(0),
        _myNumFramesDecoded(0),
        _myNumIFramesDecoded(0),
        _myReadEOF(false),
        _myBytesPerPixel(1)
    {
    }

    FFMpegDecoder2::~FFMpegDecoder2() {
        closeMovie();
        if (_myResampleContext) {
            audio_resample_close(_myResampleContext);
            _myResampleContext = 0;
        }
    }

    asl::Ptr<MovieDecoderBase> FFMpegDecoder2::instance() const {
        return asl::Ptr<MovieDecoderBase>(new FFMpegDecoder2(getDLHandle()));
    }

    std::string
    FFMpegDecoder2::canDecode(const std::string & theUrl, asl::ReadableStream * theStream) {
        if (asl::toLowerCase(asl::getExtension(theUrl)) == "mpg" ||
            asl::toLowerCase(asl::getExtension(theUrl)) == "m2v" ||
            asl::toLowerCase(asl::getExtension(theUrl)) == "avi" ||
            asl::toLowerCase(asl::getExtension(theUrl)) == "mov" ||
            asl::toLowerCase(asl::getExtension(theUrl)) == "mpeg") {
            AC_TRACE << "FFMpegDecoder2 can decode :" << theUrl << endl;
            return MIME_TYPE_MPG;
        } else {
            AC_TRACE << "FFMpegDecoder2 can not decode :" << theUrl <<
                    "responsible for extensions mpg and m2v" << endl;
            return "";
        }
    }

    void
    FFMpegDecoder2::load(Ptr<ReadableStream> theSource, const string & theFilename) {
        string theStreamID = string("acstream://") + theFilename;
        registerStream(theStreamID, theSource);
        load(theStreamID);
    }

    void
    FFMpegDecoder2::load(const std::string & theFilename) {
        AC_DEBUG << "load(" << theFilename << ")";
        AutoLocker<ThreadLock> myLock(_myLock);

        // register all formats and codecs
        static bool avRegistered = false;
        if (!avRegistered) {
            AC_INFO << "FFMpegDecoder2::load " << LIBAVCODEC_IDENT;
            av_log_set_level(AV_LOG_ERROR);
            av_register_all();
            avRegistered = true;
        }

        if (av_open_input_file(&_myFormatContext, theFilename.c_str(), 0, FFM_PACKET_SIZE, 0)
                < 0) {
            throw FFMpegDecoder2Exception(std::string("Unable to open input file: ")
                    + theFilename, PLUS_FILE_LINE);
        }

        if (av_find_stream_info(_myFormatContext) < 0) {
            throw FFMpegDecoder2Exception(std::string("Unable to find stream info: ")
                    + theFilename, PLUS_FILE_LINE);
        }

        // find video/audio streams
        for (unsigned i = 0; i < _myFormatContext->nb_streams; ++i) {
            int myCodecType =  _myFormatContext->streams[i]->codec->codec_type;
            if (_myVStreamIndex == -1 && myCodecType == CODEC_TYPE_VIDEO) {
                _myVStreamIndex = i;
                _myVStream = _myFormatContext->streams[i];
            }
            else if (_myAStreamIndex == -1 && myCodecType == CODEC_TYPE_AUDIO) {
                _myAStreamIndex = i;
                _myAStream = _myFormatContext->streams[i];
            }
        }
        _myTimeUnitsPerSecond = (int64_t)(1/ av_q2d(_myVStream->time_base));
        AC_DEBUG << "FFMpegDecoder2::_myTimeUnitsPerSecond=" << _myTimeUnitsPerSecond;
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
            AC_INFO << "FFMpegDecoder2::load " << theFilename << " no audio stream found or disabled";
            _myAudioSink = HWSampleSinkPtr(0);
            _myAStream = 0;
            _myAStreamIndex = -1;
        }
        _myDemux = asl::Ptr<Demux>(new Demux(_myFormatContext));
        if (_myVStreamIndex != -1) {
            _myDemux->enableStream(_myVStreamIndex);
        }
        if (_myAStreamIndex != -1) {
            _myDemux->enableStream(_myAStreamIndex);
        }
        createFrameCache();
    }

    void FFMpegDecoder2::startMovie(double theStartTime) {
        AC_DEBUG << "startMovie, time: " << theStartTime;

        AutoLocker<ThreadLock> myLock(_myLock);
        _myReadEOF = false;

        // seek to start
        // TODO: only do this on loop.
        int myResult = av_seek_frame(_myFormatContext, -1,
                int64_t(theStartTime*_myTimeUnitsPerSecond)+_myStartTimestamp,
                AVSEEK_FLAG_BACKWARD);
        avcodec_flush_buffers(_myVStream->codec);
        if (_myAStream) {
            avcodec_flush_buffers(_myAStream->codec);
        }

        decodeFrame();
        if (hasAudio())
        {
            readAudio();
            AC_INFO << "Start Audio";
            _myAudioSink->play();
        }

        setState(RUN);
        if (!isActive()) {
            AC_DEBUG << "Forking FFMpegDecoder Thread";
            PosixThread::fork();
        } else {
            AC_INFO << "Thread already running. No forking.";
        }
        AsyncDecoder::startMovie(theStartTime);
    }

    void FFMpegDecoder2::resumeMovie(double theStartTime) {
        AC_DEBUG << "resumeMovie, time: " << theStartTime;
        AutoLocker<ThreadLock> myLock(_myLock);
        setState(RUN);
        if (!isActive()) {
            AC_TRACE << "Forking FFMpegDecoder Thread";
            PosixThread::fork();
        } else {
            AC_INFO << "Thread already running. No forking.";
        }
        AsyncDecoder::resumeMovie(theStartTime);
    }

    void FFMpegDecoder2::stopMovie() {
        AC_DEBUG << "stopMovie";
        if (getState() != STOP) {
            AC_DEBUG << "Joining FFMpegDecoder Thread";
            join();
            if (_myAudioSink) {
                _myAudioSink->stop();
            }
            _myFrameRecycler.close();
            _myDemux->clearPacketCache();
            setState(STOP);
            _myFrameCache.clear();
            _myFrameCache.reset();
            dumpCaches();
            createFrameCache();
            AsyncDecoder::stopMovie();
        }
    }

    void
    FFMpegDecoder2::closeMovie() {
        AC_DEBUG << "closeMovie";
        // stop thread
        stopMovie();

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
            avcodec_close(_myAStream->codec);
            _myAStreamIndex = -1;
            _myAStream = 0;
        }

        av_close_input_file(_myFormatContext);
        _myFormatContext = 0;
        AsyncDecoder::closeMovie();
    }

    void FFMpegDecoder2::readAudio() {
        AC_TRACE << "---- FFMpegDecoder2::readAudio:";
        double myDestBufferedTime = _myAudioSink->getBufferedTime()+2/_myFrameRate;
        if (myDestBufferedTime > AUDIO_BUFFER_SIZE) {
            myDestBufferedTime = AUDIO_BUFFER_SIZE;
        }
        while (double(_myAudioSink->getBufferedTime()) < myDestBufferedTime) {
            AC_TRACE << "---- FFMpegDecoder2::readAudio: getBufferedTime="
                    << _myAudioSink->getBufferedTime();
//            AC_DEBUG << "---- FFMpegDecoder2::readAudio: getPacket()";
            AVPacket * myPacket = _myDemux->getPacket(_myAStreamIndex);
            if (!myPacket) {
                _myAudioSink->stop(true);
                AC_DEBUG << "---- FFMpegDecoder::readAudio(): eof" << endl;
                return;
            }
            addAudioPacket(*myPacket);
            av_free_packet(myPacket);
            delete myPacket;
        }
    }

    void FFMpegDecoder2::addAudioPacket(const AVPacket & thePacket) {
        // decode audio
        int myBytesDecoded = 0; // decompressed sample size in bytes
        unsigned char* myData = thePacket.data;
        unsigned myDataLen = thePacket.size;
        double myTime = thePacket.dts / (double)_myTimeUnitsPerSecond;

        AC_TRACE << "FFMpegDecoder2::addAudioPacket()";
        while (myDataLen > 0) {
            int myLen = avcodec_decode_audio(_myAStream->codec,
                (int16_t*)_mySamples.begin(), &myBytesDecoded, myData, myDataLen);
            if (myLen < 0 || myBytesDecoded < 0) {
                AC_WARNING << "av_decode_audio error";
                break;
            }
            int myNumChannels = _myAStream->codec->channels;
            int numFrames = myBytesDecoded/(getBytesPerSample(SF_S16)*myNumChannels);
            AC_TRACE << "FFMpegDecoder2::decode(): Frames per buffer= " << numFrames;
            // queue audio sample
            AudioBufferPtr myBuffer;
            if (_myResampleContext) {
                numFrames = audio_resample(_myResampleContext,
                        (int16_t*)(_myResampledSamples.begin()),
                        (int16_t*)(_mySamples.begin()),
                        numFrames);
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(_myResampledSamples.begin(), SF_S16, myNumChannels);
            } else {
                myBuffer = Pump::get().createBuffer(numFrames);
                myBuffer->convert(_mySamples.begin(), SF_S16, myNumChannels);
            }
            _myAudioSink->queueSamples(myBuffer);

            myData += myLen;
            myDataLen -= myLen;
            AC_TRACE << "data left " << myDataLen << " read " << myLen;

            AC_TRACE << "decoded audio time=" << myTime;
        } // while

        // adjust volume
/*
        float myVolume = getMovie()->get<VolumeTag>();
        if (!asl::almostEqual(Pump::get().getVolume(), myVolume)) {
            Pump::get().setVolume(myVolume);
        } //if
*/
    }

    bool FFMpegDecoder2::decodeFrame() {
        AC_DEBUG << "---- FFMpegDecoder2::decodeFrame";
        AVPacket * myPacket = 0;

        // until a frame is found or eof
        bool myEndOfFileFlag = false;
        while (!myEndOfFileFlag) {
            AC_TRACE << "---- FFMpegDecoder2::decodeFrame: getPacket";
            myPacket = _myDemux->getPacket(_myVStreamIndex);
            if (myPacket == 0) {
                myEndOfFileFlag = true;
                AC_DEBUG << "---- FFMpegDecoder2::decodeFrame: eof";
            } else {
                AC_DEBUG << "myPacket->dts=" << myPacket->dts;
                int myFrameCompleteFlag = 0;
                int myLen = avcodec_decode_video(_myVStream->codec, _myFrame,
                        &myFrameCompleteFlag, myPacket->data, myPacket->size);
                if (myLen < 0) {
                    AC_ERROR << "---- av_decode_video error";
                } else if (myLen < myPacket->size) {
                    AC_ERROR << "---- av_decode_video: Could not decode video in one step";
                }

                if (myFrameCompleteFlag) {
                    if (_myStartTimestamp == -1) {
                        _myStartTimestamp = myPacket->dts;
                        AC_DEBUG << "Set StartTimestamp to packet timestamp, = " << _myStartTimestamp;
                    }
                    // The following line used to calculate the presentation timestamp
                    // by hand, assuming that ffmpeg made errors in the calculation.
                    // That breaks seeks, so I've removed it - uz.
                    // _myNextPacketTimestamp += _myTimePerFrame;
                    int64_t myNextPacketTimestamp = (myPacket->dts-_myStartTimestamp);
		            AC_TRACE << "---- add frame";
                    addCacheFrame(_myFrame, myNextPacketTimestamp);
                    _myNumFramesDecoded++;
                    if (_myFrame->pict_type == FF_I_TYPE) {
                        AC_DEBUG << "***** I_FRAME *****";
                        _myNumIFramesDecoded++;
                    }
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
        img_convert(&myDestPict, _myDestinationPixelFormat,
                    (AVPicture*)theFrame, myVCodec->pix_fmt,
                    myVCodec->width, myVCodec->height);
    }

    void FFMpegDecoder2::copyFrame(FrameCache::VideoFramePtr theVideoFrame,
                                   dom::ResizeableRasterPtr theTargetRaster)
    {
        theTargetRaster->resize(getFrameWidth(), getFrameHeight());
        memcpy(theTargetRaster->pixels().begin(), theVideoFrame->getBuffer(),
                theTargetRaster->pixels().size());
    }

    void FFMpegDecoder2::createFrameCache() {
        AC_DEBUG << "createFrameCache";
        _myFrameRecycler.clear();
        _myFrameRecycler.reset();

        // create cache
        unsigned myBufferSize = 0;
        switch (_myDestinationPixelFormat) {
            case PIX_FMT_RGBA32:
                myBufferSize = getFrameWidth() * getFrameHeight() * 4;
                break;
            case PIX_FMT_GRAY8:
                myBufferSize = getFrameWidth() * getFrameHeight() * 1;
                break;
            default:
                myBufferSize = getFrameWidth() * getFrameHeight() * 3;
                break;
        }
        while (_myFrameRecycler.size() < FRAME_CACHE_SIZE+1) {
            _myFrameRecycler.push_back(FrameCache::VideoFramePtr(new FrameCache::VideoFrame(myBufferSize)));
        }
    }

    void
    FFMpegDecoder2::dumpCaches() {
        AC_DEBUG << "FrameCache size: " << _myFrameCache.size();
        AC_DEBUG << "FrameRecycler size: " << _myFrameRecycler.size();
        _myDemux->dump();
    }

    double
    FFMpegDecoder2::readFrame(double theTime, unsigned /*theFrame*/,
            dom::ResizeableRasterPtr theTargetRaster)
    {
        AC_DEBUG << "FFMpegDecoder2::readFrame, Time wanted=" << theTime;
        double myResultTime = theTime;
        ASSURE(!getEOF());
        if (theTargetRaster == 0) {
            throw FFMpegDecoder2Exception("TargetRasterPtr is null.", PLUS_FILE_LINE);
        }
        try {
            FrameCache::VideoFramePtr myVideoFrame(0);
            if (getPlayMode() == y60::PLAY_MODE_STOP) {
                AC_DEBUG << "readFrame: not playing.";
                return theTime;
            }
            double myFrameTime = theTime;
            if (_myStartTimestamp != -1) {
                myFrameTime += _myStartTimestamp/_myTimeUnitsPerSecond;
            }
            AC_TRACE << "Time=" << theTime << " - Reading FrameTimestamp: " << myFrameTime
                    << " from Cache.";
            if (shouldSeek(_myFrameCache.front()->getTimestamp(), theTime) &&
                    getPlayMode() == y60::PLAY_MODE_PAUSE)
            {
                seek(theTime);
            }

            for (;;) {
                myVideoFrame = _myFrameCache.pop_front();
                AC_DEBUG << "FFMpegDecoder2::readFrame, FrameTime="
                    << myVideoFrame->getTimestamp()
                    << ", Calculated frame #=" << myVideoFrame->getTimestamp()*getFrameRate()
                    << ", Cache size=" << _myFrameCache.size();
                double myTimeDiff = (myFrameTime - myVideoFrame->getTimestamp())*getFrameRate();
                if ((myTimeDiff > 0.5 || myTimeDiff < -0.9) && _myFrameCache.size() != 0) {
                    _myFrameRecycler.push_back(myVideoFrame);
                } else {
                    _myFrameCache.push_front(myVideoFrame);
                    break;
                }
            }
            AutoLocker<ThreadLock> myLock(_myLock);
            if (_myReadEOF && _myFrameCache.size() <= 1) {
                _myReadEOF = false;
                setEOF(true);
                return theTime;
            }

            // convert frame
            theTargetRaster->resize(getFrameWidth(), getFrameHeight());
            if (myVideoFrame) {
                AC_DEBUG << "readFrame: Frame delivered. wanted=" << theTime
                        << ", got=" << myVideoFrame->getTimestamp();
                theTime = myVideoFrame->getTimestamp();
                copyFrame(myVideoFrame, theTargetRaster);
            } else {
                AC_DEBUG << "readFrame, empty frame.";
                memset(theTargetRaster->pixels().begin(), 0, theTargetRaster->pixels().size());
            }
            getMovie()->set<CacheSizeTag>(_myFrameCache.size());

/*
            // Hack to correct the frame count in case the movie itself contains a bogus frame
            // count.
            if (getFrameCount() < (myVideoFrame->getTimestamp()*getFrameRate()+1)) {
                getMovie()->set<FrameCountTag>(int(myVideoFrame->getTimestamp()*getFrameRate()+1));
            }
*/
        } catch (asl::ThreadSemaphore::ClosedException &) {
            AC_WARNING << "Semaphore Destroyed while in readframe";
            return theTime;
        } catch (FrameCache::TimeoutException &) {
            AC_FATAL << "Semaphore Timeout";
            return theTime;
        }
        return theTime;
    }

    void FFMpegDecoder2::run() {
        AC_DEBUG << "---- run starting";
        if (_myVStream == 0 && _myAStream == 0) {
			AC_WARNING << "---- Neither audio nor video stram in FFMpegDecoder2::run";
            return;
        }
        _myReadEOF = false;

        int64_t mySeekTimestamp = AV_NOPTS_VALUE;

        // decoder loop
        while (!shouldTerminate() && !getReadEOF()) {
			AC_TRACE << "---- FFMpegDecoder2::loop";
            if (_myFrameCache.size() >= FRAME_CACHE_SIZE) {
                // do nothing...
                asl::msleep(10);
                yield();
                continue;
            }
            AC_TRACE << "---- Updating cache";
            try {
                if (!decodeFrame()) {
                    {
                        AutoLocker<ThreadLock> myLock(_myLock);
                        _myReadEOF = true;
                    }
					AC_DEBUG << "---- EOF Yielding Thread.";
                    continue;
                }
            } catch (asl::ThreadSemaphore::ClosedException &) {
                // This should never happen.
                AC_WARNING << "---- Semaphore destroyed while in run. Terminating Thread.";
                return;
            }
            if (hasAudio())
            {
                readAudio();
            }

			AC_TRACE << "---- end of loop";
        }
        AC_DEBUG << "---- FFMpegDecoder2::run terminating";
    }

    void FFMpegDecoder2::setupVideo(const std::string & theFilename) {
        AC_DEBUG << "setupVideo";
        AVCodecContext * myVCodec = _myVStream->codec;

        // open codec
        AVCodec * myCodec = avcodec_find_decoder(myVCodec->codec_id);
        if (!myCodec) {
            throw FFMpegDecoder2Exception(std::string("Unable to find video codec: ")
                    + theFilename, PLUS_FILE_LINE);
        }
        if (avcodec_open(myVCodec, myCodec) < 0 ) {
            throw FFMpegDecoder2Exception(std::string("Unable to open video codec: ")
                    + theFilename, PLUS_FILE_LINE);
        }

        Movie * myMovie = getMovie();
        AC_TRACE << "PF=" << myMovie->get<RasterPixelFormatTag>();
        
        TextureInternalFormat myTextureFormat = myMovie->getInternalEncoding();

        // Setup size and image matrix
        _myFrameWidth = myVCodec->width;
        _myFrameHeight = myVCodec->height;

        switch (myTextureFormat) {
            case TEXTURE_IFMT_RGBA8:
                AC_TRACE << "Using TEXTURE_IFMT_RGBA8 pixels";
                _myDestinationPixelFormat = PIX_FMT_RGBA32;
                _myBytesPerPixel = 4;
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::RGBA);
                break;
            case TEXTURE_IFMT_ALPHA:
            case TEXTURE_IFMT_LUMINANCE8:
                AC_TRACE << "Using GRAY pixels";
                _myDestinationPixelFormat = PIX_FMT_GRAY8;
                _myBytesPerPixel = 1;
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::ALPHA);
                break;
            case TEXTURE_IFMT_RGB8:
            default:
                AC_TRACE << "Using BGR pixels";
                _myDestinationPixelFormat = PIX_FMT_BGR24;
                _myBytesPerPixel = 3;
                myMovie->createRaster(_myFrameWidth, _myFrameHeight, 1, y60::BGR);
                break;
        }


        myMovie->getRasterPtr()->clear();


        float myXResize = float(_myFrameWidth) / asl::nextPowerOfTwo(_myFrameWidth);
        float myYResize = float(_myFrameHeight) / asl::nextPowerOfTwo(_myFrameHeight);

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(asl::Vector3f(myXResize, myYResize, 1.0f));
        myMovie->set<ImageMatrixTag>(myMatrix);

        _myFrameRate = (1.0 / av_q2d(myVCodec->time_base));
        myMovie->set<FrameRateTag>(_myFrameRate);

        if (_myVStream->duration == AV_NOPTS_VALUE) {
            AC_TRACE << "FFMpegDecoder2::setupVideo() " << theFilename
                    << " has no valid duration";
        }
        if (_myVStream->duration == AV_NOPTS_VALUE || _myVStream->duration <= 0) {
            AC_WARNING << "FFMpegDecoder2::setupVideo(): '" << theFilename
                    << "' contains no valid duration";
            myMovie->set<FrameCountTag>(INT_MAX);
        } else {
	        myMovie->set<FrameCountTag>(int(_myFrameRate * (_myVStream->duration
                    / (double) _myTimeUnitsPerSecond)));
            AC_TRACE << "FFMpegDecoder2::setupVideo(): _myVStream->duration="
                    << _myVStream->duration << ", _myTimeUnitsPerSecond="
                    << _myTimeUnitsPerSecond;
        }
        AC_INFO << "FFMpegDecoder2::setupVideo() " << theFilename << " fps="
                << _myFrameRate << " framecount=" << getFrameCount();

        // allocate frame for YUV data
        _myFrame = avcodec_alloc_frame();
        _myStartTimestamp = -1;
    }

    void
    FFMpegDecoder2::setupAudio(const std::string & theFilename) {
        AVCodecContext * myACodec = _myAStream->codec;

        // open codec
        AVCodec * myCodec = avcodec_find_decoder(myACodec->codec_id);
        if (!myCodec) {
            throw FFMpegDecoder2Exception(std::string("Unable to find audio decoder: ")
                    + theFilename, PLUS_FILE_LINE);
        }
        if (avcodec_open(myACodec, myCodec) < 0 ) {
            throw FFMpegDecoder2Exception(std::string("Unable to open audio codec: ")
                    + theFilename, PLUS_FILE_LINE);
        }

        _myAudioSink = Pump::get().createSampleSink(theFilename);

        if (myACodec->sample_rate != Pump::get().getNativeSampleRate())
        {
            _myResampleContext = audio_resample_init(myACodec->channels,
                    myACodec->channels, Pump::get().getNativeSampleRate(),
                    myACodec->sample_rate);
        }
        AC_TRACE << "FFMpegDecoder2::setupAudio() done. resampling "
            << (_myResampleContext != 0);
    }

    void FFMpegDecoder2::addCacheFrame(AVFrame* theFrame, int64_t theTimestamp) {
		AC_TRACE << "---- try to add frame at " << theTimestamp;
        try {
//            AC_DEBUG << "---- frame recycler size=" << _myFrameRecycler.size();
            FrameCache::VideoFramePtr myVideoFrame = _myFrameRecycler.pop_front();
            myVideoFrame->setTimestamp(theTimestamp/(double)_myTimeUnitsPerSecond);
            convertFrame(theFrame, myVideoFrame->getBuffer());
            _myFrameCache.push_back(myVideoFrame);
            AC_DEBUG << "---- Added Frame to cache, Frame # : "
                    << double(theTimestamp)/_myTimeUnitsPerSecond*_myFrameRate
                    << " cache size=" << _myFrameCache.size();
        } catch (FrameCache::TimeoutException &) {
            AC_FATAL << "---- Semaphore Timeout";
        }
    }

    // Uses a heuristic based on the number of I-Frames in the video to determine
    // if the two times are far enough apart to warrant a seek.
    // TODO: Test this with lots of different videos.
    bool FFMpegDecoder2::shouldSeek(double theCurrentTime, double theDestTime) {
        int myDistance = int((theDestTime-theCurrentTime)*getFrameRate());
        bool myShouldSeek;
        if (_myNumIFramesDecoded < 2) {
            myShouldSeek = myDistance>100 || myDistance < -1;
        } else {
            myShouldSeek = myDistance>2*(_myNumFramesDecoded/_myNumIFramesDecoded)
                    || myDistance < -0.5;
        }
        if (myShouldSeek) {
            AC_DEBUG << "Dest=" << theDestTime << ", Curr=" << theCurrentTime
                << ", myDistance=" << myDistance;
            AC_DEBUG << "_myNumFramesDecoded=" << _myNumFramesDecoded << ", _myNumIFramesDecoded="
                << _myNumIFramesDecoded;
        }
        return  myShouldSeek;
    }

    bool FFMpegDecoder2::getReadEOF() {
        AutoLocker<ThreadLock> myLock(_myLock);
        return _myReadEOF;
    }

    void FFMpegDecoder2::seek(double theDestTime) {
        AC_DEBUG << "seek: Joining FFMpegDecoder Thread";
        join();
        if (_myAudioSink) {
            _myAudioSink->stop();
        }
        _myFrameRecycler.close();
        _myDemux->clearPacketCache();
        _myFrameCache.clear();
        _myFrameCache.reset();
        createFrameCache();

        int64_t mySeekTime = int64_t(theDestTime*_myTimeUnitsPerSecond)+_myStartTimestamp;
        AC_DEBUG << "FFMpegDecoder2::mySeekTime=" << mySeekTime;
        int myResult = av_seek_frame(_myFormatContext, _myVStreamIndex,
                mySeekTime, AVSEEK_FLAG_BACKWARD);
        decodeFrame();
        if (hasAudio())
        {
            AC_DEBUG << "seek: Start Audio";
            _myAudioSink->setCurrentTime(theDestTime);
            if (getState() == RUN) {
                readAudio();
                _myAudioSink->play();
            }
        }

        AC_DEBUG << "seek: Forking FFMpegDecoder Thread";
        PosixThread::fork();
    }

}
