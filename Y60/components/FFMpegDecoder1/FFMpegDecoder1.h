//=============================================================================
// Copyright (C) 2004-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_FFMpegDecoder_h_
#define _ac_y60_FFMpegDecoder_h_

#include <asl/PlugInBase.h>
#include <y60/MovieDecoderBase.h>

#include <asl/Block.h>
#include <string>
#include <vector>

#ifdef WIN32
// Disable FFMPeg Warning in Windows
#pragma warning(push)
#pragma warning(disable:4244)
#define EMULATE_INTTYPES
#endif

#include <ffmpeg/avformat.h>

#ifdef WIN32
// Reenable Warning in Windows
#pragma warning(pop)
#endif

namespace y60 {

    const std::string MIME_TYPE_MPG = "application/mpg";

    DEFINE_EXCEPTION(FFMpegDecoderException, asl::Exception);

    class FFMpegDecoder1 : 
        public MovieDecoderBase,
        public asl::PlugInBase
    {

        // AVFrame with timestamp
        class VideoFrame {
        public:
            VideoFrame(unsigned theBufferSize);
            ~VideoFrame();

            inline void setTimestamp(int64_t theTimestamp) {
                _myTimestamp = theTimestamp;
            }

            inline int64_t getTimestamp() const {
                return _myTimestamp;
            }
            inline unsigned char* getBuffer() {
                return _myBuffer;
            }

        private:
            int64_t _myTimestamp;
            unsigned char* _myBuffer;
        };
        typedef asl::Ptr<VideoFrame> VideoFramePtr;

    public:
        FFMpegDecoder1(asl::DLHandle theDLHandle);
        virtual ~FFMpegDecoder1();

        /**
         * loads a movie from the file given by theFilename
         * @param theFilename file to load into the decoder
         */
        void load(const std::string & theFilename);

        /**
         * loads a movie from the stream given by theSource
         * @param theFilename file to identify theSource
         */
        void load(asl::Ptr<asl::ReadableStream> theSource, const std::string & theFilename) {
            throw asl::NotYetImplemented(JUST_FILE_LINE);
        }
    
        virtual asl::Ptr<MovieDecoderBase> instance() const;
        std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0);

        /**
         * Reads a frame.
         * @param theTime time of the frame to deliver
         * @param theFrame is ignored by this decoder
         * @param theTargetRaster image raster to render the frame into
         * @return timestamp of the frame delivered in theTargetRaster
         */
        double readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster);

        const char * getName() const { return "y60FFMpegDecoder1"; }

    private:
        void closeMovie();

        /// Decode frame at theTimestamp into theTargetRaster. Returns true if EOF was met.
        bool decodeFrame(int64_t & theTimestamp, dom::ResizeableRasterPtr theTargetRaster);
    
        // Convert frame from yuv to rgb
        void convertFrame(AVFrame* theFrame, dom::ResizeableRasterPtr theTargetRaster);


        AVFormatContext * _myFormatContext;
        AVFrame *         _myFrame;

        int               _myVStreamIndex;
        AVStream *        _myVStream;

        // All Timestamp variables are in movie time units (We've seen 1/90000 and 1/25 second
        // time units in movies).
        int64_t           _myStartTimestamp;
        int64_t           _myLastVideoTimestamp;
        int64_t           _myEOFVideoTimestamp;
    };

    typedef asl::Ptr<FFMpegDecoder1> FFMpegDecoder1Ptr;
}

#endif
