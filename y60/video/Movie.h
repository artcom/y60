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
//   $RCSfile: Movie.h,v $
//   $Author: valentin $
//   $Revision: 1.3 $
//   $Date: 2005/04/19 12:38:29 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _ac_y60_Movie_h_
#define _ac_y60_Movie_h_

#include <y60/image/Image.h>
#include <y60/base/NodeNames.h>

#include <asl/base/Ptr.h>
#include <asl/base/Stream.h>

namespace asl {
    class PackageManager;
}

namespace y60 {

    class MovieDecoderBase;

    /*! @addtogroup Y60video */
    /* @{ */

    DEFINE_EXCEPTION(MovieException, asl::Exception);

    //                  theTagName       theType      theAttributeName               theDefault
    DEFINE_ATTRIBUT_TAG(AVDelayTag,      double,      MOVIE_AVDELAY_ATTRIB,     0);
    DEFINE_ATTRIBUT_TAG(CacheSizeTag,    int,         MOVIE_FRAME_CACHE_ATTRIB, 0);
    DEFINE_ATTRIBUT_TAG(MaxCacheSizeTag, unsigned,    MOVIE_FRAME_MAX_CACHE_ATTRIB, 32);
    DEFINE_ATTRIBUT_TAG(CurrentFrameTag, int,         MOVIE_FRAME_ATTRIB,       0);
    DEFINE_ATTRIBUT_TAG(FrameCountTag,   int,         MOVIE_FRAMECOUNT_ATTRIB,  -1);
    DEFINE_ATTRIBUT_TAG(FrameRateTag,    double,      MOVIE_FRAMERATE_ATTRIB,   25);
    DEFINE_ATTRIBUT_TAG(PlaySpeedTag,    float,       MOVIE_PLAYSPEED_ATTRIB,   1);
    DEFINE_ATTRIBUT_TAG(FrameBlendingTag,bool,        MOVIE_FRAMEBLENDING_ATTRIB,false);
    DEFINE_ATTRIBUT_TAG(FrameBlendFactorTag,double,   MOVIE_FRAMEBLENDFACTOR_ATTRIB,  1.0);	
    DEFINE_ATTRIBUT_TAG(PlayModeTag,     std::string, MOVIE_PLAYMODE_ATTRIB,    "play");
    DEFINE_ATTRIBUT_TAG(VolumeTag,       float,       MOVIE_VOLUME_ATTRIB,      1);
    DEFINE_ATTRIBUT_TAG(LoopCountTag,    unsigned,    MOVIE_LOOPCOUNT_ATTRIB,   1);
    DEFINE_ATTRIBUT_TAG(AudioTag,        bool,        MOVIE_AUDIO_ATTRIB,       true);
    DEFINE_ATTRIBUT_TAG(DecoderHintTag,  std::string, MOVIE_DECODERHINT_ATTRIB, "");
    DEFINE_ATTRIBUT_TAG(MovieTimeTag,    double,      MOVIE_MOVIETIME_ATTRIB,   0);
    DEFINE_ATTRIBUT_TAG(DecoderTag,      std::string, MOVIE_DECODER_ATTRIB,     "UNKNOWN");

    /**
     * @ingroup Y60video
     * Facade for the dom movie node. A movie node is used to play a movie on
     * a texture. A movie node has a decoder that is used to decode the video
     * file given in the src attribute.
     *
     */
    class Movie :
        public Image,
        public FrameCountTag::Plug,
        public CurrentFrameTag::Plug,
        public FrameRateTag::Plug,
        public PlaySpeedTag::Plug,
        public PlayModeTag::Plug,
        public VolumeTag::Plug,
        public LoopCountTag::Plug,
        public CacheSizeTag::Plug,
        public MaxCacheSizeTag::Plug,
        public AVDelayTag::Plug,
        public AudioTag::Plug,
        public DecoderHintTag::Plug,
		public FrameBlendFactorTag::Plug,
		public FrameBlendingTag::Plug,
        public dom::DynamicAttributePlug<MovieTimeTag, Movie>,
        public dom::DynamicAttributePlug<DecoderTag, Movie>
    {
        public:
			enum MovieFrameBuffer { PRIMARY_BUFFER = 0, SECONDARY_BUFFER = 1, MAX_BUFFER = 2};

            Movie(dom::Node & theNode);
            virtual ~Movie();

            IMPLEMENT_DYNAMIC_FACADE(Movie);

            /**
             * reads the frame closest to theCurrentTime from the movie. Some
             * decoders do not support random access on the movie, so non-
             * increasing times may fail. Normal playback (increment theCurrentTime
             * by 1/framerate always works).
             *
             * @param theCurrentTime time of the frame to retrieve
             */
            void readFrame();
            void readFrame(double theCurrentTime, bool theIgnoreCurrentTime=false);
            /**
             * loads a movie from thePackageManager
             * @param thePackageManager Package Manager to retrieve the movie from
             * @see asl::PackageManager
             */
            virtual void load(asl::PackageManager & thePackageManager);
            /**
             * loads a movie from the file given in theTexturePath
             * @param theTexturePath movie file to load
             */
            virtual void load(const std::string & theTexturePath);
//            virtual void load(const std::string & theTexturePath = ".");
            /**
             * @retval true, if a reload of the movie is required
             * (due to a change in the src attribute for example)
             */
            virtual bool reloadRequired();

            /**
             * @ ensures that the movie has correct framecount
             */
            void ensureMovieFramecount();
            
            /**
             * @return locally cached playmode. It is set in the dom setter and
             * cached inside the object.
             */
            MoviePlayMode getPlayMode() const {
                return _myPlayMode;
            }

            bool getMovieTime(double & theTime) const;
            bool getDecoderName(std::string & theName) const;

            /**
             * @add another raster to movie.
             */
            dom::ResizeableRasterPtr addRasterValue(dom::ValuePtr theRaster, PixelEncoding theEncoding, unsigned theDepth);
        private:
            Movie();

            // Overwrite the Image::load() method to avoid auto-loading mechanism
            void load();
            
            void setup();
            void stop();
            void restart(double theCurrentTime);
            void setPlayMode(MoviePlayMode thePlayMode);

            double decodeFrame(double theTime, unsigned theFrame);

            double getTimeFromFrame(unsigned theFrame) const;
            unsigned getFrameFromTime(double theTime);
            void loadFile(const std::string & theSourceFile);
            void loadStream(asl::Ptr<asl::ReadableStream> theSource,
                            const std::string theName);

            void postLoad();

            asl::Ptr<MovieDecoderBase>  getDecoder(const std::string theFilename);

            asl::Ptr<MovieDecoderBase> _myDecoder;
            std::string                _myLoadedFilename;

            MoviePlayMode         _myPlayMode;
            unsigned                   _myLastDecodedFrame;
            double                     _myLastCurrentTime;
            unsigned                   _myCurrentLoopCount;
            dom::ValuePtr _myStreamData;

			// Framemodulation member
			MovieFrameBuffer           _myNextUsedBuffer;
            unsigned                   _myFirstBufferFrame;
            unsigned                   _mySecondBufferFrame;
    };

    typedef asl::Ptr<Movie, dom::ThreadingModel> MoviePtr;
    /* @} */
}
#endif
