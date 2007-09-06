//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "MovieDecoderBase.h"

#include "Movie.h"

#include <iostream>

using namespace std;

namespace y60 {
    MovieDecoderBase::MovieDecoderBase() :
        _myEndOfFileFlag(false), _myMovieTime(0.0), _myLastSystemTime(-1.0)
    {}

    MovieDecoderBase::~MovieDecoderBase() {}

    void MovieDecoderBase::initialize(Movie * theMovie) {
        _myMovie = theMovie;
    }

    std::string MovieDecoderBase::canDecode(const std::string & theUrl, 
            asl::ReadableStream * theStream) 
    {
        return "";
    }

    double MovieDecoderBase::getMovieTime(double theSystemTime) {
        AC_DEBUG << "getMovieTime start: _myMovieTime: " << _myMovieTime 
                << ", theSystemTime: " << theSystemTime << ", _myLastSystemTime: " 
                << _myLastSystemTime;
        if (theSystemTime <= _myLastSystemTime) {
            return _myMovieTime;
        }
        float myPlaySpeed = _myMovie->get<PlaySpeedTag>();
        if (_myLastSystemTime >= 0) {
            _myMovieTime += (theSystemTime - _myLastSystemTime) * myPlaySpeed;
            
            // Calculate the wraparaound for reverse playback
            if (_myMovieTime < 0) {
                unsigned myFrameCount = getFrameCount();
                if (myFrameCount == -1 || myFrameCount == 0) {
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
//        if(_myLastSystemTime == -1){
//            _myMovieTime = 0;
//        }
        _myLastSystemTime = theSystemTime;
        AC_DEBUG << "getMovieTime end: _myMovieTime: " << _myMovieTime 
                << ", theSystemTime: " << theSystemTime << ", _myLastSystemTime: " 
                << _myLastSystemTime;
        return _myMovieTime;
    }

    void MovieDecoderBase::load(asl::Ptr<asl::ReadableStream> theSource, 
            const std::string & theFilename)  
    {
        throw asl::NotYetImplemented(JUST_FILE_LINE);
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
            AC_DEBUG << "!!!!!!MovieDecoderBase::setEOF setting FrameCountTag: " <<_myMovie->get<CurrentFrameTag>();
            _myMovie->set<FrameCountTag>(_myMovie->get<CurrentFrameTag>());            
        }
    }

    void MovieDecoderBase::startMovie(double theStartTime) {
        AC_DEBUG << "MovieDecoderBase::startMovie -> " << theStartTime;
        _myMovieTime      = theStartTime;
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::resumeMovie(double theStartTime) {
        AC_DEBUG << "MovieDecoderBase::resumeMovie -> " << theStartTime;
        _myMovieTime      = theStartTime;
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::stopMovie() {
        AC_DEBUG << "MovieDecoderBase::stopMovie";
        _myMovieTime = 0.0; 
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::pauseMovie() {
        _myLastSystemTime = -1.0;
    }

    void MovieDecoderBase::closeMovie() {}

    asl::Ptr<MovieDecoderBase> MovieDecoderBase::instance() const {
        return asl::Ptr<MovieDecoderBase>(0);
    }

    const Movie * MovieDecoderBase::getMovie() const {
        return _myMovie;
    }
    Movie * MovieDecoderBase::getMovie() {
        return _myMovie;
    }

}
