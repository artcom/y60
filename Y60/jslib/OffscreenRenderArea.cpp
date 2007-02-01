//============================================================================
// Copyright (C) 2005-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "OffscreenRenderArea.h"
#include "JSApp.h"

#include <y60/Image.h>
#include <y60/PixelEncodingInfo.h>
#include "GLContextRegistry.h"
#include <asl/Logger.h>


using namespace dom;
using namespace y60;

namespace jslib {

asl::Ptr<OffscreenRenderArea>
OffscreenRenderArea::create() {
    asl::Ptr<OffscreenRenderArea> newObject = asl::Ptr<OffscreenRenderArea>(new OffscreenRenderArea());
    newObject->setSelf(newObject);
    return newObject;
}

OffscreenRenderArea::OffscreenRenderArea() :
    OffscreenBuffer(), AbstractRenderWindow(JSApp::ShellErrorReporter),
    _myWidth(0),
    _myHeight(0)
{
    AC_TRACE << "OffscreenRenderArea::CTOR";
    setRenderingCaps(0);

    asl::Ptr<AbstractRenderWindow> myContextWindow = GLContextRegistry::get().getContext();
    if (myContextWindow) {
        setGLContext(myContextWindow->getGLContext());
    } else {
        throw OffscreenRendererException("Could not find active render window. "
            "Offscreen render areas always need an gl context to share.", PLUS_FILE_LINE);
    }
}

OffscreenRenderArea::~OffscreenRenderArea() {
}

// IEventSink
void OffscreenRenderArea::handle(y60::EventPtr theEvent) {
}

// AbstractRenderWindow
void OffscreenRenderArea::initDisplay() {
}

void
OffscreenRenderArea::activate() {
    AC_TRACE << "OffscreenRenderArea::activate";
    ImagePtr myTexture = getImage();
    if ( ! myTexture) {
        AC_ERROR << "OffscreenRenderArea::activate has no canvas / image to render... ignoring";
        return;
    }
    y60::OffscreenBuffer::activate(myTexture, getMultisamples());
}

void
OffscreenRenderArea::deactivate(bool theCopyToImageFlag) {
    AC_TRACE << "OffscreenRenderArea::deactivate";
    ImagePtr myTexture = getImage();
    if ( ! myTexture) {
        AC_ERROR << "OffscreenRenderArea::deactivate has no canvas / image to render... ignoring";
        return;
    }
    y60::OffscreenBuffer::deactivate(myTexture, theCopyToImageFlag);
}

void
OffscreenRenderArea::renderToCanvas(bool theCopyToImageFlag) {
    AC_TRACE << "OffscreenRenderArea::renderToCanvas copyToImage=" << theCopyToImageFlag;
    MAKE_SCOPE_TIMER(OffscreenRenderArea_renderToCanvas);

    ImagePtr myTexture = getImage();
    if ( ! myTexture) {
        AC_ERROR << "OffscreenRenderArea::renderToCanvas has no canvas / image to render... ignoring";
        return;
    }

    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_onframe_sceneupdate);
        _myScene->updateAllModified();
    }

    activate(); //y60::OffscreenBuffer::activate(myTexture);
    clearBuffers( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_render);
        preRender();
        render();
        postRender();
    }

    deactivate(theCopyToImageFlag);
}

void
OffscreenRenderArea::downloadFromViewport(const dom::NodePtr & theImageNode) {
    if ( ! theImageNode ) {
        throw OffscreenRendererException("No Image.", PLUS_FILE_LINE);
    }
    ImagePtr myImage = theImageNode->getFacade<Image>();
    ResizeableRasterPtr myRaster = myImage->getRasterPtr();

    if (myRaster->width() != getWidth() || myRaster->height() != getHeight()) {
        myRaster->resize(getWidth(), getHeight());
    }

    OffscreenBuffer::copyToImage(myImage);
}

void
OffscreenRenderArea::setRenderingCaps(unsigned int theRenderingCaps) {
    AbstractRenderWindow::setRenderingCaps(theRenderingCaps);
    OffscreenBuffer::setUseFBO(theRenderingCaps & y60::FRAMEBUFFER_SUPPORT);
}

void
OffscreenRenderArea::setWidth(unsigned theWidth) {
    // TODO: some kind of range checking vs. image size
    _myWidth = theWidth;
}

void
OffscreenRenderArea::setHeight(unsigned theHeight) {
    // TODO: some kind of range checking vs. image size
    _myHeight = theHeight;
}

bool
OffscreenRenderArea::setCanvas(const NodePtr & theCanvas) {
    if (AbstractRenderWindow::setCanvas(theCanvas)) {
        ImagePtr myImage = getImage();
        if (myImage) {
//            ensureRaster(myImage);
            _myWidth  = myImage->get<ImageWidthTag>();
            _myHeight = myImage->get<ImageHeightTag>();
        } else {
            throw OffscreenRendererException(std::string("No target set for canvas: ") + theCanvas->getAttributeString(ID_ATTRIB), PLUS_FILE_LINE);
        }
        return true;
    } else {
        return false;
    }
}

const ImagePtr
OffscreenRenderArea::getImage() const {
    if (getCanvas()) {
        return getCanvas()->getFacade<Canvas>()->getTarget( getCurrentScene() );
    } else {
        AC_WARNING << "No canvas.";
        return ImagePtr(0);
    }
}

ImagePtr
OffscreenRenderArea::getImage() {
    if (getCanvas()) {
        return getCanvas()->getFacade<Canvas>()->getTarget( getCurrentScene() );
    } else {
        AC_WARNING << "No canvas.";
        return ImagePtr(0);
    }
}

} //namespace jslib
