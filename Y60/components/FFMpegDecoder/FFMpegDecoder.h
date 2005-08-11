//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: FFMpegDecoder.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  ffmpeg movie decoder.
//
//=============================================================================

#ifndef _ac_y60_FFMpegDecoder_h_
#define _ac_y60_FFMpegDecoder_h_

#include "VideoFrame.h"
#include <y60/MovieDecoderBase.h>
#include <audio/BufferedSource.h>
#include <asl/PlugInBase.h>
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

    class FFMpegDecoder : 
        public MovieDecoderBase,
        public asl::PlugInBase
    {
        public:
            FFMpegDecoder(asl::DLHandle theDLHandle);
            virtual ~FFMpegDecoder();

            bool hasVideo() const;
            bool hasAudio() const;
            void setupVideo(const std::string & theFilename);
            void setupAudio(const std::string & theFilename);

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
                throw NotYetImplemented(JUST_FILE_LINE);
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

        private:
            void initBufferedSource(unsigned theNumChannels, unsigned theSampleRate, unsigned theSampleBits);
            void addAudioPacket(const AVPacket & thePacket);

            /// Decode frame at theTimestamp into theTargetRaster. Returns true if EOF was met.
            bool decodeFrame(int64_t & theTimestamp, dom::ResizeableRasterPtr theTargetRaster);
            void convertFrame(AVFrame* theFrame, unsigned char* theBuffer);
            void copyFrame(VideoFramePtr theVideoFrame, dom::ResizeableRasterPtr theTargetRaster);

            AVFormatContext * _myFormatContext;           
            AVFrame *         _myFrame;

            VideoFramePtr     _myVideoFrame;        
            int               _myVStreamIndex;
            AVStream *        _myVStream;

            AudioBase::BufferedSource * _myAudioBufferedSource;
            unsigned char *   _myAudioSamples;
            int               _myAStreamIndex;
            AVStream *        _myAStream;
    };

    typedef asl::Ptr<FFMpegDecoder> FFMpegDecoderPtr;
}

#endif
