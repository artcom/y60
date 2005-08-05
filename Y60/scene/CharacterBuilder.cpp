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
//   $Id: CharacterBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: CharacterBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//=============================================================================

#include "CharacterBuilder.h"
#include "ClipBuilder.h"
#include <y60/NodeNames.h>

#include <dom/Nodes.h>

using namespace asl;

namespace y60 {
    CharacterBuilder::CharacterBuilder(const std::string & theName) :
        BuilderBase(CHARACTER_NODE_NAME)
    {
        dom::NodePtr myNode = getNode();
        myNode->appendAttribute(NAME_ATTRIB, theName);
    }

    CharacterBuilder::~CharacterBuilder() {
    }

    const std::string &
    CharacterBuilder::appendClip(ClipBuilder & theClip) {
        return appendNodeWithId(theClip, getNode());
    }
}
