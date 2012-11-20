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
*/
//
//   $RCSfile: $
//
//   $Author: $
//
//   $Revision: $
//
//=============================================================================

// own header
#include "CanvasBuilder.h"

#include "Canvas.h"
#include <y60/base/PropertyNames.h>
#include <y60/base/property_functions.h>
#include <y60/base/CommonTags.h>
#include <y60/base/NodeNames.h>

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
