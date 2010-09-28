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

// own header
#include "MovieDecoderBase.h"

#include "Movie.h"

#include <iostream>

using namespace std;

namespace y60 {

    void copyPlaneToRaster(unsigned char * theDestination, unsigned char * theSource, int theStride, unsigned theWidth, unsigned theHeight)
    {
        unsigned char * mySource = theSource;
        unsigned char * myDestination = theDestination;
        for (unsigned y=0; y<theHeight; y++) {
            memcpy(myDestination, mySource, theWidth);
            mySource      += theStride;
            myDestination += theWidth;
        }

    }

    MovieDecoderBase::MovieDecoderBase() :
        _myMovieTime(0.0), _myEndOfFileFlag(false), _myLastSystemTime(-1.0), _myDecodeAudioFlag(true)
    {}

    MovieDecoderBase::~MovieDecoderBase() {}

    void MovieDecoderBase::initialize(Movie * theMovie) {
        _myMovie = theMovie;
    }
#if 0
    std::string MovieDecoderBase::canDecode(const std::string & theUrl,
            asl::Ptr<asl::ReadableStreamHandle> theStream)
    {
        AC_WARNING << "MovieDecoderBase::canDecode: Using deprecated interface";
        return canDecode(theUrl, theStream::getStream());
    }
#endif
    double MovieDecoderBase::getMovieTime(double theSystemTime) {
        AC_TRACE << "getMovieTime start: _myMovieTime: " << _myMovieTime
                << ", theSystemTime: " << theSystemTime << ", _myLastSystemTime: "
                << _myLastSystemTime;
        if (theSystemTime <= _myLastSystemTime) {
            return _myMovieTime;
        }
        float myPlaySpeed = _myMovie->get<PlaySpeedTag>();
        if (_myLastSystemTime >= 0) {
            _myMovieTime += (theSystemTime - _myLastSystemTime) * myPlaySpeed;
            // if difference between last system time and current system time is bigger than movie duration
            if (getFrameCount() != -1 && _myMovieTime * getFrameRate() > getFrameCount()) {
                _myMovieTime = getFrameCount() / getFrameRate();
            }
            // Calculate the wraparaound for reverse playback
            if (_myMovieTime < 0) {
                unsigned myFrameCount = getFrameCount();
                if (myFrameCount == static_cast<unsigned>(-1) || myFrameCount == 0) {
                    AC_WARNING << "Movie cannot play backwards, because its framecount is unknown (framecount="
                               << myFrameCount << ")";
                    _myMovieTime = 0;
                } else {
                    double myIncrement = getFrameCount() / getFrameRate();
                    while (_myMovieTime < 0) {
                        _myMovieTime += myIncrement;
                    }
                }
            }
        }
        _myLastSystemTime = theSystemTime;
        AC_TRACE << "getMovieTime end: _myMovieTime: " << _myMovieTime
                << ", theSystemTime: " << theSystemTime << ", _myLastSystemTime: "
                << _myLastSystemTime;
        return _myMovieTime;
    }

    void MovieDecoderBase::load(asl::Ptr<asl::ReadableStream> theSource,
            const std::string & theFilename)
    {
        throw asl::NotYetImplemented(JUST_FILE_LINE);
    }
    void MovieDecoderBase::setDecodeAudioFlag(bool theDecodeAudioFlag) {
        _myDecodeAudioFlag = theDecodeAudioFlag;
    }

    bool MovieDecoderBase::getDecodeAudioFlag() const {
        return _myDecodeAudioFlag;
    }

    int MovieDecoderBase::getFrameCount() const {
        return _myMovie->get<FrameCountTag>();
    }

    unsigned MovieDecoderBase::getFrameWidth() const {
        return _myMovie->get<ImageWidthTag>();
    }

    unsigned MovieDecoderBase::getFrameHeight() const {
        return _myMovie->get<ImageHeightTag>();
    }

    double MovieDecoderBase::getFrameRate() const {
        return _myMovie->get<FrameRateTag>();
    }
    bool MovieDecoderBase::getAudioFlag() const {
        return _myMovie->get<AudioTag>();
    }

    const asl::Matrix4f & MovieDecoderBase::getImageMatrix() const {
        return _myMovie->get<ImageMatrixTag>();
    }

    MoviePlayMode MovieDecoderBase::getPlayMode() const {
        return _myMovie->getPlayMode();
    }

    bool MovieDecoderBase::getEOF() const {
        return _myEndOfFileFlag;
    }

    void MovieDecoderBase::setEOF(bool theEndOfFileFlag) {
        _myEndOfFileFlag = theEndOfFileFlag;
        if (_myEndOfFileFlag && _myMovie->get<FrameCountTag>() == -1) {
            if (_myMovie->get<CurrentFrameTag>() == 0) {
                throw asl::Exception("no decodable video stream found. (are width and height both multiples of two?) ", PLUS_FILE_LINE);
            }
            AC_DEBUG << "MovieDecoderBase::setEOF setting FrameCountTag: " <<_myMovie->get<CurrentFrameTag>();
            _myMovie->set<FrameCountTag>(_myMovie->get<CurrentFrameTag>());
        }
    }

    void MovieDecoderBase::startMovie(double theStartTime, bool theStartAudioFlag) {
        AC_DEBUG << "MovieDecoderBase::startMovie -> " << theStartTime;
        _myMovieTime      = theStartTime;
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::resumeMovie(double theStartTime, bool theResumeAudioFlag) {
        AC_DEBUG << "MovieDecoderBase::resumeMovie -> " << theStartTime;
        _myMovieTime      = theStartTime;
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::stopMovie(bool theStopAudioFlag) {
        AC_DEBUG << "MovieDecoderBase::stopMovie";
        _myMovieTime = 0.0;
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::pauseMovie(bool thePauseAudioFlag) {
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::closeMovie() {}

    asl::Ptr<MovieDecoderBase> MovieDecoderBase::instance() const {
        return asl::Ptr<MovieDecoderBase>();
    }

    const Movie * MovieDecoderBase::getMovie() const {
        return _myMovie;
    }
    Movie * MovieDecoderBase::getMovie() {
        return _myMovie;
    }

}
