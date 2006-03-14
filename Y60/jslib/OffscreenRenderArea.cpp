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
    AbstractRenderWindow(JSApp::ShellErrorReporter),
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
OffscreenRenderArea::renderToCanvas(bool theCopyToImageFlag) {
    AC_TRACE << "OffscreenRenderArea::renderToCanvas ";
    ImagePtr myTexture = getImage();
    if ( ! myTexture) {
        AC_ERROR << "OffscreenRenderArea::renderToCanvas has no canvas / image to render... ignoring";
        return;
    }
    onFrame();

    _myScene->updateAllModified();

    preOffscreenRender(myTexture);

    preRender();
    render();
    postRender();

    postOffscreenRender(myTexture, theCopyToImageFlag);
}    

void 
OffscreenRenderArea::downloadFromViewport(const dom::NodePtr & theImageNode) {
    if ( ! theImageNode ) {
        throw OffscreenRendererException("No Image.", PLUS_FILE_LINE);
    }
    ImagePtr myImage = theImageNode->getFacade<Image>();
    ResizeableRasterPtr myRaster = myImage->getRasterPtr();

    if (!myRaster) {
        myImage->set(getWidth(), getHeight(), 1, myImage->getEncoding());
        myRaster = myImage->getRasterPtr();
    }
    if (myImage->get<ImageWidthTag>() != myRaster->width() ||
        myImage->get<ImageHeightTag>() != myRaster->height())
    {
        myRaster->resize(getWidth(), getHeight());
    }

    copyFrameBufferToImage( myImage );
}

void 
OffscreenRenderArea::setRenderingCaps(unsigned int theRenderingCaps) {
    AbstractRenderWindow::setRenderingCaps(theRenderingCaps);
    OffscreenBuffer::setUseGLFramebufferObject(theRenderingCaps & y60::FRAMEBUFFER_SUPPORT);
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
            ensureRaster(myImage);
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

int
OffscreenRenderArea::getWidth() const {
    return _myWidth;
}

int
OffscreenRenderArea::getHeight() const {
    return _myHeight;
}

ResizeableRasterPtr
OffscreenRenderArea::ensureRaster(ImagePtr theImage) {
    if ( ! theImage ) {
        throw OffscreenRendererException("No image.", PLUS_FILE_LINE);
    }
    ResizeableRasterPtr myRaster = theImage->getRasterPtr();

    if (!myRaster) {
        theImage->set(theImage->get<ImageWidthTag>(),
                      theImage->get<ImageHeightTag>(), 1, theImage->getEncoding());
        myRaster = theImage->getRasterPtr();
    }
    if (theImage->get<ImageWidthTag>() != myRaster->width() ||
        theImage->get<ImageHeightTag>() != myRaster->height())
    {
        myRaster->resize(theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>());
    }
    return myRaster;
}

} //namespace jslib
