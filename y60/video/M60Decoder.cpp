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

// own header
#include "M60Decoder.h"

#include <limits>

#include "Movie.h"
#include "MovieEncoding.h"
#include <asl/base/Ptr.h>
#include <asl/base/RunLengthEncoder.h>

using namespace asl;

///////////////////////////////////////////////////////////////////////////////
//
// M60Decoder File Format
//
// M60Header struct (see M60Header.h)
// asl::AC_SIZE_TYPE  - framesize (first frame)
// [...]          - framedata
// asl::AC_SIZE_TYPE  - framesize (second frame)
// [...]          - framedata
//
///////////////////////////////////////////////////////////////////////////////

#include "M60Header.h"
#include <asl/base/Dashboard.h>

#define DB(x) //x

namespace y60 {

    M60Decoder::M60Decoder() :
        _myLastDecodedFrame(std::numeric_limits<unsigned>::max()), _myFilePos(0), _myMovieHeaderSize(0)
    {}

    M60Decoder::~M60Decoder() {
    }

    std::string M60Decoder::canDecode(const std::string & theUrl,
                                        asl::Ptr<asl::ReadableStreamHandle> theStream)
    {
        if (theUrl.find(".m60") != std::string::npos) {
            return "video/x-m60";
        } else {
            return "";
        }
    }

    void
    M60Decoder::load(const std::string & theFilename) {
        _myMovieBlock = asl::Ptr<ReadableStream>(new ConstMappedBlock(theFilename));
        M60Header myHeader(*_myMovieBlock);

        if (!myHeader.checkMagicNumber()) {
            throw M60DecoderException(std::string("Movie ") + theFilename +
                " has a wrong magic number. '" + M60_MAGIC_NUMBER + "' expected.", PLUS_FILE_LINE);
        }

        if (!myHeader.checkHeadersize()) {
            throw M60DecoderException(std::string("Movie ") + theFilename +
                " has a wrong headersize number.", PLUS_FILE_LINE);
        }

        if (myHeader.version != CURRENT_MOVIE_FORMAT_VERSION) {
            throw M60DecoderException(std::string("M60Decoder ") + theFilename + " file format version: " +
                as_string(myHeader.version) + " does not match current reader version: " +
                as_string(CURRENT_MOVIE_FORMAT_VERSION), PLUS_FILE_LINE);
        }

        if (MovieEncoding(myHeader.compression) != MOVIE_DRLE &&
            MovieEncoding(myHeader.compression) != MOVIE_RLE)
        {
            throw M60DecoderException(std::string("M60Decoder ") + theFilename + " compression: " +
                as_string(myHeader.compression) + " not supported. Currently only RLE and DRLE supported.", PLUS_FILE_LINE);
        }
        _myEncoding = MovieEncoding(myHeader.compression);

        Movie * myMovie = getMovie();

        myMovie->createRaster(myHeader.framewidth, myHeader.frameheight, 1, PixelEncoding(myHeader.pixelformat));
        myMovie->getRasterPtr()->clear();
        myMovie->set<FrameRateTag>(myHeader.fps);
        myMovie->set<FrameCountTag>(myHeader.framecount);

        // Setup size and image matrix
        float myXResize = float(myHeader.width) / myHeader.framewidth;
        float myYResize = float(myHeader.height) / myHeader.frameheight;

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(Vector3f(myXResize, myYResize, 1.0f));
        myMovie->set<ImageMatrixTag>(myMatrix);

        _myMovieHeaderSize = myHeader.headersize;
        _myFilePos         = _myMovieHeaderSize;
    }

    void
    M60Decoder::stopMovie(bool theStopAudioFlag) {
        _myLastDecodedFrame = std::numeric_limits<unsigned>::max();
    }

    void
    M60Decoder::startMovie(double theStartTime, bool theStartAudioFlag) {
        _myLastDecodedFrame = std::numeric_limits<unsigned>::max();
        MovieDecoderBase::startMovie(theStartTime, theStartAudioFlag);

    }

    double
    M60Decoder::readFrame(double theTime, unsigned theFrame, RasterVector theTargetRaster) {
        DB(AC_DEBUG << "Read frame: " << theFrame << ", count: " << getFrameCount() << ", last decoded frame: " << _myLastDecodedFrame << ", theTime: " << theTime);

        if (static_cast<int>(theFrame) >= getFrameCount()) {
            setEOF(true);
            return theTime;
        }
        bool myReverseFlag = getMovie()->get<PlaySpeedTag>() < 0;

        // Handle frame wrap around
        if (_myLastDecodedFrame > theFrame) {
            if ( _myLastDecodedFrame != std::numeric_limits<unsigned>::max()  && !myReverseFlag) {
                setEOF(true);
                DB(AC_DEBUG << "Wraparound detected");
                return theTime;
            } else {
                _myFilePos = _myMovieHeaderSize;
                decodeFrame(0, theTargetRaster[0]);
            }
        }

        if (_myEncoding == MOVIE_DRLE) {
            // Sequential access
            while (_myLastDecodedFrame < theFrame) {
                decodeFrame(_myLastDecodedFrame + 1, theTargetRaster[0]);
            }
        } else {
            // Random access
            asl::AC_SIZE_TYPE myLastFrameRead = _myLastDecodedFrame;
            while ((myLastFrameRead + 1) < theFrame) {
                asl::AC_SIZE_TYPE theFrameSize = 0;
                _myFilePos = _myMovieBlock->readUnsigned32(theFrameSize, _myFilePos);
                _myFilePos += theFrameSize;
                myLastFrameRead++;
            }
            if (theFrame != _myLastDecodedFrame) {
                decodeFrame(theFrame, theTargetRaster[0]);
            }
        }
        return theTime;
    }

    void
    M60Decoder::decodeFrame(unsigned theFrameNumber, dom::ResizeableRasterPtr theTargetRaster) {
        DB(AC_TRACE << "M60Decoder::decodeFrame: " << theFrameNumber);
        asl::AC_SIZE_TYPE myFrameSize;
        _myFilePos = _myMovieBlock->readUnsigned32(myFrameSize, _myFilePos);
        DB(AC_TRACE << "    frame size: " << myFrameSize << " file position: " << _myFilePos);

#ifdef USE_MAPPEDBLOCK_MOVIE
        const unsigned char * myFrameData = _myMovieBlock->begin() + _myFilePos;
        asl::ReadableBlockAdapter myFrameBlock(myFrameData, myFrameData + myFrameSize);
#endif
        dom::ValuePtr myTargetRasterValue = dynamic_cast_Ptr<dom::ValueBase>(theTargetRaster);
        if (_myEncoding == MOVIE_DRLE && theFrameNumber) {
            // TODO: make faster using a dedicated RLE-decoder that adds delta during decoding
            dom::ValuePtr myOldRasterValue = dom::ValuePtr(myTargetRasterValue->clone(0));
#ifdef USE_MAPPEDBLOCK_MOVIE
            myTargetRasterValue->debinarize(myFrameBlock,0);
#else
            myTargetRasterValue->debinarize(*_myMovieBlock, _myFilePos);
#endif
            theTargetRaster->add(*myOldRasterValue);
        } else {
#ifdef USE_MAPPEDBLOCK_MOVIE
            myTargetRasterValue->debinarize(myFrameBlock, 0);
#else
            myTargetRasterValue->debinarize(*_myMovieBlock, _myFilePos);
#endif
        }
        _myFilePos += myFrameSize;
        _myLastDecodedFrame = theFrameNumber;
    }
}
