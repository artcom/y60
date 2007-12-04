//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Viewport.h" 
#include "Canvas.h" 
#include "Camera.h" 

#include <asl/Logger.h> 

using namespace asl;
using namespace dom;
using namespace std;

namespace y60 {

bool 
Viewport::getLeft(int & theLeft) const {
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
         int newLeft = int(get<Position2DTag>()[0] * myCanvasNode->getFacade<Canvas>()->getWidth());
         if (theLeft != newLeft) {
             theLeft = newLeft;
             return true;
         }
    } else {
        AC_ERROR << "Canvas is null!";
    } 
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
        int newTop = int(get<Position2DTag>()[1] * myCanvasNode->getFacade<Canvas>()->getHeight());
        if (theTop != newTop) {
            theTop = newTop;
            return true;
        }
    } else {
        AC_ERROR << "Canvas is null!";
    }
    return false;
}

bool 
Viewport::getWidth(unsigned & theWidth) const { 
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
        const CanvasPtr & myCanvas = myCanvasNode->getFacade<Canvas>();
        unsigned newWidth = unsigned(get<Size2DTag>()[0] * myCanvas->getWidth());
        if (theWidth != newWidth) {
            theWidth = newWidth;
            return true;
        }
    } 
    return false;
}

bool 
Viewport::getHeight(unsigned & theHeight) const {
    const Node * myCanvasNode = this->getNode().parentNode();
    if (myCanvasNode) {
        unsigned newHeight = unsigned(get<Size2DTag>()[1] * myCanvasNode->getFacade<Canvas>()->getHeight());
        if (newHeight != theHeight) {
            theHeight = newHeight;
            return true;
        }
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
