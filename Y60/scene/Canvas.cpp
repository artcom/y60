/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2003, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//   $RCSfile: Canvas.cpp,v $
//
//   $Revision: 1.4 $
//
//   Description:
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Scene.h"
#include "Canvas.h"
#include <dom/Nodes.h>

using namespace std;

namespace y60 {

unsigned 
Canvas::getWidth() const { 
    asl::Ptr<IFrameBuffer> myFrameBuffer = _myFrameBuffer.lock();
    //AC_TRACE << "Canvas @" << this << "getting width from framebuffer@" << &(*myFrameBuffer) << endl;
    return myFrameBuffer ? myFrameBuffer->getWidth() : 0;
}

unsigned 
Canvas::getHeight() const {
    asl::Ptr<IFrameBuffer> myFrameBuffer = _myFrameBuffer.lock();
    return myFrameBuffer ? myFrameBuffer->getHeight() : 0;
}

bool
Canvas::setFrameBuffer(asl::Ptr<IFrameBuffer> theFrameBuffer) {
    if (theFrameBuffer) {
        asl::Ptr<IFrameBuffer> myFrameBuffer = _myFrameBuffer.lock();
        if (! myFrameBuffer) {
            //AC_TRACE << "Canvas @"<< this << ", setting framebuffer @ " << &(*theFrameBuffer) << endl;
            _myFrameBuffer = theFrameBuffer;
            return true;
        } else {
            // framebuffer already set
            return false;
        } 
    } else {
        // unset framebuffer
        _myFrameBuffer = asl::Ptr<IFrameBuffer>(0);
        return true;
    }
}

ImagePtr
Canvas::getTarget(asl::Ptr<Scene> theScene) {
    dom::NodePtr myImageNode = theScene->getSceneDom()->getElementById( get<CanvasTargetTag>() );
    if (myImageNode) {
        return myImageNode->getFacade<Image>();
    }
    return ImagePtr(0);
}

}
