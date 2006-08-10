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
#include "VideoMsgQueue.h"
#include "Demux.h"

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
        public asl::PosixThread,
        public asl::PlugInBase
    {
        static const unsigned FRAME_CACHE_SIZE;  ///< Number of frames to cache in the Framecache.
        static const double AUDIO_BUFFER_SIZE;   ///< Audio cache size in seconds.
        
    public:
        FFMpegDecoder2(asl::DLHandle theDLHandle);
        virtual ~FFMpegDecoder2();

        virtual asl::Ptr<MovieDecoderBase> instance() const;
        std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0);

        /**
         * loads a movie from the file given by theFilename
         * @param theFilename file to load into the decoder
         */
        void load(const std::string & theFilename);
        double readFrame(double theTime, unsigned, dom::ResizeableRasterPtr theTargetRaster);

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
        
        const char * getName() const { return "y60FFMpegDecoder2"; }
        
    private:
        // Called from main thread
        void run();
        
        void setupVideo(const std::string & theFilename);
        void setupAudio(const std::string & theFilename);
        void dumpCache();
        bool shouldSeek(double theCurrentTime, double theDestTime);
        void seek(double theDestTime);
       
        
        // Called from both threads
        /**
         *  updates the Framecache depending on the current position
         */
        bool decodeFrame();
        void readAudio();
        /**
         * Add an audio packet to the buffered source.
         * @param thePacket packet to add
         * @retval true if successful
         */
        void addAudioPacket(const AVPacket & thePacket);
        /**
         * Add theFrame to the framecache with the timestamp theTimestamp.
         * @throws asl::ThreadSemaphore::ClosedException
         * @param theFrame frame to put into the cache
         * @param theTimestamp timestamp to use
         */
        void addCacheFrame(AVFrame* theFrame, double theTime);
        void convertFrame(AVFrame* theFrame, unsigned char* theBuffer);
        VideoMsgPtr createFrame(double theTimestamp);
        
        // Used in main thread
        VideoMsgPtr _myLastVideoFrame;
        
        // Used in both threads
        AVFormatContext * _myFormatContext;
        int _myVStreamIndex;
        AVStream * _myVStream;

        int _myAStreamIndex;
        AVStream * _myAStream;

        VideoMsgQueue _myMsgQueue;
        AVFrame * _myFrame;

        DemuxPtr _myDemux;
       
        int64_t _myStartTimestamp;
        double _myLastFrameTime;  // Only used for mpeg1/2 end of file handling.
        int _myDestinationPixelFormat;

        ReSampleContext * _myResampleContext;
        static asl::Block _mySamples;
        static asl::Block _myResampledSamples;

        int _myNumFramesDecoded;
        int _myNumIFramesDecoded;
        
        //XXX: Since time_base is specified per stream by ffmpeg, we should really be 
        //     calculating this per stream and not per file.
        int64_t _myTimeUnitsPerSecond;
       
        // worker thread values to prevent dom access and thus race conditions.
        double _myFrameRate;
        int _myFrameWidth;
        int _myFrameHeight;
        unsigned  _myBytesPerPixel;
    };
    typedef asl::Ptr<FFMpegDecoder2> FFMpegDecoder2Ptr;
}

#endif
