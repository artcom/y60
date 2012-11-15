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
//   $RCSfile: M60Decoder.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:04 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _ac_y60_M60Decoder_h_
#define _ac_y60_M60Decoder_h_

#include "y60_video_settings.h"

#include "MovieDecoderBase.h"
#include "MovieEncoding.h"
#include <asl/base/MappedBlock.h>

namespace y60 {

    DEFINE_EXCEPTION(M60DecoderException, asl::Exception);

    /**
     * @ingroup y60-video
     * Decoder for M60 Files.
     *
     */
    class M60Decoder : public MovieDecoderBase {
        public:
            M60Decoder();
            virtual ~M60Decoder();
            std::string canDecode(const std::string & theUrl, asl::Ptr<asl::ReadableStreamHandle> theStream = asl::Ptr<asl::ReadableStreamHandle>());

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

            /**
             * reads a frame.
             * @param theTime is ignored by this decoder
             * @param theFrame frame of the movie to deliver
             * @param theTargetRaster vector of image raster to render the frame into
             * @return theTime
             */
            double readFrame(double theTime, unsigned theFrame, RasterVector theTargetRaster);

            void stopMovie(bool theStopAudioFlag = true);
            void startMovie(double theStartTime = 0.0, bool theStartAudioFlag = true);

            const char* getName() const { return "M60Decoder"; }

            const bool hasAudio() const { return false; }

        private:
            void decodeFrame(unsigned theFrameNumber, dom::ResizeableRasterPtr theTargetRaster);
            MovieEncoding                   _myEncoding;
            unsigned                        _myLastDecodedFrame;
#ifdef USE_MAPPEDBLOCK_MOVIE
            asl::Ptr<asl::ConstMappedBlock> _myMovieBlock;
#else
            asl::Ptr<asl::ReadableStream> _myMovieBlock;
#endif
            asl::AC_SIZE_TYPE               _myFilePos;
            asl::AC_SIZE_TYPE               _myMovieHeaderSize;
    };
    typedef asl::Ptr<M60Decoder> M60DecoderPtr;
}
#endif
