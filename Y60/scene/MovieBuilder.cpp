//============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//         $Id: MovieBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//    $RCSfile: MovieBuilder.cpp,v $
//     $Author: christian $
//   $Revision: 1.1 $
//       $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "MovieBuilder.h"
#include <y60/NodeNames.h>
#include <y60/Movie.h>
#include <dom/Nodes.h>

namespace y60 {
    MovieBuilder::MovieBuilder(const std::string & theName,
                               const std::string & theFileName)
        : ImageBuilder(MOVIE_NODE_NAME, theName, false)
    {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            //myNode->appendAttribute(NAME_ATTRIB, theName);
            myNode->appendAttribute(IMAGE_SRC_ATTRIB, theFileName);
            myNode->appendAttribute(MOVIE_FRAME_ATTRIB, "0");
            myNode->appendAttribute(MOVIE_FRAMECOUNT_ATTRIB, "0");
            myNode->appendAttribute(MOVIE_LOOPCOUNT_ATTRIB, "0");
        } else {
            MoviePtr myMovie = myNode->getFacade<Movie>();
            //myMovie->set<NameTag>(theName);
            myMovie->set<ImageSourceTag>(theFileName);
        }
    }

    MovieBuilder::~MovieBuilder() {
    }

    /*const std::string &
    MovieBuilder::getName() const {
        return getNode()->getAttribute(NAME_ATTRIB)->nodeValue();
    }
*/
    void 
    MovieBuilder::setLoopCount(unsigned theLoopCount) {
        getNode()->getAttribute(MOVIE_LOOPCOUNT_ATTRIB)->nodeValueAssign(theLoopCount);
    }
}
