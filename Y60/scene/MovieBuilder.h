//============================================================================
// Copyright (C) 2004-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_MovieBuilder_h_
#define _ac_MovieBuilder_h_

#include "ImageBuilder.h"
#include <asl/base/Ptr.h>

namespace y60 {

    class MovieBuilder : public ImageBuilder {
        public:
            DEFINE_EXCEPTION(MovieBuilderException, asl::Exception);

            MovieBuilder(const std::string & theName,
                         const std::string & theFileName);
            virtual ~MovieBuilder();

            void setLoopCount(unsigned theLoopCount);

        private:
    };

    typedef asl::Ptr<MovieBuilder>  MovieBuilderPtr;
}

#endif
