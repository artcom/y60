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

#include "MovieDecoderBase.h"
#include "MovieEncoding.h"
#include <asl/MappedBlock.h>

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
             * @param theTargetRaster image raster to render the frame into
             * @return theTime
             */
            double readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster);

            void stopMovie();
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
