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
//   $Id: LightBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: LightBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin for light sources
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "LightBuilder.h"
#include <y60/NodeNames.h>
#include <dom/Nodes.h>

namespace y60 {
    LightBuilder::LightBuilder(const std::string & theLightSourceId, const std::string & theName)
        : TransformBuilderBase(LIGHT_NODE_NAME, theName)
    {
        if (!getNode()->hasFacade()) {
            getNode()->appendAttribute(LIGHT_SOURCE_ATTRIB, theLightSourceId);
        }
    }

    LightBuilder::~LightBuilder() {
    }
}
