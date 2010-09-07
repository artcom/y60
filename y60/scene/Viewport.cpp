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

// own header
#include "Viewport.h"

#include "Canvas.h"
#include "Camera.h"

#include <asl/base/Logger.h>

using namespace asl;
using namespace dom;
using namespace std;

namespace y60 {

bool
Viewport::getLeft(int & theLeft) const {
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
         theLeft = int(get<Position2DTag>()[0] * myCanvasNode->getFacade<Canvas>()->getWidth());
         return true;
    } 
    AC_ERROR << "Canvas is null!";
    return false;
}

int
Viewport::getLower() const {
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
        return int((1.0 - get<Position2DTag>()[1] - get<Size2DTag>()[1]) * myCanvasNode->getFacade<Canvas>()->getHeight());
    }
    return 0;
}

bool
Viewport::getTop(int & theTop) const {
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
        theTop = int(get<Position2DTag>()[1] * myCanvasNode->getFacade<Canvas>()->getHeight());
        return true;
    }
    AC_ERROR << "Canvas is null!";
    return false;
}

bool
Viewport::getWidth(unsigned & theWidth) const {
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
        const CanvasPtr & myCanvas = myCanvasNode->getFacade<Canvas>();
        theWidth = unsigned(get<Size2DTag>()[0] * myCanvas->getWidth());
        return true;
    }
    return false;
}

bool
Viewport::getHeight(unsigned & theHeight) const {
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
        theHeight = unsigned(get<Size2DTag>()[1] * myCanvasNode->getFacade<Canvas>()->getHeight());
        return true;
    }
    return false;
}

void
Viewport::applyAspectToCamera() {
    dom::NodePtr myCameraNode = getNode().getElementById( get<CameraTag>() );
    if (myCameraNode) {
        CameraPtr myCamera = myCameraNode->getFacade<Camera>();

        myCamera->updateFrustum( get<ResizePolicyTag>(),
                float( get<ViewportWidthTag>() ) / float( get<ViewportHeightTag>()));
    }
}

}
