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
//   $RCSfile: M60Decoder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:04 $
//
//  Description: A simple scene class.
//
//=============================================================================
#include "M60Decoder.h"
#include "MovieEncoding.h"
#include <asl/RunLengthEncoder.h>

using namespace std;
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
#include <asl/Dashboard.h>

#define DB(x) //x

namespace y60 {

    M60Decoder::M60Decoder() :
        _myLastDecodedFrame(UINT_MAX), _myFilePos(0), _myMovieHeaderSize(0)
    {}

    M60Decoder::~M60Decoder() {
    }

    void
    M60Decoder::load(const std::string & theFilename) {
        _myMovieBlock = Ptr<ReadableStream>(new ConstMappedBlock(theFilename));
        M60Header myHeader(*_myMovieBlock);

        if (!myHeader.checkMagicNumber()) {
            throw M60DecoderException(string("Movie ") + theFilename +
                " has a wrong magic number. '" + M60_MAGIC_NUMBER + "' expected.", PLUS_FILE_LINE);
        }

        if (!myHeader.checkHeadersize()) {
            throw M60DecoderException(string("Movie ") + theFilename +
                " has a wrong headersize number.", PLUS_FILE_LINE);
        }

        if (myHeader.version != CURRENT_MOVIE_FORMAT_VERSION) {
            throw M60DecoderException(string("M60Decoder ") + theFilename + " file format version: " +
                as_string(myHeader.version) + " does not match current reader version: " +
                as_string(CURRENT_MOVIE_FORMAT_VERSION), PLUS_FILE_LINE);
        }

        if (MovieEncoding(myHeader.compression) != MOVIE_DRLE &&
            MovieEncoding(myHeader.compression) != MOVIE_RLE)
        {
            throw M60DecoderException(string("M60Decoder ") + theFilename + " compression: " +
                as_string(myHeader.compression) + " not supported. Currently only RLE and DRLE supported.", PLUS_FILE_LINE);
        }
        _myEncoding = MovieEncoding(myHeader.compression);

        setFrameCount(myHeader.framecount);
        setFrameWidth(myHeader.framewidth);
        setFrameHeight(myHeader.frameheight);
        setPixelFormat(PixelEncoding(myHeader.pixelformat));
        setFrameRate(myHeader.fps);

        // Setup size and image matrix
        float myXResize = float(myHeader.width) / myHeader.framewidth;
        float myYResize = float(myHeader.height) / myHeader.frameheight;

        asl::Matrix4f myMatrix;
        myMatrix.makeScaling(Vector3f(myXResize, myYResize, 1.0f));
        setImageMatrix(myMatrix);

        _myMovieHeaderSize = myHeader.headersize;
        _myFilePos         = _myMovieHeaderSize;
    }

    void
    M60Decoder::stopMovie() {
        _myLastDecodedFrame = UINT_MAX;
    }

    double
    M60Decoder::readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) {
        DB(cerr << "Read frame: " << theFrame << ", last decoded frame: " << _myLastDecodedFrame << endl);

        if (theFrame >= getFrameCount()) {
            setEOF(true);
            return theTime;
        }

        // Handle frame wrap around
        if (_myLastDecodedFrame > theFrame) {
            _myFilePos = _myMovieHeaderSize;
            decodeFrame(0, theTargetRaster);
        }

        if (_myEncoding == MOVIE_DRLE) {
            // Sequential access
            while (_myLastDecodedFrame < theFrame) {
                decodeFrame(_myLastDecodedFrame + 1, theTargetRaster);
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
                decodeFrame(theFrame, theTargetRaster);
            }
        }
        return theTime;
    }

    void
    M60Decoder::decodeFrame(unsigned theFrameNumber, dom::ResizeableRasterPtr theTargetRaster) {
        DB(cerr << "M60Decoder::decodeFrame: " << theFrameNumber << endl);
        asl::AC_SIZE_TYPE myFrameSize;
        _myFilePos = _myMovieBlock->readUnsigned32(myFrameSize, _myFilePos);
        DB(cerr << "    frame size: " << myFrameSize << " file position: " << _myFilePos << endl);

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
