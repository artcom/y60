//============================================================================
// Copyright (C) 2005-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "OffscreenBuffer.h"
#include "GLUtils.h"
#include "PixelEncodingInfo.h"

#include <y60/Image.h>
#include <asl/Logger.h>
#include <asl/numeric_functions.h>

//#define DUMP_BUFFER
#ifdef DUMP_BUFFER
#include <string>
#include <iostream>
#include <sstream>
#include <paintlib/plpngenc.h>
#include <paintlib/pltiffenc.h>
#include <paintlib/planybmp.h>
#endif

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
        case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT" << endl;
            break;
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
        /* [DS] not found in current headers
         * [UH] has been removed from spec
         case GL_FRAMEBUFFER_STATUS_ERROR_EXT:
             os << "GL_FRAMEBUFFER_STATUS_ERROR_EXT" << endl;
             break;
        */
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
    _myUseFBO(true),
    _myImageNodeVersion(0),
    _myBlitFilter(GL_NEAREST)
{
    reset();
}


void OffscreenBuffer::setUseFBO(bool theUseFlag)
{
#ifdef GL_EXT_framebuffer_object
    if (!IS_SUPPORTED(glGenFramebuffersEXT)) {
        AC_WARNING << "OffscreenBuffer::setUseFBO: OpenGL framebuffer extension is not supported";
        theUseFlag = false;
    }
#endif
    _myUseFBO = theUseFlag;
}


void OffscreenBuffer::activate(ImagePtr theImage, unsigned theSamples)
{
    if (_myUseFBO) {
        bindOffscreenFrameBuffer(theImage, theSamples);
    }
}


void OffscreenBuffer::deactivate(ImagePtr theImage, bool theCopyToImageFlag)
{
#ifdef GL_EXT_framebuffer_object
    if (_myUseFBO) {
        if (_myFrameBufferObject[1]) {
            // blit multisample buffer to texture
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, _myFrameBufferObject[1]);
            glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, _myFrameBufferObject[0]);

            unsigned myWidth = theImage->get<ImageWidthTag>();
            unsigned myHeight = theImage->get<ImageHeightTag>();
            glBlitFramebufferEXT(0, 0, myWidth, myHeight,
                                 0, 0, myWidth, myHeight,
                                 GL_COLOR_BUFFER_BIT, _myBlitFilter);

            // restore
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
            glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
            glDrawBuffer(GL_BACK);
        } else {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }

        // generate mipmap levels
        if (IS_SUPPORTED(glGenerateMipmapEXT) && theImage->get<ImageMipmapTag>()) {
            AC_TRACE << "OffscreenBuffer::deactivate: generating mipmap levels";
            glBindTexture(GL_TEXTURE_2D, theImage->ensureTextureId());
            glGenerateMipmapEXT(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    else
#endif
    {
        // copy backbuffer to texture
        glBindTexture(GL_TEXTURE_2D, theImage->ensureTextureId());
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0 /*MIPMAP level*/, 0, 0,
                0, 0, theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>());

        // generate mipmap levels
        if (IS_SUPPORTED(glGenerateMipmapEXT) && theImage->get<ImageMipmapTag>()) {
            AC_TRACE << "OffscreenBuffer::deactivate: generating mipmap levels";
            glGenerateMipmapEXT(GL_TEXTURE_2D);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (theCopyToImageFlag) {
        copyToImage(theImage);
    }
}


void OffscreenBuffer::copyToImage(ImagePtr theImage)
{
    AC_TRACE << "OffscreenBuffer::copyToImage id=" << theImage->get<IdTag>();

#ifdef GL_EXT_framebuffer_object
    if (_myUseFBO) {
        if (_myFrameBufferObject[1]) { // UH: not a bug, to determine if multisampling is enabled
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, _myFrameBufferObject[0]);
        } else {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFrameBufferObject[0]);
        }
    }
#endif
 
    PixelEncodingInfo myPixelEncodingInfo = getDefaultGLTextureParams(theImage->getRasterEncoding());
    myPixelEncodingInfo.internalformat = asGLTextureInternalFormat(theImage->getInternalEncoding());

    AC_TRACE << "pixelformat " << theImage->get<RasterPixelFormatTag>();
    AC_TRACE << "size " << theImage->get<ImageWidthTag>() << " " << theImage->get<ImageHeightTag>();

    glReadPixels(0, 0, theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>(),
                myPixelEncodingInfo.externalformat, myPixelEncodingInfo.pixeltype,
                theImage->getRasterPtr()->pixels().begin());        

#ifdef DUMP_BUFFER
    PixelEncoding myEncoding;
    switch(long(myPixelEncodingInfo.bytesPerPixel)) {
      case 1:
          myEncoding = GRAY;
          break;
      case 3:
          myEncoding = RGB;
          break;
      case 4:
          myEncoding = RGBA;
          break;
    }

    PLPixelFormat pf;
    mapPixelEncodingToFormat(myEncoding, pf);

    std::string myFilename("buffer.png");
    PLAnyBmp myBmp;    
    myBmp.Create( theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>(), pf, 
                  theImage->getRasterPtr()->pixels().begin(),
                  theImage->get<ImageWidthTag>() * myPixelEncodingInfo.bytesPerPixel);
    PLPNGEncoder myEncoder;
    myEncoder.MakeFileFromBmp(myFilename.c_str(), &myBmp);               
#endif

#ifdef GL_EXT_framebuffer_object
    if (_myUseFBO) {
        if (_myFrameBufferObject[1]) { // UH: not a bug, to determine if multisampling is enabled
            glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
        } else {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }
    }
#endif
}


void OffscreenBuffer::reset()
{
    _myFrameBufferObject[0] = _myFrameBufferObject[1] = 0;
    _myColorBuffer[0] = _myColorBuffer[1] = 0;
    _myDepthBuffer[0] = _myDepthBuffer[1] = 0;
}


void OffscreenBuffer::bindOffscreenFrameBuffer(ImagePtr theImage, unsigned theSamples)
{
#ifdef GL_EXT_framebuffer_object

    // rebind texture if target image has changed
    if (_myFrameBufferObject[0] && theImage->getNode().nodeVersion() != _myImageNodeVersion) {
        AC_DEBUG << "Tearing down FBO since Image has changed " << theImage->getNode().nodeVersion() << " != " << _myImageNodeVersion;

        glDeleteFramebuffersEXT(2, &_myFrameBufferObject[0]);
        glDeleteRenderbuffersEXT(2, &_myColorBuffer[0]);
        glDeleteRenderbuffersEXT(2, &_myDepthBuffer[0]);

        reset();
    }

    if (!_myFrameBufferObject[0]) {

        /*
         * create FBO
         */

        // XXX use power-of-two for image size since Y60 textures are POT only at the moment.
        // Once we support non-power-of-two textures this should be replaced with the actual
        // texture size (should come from Image/Texture object so that NPOT support is handled
        // properly)
        unsigned myWidth = asl::nextPowerOfTwo(theImage->get<ImageWidthTag>());
        unsigned myHeight = asl::nextPowerOfTwo(theImage->get<ImageHeightTag>());

        if (theSamples >= 1 && !(IS_SUPPORTED(glRenderbufferStorageMultisampleEXT) && IS_SUPPORTED(glBlitFramebufferEXT))) {
            AC_WARNING << "Multisampling requested but not supported, turning it off";
            theSamples = 0;
        }
        if (theSamples >= 1) {

            /*
             * setup multisample framebuffer
             */
            AC_DEBUG << "OffscreenBuffer::bindOffscreenFrameBuffer setup multisample framebuffer multisampling samples=" << theSamples << " size=" << myWidth << "x" << myHeight;

            // color buffer
            glGenRenderbuffersEXT(1, &_myColorBuffer[1]);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myColorBuffer[1]);
            glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,
                    theSamples, GL_RGB8,
                    myWidth, myHeight);
            checkOGLError(PLUS_FILE_LINE);

            // depth buffer
            glGenRenderbuffersEXT(1, &_myDepthBuffer[1]);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myDepthBuffer[1]);
            glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT,
                    theSamples, GL_DEPTH_COMPONENT24,
                    myWidth, myHeight);
            checkOGLError(PLUS_FILE_LINE);

            // multisample framebuffer
            glGenFramebuffersEXT(1, &_myFrameBufferObject[1]);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFrameBufferObject[1]);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                    GL_RENDERBUFFER_EXT, _myColorBuffer[1]);
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                    GL_RENDERBUFFER_EXT, _myDepthBuffer[1]);
            checkOGLError(PLUS_FILE_LINE);
        }

        /*
         * setup render-to-texture framebuffer
         */
        _myImageNodeVersion = theImage->getNode().nodeVersion();
        _myColorBuffer[0] = theImage->ensureTextureId();
        AC_DEBUG << "OffscreenBuffer::bindOffscreenFrameBuffer setup RTT framebuffer, nodeVersion=" << _myImageNodeVersion << " textureID=" << _myColorBuffer[0];

        // framebuffer
        glGenFramebuffersEXT(1, &_myFrameBufferObject[0]);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFrameBufferObject[0]);

        // color buffer
        glBindTexture(GL_TEXTURE_2D, _myColorBuffer[0]);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                GL_TEXTURE_2D, _myColorBuffer[0], 0);
        checkOGLError(PLUS_FILE_LINE);

        if (theSamples == 0) {
            // depth buffer (only necessary when not multisampling)
            glGenRenderbuffersEXT(1, &_myDepthBuffer[0]);
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myDepthBuffer[0]);
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 
                    myWidth, myHeight);
            checkOGLError(PLUS_FILE_LINE);

            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                    GL_RENDERBUFFER_EXT, _myDepthBuffer[0]);
            checkOGLError(PLUS_FILE_LINE);
        }

        checkFramebufferStatus();
    } else {

        /*
         * bind FBO
         */
        if (_myFrameBufferObject[1]) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFrameBufferObject[1]);
            glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
        } else {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myFrameBufferObject[0]);
        }
    }
#else
    throw OpenGLException("GL_EXT_framebuffer_object support not compiled", PLUS_FILE_LINE);
#endif
}

}
