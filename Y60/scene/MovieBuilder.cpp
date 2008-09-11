//============================================================================
// Copyright (C) 2004-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "MovieBuilder.h"
#include <y60/base/NodeNames.h>
#include <y60/video/Movie.h>
#include <asl/dom/Nodes.h>

namespace y60 {
    MovieBuilder::MovieBuilder(const std::string & theName,
                               const std::string & theFileName)
        : ImageBuilder(MOVIE_NODE_NAME, theName)
    {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            myNode->appendAttribute(IMAGE_SRC_ATTRIB, theFileName);
            myNode->appendAttribute(MOVIE_FRAME_ATTRIB, "0");
            myNode->appendAttribute(MOVIE_FRAMECOUNT_ATTRIB, "0");
            myNode->appendAttribute(MOVIE_LOOPCOUNT_ATTRIB, "0");
        } else {
            MoviePtr myMovie = myNode->getFacade<Movie>();
            myMovie->set<ImageSourceTag>(theFileName);
        }
    }

    MovieBuilder::~MovieBuilder() {
    }

    void 
    MovieBuilder::setLoopCount(unsigned theLoopCount) {
        getNode()->getAttribute(MOVIE_LOOPCOUNT_ATTRIB)->nodeValueAssign(theLoopCount);
    }
}
