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
    AbstractRenderWindow(JSApp::ShellErrorReporter),
    _myOffscreenBuffer(OffscreenBufferPtr(new OffscreenBuffer())), 
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
    std::vector<TexturePtr> myTextures = getRenderTargets();
    if (myTextures.empty()) {
        return 0;
    }
    // mrt's have to have same texture size
    return myTextures.front()->get<TextureWidthTag>();
}

int OffscreenRenderArea::getHeight() const {
    if (_myHeight > 0) {
        return _myHeight;
    }
    std::vector<TexturePtr> myTextures = getRenderTargets();
    if (myTextures.empty()) {
        return 0;
    }
    // mrt's have to have same texture size
    return myTextures.front()->get<TextureHeightTag>();
}

// AbstractRenderWindow
void OffscreenRenderArea::initDisplay() {
}

void
OffscreenRenderArea::activate(unsigned theCubemapFace) {
    std::vector<TexturePtr> myTextures = getRenderTargets();
    if (myTextures.empty()) {
        AC_ERROR << "OffscreenRenderArea::renderToCanvas has no canvas / image to render... ignoring";
        return;
    }
    activate(myTextures, theCubemapFace);
}

void
OffscreenRenderArea::activate(std::vector<TexturePtr> & theTextures, unsigned theCubemapFace) {
    std::string myString((theTextures.size() == 1) ? "texture="
        + theTextures.front()->get<NameTag>() : + "multiple render targets (" + asl::as_string((std::vector<TexturePtr>::size_type)theTextures.size()));
    AC_TRACE << "OffscreenRenderArea::activate " << myString;

    ensureRenderTargets(theTextures);
    // fetch size from texture, mrt's have to have same texture size
    if (_myWidth == 0 || _myHeight == 0) {
        setWidth(theTextures.front()->get<TextureWidthTag>());
        setHeight(theTextures.front()->get<TextureHeightTag>());
    }
    
    _myOffscreenBuffer->activate(theTextures, getMultisamples(), theCubemapFace);
}

void
OffscreenRenderArea::deactivate(bool theCopyToImageFlag) {
    std::vector<TexturePtr> myTextures = getRenderTargets();
    if (myTextures.empty()) {
        AC_ERROR << "OffscreenRenderArea::deactivate has no canvas / image to render... ignoring";
        return;
    }
    deactivate(myTextures, theCopyToImageFlag);
}

void
OffscreenRenderArea::deactivate(const std::vector<TexturePtr> & theTextures, bool theCopyToImageFlag) {
    AC_TRACE << "OffscreenRenderArea::deactivate copyToImage=" << theCopyToImageFlag;
    _myOffscreenBuffer->deactivate(theTextures, theCopyToImageFlag);
}

void
OffscreenRenderArea::renderToCanvas(bool theCopyToImageFlag, unsigned theCubemapFace,
                                    bool theClearColorBufferFlag, bool theClearDepthBufferFlag) {
    AC_TRACE << "OffscreenRenderArea::renderToCanvas copyToImage=" << theCopyToImageFlag;
    MAKE_SCOPE_TIMER(OffscreenRenderArea_renderToCanvas);

    std::vector<TexturePtr> myTextures = getRenderTargets();
    if (myTextures.empty()) {
        AC_ERROR << "OffscreenRenderArea::renderToCanvas has no canvas / image to render... ignoring";
        return;
    }

    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_onframe_sceneupdate);
        _myScene->updateAllModified();
    }

    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_activate);
        activate(myTextures, theCubemapFace);
    }

    GLuint myClearMask = 0;
    myClearMask |= theClearColorBufferFlag ? GL_COLOR_BUFFER_BIT : 0;
    myClearMask |= theClearDepthBufferFlag ? GL_DEPTH_BUFFER_BIT : 0;
    clearBuffers(myClearMask);

    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_prerender);
        preRender();
    }
    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_render);
        render();
    }
    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_postrender);
        postRender();
    }
    {
        MAKE_SCOPE_TIMER(OffscreenRenderArea_deactivate);
        deactivate(myTextures, theCopyToImageFlag);
    }

    
}

void
OffscreenRenderArea::downloadFromViewport(const dom::NodePtr & theTextureNode) {
    AC_DEBUG << "OffscreenRenderArea::downloadFromViewport";

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
        AC_DEBUG << "Resizing raster of image id=" << myImage->get<IdTag>() << " to " << getWidth() << "x" << getHeight();
        myRaster->resize(getWidth(), getHeight());
    }

    _myOffscreenBuffer->copyToImage(myTexture);
}

void
OffscreenRenderArea::setRenderingCaps(unsigned int theRenderingCaps) {
    AC_DEBUG << "OffscreenRenderArea::setRenderingCaps";

    AbstractRenderWindow::setRenderingCaps(theRenderingCaps);

    bool myFBOFlag = ((theRenderingCaps & y60::FRAMEBUFFER_SUPPORT) != 0);

    if(myFBOFlag) {
        AC_DEBUG << "Will use GL FBO for rendering";
    } else {
        AC_DEBUG << "Will use GL backbuffer for rendering";
    }

    _myOffscreenBuffer->setUseFBO(myFBOFlag);
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
    AC_DEBUG << "OffscreenRenderArea::setCanvas";
    if (AbstractRenderWindow::setCanvas(theCanvas)) {
        CanvasPtr myCanvas = theCanvas->getFacade<Canvas>();
        std::vector<TexturePtr> myTextures = myCanvas->getTargets(getCurrentScene());
        if (!myTextures.empty()) {
            ensureRenderTargets(myTextures);
            setWidth(myTextures.front()->get<TextureWidthTag>());
            setHeight(myTextures.front()->get<TextureHeightTag>());
        } else {
            throw OffscreenRendererException(std::string("No target textures defined for canvas: ") + theCanvas->getAttributeString(ID_ATTRIB), PLUS_FILE_LINE);
        }
        return true;
    } else {
        return false;
    }
}

const std::vector<TexturePtr>
OffscreenRenderArea::getRenderTargets() const {
    std::vector<TexturePtr> myTextures;
    NodePtr myCanvas = getCanvas();
    if (myCanvas) {
        CanvasPtr myCanvasFacade = myCanvas->getFacade<Canvas>();
        myTextures = myCanvasFacade->getTargets(getCurrentScene());
        if (myTextures.empty()) {
            AC_WARNING << "No target textures defined for canvas: " << myCanvas->getAttributeString(ID_ATTRIB);
        }
    } else {
        AC_WARNING << "No canvas. Forgot to call setCanvas?";
    }
    return myTextures;
}

void
OffscreenRenderArea::ensureRenderTargets(std::vector<TexturePtr> & theTextures) {
    for (std::vector<TexturePtr>::size_type i = 0; i < theTextures.size(); ++i) {
        // ensure texture object exists
        unsigned int myTextureId = theTextures[i]->applyTexture();
        AC_DEBUG << "OffscreenRenderArea::ensureRenderTargets applyTexture id = " << myTextureId;
        // ensure render targets have the same size
        if (theTextures[i]->get<TextureWidthTag>() != theTextures.front()->get<TextureWidthTag>() ||
            theTextures[i]->get<TextureHeightTag>() != theTextures.front()->get<TextureHeightTag>()) 
        {
            throw OffscreenRendererException("render targets: " +  theTextures.front()->get<NameTag>() + " , "
                                             + theTextures[i]->get<NameTag>() + " have different size", PLUS_FILE_LINE);
        }
    }
}
} //namespace jslib
