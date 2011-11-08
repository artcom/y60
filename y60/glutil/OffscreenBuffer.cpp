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
    _myTextureNodeVersion(0),
    _myTextureWidth(0),
    _myTextureHeight(0),
    _myBlitFilter(GL_NEAREST),
    _myFBO(0),
    _myColorBuffer(0),
    _myDepthBuffer(0),
    _myMultisampleFBO(0),
    _myMultisampleColorBuffer(0),
    _myMultisampleDepthBuffer(0)
{}

OffscreenBuffer::~OffscreenBuffer() {
    AC_DEBUG << "OffscreenBuffer:dtor called.";
    reset();
}

void
OffscreenBuffer::reset() {
    if (_myFBO) glDeleteFramebuffersEXT(1, &_myFBO);
    if (_myColorBuffer) glDeleteRenderbuffersEXT(1, &_myColorBuffer);
    if (_myDepthBuffer) glDeleteRenderbuffersEXT(1, &_myDepthBuffer);
    if (_myMultisampleFBO) glDeleteFramebuffersEXT(1, &_myFBO);
    if (_myMultisampleColorBuffer) glDeleteRenderbuffersEXT(1, &_myMultisampleColorBuffer);
    if (_myMultisampleDepthBuffer) glDeleteRenderbuffersEXT(1, &_myMultisampleDepthBuffer);
    _myFBO = 0; _myMultisampleFBO = 0;
    _myColorBuffer = 0; _myMultisampleColorBuffer = 0;
    _myDepthBuffer = 0; _myMultisampleDepthBuffer = 0;
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
OffscreenBuffer::activate(std::vector<TexturePtr> & theTextures, unsigned theSamples,
                          unsigned theCubmapFace)
{
    for (y60::VectorOfString::size_type i = 0; i < theTextures.size(); ++i) {
        unsigned myTextureId = theTextures[i]->getTextureId();
        AC_DEBUG << "OffscreenBuffer:activate texture id = " << myTextureId << " theSamples = "<<theSamples;
        // ensure texture object exists
        myTextureId = theTextures[i]->applyTexture();
    }
    if (_myUseFBO) {
        bindOffscreenFrameBuffer(theTextures, theSamples, theCubmapFace);
    }
}


void
OffscreenBuffer::deactivate(std::vector<TexturePtr> & theTextures, bool theCopyToImageFlag) {
    AC_DEBUG << "OffscreenBuffer:deactivate, render targets: " << theTextures.size() << " theCopyToImageFlag = "<<theCopyToImageFlag;

    if (_myUseFBO) {
        //TODO: multisampling && mrt
        TexturePtr myTexture = theTextures.front();
        unsigned myTextureId = myTexture->getTextureId();
        if (_myMultisampleFBO) {
            AC_DEBUG << "OffscreenBuffer:deactivate texture id = " << myTextureId << ", blit multisample buffer to texture";
            // blit multisample buffer to texture
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, _myMultisampleFBO);
            glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, _myFBO);
            CHECK_OGL_ERROR;

            unsigned myWidth = myTexture->get<TextureWidthTag>();
            unsigned myHeight = myTexture->get<TextureHeightTag>();
            glBlitFramebufferEXT(0, 0, myWidth, myHeight,
                                 0, 0, myWidth, myHeight,
                                 GL_COLOR_BUFFER_BIT, _myBlitFilter);
            CHECK_OGL_ERROR;

        }
        AC_DEBUG << "OffscreenBuffer:deactivate, unbinding framebuffer";
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        CHECK_OGL_ERROR;

        // generate mipmap levels
        if (myTexture->get<TextureMipmapTag>()) {
            AC_DEBUG << "OffscreenBuffer::deactivate: generating mipmap levels";
            glBindTexture(GL_TEXTURE_2D, myTextureId);
            glGenerateMipmapEXT(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            CHECK_OGL_ERROR;
        }
    } else { // use backbuffer
        if (theTextures.size() > 1) {
            AC_WARNING << "multiple render targets only supported when using FBOs, copying backbuffer to first target";
        }
        TexturePtr myTexture = theTextures.front();
        unsigned myTextureId = myTexture->getTextureId();
    
        AC_DEBUG << "OffscreenBuffer:deactivate texture id = " << myTextureId << "zeroing texture";

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
OffscreenBuffer::copyToImage(std::vector<TexturePtr> & theTextures) {
    //XXX: does not really work yet
    for (y60::VectorOfString::size_type i = 0; i < theTextures.size(); ++i) {
        copyToImage(theTextures[i]);
    }
}

void
OffscreenBuffer::copyToImage(TexturePtr theTexture) {
    ImagePtr myImage = theTexture->getImage();
    if (!myImage) {
        AC_ERROR << "OffscreenBuffer::copyToImage: Texture id='" << theTexture->get<IdTag>() << "' has no image associated";
        return;
    }
    AC_DEBUG << "OffscreenBuffer::copyToImage texture=" << theTexture->get<NameTag>()
             << " image=" << myImage->get<NameTag>();

    if (_myUseFBO) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFBO);
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

//TODO: use mrts
void
OffscreenBuffer::bindOffscreenFrameBuffer(std::vector<TexturePtr> & theTextures, unsigned theSamples,
                                          unsigned theCubemapFace)
{
    TexturePtr theTexture = theTextures.front();

    AC_DEBUG << "OffscreenBuffer::bindOffscreenFrameBuffer to texture=" << theTexture->get<IdTag>();

    unsigned int myWidth = theTexture->get<TextureWidthTag>();
    unsigned int myHeight = theTexture->get<TextureHeightTag>();
    bool myTextureSizeHasChangedFlag = (_myTextureWidth != myWidth || _myTextureHeight != myHeight);
    _myTextureWidth = myWidth;
    _myTextureHeight = myHeight;
    // rebind texture if target image has changed
    if (_myFBO && (theTexture->getNode().nodeVersion() != _myTextureNodeVersion || myTextureSizeHasChangedFlag))
    {
        AC_DEBUG << "Tearing down FBO since Texture has changed "
                 << theTexture->getNode().nodeVersion() << " != " << _myTextureNodeVersion 
                 << " image size changed: " << myWidth << ", " << myHeight;
        reset();
    }

    unsigned myTextureId = theTexture->getTextureId();
    // create FBO
     if (!_myFBO) {
        AC_DEBUG << "OffscreenBuffer::bindOffscreenFrameBuffer creating FBO, texture=" << theTexture->get<IdTag>();
        AC_DEBUG << "setup RTT framebuffer texture=" << theTexture->get<NameTag>()
                 << " size=" << myWidth << "x" << myHeight;
        if (theSamples >= 1 && !_myHasFBOMultisample)
        {
            AC_WARNING << "Multisampling requested but not supported, turning it off";
            theSamples = 0;
        }

#ifdef GL_EXT_framebuffer_multisample // TODO: ugly hack to support older glew versions (<1.4.0), update linux buildserver
        if (theSamples >= 1) {
            /*
             * setup multisample framebuffer
             */
            GLint myMaxSamples;
            glGetIntegerv(GL_MAX_SAMPLES_EXT, &myMaxSamples);
            AC_DEBUG << "setup multisample framebuffer multisampling samples="
                     << theSamples << " max.samples=" << myMaxSamples;

            // color buffer
            glGenRenderbuffersEXT(1, &_myMultisampleColorBuffer);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myMultisampleColorBuffer);
            TextureInternalFormat myImageFormat = theTexture->getInternalEncoding();
            glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,
                                                theSamples,
                                                asGLTextureInternalFormat(myImageFormat),
                                                myWidth, myHeight);
            checkOGLError(PLUS_FILE_LINE);

            // depth buffer
            glGenRenderbuffersEXT(1, &_myMultisampleDepthBuffer);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myMultisampleDepthBuffer);
            glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,
                    theSamples, GL_DEPTH_COMPONENT24,
                    myWidth, myHeight);
            checkOGLError(PLUS_FILE_LINE);

            // multisample framebuffer
            glGenFramebuffersEXT(1, &_myMultisampleFBO);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myMultisampleFBO);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                    GL_RENDERBUFFER_EXT, _myMultisampleColorBuffer);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                    GL_RENDERBUFFER_EXT, _myMultisampleDepthBuffer);
            checkOGLError(PLUS_FILE_LINE);
        }
#endif

        /*
         * setup render-to-texture framebuffer
         */
        _myTextureNodeVersion = theTexture->getNode().nodeVersion();

        _myColorBuffer = myTextureId;

        AC_TRACE << "nodeVersion=" << _myTextureNodeVersion << " textureID="
                 << _myColorBuffer;

        // framebuffer
        glGenFramebuffersEXT(1, &_myFBO);
        checkOGLError(PLUS_FILE_LINE);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFBO);
        checkOGLError(PLUS_FILE_LINE);

        // color buffer
        switch (theTexture->getType()) {
            case TEXTURE_2D:
				{AC_DEBUG << "attaching 2D texture";}
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                          GL_TEXTURE_2D, _myColorBuffer, 0);
                checkOGLError(PLUS_FILE_LINE);
                break;
            case TEXTURE_CUBEMAP:
                {AC_DEBUG << "attaching cubemap";}
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                          asGLCubemapFace(theCubemapFace),
                                          _myColorBuffer, 0);
                break;
            default:
                throw TextureException(std::string("Unknown texture type '")+
                        theTexture->get<NameTag>() + "'", PLUS_FILE_LINE);
        }
        checkOGLError(PLUS_FILE_LINE);

        if (theSamples == 0) {
            // depth buffer (only necessary when not multisampling)
            glGenRenderbuffersEXT(1, &_myDepthBuffer);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myDepthBuffer);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                                     myWidth, myHeight);
            checkOGLError(PLUS_FILE_LINE);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                    GL_RENDERBUFFER_EXT, _myDepthBuffer);
            checkOGLError(PLUS_FILE_LINE);
        }

        checkFramebufferStatus();
    } else {
        AC_DEBUG << "OffscreenBuffer::bindOffscreenFrameBuffer binding FBO, texture=" << theTexture->get<IdTag>();
        /*
         * bind FBO
         */
        if (_myMultisampleFBO) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myMultisampleFBO);
        } else {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFBO);
        }

        if (_myColorBuffer != myTextureId) {
            _myColorBuffer = myTextureId;
            if (theTexture->getType() == TEXTURE_CUBEMAP) {
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                          asGLCubemapFace(theCubemapFace),
                                          _myColorBuffer, 0);
            } else {
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                          GL_TEXTURE_2D, _myColorBuffer, 0);
            }
            checkOGLError(PLUS_FILE_LINE);
        }
    }
}

}
