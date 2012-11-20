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

#ifndef _ac_y60_FFMpegDecoder_h_
#define _ac_y60_FFMpegDecoder_h_

#include "y60_ffmpegdecoder1_settings.h"

#include <asl/base/PlugInBase.h>
#include <asl/base/Block.h>

#include <y60/video/MovieDecoderBase.h>

#include <string>
#include <vector>

#ifdef OSX
    extern "C" {
#       include <libavformat/avformat.h>
#       if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(51,38,0)
#           include <libswscale/swscale.h>
#       endif
    }
#   undef AV_NOPTS_VALUE
#   define AV_NOPTS_VALUE 0x8000000000000000LL
#else
#   if defined(_MSC_VER)
#       pragma warning(push,1)
#   endif
    extern "C" {
#   include <avformat.h>
#   if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(51,38,0)
#       include <swscale.h>
#   endif
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
#endif

#ifndef AV_VERSION_INT
#define AV_VERSION_INT(a,b,c) (a<<16 | b<<8 | c)
#endif

namespace y60 {

    const std::string MIME_TYPE_MPG = "application/mpg";

    DEFINE_EXCEPTION(FFMpegDecoderException, asl::Exception);

    class Y60_FFMPEGDECODER1_DECL FFMpegDecoder1 :
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
        void shutdown();

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
        std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>());

        /**
         * Reads a frame.
         * @param theTime time of the frame to deliver
         * @param theFrame is ignored by this decoder
         * @param theTargetRaster vector of image raster to render the frame into
         * @return timestamp of the frame delivered in theTargetRaster
         */
        double readFrame(double theTime, unsigned theFrame, RasterVector theTargetRaster);

        const char * getName() const { return "FFMpegDecoder1"; }

        const bool hasAudio() const { AC_WARNING << "hasAudio always returns false for FFMpegDecoder1"; return false; }

    private:
        void setMovieParameters(int myIndex);
        void closeMovie();

        /// Decode frame at theTimestamp into theTargetRaster. Returns true if EOF was met.
        bool decodeFrame(double & theTimestamp, dom::ResizeableRasterPtr theTargetRaster);

        // Convert frame from yuv to rgb
        void convertFrame(AVFrame* theFrame, dom::ResizeableRasterPtr theTargetRaster);
    private:
        AVFormatContext * _myFormatContext;
        AVFrame *         _myFrame;

        // int               _myVStreamIndex;
        AVStream *        _myVStream;
        int               _myLastVStreamIndex;

        // All Timestamp variables are in movie time units (We've seen 1/90000 and 1/25 second
        // time units in movies).
        int64_t           _myStartTimestamp;
        double           _myLastVideoTimestamp;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52,20,0)
        PixelFormat
#else
        int
#endif
        _myDestinationPixelFormat;

        unsigned          _myBytesPerPixel;
        bool _hasShutDown;
        bool _myFirstFrameDecodedFlag;
    };

    typedef asl::Ptr<FFMpegDecoder1> FFMpegDecoder1Ptr;
}

#endif
