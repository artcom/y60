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
#include "AudioFrame.h"
#include "FrameConveyor.h"
#include "DecoderContext.h"
#include <y60/MovieDecoderBase.h>
#include <audio/BufferedSource.h>
#include <asl/PlugInBase.h>
#include <asl/Block.h>
#include <string>
#include <vector>

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
            void addAudioPacket();

            /// Decode frame at theTimestamp into theTargetRaster. Returns true if EOF was met.
            bool decodeFrame(long long & theTimestamp, dom::ResizeableRasterPtr theTargetRaster);

            /// Convert frame vom YUV to RGB
            void convertFrame(AVFrame * theFrame, dom::ResizeableRasterPtr theTargetRaster);

            /// Copy raster to raster
            void copyFrame(dom::ResizeableRasterPtr theVideoFrame, dom::ResizeableRasterPtr theTargetRaster);

            AVFrame *         _myVideoFrame;
            AudioBase::BufferedSource * _myAudioBufferedSource;
            AudioFrame        _myAudioFrame;

            DecoderContext    _myContext;
            FrameConveyor     _myFrameConveyor;
    };

    typedef asl::Ptr<FFMpegDecoder> FFMpegDecoderPtr;
}

#endif
