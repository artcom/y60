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

#ifndef _ac_y60_FFMpegDecoder_h_
#define _ac_y60_FFMpegDecoder_h_

#include "y60_ffmpegdecoder1_settings.h"

#include <asl/base/PlugInBase.h>
#include <y60/video/MovieDecoderBase.h>

#include <asl/base/Block.h>
#include <string>
#include <vector>

#ifdef OSX
    extern "C" {
#       include <libavformat/avformat.h>
#       if LIBAVCODEC_VERSION_INT >= ((51<<16)+(38<<8)+0) 
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
#ifndef AC_BUILT_WITH_CMAKE
#       include <ffmpeg/avformat.h>
#else
#       include <avformat.h>
#endif
#       if LIBAVCODEC_VERSION_INT >= ((51<<16)+(38<<8)+0) 
#ifdef AC_BUILT_WITH_CMAKE
#       include <swscale.h>
#endif
#       endif
    }
#   if defined(_MSC_VER)
#       pragma warning(pop)
#   endif
#endif

namespace y60 {

    const std::string MIME_TYPE_MPG = "application/mpg";

    DEFINE_EXCEPTION(FFMpegDecoderException, asl::Exception);

    class Y60_FFMPEGDECODER1_EXPORT FFMpegDecoder1 : 
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
        std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>(0));

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
        void setMovieParameters(int myIndex);
        void closeMovie();

        /// Decode frame at theTimestamp into theTargetRaster. Returns true if EOF was met.
        bool decodeFrame(double & theTimestamp, dom::ResizeableRasterPtr theTargetRaster);
    
        // Convert frame from yuv to rgb
        void convertFrame(AVFrame* theFrame, dom::ResizeableRasterPtr theTargetRaster);


        AVFormatContext * _myFormatContext;
        AVFrame *         _myFrame;

        // int               _myVStreamIndex;
        AVStream *        _myVStream;
        int               _myLastVStreamIndex;

        // All Timestamp variables are in movie time units (We've seen 1/90000 and 1/25 second
        // time units in movies).
        int64_t           _myStartTimestamp;
        double           _myLastVideoTimestamp;
        int               _myDestinationPixelFormat;
        unsigned          _myBytesPerPixel;

    };

    typedef asl::Ptr<FFMpegDecoder1> FFMpegDecoder1Ptr;
}

#endif
