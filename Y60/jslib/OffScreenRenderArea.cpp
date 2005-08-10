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


#include "OffScreenRenderArea.h"
#include "JSApp.h"

#include <y60/Image.h>
#include <y60/PixelEncodingInfo.h>
#include <asl/Logger.h>


using namespace dom;
using namespace y60;

namespace jslib {

asl::Ptr<OffScreenRenderArea>
OffScreenRenderArea::create() {
    asl::Ptr<OffScreenRenderArea> newObject = asl::Ptr<OffScreenRenderArea>(new OffScreenRenderArea());
    newObject->setSelf(newObject);
    return newObject;
}

OffScreenRenderArea::OffScreenRenderArea()
: AbstractRenderWindow(JSApp::ShellErrorReporter)
{
    AC_TRACE << "OffScreenRenderArea::CTOR";
    setRenderingCaps(0);
}


OffScreenRenderArea::~OffScreenRenderArea() {
}

// IEventSink
void OffScreenRenderArea::handle(y60::EventPtr theEvent) {
}

// IGLContext
void OffScreenRenderArea::activateGLContext() {
}
void OffScreenRenderArea::deactivateGLContext() {
}

// AbstractRenderWindow
void OffScreenRenderArea::initDisplay() {
}

void OffScreenRenderArea::renderToCanvas(bool theCopyToImageFlag) {
    AC_TRACE << "OffScreenRenderArea::renderToCanvas ";
    ImagePtr myTexture = getImage();
    if ( ! myTexture) {
        AC_ERROR << "OffScreenRenderArea::renderToCanvas has no canvas / image to render... igoring";
        return;
    }
    preOffScreenRender(myTexture);

    preRender();
    render();
    postRender();

    postOffScreenRender(myTexture, theCopyToImageFlag);
}    

bool OffScreenRenderArea::setCanvas(const NodePtr & theCanvas) {
    if (AbstractRenderWindow::setCanvas(theCanvas)) {
        ensureRaster(getImage());
        return true;
    } else {
        return false;
    }
}

const ImagePtr OffScreenRenderArea::getImage() const {
    return getCanvas() ?
        getCanvas()->getFacade<Canvas>()->getTarget( getCurrentScene() ) : ImagePtr(0);
}

ImagePtr OffScreenRenderArea::getImage() {
    return getCanvas() ?
        getCanvas()->getFacade<Canvas>()->getTarget( getCurrentScene() ) : ImagePtr(0);
}

int OffScreenRenderArea::getWidth() const {
    ImagePtr myImage = getImage();
    return myImage ? myImage->get<ImageWidthTag>() : 0;
}

int OffScreenRenderArea::getHeight() const {
    ImagePtr myImage = getImage();
    return myImage ? myImage->get<ImageHeightTag>() : 0;
}

ResizeableRasterPtr
OffScreenRenderArea::ensureRaster(ImagePtr theImage) {
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
