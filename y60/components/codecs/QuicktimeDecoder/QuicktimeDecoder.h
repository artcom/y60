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

#include <asl/base/PlugInBase.h>
#include <y60/video/MovieDecoderBase.h>

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
             * @param theTargetRaster vector of image raster to render the frame into
             * @return timestamp of the frame delivered in theTargetRaster
             */
            double readFrame(double theTime, unsigned theFrame, RasterVector theTargetRaster);

            const char * getName() const { return "QuicktimeDecoder"; }

            const bool hasAudio() const { AC_WARNING << "hasAudio always returns false for QuicktimeDecoder"; return false; }

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
