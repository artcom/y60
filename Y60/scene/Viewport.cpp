/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: Viewport.cpp,v $
//
//   $Revision: 1.4 $
//
//   Description:
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

bool 
Viewport::getProjectionMatrix(asl::Matrix4f & theProjection) const {
    // TODO: optimize by checking version counters
    const dom::NodePtr myCameraNode = getNode().getElementById(get<CameraTag>()); 
    const CameraPtr & myCamera = myCameraNode->getFacade<Camera>();
    float myAspect = float(get<ViewportWidthTag>())/float(get<ViewportHeightTag>());
    if (get<ViewportOrientationTag>() == PORTRAIT_ORIENTATION) {
        myAspect = 1.0f/myAspect;
    }
    dom::NodePtr myFrustumNode = dom::NamedNodeMap::getNamedItem(VIEWPORT_FRUSTUM_ATTRIB); 
    dom::Node::WritableValue<Frustum> myFrustum(myFrustumNode);  
    
    myFrustum.get().updateCorners(myCamera->get<NearPlaneTag>(), myCamera->get<FarPlaneTag>(),
                    myCamera->get<HfovTag>(), myCamera->get<OrthoWidthTag>(), myAspect);
    
    myFrustum.get().getProjectionMatrix(theProjection);

    if (get<ViewportOrientationTag>() == PORTRAIT_ORIENTATION) {
        theProjection.rotateZ(float(asl::PI_2));
    }
    return true;
}

void 
Viewport::updateClippingPlanes() {
    // TODO: optimize by checking version counters
    const dom::NodePtr myCameraNode = getNode().getElementById(get<CameraTag>()); 
    const CameraPtr & myCamera = myCameraNode->getFacade<Camera>();

    dom::NodePtr myFrustumNode = dom::NamedNodeMap::getNamedItem(VIEWPORT_FRUSTUM_ATTRIB); 
    dom::Node::WritableValue<Frustum> myFrustum(myFrustumNode);  
    
    myFrustum.get().updateCorners(myCamera->get<NearPlaneTag>(), myCamera->get<FarPlaneTag>(),
                    myCamera->get<HfovTag>(), myCamera->get<OrthoWidthTag>(), 
                    float(get<ViewportWidthTag>())/float(get<ViewportHeightTag>()));
    myFrustum.get().updatePlanes(myCamera->get<GlobalMatrixTag>(), myCamera->get<InverseGlobalMatrixTag>());
}

}
