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
    : AbstractRenderWindow(JSApp::ShellErrorReporter),
      _myOffScreenBuffer(0), _myDepthBuffer(0) {
    AC_TRACE << "OffScreenRenderArea::CTOR";
    setRenderingCaps(0);
}


OffScreenRenderArea::~OffScreenRenderArea() {
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

bool OffScreenRenderArea::setCanvas(const NodePtr & theCanvas) {
    if (AbstractRenderWindow::setCanvas(theCanvas)) {
        ensureRaster(getImage());
        return true;
    } else {
        return false;
    }
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


void OffScreenRenderArea::copyFrameBufferToTexture(ImagePtr theTexture) {
    AC_TRACE << "OffScreenRenderArea::copyFrameBufferToTexture ";
    glBindTexture (GL_TEXTURE_2D, theTexture->getGraphicsId() );
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0 /*MIPMAP levels*/, 0, 0,
            0, 0, theTexture->get<ImageWidthTag>(), theTexture->get<ImageHeightTag>() );
    glBindTexture (GL_TEXTURE_2D, 0);
}

void OffScreenRenderArea::bindOffScreenFrameBuffer(ImagePtr theTexture) {
    AC_TRACE << "OffScreenRenderArea::bindOffScreenFrameBuffer ";
#ifdef GL_EXT_framebuffer_object
    if ( ! _myOffScreenBuffer) {
        glGenFramebuffersEXT(1, &_myOffScreenBuffer);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffScreenBuffer);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
            GL_TEXTURE_2D, theTexture->getGraphicsId(), 0);

        //we need a depth buffer as well
        glGenRenderbuffersEXT(1, &_myDepthBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myDepthBuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
                GL_DEPTH_COMPONENT24, getWidth(), getHeight());

        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _myDepthBuffer);

    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffScreenBuffer);
    }

    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            /* choose different formats */
            break;
        default:
            /* programming error; will fail on all hardware */
            throw OpenGLException("GL_FRAMEBUFFER_EXT status broken, got "
                        + asl::as_string(status), PLUS_FILE_LINE);
    }
#else
    throw OpenGLException("GL_EXT_framebuffer_object support not compiled", PLUS_FILE_LINE);
#endif
}

void OffScreenRenderArea::bindTexture(ImagePtr theTexture) {
    AC_TRACE << "OffScreenRenderArea::bindTexture ";
#ifdef GL_EXT_framebuffer_object
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
    glBindTexture (GL_TEXTURE_2D, theTexture->getGraphicsId() );
}

void OffScreenRenderArea::renderToTexture(bool theCopyToImageFlag) {
    AC_TRACE << "OffScreenRenderArea::renderToTexture ";
    ImagePtr myImage = getImage();
    if ( ! myImage) {
        AC_ERROR << "OffScreenRenderArea::renderToTexture has no canvas / image to render... igoring";
        return;
    }

    if (getRenderingCaps() & y60::FRAMEBUFFER_SUPPORT) {
        bindOffScreenFrameBuffer(myImage);
    }

    preRender();
    render();
    postRender();

    if (getRenderingCaps() & y60::FRAMEBUFFER_SUPPORT) {
        bindTexture(myImage);
    } else {
        copyFrameBufferToTexture(myImage);
    }

    if (theCopyToImageFlag) {
        copyTextureToImage(myImage);
    }
}

void OffScreenRenderArea::copyTextureToImage(ImagePtr theImage) {
    AC_TRACE << "OffScreenRenderArea::copyTextureToImage ";

#ifdef GL_EXT_framebuffer_object
    if (getRenderingCaps() & y60::FRAMEBUFFER_SUPPORT) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffScreenBuffer);
    }
#endif

    PixelEncodingInfo myPixelEncodingInfo = getDefaultGLTextureParams(theImage->getEncoding());

    glReadPixels(0, 0, theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>(),
                myPixelEncodingInfo.externalformat, myPixelEncodingInfo.pixeltype,
                theImage->getRasterPtr()->pixels().begin());

    theImage->getRasterValueNode()->bumpVersion();

#ifdef GL_EXT_framebuffer_object
    if (getRenderingCaps() & y60::FRAMEBUFFER_SUPPORT) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
#endif
}

} //namespace jslib
