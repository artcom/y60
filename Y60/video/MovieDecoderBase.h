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
    /**
     * @ingroup Y60video
     * Abstract base class for Movie Decoders.
     *
     */
    class MovieDecoderBase : public IDecoder {
        public:
            MovieDecoderBase();

            virtual ~MovieDecoderBase();

            void initialize(Movie * theMovie);
            virtual std::string canDecode(const std::string & theUrl, 
                    asl::ReadableStream * theStream = 0);

            /**
             * @return the time that audio is behind video in seconds
             */
            virtual double getAudioVideoDelay() const; 

            /**
             *
             * @param theSystemTime used for calculation of delta t in free running mode.
             *                      Should be increasing constantly over time. May be ignored
             *                      if the audioclock or the framecache are used.
             * @return current movie time
             */
            virtual double getMovieTime(double theSystemTime);

            /**
             * Reads a frame from the decoder.
             * @param theTime time of the frame to fetch
             * @param theFrame index of the frame to fetch
             * @param theTargetRaster raster to read the frame into
             * @return real time of the frame return in theTargetRaster
             */
            virtual double readFrame(double theTime, unsigned theFrame, 
                    dom::ResizeableRasterPtr theTargetRaster) = 0;

            /**
             * loads a movie from the file given by theFilename
             * @param theFilename file to load into the decoder
             */
            virtual void load(const std::string & theFilename) = 0;

            /**
             * loads a movie from the stream given by theSource
             * @param theFilename file to identify theSource
             */
            virtual void load(asl::Ptr<asl::ReadableStream> theSource, 
                    const std::string & theFilename);

            PixelEncoding getPixelFormat() const; 
            unsigned getFrameCount() const; 
            unsigned getFrameWidth() const; 
            unsigned getFrameHeight() const; 
            double getFrameRate() const;
            bool getAudioFlag() const; 
            const asl::Matrix4f & getImageMatrix() const; 
            MoviePlayMode getPlayMode() const; 
            bool getEOF() const; 
            void setEOF(bool theEndOfFileFlag); 

            /**
             * Starts the movie playback at theStartTime.
             * @param theStartTime start time of the movie. Defaults to 0.0
             */
            virtual void startMovie(double theStartTime = 0.0);

            /**
             * Is called before start movie, if a movie was paused before.
             */
            virtual void resumeMovie(double theStartTime = 0.0); 

            /**
             * Stops the movie playback. Future calls of startMovie will start from the 
             * beginning of the movie.
             */
            virtual void stopMovie();
            
            /**
             * Pauses the current movie. Playback will continue at the current position after startMovie
             * is called.
             */
            virtual void pauseMovie();
            
            /**
             * Closes the current movie and releases all internal data.
             */
            virtual void closeMovie();

            /**
             * @return new instance of the concrete MovieDecoder.
             */
            virtual asl::Ptr<MovieDecoderBase> instance() const;
            
            virtual const char * getName() const { return "";}

        protected:
            const Movie * getMovie() const;
            Movie * getMovie(); 

        private:
            Movie *        _myMovie;
            bool           _myEndOfFileFlag;
            double         _myMovieTime;
            double         _myLastSystemTime;
    };

    typedef asl::Ptr<MovieDecoderBase> MovieDecoderBasePtr;
}
#endif

