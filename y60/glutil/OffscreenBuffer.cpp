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
#include "OffscreenBuffer.h"

#include "PixelEncodingInfo.h"
#include "GLUtils.h"

#include <y60/image/Image.h>
#include <y60/scene/Texture.h>
#include <asl/base/Logger.h>
#include <asl/math/numeric_functions.h>


using namespace std;
using namespace dom;


namespace y60 {

static void checkFramebufferStatus()
{
    GLenum myStatus;
    myStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    bool isOK = false;
    ostringstream os;
    switch(myStatus) {
        case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
            isOK = true;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << endl;
            break;
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT
// EXT_framebuffer_object version < 117
        case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT" << endl;
            break;
#else
// EXT_framebuffer_object version >= 117
//   GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT has been removed from the extension [sh]
#endif
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << endl;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            os << "GL_FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
            break;
        default:
            /* programming error; will fail on all hardware */
            throw OpenGLException("GL_FRAMEBUFFER_EXT status broken, got "
                        + asl::as_string(myStatus), PLUS_FILE_LINE);
    }

    if (!isOK) {
        throw OffscreenRendererException(os.str(), PLUS_FILE_LINE);
    }
}


OffscreenBuffer::OffscreenBuffer() :
    _myUseFBO(hasCap("GL_EXT_framebuffer_object")),
    _myHasFBOMultisample(hasCap("GL_EXT_framebuffer_multisample GL_EXT_framebuffer_blit")),
    _myTextureWidth(0),
    _myTextureHeight(0),
    _myBlitFilter(GL_NEAREST),
    _myFBO(0),
    _myDepthBuffer(0),
    _myMultisampleFBO(0),
    _myMultisampleDepthBuffer(0)
{}

OffscreenBuffer::~OffscreenBuffer() {
    AC_DEBUG << "OffscreenBuffer:dtor called.";
    reset();
}

void
OffscreenBuffer::reset() {
    if (_myFBO) glDeleteFramebuffersEXT(1, &_myFBO);
    if (_myDepthBuffer) glDeleteRenderbuffersEXT(1, &_myDepthBuffer);
    if (_myMultisampleFBO) glDeleteFramebuffersEXT(1, &_myMultisampleFBO);
    if (_myMultisampleDepthBuffer) glDeleteRenderbuffersEXT(1, &_myMultisampleDepthBuffer);
    _myFBO = 0; _myMultisampleFBO = 0;
    _myDepthBuffer = 0; _myMultisampleDepthBuffer = 0;

    _myColorBuffers.clear();
    if (!_myMultisampleColorBuffers.empty()) glDeleteRenderbuffersEXT(_myMultisampleColorBuffers.size(), &_myMultisampleColorBuffers[0]);
    _myMultisampleColorBuffers.clear();
    _myTextureNodeVersions.clear();
}

void
OffscreenBuffer::setUseFBO(bool theUseFlag) {
    if (theUseFlag && !hasCap("GL_EXT_framebuffer_object")) {
        AC_WARNING << "OpenGL FBO rendering requested but not supported, "
                   << "falling back to backbuffer rendering";
        theUseFlag = false;
    }
    _myUseFBO = theUseFlag;
}


void
OffscreenBuffer::activate(const std::vector<TexturePtr> & theTextures, unsigned int theSamples,
                          unsigned int theCubmapFace)
{
    GLint maxbuffers;
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxbuffers);
    AC_DEBUG << "OffscreenBuffer::activate, render targets: " << theTextures.size() << ", max supported render targets: " << maxbuffers
             << " multisamples= " << theSamples << " cubemapface: " << theCubmapFace;
    if (static_cast<GLint>(theTextures.size()) > maxbuffers) {
        throw OffscreenRendererException("more render targets requested than available from the driver, wanted: "
                                         + asl::as_string(theTextures.size()) + " , supported: " + asl::as_string(maxbuffers), PLUS_FILE_LINE);
    }
    if (_myUseFBO) {
        bindFBO(theTextures, theSamples, theCubmapFace);
    }
}

void
OffscreenBuffer::blitToTexture(const std::vector<TexturePtr> & theTextures) {
    if (_myMultisampleFBO) {
        for (std::vector<TexturePtr>::size_type i = 0; i < theTextures.size(); ++i) {
            AC_DEBUG << "OffscreenBuffer::blitToTexture texture id = " << theTextures[i]->getTextureId() << ", blit multisample buffer to texture";
            // blit multisample buffer to texture
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, _myMultisampleFBO);
            glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + i);
            glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, _myFBO);
            glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT + i);
            CHECK_OGL_ERROR;

            glBlitFramebufferEXT(0, 0, _myTextureWidth, _myTextureHeight,
                                 0, 0, _myTextureWidth, _myTextureHeight,
                                 GL_COLOR_BUFFER_BIT, _myBlitFilter);
            CHECK_OGL_ERROR;
        }
    }
}

void
OffscreenBuffer::deactivate(const std::vector<TexturePtr> & theTextures, bool theCopyToImageFlag) {
    AC_DEBUG << "OffscreenBuffer::deactivate, render targets: " << theTextures.size() << " theCopyToImageFlag = "<<theCopyToImageFlag;

    if (_myUseFBO) {
        blitToTexture(theTextures);
        AC_DEBUG << "OffscreenBuffer::deactivate, unbinding FBO";
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glDrawBuffer(GL_BACK);
        CHECK_OGL_ERROR;

        // generate mipmap levels
        for (std::vector<TexturePtr>::size_type i = 0; i < theTextures.size(); ++i) {
            if (theTextures[i]->get<TextureMipmapTag>()) {
                AC_DEBUG << "OffscreenBuffer::deactivate: generating mipmap levels";
                glBindTexture(GL_TEXTURE_2D, theTextures[i]->getTextureId());
                glGenerateMipmapEXT(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, 0);
                CHECK_OGL_ERROR;
            }
        }
    } else { // use backbuffer
        if (theTextures.size() > 1) {
            AC_WARNING << "multiple render targets only supported when using FBOs, copying backbuffer to first target";
        }
        TexturePtr myTexture = theTextures.front();
        unsigned myTextureId = myTexture->getTextureId();
    
        AC_DEBUG << "OffscreenBuffer::deactivate texture id = " << myTextureId << "zeroing texture";

        // copy backbuffer to texture
        glBindTexture(GL_TEXTURE_2D, myTextureId);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0 /*MIPMAP level*/, 0, 0,
                0, 0, myTexture->get<TextureWidthTag>(), myTexture->get<TextureHeightTag>());
        CHECK_OGL_ERROR;

        // generate mipmap levels
        if (myTexture->get<TextureMipmapTag>()) {
            AC_DEBUG << "OffscreenBuffer::deactivate: generating mipmap levels";
            glGenerateMipmapEXT(GL_TEXTURE_2D);
            CHECK_OGL_ERROR;
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    if (theCopyToImageFlag) {
        copyToImage(theTextures);
    }
}

void
OffscreenBuffer::copyToImage(const std::vector<TexturePtr> & theTextures) {
    for (std::vector<TexturePtr>::size_type i = 0; i < theTextures.size(); ++i) {
        copyToImage(theTextures[i], i);
    }
}

void
OffscreenBuffer::copyToImage(TexturePtr theTexture, unsigned int theColorBufferIndex) {
    ImagePtr myImage = theTexture->getImage();
    if (!myImage) {
        AC_ERROR << "OffscreenBuffer::copyToImage: Texture id='" << theTexture->get<IdTag>() << "' has no image associated";
        return;
    }
    AC_DEBUG << "OffscreenBuffer::copyToImage texture=" << theTexture->get<NameTag>()
             << " image=" << myImage->get<NameTag>();

    if (_myUseFBO) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFBO);
        glReadBuffer(GL_COLOR_ATTACHMENT0_EXT + theColorBufferIndex);
    }

    PixelEncodingInfo myPixelEncodingInfo =
        getDefaultGLTextureParams(myImage->getRasterEncoding());
    myPixelEncodingInfo.internalformat =
        asGLTextureInternalFormat(theTexture->getInternalEncoding());

    unsigned myWidth = myImage->get<ImageWidthTag>();
    unsigned myHeight = myImage->get<ImageHeightTag>();
    AC_TRACE << "pixelformat " << myImage->get<RasterPixelFormatTag>();
    AC_TRACE << "image size " << myWidth << "x" << myHeight;

    dom::ResizeableRasterPtr myRaster = myImage->getRasterPtr();
    asl::WriteableBlock & myBlock = myRaster->pixels();

    glReadPixels(0, 0, myWidth, myHeight,
                myPixelEncodingInfo.externalformat, myPixelEncodingInfo.pixeltype,
                myBlock.begin());
    CHECK_OGL_ERROR;

    myImage->getRasterValueNode()->bumpVersion();

    if (_myUseFBO) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        CHECK_OGL_ERROR;
    }
}

bool
OffscreenBuffer::FBOrebindRequired(const std::vector<TexturePtr> & theTextures) const {
    // mrt's have to have same texture size
    unsigned int myWidth = theTextures.front()->get<TextureWidthTag>();
    unsigned int myHeight = theTextures.front()->get<TextureHeightTag>();
    bool myTextureSizeHasChanged = (_myTextureWidth != myWidth || _myTextureHeight != myHeight);
    bool myTextureNodeVersionsChanged = false;
    for (std::vector<asl::Unsigned64>::size_type i = 0; i < asl::minimum(_myTextureNodeVersions.size(), theTextures.size()); ++i) {
        myTextureNodeVersionsChanged |= _myTextureNodeVersions[i] != theTextures[i]->getNode().nodeVersion();
    }
    return (myTextureNodeVersionsChanged || myTextureSizeHasChanged || (theTextures.size() != _myColorBuffers.size()));
}

void
OffscreenBuffer::bindFBO(const std::vector<TexturePtr> & theTextures, unsigned int theSamples,
                                          unsigned int theCubemapFace)
{
    std::string myString("OffscreenBuffer::bindFBO to " + ((theTextures.size() == 1) ? "texture="
        + theTextures.front()->get<IdTag>() : + "multiple render targets (" + asl::as_string((std::vector<TexturePtr>::size_type)theTextures.size())
        + ") multisamples: " + asl::as_string(theSamples)));
    AC_DEBUG << myString;

    if (!_myFBO || FBOrebindRequired(theTextures)) {
        AC_DEBUG << "OffscreenBuffer::bindFBO tearing down FBO since render targets have changed";
        reset(); // deletes fbos if any
        // mrt's have to have same texture size
        _myTextureWidth = theTextures.front()->get<TextureWidthTag>();
        _myTextureHeight = theTextures.front()->get<TextureHeightTag>();
        _myTextureNodeVersions.resize(theTextures.size());
        for (std::vector<TexturePtr>::size_type i = 0; i < theTextures.size(); ++i) {
            _myTextureNodeVersions[i] = theTextures[i]->getNode().nodeVersion();
        }
        setupFBO(theTextures, theSamples, theCubemapFace);
    }
    
    if (_myMultisampleFBO) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myMultisampleFBO);
        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
    } else {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFBO);
        std::vector<GLenum> buffers(_myColorBuffers.size());
        for (std::vector<GLuint>::size_type i = 0; i < _myColorBuffers.size(); ++i) {
            buffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;
        }
        glDrawBuffers(theTextures.size(), &buffers[0]);
    }
    checkOGLError(PLUS_FILE_LINE);
}

void
OffscreenBuffer::setupFBO(const std::vector<TexturePtr> & theTextures, unsigned int theSamples,
                                          unsigned int theCubemapFace)
{
    GLint mySamples = theSamples;
    if (mySamples >= 1 && !_myHasFBOMultisample) {
        AC_WARNING << "Multisampling requested but not supported, turning it off";
        mySamples = 0;
    }

    if (mySamples >= 1) { // setup multisample framebuffer
        GLint myMaxSamples;
        glGetIntegerv(GL_MAX_SAMPLES_EXT, &myMaxSamples);
        AC_DEBUG << "OffscreenBuffer::setupFBO setup multisample framebuffer multisampling samples="
                 << mySamples << " maxSamples=" << myMaxSamples << " with " << theTextures.size() << " render targets";
        if (mySamples > myMaxSamples) {
            AC_WARNING << "wanted " << mySamples << " multisamples but only " << myMaxSamples << " multisamples supported";
            mySamples = myMaxSamples;
        }
        // multisample framebuffer
        glGenFramebuffersEXT(1, &_myMultisampleFBO);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myMultisampleFBO);
        
        // color buffer
        _myMultisampleColorBuffers.resize(theTextures.size());
        glGenRenderbuffersEXT(_myMultisampleColorBuffers.size(), &_myMultisampleColorBuffers[0]);
        for (std::vector<TexturePtr>::size_type i = 0; i < theTextures.size(); ++i) {
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myMultisampleColorBuffers[i]);
            TextureInternalFormat myImageFormat = theTextures[i]->getInternalEncoding();
            glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,
                                                mySamples,
                                                asGLTextureInternalFormat(myImageFormat),
                                                _myTextureWidth, _myTextureHeight);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i,
                    GL_RENDERBUFFER_EXT, _myMultisampleColorBuffers[i]);
            checkOGLError(PLUS_FILE_LINE);
        }

        // depth buffer
        glGenRenderbuffersEXT(1, &_myMultisampleDepthBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myMultisampleDepthBuffer);
        glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,
                mySamples, GL_DEPTH_COMPONENT24,
                _myTextureWidth, _myTextureHeight);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                GL_RENDERBUFFER_EXT, _myMultisampleDepthBuffer);
        checkOGLError(PLUS_FILE_LINE);

        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        checkOGLError(PLUS_FILE_LINE);

    }

    // framebuffer
    AC_DEBUG << "OffscreenBuffer::setupFBO with " << theTextures.size() << " render targets";
    glGenFramebuffersEXT(1, &_myFBO);
    checkOGLError(PLUS_FILE_LINE);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFBO);
    checkOGLError(PLUS_FILE_LINE);

    // color buffer
    _myColorBuffers.resize(theTextures.size());
    for (std::vector<TexturePtr>::size_type i = 0; i < theTextures.size(); ++i) {
        _myColorBuffers[i] = theTextures[i]->getTextureId();
        switch (theTextures[i]->getType()) {
            case TEXTURE_CUBEMAP:
                {AC_DEBUG << "OffscreenBuffer::setupFBO attaching cubemap render target " << theTextures[i]->get<IdTag>();}
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i,
                                          asGLCubemapFace(theCubemapFace),
                                          _myColorBuffers[i], 0);
                break;
            case TEXTURE_2D:
                {AC_DEBUG << "OffscreenBuffer::setupFBO attaching 2D render target " << theTextures[i]->get<IdTag>();}
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i,
                                          GL_TEXTURE_2D, _myColorBuffers[i], 0);
                break;
            default:
                throw TextureException(std::string("Unknown texture type '")+
                                       asl::as_string(theTextures[i]->getType()) + "'", PLUS_FILE_LINE);
        }
        checkOGLError(PLUS_FILE_LINE);
    }

    if (theSamples == 0) {
        // depth buffer (only necessary when not multisampling)
        glGenRenderbuffersEXT(1, &_myDepthBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myDepthBuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                                 _myTextureWidth, _myTextureHeight);
        checkOGLError(PLUS_FILE_LINE);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                GL_RENDERBUFFER_EXT, _myDepthBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        checkOGLError(PLUS_FILE_LINE);
    }
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
        
    checkFramebufferStatus();
}
}
