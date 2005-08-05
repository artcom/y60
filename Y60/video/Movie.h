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

#include <y60/Image.h>
#include <y60/NodeNames.h>

#include <asl/Ptr.h>
#include <asl/Stream.h>

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
    DEFINE_ATTRIBUT_TAG(CurrentFrameTag, int,         MOVIE_FRAME_ATTRIB,       0);
    DEFINE_ATTRIBUT_TAG(FrameCountTag,   unsigned,    MOVIE_FRAMECOUNT_ATTRIB,  0);
    DEFINE_ATTRIBUT_TAG(FrameRateTag,    double,      MOVIE_FRAMERATE_ATTRIB,   25);
    DEFINE_ATTRIBUT_TAG(PlaySpeedTag,    float,       MOVIE_PLAYSPEED_ATTRIB,   1);
    DEFINE_ATTRIBUT_TAG(PlayModeTag,     std::string, MOVIE_PLAYMODE_ATTRIB,    "play");
    DEFINE_ATTRIBUT_TAG(VolumeTag,       float,       MOVIE_VOLUME_ATTRIB,      1);
    DEFINE_ATTRIBUT_TAG(LoopCountTag,    unsigned,    MOVIE_LOOPCOUNT_ATTRIB,   1);
    DEFINE_ATTRIBUT_TAG(AudioTag,        bool,        MOVIE_AUDIO_ATTRIB,       true);

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
        public AVDelayTag::Plug,
        public AudioTag::Plug
    {
        public:
            Movie(dom::Node & theNode);
            virtual ~Movie();

            IMPLEMENT_FACADE(Movie);

            /**
             * reads the frame closest to theCurrentTime from the movie. Some
             * decoders do not support random access on the movie, so non-
             * increasing times may fail. Normal playback (increment theCurrentTime
             * by 1/framerate always works).
             *
             * @param theCurrentTime time of the frame to retrieve
             */
            void readFrame(double theCurrentTime);
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
            virtual void load(const std::string & theTexturePath = ".");
            /**
             * @retval true, if a reload of the movie is required
             * (due to a change in the src attribute for example)
             */
            virtual bool reloadRequired() const;

            /**
             * @return locally cached playmode. It is set in the dom setter and
             * cached inside the object.
             */
            MoviePlayMode getPlayMode() const {
                return _myPlayMode;
            }

        private:
            Movie();

            void setup();

            void stop();
            void restart();
            void setPlayMode(MoviePlayMode thePlayMode);

            double decodeFrame(double theTime, unsigned theFrame);

            double getTimeFromFrame(unsigned theFrame) const;
            unsigned getFrameFromTime(double theTime) const;
            void loadFile(const std::string & theSourceFile);
            void loadStream(asl::Ptr<asl::ReadableStream> theSource,
                            const std::string theName);

            void postLoad();

            asl::Ptr<MovieDecoderBase> _myDecoder;
            std::string                _myLoadedFilename;

            MoviePlayMode         _myPlayMode;
            unsigned                   _myLastDecodedFrame;
            double                     _myLastCurrentTime;
            unsigned                   _myCurrentLoopCount;

            dom::ValuePtr _myStreamData;
    };

    typedef asl::Ptr<Movie, dom::ThreadingModel> MoviePtr;
    /* @} */
}
#endif
