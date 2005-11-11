
#include "OffScreenBuffer.h"
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
        copyFrameBufferToImage(theTexture);
    }

}

void OffScreenBuffer::copyFrameBufferToImage(ImagePtr theImage) {
    AC_TRACE << "OffScreenBuffer::copyFrameBufferToImage ";

#ifdef GL_EXT_framebuffer_object
    if (_myUseGLFramebufferObject) {
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _myOffScreenBuffer);
    }
#endif

    PixelEncodingInfo myPixelEncodingInfo = getDefaultGLTextureParams(theImage->getEncoding());
    std::cout <<"RGB: " << (myPixelEncodingInfo.externalformat==GL_RGB) << std::endl;
    std::cout <<"DEPTH: " << (myPixelEncodingInfo.externalformat==GL_DEPTH_COMPONENT) << std::endl;
    std::cout <<"RGBA: " << (myPixelEncodingInfo.externalformat==GL_RGBA) << std::endl;
    std::cout <<"bytes per pixel: " << myPixelEncodingInfo.bytesPerPixel << std::endl;
    std::cout <<"GLFLOAT: " << (myPixelEncodingInfo.pixeltype==GL_FLOAT) << std::endl;

    glReadPixels(0, 0, theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>(),
                myPixelEncodingInfo.externalformat, myPixelEncodingInfo.pixeltype,
                theImage->getRasterPtr()->pixels().begin());
#ifdef DUMP_BUFFER
float myTestBuffer[512*512];
/*unsigned short * myDest = &myTestBuffer[0];
unsigned long * myStartPtr = (unsigned long*)theImage->getRasterPtr()->pixels().begin();

for (int x = 0 ; x < theImage->get<ImageWidthTag>(); x++) {
    for (int y = 0 ; y < theImage->get<ImageHeightTag>(); y++) {        
        unsigned long myValue= *myStartPtr;
        *myDest = (myValue>>16);
        myStartPtr++;
        myDest++;
    }
}
*/
//FILE * myFd = fopen("buffer.raw", "w");
//fwrite(theImage->getRasterPtr()->pixels().begin(), theImage->get<ImageWidthTag>() * theImage->get<ImageHeightTag>() * 4 ,1, myFd);
//fwrite(&myTestBuffer[0], theImage->get<ImageWidthTag>() * theImage->get<ImageHeightTag>() * 4,1, myFd);
//fclose(myFd);
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
// texture is already uploaded, either by bindTexture(..) or by copyFrameBufferToTexture(..) VS/UH
    //theImage->getRasterValueNode()->bumpVersion(); 

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

    GLenum myStatus;
    myStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    bool isOK(true);
    ostringstream os;
    switch(myStatus) {                                          
        case GL_FRAMEBUFFER_COMPLETE_EXT: // Everything's OK
            isOK = true;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT" << endl;
            isOK = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT" << endl;
            isOK = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT" << endl;
            isOK = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT" << endl;
            isOK = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT" << endl;
            isOK = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT" << endl;
            isOK = false;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            os << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT" << endl;
            isOK = false;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            os << "GL_FRAMEBUFFER_UNSUPPORTED_EXT" << endl;
            isOK = false;
            break;
        /* [DS] not found in current headers
        case GL_FRAMEBUFFER_STATUS_ERROR_EXT:
            os << "GL_FRAMEBUFFER_STATUS_ERROR_EXT" << endl;
            isOK = false;
            break;
        */
        default:
            /* programming error; will fail on all hardware */
            throw OpenGLException("GL_FRAMEBUFFER_EXT status broken, got "
                        + asl::as_string(myStatus), PLUS_FILE_LINE);
    }

    if (! isOK) {
        throw OffscreenRendererException(os.str(), PLUS_FILE_LINE);
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
