//============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "Scene.h"
#include "Canvas.h"
#include <asl/dom/Nodes.h>

using namespace std;

namespace y60 {

unsigned 
Canvas::getWidth() const { 
    asl::Ptr<IFrameBuffer> myFrameBuffer = _myFrameBuffer.lock();
    AC_TRACE << "Canvas::getWidth '" << get<NameTag>() << "' framebuffer=" << &*myFrameBuffer;
    return myFrameBuffer ? myFrameBuffer->getWidth() : 0;
}

unsigned 
Canvas::getHeight() const {
    asl::Ptr<IFrameBuffer> myFrameBuffer = _myFrameBuffer.lock();
    AC_TRACE << "Canvas::getHeight '" << get<NameTag>() << "' framebuffer=" << &*myFrameBuffer;
    return myFrameBuffer ? myFrameBuffer->getHeight() : 0;
}

bool
Canvas::setFrameBuffer(asl::Ptr<IFrameBuffer> theFrameBuffer) {
    AC_DEBUG << "Canvas::setFrameBuffer '" << get<NameTag>() << " to " << &*theFrameBuffer;
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

TexturePtr
Canvas::getTarget(asl::Ptr<Scene,dom::ThreadingModel> theScene) {
    dom::NodePtr myTextureNode = theScene->getSceneDom()->getElementById( get<CanvasTargetTag>() );
    if (myTextureNode) {
        return myTextureNode->getFacade<Texture>();
    }
    return TexturePtr(0);
}

bool 
Canvas::hasRenderTarget() const {
    return ! get<CanvasTargetTag>().empty();
}
}
