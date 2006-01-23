//============================================================================
//
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

void
checkFramebufferStatus() {
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

OffscreenBuffer::OffscreenBuffer(bool theUseGLFramebufferObject) 
    : _myUseGLFramebufferObject(theUseGLFramebufferObject),
      _myOffscreenBuffer(0), _myDepthBuffer(0)
{
}

void OffscreenBuffer::preOffscreenRender( ImagePtr theTexture) {
    AC_TRACE << "OffscreenBuffer::preOffscreenRender " 
             << "w/ GL framebuffer extension " << _myUseGLFramebufferObject;
    if (_myUseGLFramebufferObject) {
        bindOffscreenFrameBuffer(theTexture);
    }
}


void OffscreenBuffer::postOffscreenRender( ImagePtr theTexture, bool theCopyToImageFlag) {
    AC_TRACE << "OffscreenBuffer::postOffscreenRender " 
             << "w/ GL framebuffer extension " << _myUseGLFramebufferObject;

    if (_myUseGLFramebufferObject) {
#ifdef GL_EXT_framebuffer_object
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
        glBindTexture (GL_TEXTURE_2D, theTexture->getGraphicsId() );
#ifdef GL_EXT_framebuffer_object
        if (glGenerateMipmapEXT && theTexture->get<ImageMipmapTag>()) {
            AC_TRACE << "OffscreenBuffer::postOffscreenRender: generating mipmap levels";
            glGenerateMipmapEXT(GL_TEXTURE_2D);
        }
#endif
    } else {
        copyFrameBufferToTexture(theTexture);
    }

    if (theCopyToImageFlag) {
        copyFrameBufferToImage(theTexture);
    }
 
    // cleanly unbind the texture
    if (_myUseGLFramebufferObject) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


void OffscreenBuffer::copyFrameBufferToImage(ImagePtr theImage) {
    AC_TRACE << "OffscreenBuffer::copyFrameBufferToImage";

#ifdef GL_EXT_framebuffer_object
    if (_myUseGLFramebufferObject) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffscreenBuffer);
    }
#endif
 
    PixelEncodingInfo myPixelEncodingInfo = getDefaultGLTextureParams(theImage->getEncoding());

    AC_DEBUG << "RGBA: " << (myPixelEncodingInfo.externalformat==GL_RGBA);
    AC_DEBUG << "RGB:  " << (myPixelEncodingInfo.externalformat==GL_RGB);
    AC_DEBUG << "DEPTH:" << (myPixelEncodingInfo.externalformat==GL_DEPTH_COMPONENT);
    AC_DEBUG << "pixeltype is GLFLOAT: " << (myPixelEncodingInfo.pixeltype==GL_FLOAT);
    AC_DEBUG << "bytes-per-pixel=" << myPixelEncodingInfo.bytesPerPixel;

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

    // texture is already uploaded, either by bindTexture(..) or
    // by copyFrameBufferToTexture(..) VS/UH
    //theImage->getRasterValueNode()->bumpVersion(); 

#ifdef GL_EXT_framebuffer_object
    if (_myUseGLFramebufferObject) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
#endif
}


void OffscreenBuffer::copyFrameBufferToTexture(ImagePtr theTexture) {
    AC_TRACE << "OffscreenBuffer::copyFrameBufferToTexture ";
    glBindTexture (GL_TEXTURE_2D, theTexture->getGraphicsId() );
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0 /*MIPMAP level*/, 0, 0,
            0, 0, theTexture->get<ImageWidthTag>(), theTexture->get<ImageHeightTag>() );
    glBindTexture (GL_TEXTURE_2D, 0);
}

void OffscreenBuffer::bindOffscreenFrameBuffer(ImagePtr theTexture) {
    AC_TRACE << "OffscreenBuffer::bindOffscreenFrameBuffer";
#ifdef GL_EXT_framebuffer_object
    if ( ! _myOffscreenBuffer) {
        glGenFramebuffersEXT(1, &_myOffscreenBuffer);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffscreenBuffer);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
            GL_TEXTURE_2D, theTexture->getGraphicsId(), 0);

        //we need a depth buffer as well
        glGenRenderbuffersEXT(1, &_myDepthBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _myDepthBuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 
                theTexture->get<ImageWidthTag>(), theTexture->get<ImageHeightTag>() );

        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _myDepthBuffer);

    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffscreenBuffer);
    }

    checkFramebufferStatus();
#else
    throw OpenGLException("GL_EXT_framebuffer_object support not compiled", PLUS_FILE_LINE);
#endif
}
}
