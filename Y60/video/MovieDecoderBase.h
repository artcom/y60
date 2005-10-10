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
//   $RCSfile: MovieDecoderBase.h,v $
//   $Author: valentin $
//   $Revision: 1.6 $
//   $Date: 2005/04/19 12:38:29 $
//
//
//=============================================================================

#ifndef _ac_y60_MovieDecoderBase_h_
#define _ac_y60_MovieDecoderBase_h_

#include "Movie.h"
#include <y60/PixelEncoding.h>
#include <y60/NodeValueNames.h>
#include <y60/IDecoder.h>

#include <asl/Ptr.h>
#include <asl/Matrix4.h>
#include <asl/Stream.h>
#include <string>
#include <asl/Exception.h>

namespace asl {
    class ReadableBlock;
}

namespace y60 {
    DEFINE_EXCEPTION(NotYetImplemented,asl::Exception);
    /**
     * @ingroup Y60video
     * Abstract base class for Movie Decoders.
     *
     */
    class MovieDecoderBase : public IDecoder {
        public:
            MovieDecoderBase() :
                _myEndOfFileFlag(false), _myMovieTime(0.0), _myLastSystemTime(-1.0)
            {}

            virtual ~MovieDecoderBase() {}

            void initialize(Movie * theMovie) {
                _myMovie = theMovie;
            }

            virtual std::string canDecode(const std::string & theUrl, asl::ReadableStream * theStream = 0) {
                return "";
            }

            /**
             * @return the time that audio is behind video in seconds
             */
            virtual double getAudioVideoDelay() const {
                return 0.0;
            }

            /**
             *
             * @param theSystemTime used for calculation of delta t in free running mode.
             *                      Should be increasing constantly over time. May be ignored
             *                      if the audioclock or the framecache are used.
             * @return current movie time
             */
            virtual double getMovieTime(double theSystemTime) {
                if (theSystemTime == _myLastSystemTime) {
                    return _myMovieTime;
                }
                float myPlaySpeed = _myMovie->get<PlaySpeedTag>();
                if (_myLastSystemTime >= 0) {
                    _myMovieTime += (theSystemTime - _myLastSystemTime) * myPlaySpeed;
                    double myIncrement = getFrameCount() / getFrameRate();
                    while (_myMovieTime < 0) {
                        _myMovieTime += myIncrement;
                    }
                }
                _myLastSystemTime = theSystemTime;
                return _myMovieTime;
            }

            /**
             * Reads a frame from the decoder.
             * @param theTime time of the frame to fetch
             * @param theFrame index of the frame to fetch
             * @param theTargetRaster raster to read the frame into
             * @return real time of the frame return in theTargetRaster
             */
            virtual double readFrame(double theTime, unsigned theFrame, dom::ResizeableRasterPtr theTargetRaster) = 0;

            /**
             * loads a movie from the file given by theFilename
             * @param theFilename file to load into the decoder
             */
            virtual void load(const std::string & theFilename) = 0;

            /**
             * loads a movie from the stream given by theSource
             * @param theFilename file to identify theSource
             */
            virtual void load(asl::Ptr<asl::ReadableStream> theSource, const std::string & theFilename)  {
                throw NotYetImplemented(JUST_FILE_LINE);
            }

            PixelEncoding getPixelFormat() const {
                return PixelEncoding(asl::getEnumFromString(_myMovie->get<ImagePixelFormatTag>(), PixelEncodingString));
            }
            unsigned getFrameCount() const {
                return _myMovie->get<FrameCountTag>();
            }
            unsigned getFrameWidth() const {
                return _myMovie->get<ImageWidthTag>();
            }
            unsigned getFrameHeight() const {
                return _myMovie->get<ImageHeightTag>();
            }
            double getFrameRate() const {
                return _myMovie->get<FrameRateTag>();
            }
            bool getAudioFlag() const {
                return _myMovie->get<AudioTag>();
            }
            const asl::Matrix4f & getImageMatrix() const {
                return _myMovie->get<ImageMatrixTag>();
            }
            MoviePlayMode getPlayMode() const {
                return _myMovie->getPlayMode();
            }

            bool getEOF() const {
                return _myEndOfFileFlag;
            }
            void setEOF(bool theEndOfFileFlag) {
                _myEndOfFileFlag = theEndOfFileFlag;
            }

            /**
             * Starts the movie playback at theStartTime.
             * @param theStartTime start time of the movie. Defaults to 0.0
             */
            virtual void startMovie(double theStartTime = 0.0) {
                _myMovieTime      = theStartTime;
                _myLastSystemTime = -1.0;
            }

            /**
             * Stops the movie playback. Future calls of startMovie will start from the beginning
             * of the movie.
             */
            virtual void stopMovie() {
                _myMovieTime = 0.0; 
                _myLastSystemTime = -1.0;
            }
            
            /**
             * Pauses the current movie. Playback will continue at the current position after startMovie
             * is called.
             */
            virtual void pauseMovie() {
                _myLastSystemTime = -1.0;
            }
            
            /**
             * Closes the current movie and releases all internal data.
             */
            virtual void closeMovie() {}

            /**
             * @return new instance of the concrete MovieDecoder.
             */
            virtual asl::Ptr<MovieDecoderBase> instance() const {
                return asl::Ptr<MovieDecoderBase>(0);
            }

        protected:
            const Movie * getMovie() const {
                return _myMovie;
            }
            Movie * getMovie() {
                return _myMovie;
            }

        private:
            Movie *        _myMovie;
            bool           _myEndOfFileFlag;
            double         _myMovieTime;
            double         _myLastSystemTime;
    };

    typedef asl::Ptr<MovieDecoderBase> MovieDecoderBasePtr;
}
#endif

