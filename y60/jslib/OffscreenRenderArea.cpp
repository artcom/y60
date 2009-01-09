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
#include "OffscreenRenderArea.h"

#include "JSApp.h"

#include <y60/image/Image.h>
#include <y60/glutil/PixelEncodingInfo.h>
#include "GLContextRegistry.h"
#include <asl/base/Logger.h>


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

// IFrameBuffer
int OffscreenRenderArea::getWidth() const {
    if (_myWidth > 0) {
        return _myWidth;
    }
    TexturePtr myTexture = getTexture();
    if (!myTexture) {
        return 0;
    }
    return myTexture->get<TextureWidthTag>();
}

int OffscreenRenderArea::getHeight() const {
    if (_myHeight > 0) {
        return _myHeight;
    }
    TexturePtr myTexture = getTexture();
    if (!myTexture) {
        return 0;
    }
    return myTexture->get<TextureHeightTag>();
}

// AbstractRenderWindow
void OffscreenRenderArea::initDisplay() {
}

void
OffscreenRenderArea::activate(unsigned theCubemapFace) {
    TexturePtr myTexture = getTexture();
    if (!myTexture) {
        AC_ERROR << "OffscreenRenderArea::activate has no canvas / texture to render... ignoring";
        return;
    }
    AC_TRACE << "OffscreenRenderArea::activate '" << myTexture->get<NameTag>() << "'";

    y60::OffscreenBuffer::activate(myTexture, getMultisamples(), theCubemapFace);

    // fetch size from texture
    if (_myWidth == 0 || _myHeight == 0) {
        setWidth(myTexture->get<TextureWidthTag>());
        setHeight(myTexture->get<TextureHeightTag>());
    }
}

void
OffscreenRenderArea::deactivate(bool theCopyToImageFlag) {
    AC_TRACE << "OffscreenRenderArea::deactivate";
    TexturePtr myTexture = getTexture();
    if ( ! myTexture) {
        AC_ERROR << "OffscreenRenderArea::deactivate has no canvas / image to render... ignoring";
        return;
    }
    y60::OffscreenBuffer::deactivate(myTexture, theCopyToImageFlag);
}

void
OffscreenRenderArea::renderToCanvas(bool theCopyToImageFlag, unsigned theCubemapFace,
                                    bool theClearColorBufferFlag, bool theClearDepthBufferFlag) {
    AC_TRACE << "OffscreenRenderArea::renderToCanvas copyToImage=" << theCopyToImageFlag;
    MAKE_SCOPE_TIMER(OffscreenRenderArea_renderToCanvas);

    TexturePtr myTexture = getTexture();
    if ( ! myTexture) {
        AC_ERROR << "OffscreenRenderArea::renderToCanvas has no canvas / image to render... ignoring";
        return;
    }

    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_onframe_sceneupdate);
        _myScene->updateAllModified();
    }

    activate(theCubemapFace); //y60::OffscreenBuffer::activate(myTexture);
    GLuint myClearMask = 0;
    myClearMask |= theClearColorBufferFlag ? GL_COLOR_BUFFER_BIT : 0;
    myClearMask |= theClearDepthBufferFlag ? GL_DEPTH_BUFFER_BIT : 0;
    clearBuffers(myClearMask);
    

    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_render);
        preRender();
        render();
        postRender();
    }

    deactivate(theCopyToImageFlag);
}

void
OffscreenRenderArea::downloadFromViewport(const dom::NodePtr & theTextureNode) {
    if (!theTextureNode) {
        throw OffscreenRendererException("No Texture.", PLUS_FILE_LINE);
    }

    TexturePtr myTexture = theTextureNode->getFacade<Texture>();
    ImagePtr myImage = myTexture->getImage();
    if (!myImage) {
        AC_ERROR << "Texture id=" << myTexture->get<IdTag>() << " has no image associated";
        return;
    }

    ResizeableRasterPtr myRaster = myImage->getRasterPtr();
    if ( static_cast<int>(myRaster->width ()) != getWidth()
      || static_cast<int>(myRaster->height()) != getHeight()) {
        AC_DEBUG << "Resizing image id=" << myImage->get<IdTag>() << " to " << getWidth() << "x" << getHeight();
        myRaster->resize(getWidth(), getHeight());
    }
    OffscreenBuffer::copyToImage(myTexture);
}

void
OffscreenRenderArea::setRenderingCaps(unsigned int theRenderingCaps) {
    AbstractRenderWindow::setRenderingCaps(theRenderingCaps);
    OffscreenBuffer::setUseFBO( 0 != (theRenderingCaps & y60::FRAMEBUFFER_SUPPORT) );
}

void
OffscreenRenderArea::setWidth(unsigned theWidth) {
    AC_DEBUG << "OffscreenRenderArea::setWidth " << theWidth;
    // TODO: some kind of range checking vs. image size
    _myWidth = theWidth;
}

void
OffscreenRenderArea::setHeight(unsigned theHeight) {
    AC_DEBUG << "OffscreenRenderArea::setHeight " << theHeight;
    // TODO: some kind of range checking vs. image size
    _myHeight = theHeight;
}

bool
OffscreenRenderArea::setCanvas(const NodePtr & theCanvas) {
    if (AbstractRenderWindow::setCanvas(theCanvas)) {
        TexturePtr myTexture = getTexture();
        if (myTexture) {
//            ensureRaster(myTexture);
            setWidth(myTexture->get<TextureWidthTag>());
            setHeight(myTexture->get<TextureHeightTag>());
        } else {
            throw OffscreenRendererException(std::string("No target set for canvas: ") + theCanvas->getAttributeString(ID_ATTRIB), PLUS_FILE_LINE);
        }
        return true;
    } else {
        return false;
    }
}

const TexturePtr
OffscreenRenderArea::getTexture() const {
    if (getCanvas()) {
        return getCanvas()->getFacade<Canvas>()->getTarget( getCurrentScene() );
    }
    AC_WARNING << "No canvas.";
    return TexturePtr(0);
}

TexturePtr
OffscreenRenderArea::getTexture() {
    if (getCanvas()) {
        return getCanvas()->getFacade<Canvas>()->getTarget( getCurrentScene() );
    }
    AC_WARNING << "No canvas.";
    return TexturePtr(0);
}

} //namespace jslib
