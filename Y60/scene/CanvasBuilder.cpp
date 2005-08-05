//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

#include "CanvasBuilder.h"
#include "Canvas.h"
#include <y60/PropertyNames.h>
#include <y60/property_functions.h>
#include <y60/CommonTags.h>
#include <y60/NodeNames.h>

namespace y60 {
    CanvasBuilder::CanvasBuilder(const std::string & theName) : BuilderBase(CANVAS_NODE_NAME) {
        dom::NodePtr myCanvas = getNode();

        if (!myCanvas->hasFacade()) {
            myCanvas->appendAttribute(NAME_ATTRIB, theName);
            myCanvas->appendAttribute(ID_ATTRIB,IdTag::getDefault());
        } else {
            myCanvas->getFacade<Canvas>()->set<NameTag>(theName);
        }

        // Add necessary Childnodes
        dom::NodePtr myViewport = dom::NodePtr(new dom::Element(VIEWPORT_NODE_NAME));
        myCanvas->appendChild(myViewport);

        if (!myViewport->hasFacade()) {
            myViewport->appendAttribute(ID_ATTRIB,IdTag::getDefault());
            myViewport->appendAttribute(ORIENTATION_ATTRIB, LANDSCAPE_ORIENTATION);
            myViewport->appendAttribute(NAME_ATTRIB, "MainViewport");
        } 

        dom::NodePtr myOverlayList(new dom::Element(OVERLAY_LIST_NAME));
        myViewport->appendChild(myOverlayList);
    }

    CanvasBuilder::~CanvasBuilder() {
    }
}
