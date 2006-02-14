//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_FFMpegDecoder2_h_
#define _ac_y60_FFMpegDecoder2_h_

#include "AsyncDecoder.h"
#include "FrameCache.h"

#include <y60/MovieEncoding.h>

#include <asl/PlugInBase.h>

#include <asl/PosixThread.h>
#include <asl/ThreadLock.h>
#include <asl/ThreadSemaphore.h>

#include <string>
#include <list>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4244)
#define EMULATE_INTTYPES
#endif

#include <ffmpeg/avformat.h>

#ifdef WIN32
#pragma warning(pop)
#endif

#include <asl/Stream.h>
#include <asl/Ptr.h>

namespace AudioBase {
    class BufferedSource;
}

namespace y60 {

    DEFINE_EXCEPTION(FFMpegDecoder2Exception, asl::Exception);
    const std::string MIME_TYPE_MPG = "application/mpg";

    /**
     * @ingroup Y60video
     * Asyncronous decoder using ffmpeg. It features audio and
     * video decoding but no seeking.
     *
     */
    class FFMpegDecoder2 :
        public AsyncDecoder,
        public PosixThread,
        public asl::PlugInBase
    {
        static const unsigned FRAME_CACHE_SIZE = 8;  ///< Number of frames to cache in the Framecache

    public:
        FFMpegDecoder2(asl::DLHandle theDLHandle);
        virtual ~FFMpegDecoder2();


        virtual asl::Ptr<MovieDecoderBase> instance() const;
        std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0);

        void convertFrame(AVFrame* theFrame, unsigned char* theBuffer);
        void copyFrame(FrameCache::VideoFramePtr theVideoFrame, dom::ResizeableRasterPtr theTargetRaster);
        void fillCache(double theStartTime = 0.0f);

        /**
         * loads a movie from the file given by theFilename
         * @param theFilename file to load into the decoder
         */
        void load(const std::string & theFilename);
        double readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster);

        /**
         * loads a movie from the stream given by theSource
         * @param theFilename file to identify theSource
         */
        void load(asl::Ptr<asl::ReadableStream> theSource, const std::string & theFilename);
        /**
         * Starts movie decoding
         * @param theStartTime movie-time to start decoding at.
         */
        void startMovie(double theStartTime = 0.0f);

        void resumeMovie(double theStartTime = 0.0f);

        /**
         * Called to stop the decoding.
         */
        void stopMovie();
        void closeMovie();
        /**
         * Tries to generate a frame from thePacket. If a frame was generated,
         * it is added to the cache, by calling addCacheFrame and true is returned,
         * otherwise false is returned
         * is returned.
         * @warn blocks if there is no room in the cache
         * @throws asl::ThreadSemaphore::ClosedException
         * @param thePacket video packet to decode and add to the cache
         * @retval true, if a frame was added
         */
        bool addVideoPacket(const AVPacket & thePacket);
        /**
         * Add an audio packet to the buffered source.
         * @param thePacket packet to add
         * @retval true if successful
         */
        bool addAudioPacket(const AVPacket & thePacket);

        const char * getName() const { return "FFMpegDecoder2"; }
    private:
        AVFormatContext * _myFormatContext;

        int               _myVStreamIndex;
        AVStream *        _myVStream;
        int64_t           _myStartTimestamp;

        int               _myAStreamIndex;
        AVStream *        _myAStream;

        FrameCache        _myFrameCache;
        FrameCache        _myFrameRecycler;
        AVFrame *         _myFrame;
        int64_t           _mySeekTimestamp;
        int64_t           _myLastSeekTimestamp;

        int64_t           _myEOFVideoTimestamp;
        int64_t           _myNextPacketTimestamp;
        int64_t           _myTimePerFrame;
        double            _myLastAudioTimeStamp;
        unsigned          _myLineSizeBytes;
        int               _myDestinationPixelFormat;
        bool              _myCachingFlag;

        ReSampleContext *   _myResampleContext;

        static asl::Block     _mySamples;
        static asl::Block     _myResampledSamples;

        /**
         * Thread run method.
         */
        void run();

        int64_t getTimestampFromFrame(unsigned theFrame);
        unsigned getFrameFromTimestamp(int64_t theTimestamp);

        /// Returns timestamp.
        int64_t seekToFrame(unsigned theFrame);
        void seekToTimestamp(int64_t theTimestamp);

        void setupVideo(const std::string & theFilename);
        void setupAudio(const std::string & theFilename);
        /**
         *  updates the Framecache depending of the current position
         */
        bool updateCache();
        /**
         * Add theFrame to the framecache with the timestamp theTimestamp.
         * @warn this method blocks until the cache has room to take the
         *       given frame
         * @throws asl::ThreadSemaphore::ClosedException
         * @param theFrame frame to put into the cache
         * @param theTimestamp timestamp to use
         */
        void addCacheFrame(AVFrame* theFrame, int64_t theTimestamp);
        void createCache();
    };
    typedef asl::Ptr<FFMpegDecoder2> FFMpegDecoder2Ptr;
}

#endif
