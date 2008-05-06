//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: QuicktimeDecoder.h,v $
//   $Author: valentin $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 10:12:21 $
//
//  Quicktime movie decoder.
//
//=============================================================================

#ifndef _ac_y60_QuicktimeDecoder_h_
#define _ac_y60_QuicktimeDecoder_h_

#include <asl/PlugInBase.h>
#include <y60/MovieDecoderBase.h>

#include <string>
#include <vector>

#ifdef WIN32
// Disable Quicktime Warning in Windows
#pragma warning(push)
#pragma warning(disable:4244)
#define EMULATE_INTTYPES
#endif

#include <qt/Movies.h>

#ifdef WIN32
// Reenable Warning in Windows
#pragma warning(pop)
#endif

namespace y60 {

    const std::string MIME_TYPE_MOV = "application/mov";

    DEFINE_EXCEPTION(QuicktimeDecoderException, asl::Exception);

    class QuicktimeDecoder : 
        public MovieDecoderBase,
        public asl::PlugInBase
    {
        public:
            QuicktimeDecoder(asl::DLHandle theDLHandle);
            virtual ~QuicktimeDecoder();
    
            bool hasVideo() const;
            bool hasAudio() const;
            void stopMovie();

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
    
            const char * getName() const { return "y60QuicktimeDecoder"; }
        private:
            void closeMovie();
            void decodeFrame(unsigned theFrameNumber, dom::ResizeableRasterPtr theTargetRaster);
            unsigned int getFramecount(::Movie theMovie);

            ::Movie                  _myMovie;    
            unsigned                 _myLastDecodedFrame;
            ::TimeValue              _myInternalMovieTime; 
            ::TimeValue              _myFrameTimeStep;             
            GWorldPtr                _myOffScreenWorld;

        };
    
        typedef asl::Ptr<QuicktimeDecoder> QuicktimeDecoderPtr;
}

#endif
