//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_FFMpegDecoder_h_
#define _ac_y60_FFMpegDecoder_h_

#include "FrameConveyor.h"
#include <y60/video/MovieDecoderBase.h>
#include <asl/base/PlugInBase.h>
#include <string>

namespace y60 {

    const std::string MIME_TYPE_MPG = "application/mpg";

    DEFINE_EXCEPTION(FFMpegDecoderException, asl::Exception);

    class DecoderContext;

    class FFMpegDecoder : 
        public MovieDecoderBase,
        public asl::PlugInBase
    {
        public:
            FFMpegDecoder(asl::DLHandle theDLHandle);
            virtual ~FFMpegDecoder();

            /**
            * loads a movie from the file given by theFilename
            * @param theFilename file to load into the decoder
            */
            void load(const std::string & theFilename);
            void startMovie(double theStartTime);
            void resumeMovie(double theStartTime);
            void stopMovie();
            void pauseMovie();                
        
            virtual asl::Ptr<MovieDecoderBase> instance() const;
            std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>(0));

            double getMovieTime(double theSystemTime);

            /**
            * Reads a frame.
            * @param theTime time of the frame to deliver
            * @param theFrame is ignored by this decoder
            * @param theTargetRaster image raster to render the frame into
            * @return timestamp of the frame delivered in theTargetRaster
            */
            double readFrame(double theTime, unsigned theFrame, 
                    dom::ResizeableRasterPtr theTargetRaster);
            
            const char * getName() const { return "y60FFMpegDecoder"; }

        private:
            void setupMovie(asl::Ptr<DecoderContext> theContext);

            FrameConveyor _myFrameConveyor;
    };
}

#endif
