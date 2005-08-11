
#include "OffScreenBuffer.h"

#include <y60/Image.h>
#include <y60/PixelEncodingInfo.h>
#include <asl/Logger.h>


using namespace dom;

namespace y60 {


OffScreenBuffer::OffScreenBuffer(bool theUseGLFramebufferObject) 
    : _myUseGLFramebufferObject(theUseGLFramebufferObject),
      _myOffScreenBuffer(0), _myDepthBuffer(0)
{
}

void OffScreenBuffer::preOffScreenRender( ImagePtr theTexture) {
    AC_TRACE << "OffScreenBuffer::preOffScreenRender " 
             << " w/ gl framebuffer extension " << _myUseGLFramebufferObject;
    if (_myUseGLFramebufferObject) {
        bindOffScreenFrameBuffer(theTexture);
    }
}


void OffScreenBuffer::postOffScreenRender( ImagePtr theTexture, bool theCopyToImageFlag) {
    AC_TRACE << "OffScreenBuffer::postOffScreenRender " 
             << " w/ gl framebuffer extension " << _myUseGLFramebufferObject;
    if (_myUseGLFramebufferObject) {
        bindTexture(theTexture);
    } else {
        copyFrameBufferToTexture(theTexture);
    }

    if (theCopyToImageFlag) {
        copyTextureToImage(theTexture);
    }
}

void OffScreenBuffer::copyTextureToImage(ImagePtr theImage) {
    AC_TRACE << "OffScreenBuffer::copyTextureToImage ";

#ifdef GL_EXT_framebuffer_object
    if (_myUseGLFramebufferObject) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffScreenBuffer);
    }
#endif

    PixelEncodingInfo myPixelEncodingInfo = getDefaultGLTextureParams(theImage->getEncoding());

    glReadPixels(0, 0, theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>(),
                myPixelEncodingInfo.externalformat, myPixelEncodingInfo.pixeltype,
                theImage->getRasterPtr()->pixels().begin());

    theImage->getRasterValueNode()->bumpVersion();

#ifdef GL_EXT_framebuffer_object
    if (_myUseGLFramebufferObject) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    }
#endif
}


void OffScreenBuffer::copyFrameBufferToTexture(ImagePtr theTexture) {
    AC_TRACE << "OffScreenBuffer::copyFrameBufferToTexture ";
    glBindTexture (GL_TEXTURE_2D, theTexture->getGraphicsId() );
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0 /*MIPMAP levels*/, 0, 0,
            0, 0, theTexture->get<ImageWidthTag>(), theTexture->get<ImageHeightTag>() );
    glBindTexture (GL_TEXTURE_2D, 0);
}

void OffScreenBuffer::bindOffScreenFrameBuffer(ImagePtr theTexture) {
    AC_TRACE << "OffScreenBuffer::bindOffScreenFrameBuffer ";
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
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 
                theTexture->get<ImageWidthTag>(), theTexture->get<ImageHeightTag>() );

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

void OffScreenBuffer::bindTexture(ImagePtr theTexture) {
    AC_TRACE << "OffScreenBuffer::bindTexture ";
#ifdef GL_EXT_framebuffer_object
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
    glBindTexture (GL_TEXTURE_2D, theTexture->getGraphicsId() );
}


}
