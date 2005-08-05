//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: ClipBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: ClipBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "ClipBuilder.h"
#include "AnimationBuilder.h"
#include <y60/NodeNames.h>

#include <dom/Nodes.h>

namespace y60 {
    ClipBuilder::ClipBuilder(const std::string & theName) :
        BuilderBase(CLIP_NODE_NAME)
    {
        dom::NodePtr myNode = getNode();
        myNode->appendAttribute(NAME_ATTRIB, theName);
    }

    ClipBuilder::~ClipBuilder() {
    }

    const std::string &
    ClipBuilder::appendAnimation(AnimationBuilder & theAnimation) {
        return appendNodeWithId(theAnimation, getNode());
    }

}
