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
*/

#ifndef _ac_y60_FFMpegDecoder2_h_
#define _ac_y60_FFMpegDecoder2_h_

#include "y60_ffmpegdecoder2_settings.h"

#include "Demux.h"

#include <y60/video/VideoMsgQueue.h>
#include <y60/video/AsyncDecoder.h>
#include <y60/video/MovieEncoding.h>

#include <asl/base/PlugInBase.h>
#include <asl/base/PosixThread.h>
#include <asl/base/ThreadSemaphore.h>
#include <asl/base/Stream.h>
#include <asl/base/Ptr.h>

#include <string>
#include <list>

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
extern "C" {
#    include <libavformat/avformat.h>
#    include <libswscale/swscale.h>
}
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#ifndef AV_VERSION_INT
#define AV_VERSION_INT(a,b,c) (a<<16 | b<<8 | c)
#endif

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
        static const double AUDIO_BUFFER_SIZE;   ///< Audio cache size in seconds.

    public:
        FFMpegDecoder2(asl::DLHandle theDLHandle);
        virtual ~FFMpegDecoder2();

        virtual asl::Ptr<MovieDecoderBase> instance() const;
        std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>());

        /**
         * loads a movie from the file given by theFilename
         * @param theFilename file to load into the decoder
         */
        void load(const std::string & theFilename);
        double readFrame(double theTime, unsigned, RasterVector theTargetRaster);

        /**
         * Starts movie decoding
         * @param theStartTime movie-time to start decoding at.
         */
        void startMovie(double theStartTime = 0.0f, bool theStartAudioFlag = true);

        void resumeMovie(double theStartTime = 0.0f, bool theResumeAudioFlag = true);

        /**
         * Called to stop the decoding.
         */
        void stopMovie(bool theStopAudioFlag = true);
        void closeMovie();

        const char * getName() const { return "FFMpegDecoder2"; }

        void shutdown();

    private:
        // Called from main thread
        void run();

        void startOverAgain();
        void openStreams(const std::string & theFilename);
        void setupVideo(const std::string & theFilename);
        void setupAudio(const std::string & theFilename);
        void getVideoProperties(const std::string & theFilename);
        void dumpCache();
        bool shouldSeek(double theCurrentTime, double theDestTime);
        void seek(double theDestTime);
        void doSeek(double theSeekTime, bool theSeekAudioFlag = true);


        // Called from both threads
        /**
         *  updates the Framecache depending on the current position
         */
        bool decodeFrame();
        bool readAudio();
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
        void checkAudioStream();

        template<typename T>
        int downmix5p1ToStereo(T * theBuffer, int theBytesDecoded) {
            T * myStereoPtr = theBuffer;
            T * my5p1Ptr = theBuffer;
            for (int i = 0; i < theBytesDecoded/3; ++i) {
                *myStereoPtr++ = *my5p1Ptr;
                *myStereoPtr++ = *(my5p1Ptr+2);
                my5p1Ptr += 6;
            }
            return theBytesDecoded/3;
        }
        
    
        // Used in main thread
        VideoMsgPtr _myLastVideoFrame;

        AVFormatContext * _myFormatContext;
        int _myVStreamIndex;
        AVStream * _myVStream;

        unsigned int _myAStreamIndexDom; // this index counts from 0, comes vom y60-dom
        std::vector<int> _myAllAudioStreamIndicies;
        int _myAStreamIndex;    // this index points in ffmpeg stream ordering
        AVStream * _myAStream;

        VideoMsgQueue _myMsgQueue;
        AVFrame * _myFrame;

        DemuxPtr _myDemux;

        int64_t _myVideoStartTimestamp;
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,20,0)
        PixelFormat
#else
        int
#endif
        _myDestinationPixelFormat;

        ReSampleContext * _myResampleContext;
        static asl::Block _myResampledSamples;

        int _myNumFramesDecoded;
        int _myNumIFramesDecoded;

        double _myVideoStreamTimeBase;

        // worker thread values to prevent dom access and thus race conditions.
        double _myFrameRate;
        unsigned _myMaxCacheSize;
        int _myFrameWidth;
        int _myFrameHeight;
        unsigned  _myBytesPerPixel;
        double _myLastFrameTime;  // Only used for mpeg1/2 end of file handling.
        bool _myAdjustAudioOffsetFlag;
        bool _hasShutDown;

    };
    typedef asl::Ptr<FFMpegDecoder2> FFMpegDecoder2Ptr;
}

#endif
