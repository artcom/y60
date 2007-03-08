//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "GLBufferAdapter.h"
#include "GLUtils.h"
#include "PixelEncodingInfo.h"

#include <asl/string_functions.h>
#include <y60/PixelEncoding.h>

#include <paintlib/plpngenc.h>
#include <paintlib/pljpegenc.h>
#include <paintlib/plbmpenc.h>
#include <paintlib/pltiffenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/plfilterfliprgb.h>

#include <iostream>

#ifdef WIN32
#   include <windows.h>
#endif

#include <GL/gl.h>

using namespace std;
using namespace asl;

namespace y60 {

    BufferAdapter::BufferAdapter (unsigned theWidth, unsigned theHeight, unsigned theComponents) :
                _myWidth(theWidth), _myHeight(theHeight), _myComponents(theComponents) {
        alloc(_myWidth * _myHeight * _myComponents);
    }
    BufferAdapter::~BufferAdapter() {
    }

    void 
    BufferAdapter::alloc(const unsigned myMemorySize) {
        _myData.resize(myMemorySize);
    }
        
    asl::Block &
    BufferAdapter::getBlock(){
        return _myData;
    }

    unsigned 
    BufferAdapter::getWidth() const {
        return _myWidth;
    }

    unsigned 
    BufferAdapter::getHeight() const {
        return _myHeight;
    }

    unsigned 
    BufferAdapter::getComponents() const {
        return _myComponents;
    }

    unsigned
    BufferAdapter::getBufferFormat(GLSourceBuffer theSourceBuffer) const {
        GLenum myFormat = 0;
        switch (theSourceBuffer) {
            case FRAME_BUFFER:
                switch (getComponents()) {
                    case 1:
                        myFormat = GL_LUMINANCE;
                        break;
                    case 2:
                        myFormat = GL_LUMINANCE_ALPHA;
                        break;
                    case 3:
                        myFormat = GL_RGB;
                        break;
                    case 4:
                        myFormat = GL_RGBA;
                        break;
                    default:
                        throw GLBufferAdapterException(std::string("Unsupported number of components: " ) + asl::as_string(getComponents()), PLUS_FILE_LINE);
                }
                break;
            case DEPTH_BUFFER:
                myFormat = GL_DEPTH_COMPONENT;
                break;
            default:
                throw GLBufferAdapterException(std::string("Unknown Sourcebuffer: " ) + asl::as_string(theSourceBuffer), PLUS_FILE_LINE);
        }
        return myFormat;
    }
    
    void
    BufferAdapter::performAction(GLSourceBuffer theSourceBuffer) {
        GLenum myFormat = getBufferFormat(theSourceBuffer);
        if (theSourceBuffer == FRAME_BUFFER) {
            glReadBuffer(GL_BACK);
        }
        glReadPixels(0, 0, _myWidth, _myHeight, myFormat, GL_UNSIGNED_BYTE, _myData.begin());
    }

    // ----------------------------------------------------------------------------------------    
    BufferToFile::BufferToFile(const std::string & theFilename, unsigned theFormat,
            unsigned theWidth, unsigned theHeight, unsigned theComponents) : 
        BufferAdapter(theWidth, theHeight, theComponents),
        _myFilename(theFilename), _myFormat(theFormat)
    {
    }
    
    BufferToFile::~BufferToFile() {
    }    

    void
    BufferToFile::performAction(GLSourceBuffer theSourceBuffer)
    {
        BufferAdapter::performAction(theSourceBuffer);

        Path myPath(_myFilename, UTF8);
        PLAnyPicDecoder myDecoder;
        PLAnyBmp myBmp;
        PixelEncoding myEncoding;

        switch(getComponents()) {
          case 1:
              myEncoding = GRAY;
              break;
          case 3:
              myEncoding = RGB;
              break;
          case 4:
          default:
              myEncoding = RGBA;
              break;
        }

        PLPixelFormat pf;
        if (!mapPixelEncodingToFormat(myEncoding, pf)) {
            throw GLBufferAdapterException(std::string("unsupported pixel format"), PLUS_FILE_LINE); 
        }
        myBmp.Create( getWidth(), getHeight(), pf, 
                      getBlock().begin() + getWidth() * (getHeight()-1) * getComponents(), -1 * getWidth() * getComponents());

        switch(_myFormat) {
            case PL_FT_PNG:
                {
                    PLPNGEncoder myEncoder;
                    myEncoder.MakeFileFromBmp(myPath.toLocale().c_str(), &myBmp);               
                }
                break;
            case PL_FT_JPEG:
                {
                    PLJPEGEncoder myEncoder;
                    myBmp.ApplyFilter(PLFilterFlipRGB());
                    myEncoder.MakeFileFromBmp(myPath.toLocale().c_str(), &myBmp);               
                }
                break;
            case PL_FT_WINBMP:
                {
                    PLBmpEncoder myEncoder;
                    myBmp.ApplyFilter(PLFilterFlipRGB());
                    myEncoder.MakeFileFromBmp(myPath.toLocale().c_str(), &myBmp);               
                }
                break;
            case PL_FT_TIFF:
                {
                    PLTIFFEncoder myEncoder;
                    myBmp.ApplyFilter(PLFilterFlipRGB());
                    myEncoder.MakeFileFromBmp(myPath.toLocale().c_str(), &myBmp);               
                }
                break;
             default:
                throw GLBufferAdapterException(std::string("Unknown target image format: " ) + asl::as_string(_myFormat),
                                               PLUS_FILE_LINE);
        }
    }        

    // ----------------------------------------------------------------------------------------    
    BufferToImage::BufferToImage(ImagePtr theImage, const asl::Vector2i & theOffset, bool theCopyToRasterFlag) :
        BufferAdapter(theImage->get<ImageWidthTag>(), theImage->get<ImageHeightTag>(), 4),
        _myImage(theImage), _myOffset(theOffset), _myCopyToRasterFlag(theCopyToRasterFlag)
    {
    }

    BufferToImage::~BufferToImage() {
    }

    void
    BufferToImage::performAction(GLSourceBuffer theSourceBuffer)
    {
        if (theSourceBuffer == FRAME_BUFFER) {
            glReadBuffer(GL_BACK);
        }

        if (_myCopyToRasterFlag) {
            AC_DEBUG << "BufferToImage::performAction copy framebuffer to Image raster";

            // copy framebuffer to Image raster
            PixelEncodingInfo myPixelEncodingInfo = getDefaultGLTextureParams(_myImage->getRasterEncoding());
            myPixelEncodingInfo.internalformat = asGLTextureInternalFormat(_myImage->getInternalEncoding());

            glReadPixels(_myOffset[0],_myOffset[1], getWidth(),getHeight(),
                    myPixelEncodingInfo.externalformat, myPixelEncodingInfo.pixeltype,
                    _myImage->getRasterPtr()->pixels().begin());
            CHECK_OGL_ERROR;

            _myImage->triggerUpload();
        } else {
            AC_DEBUG << "BufferToImage::performAction copy framebuffer to Image '" << _myImage->get<NameTag>() << "' id=" << _myImage->get<IdTag>() << " texid=" << _myImage->getGraphicsId() << " size=" << getWidth() << "x" << getHeight();

            // copy framebuffer to texture
            glBindTexture(GL_TEXTURE_2D, _myImage->ensureTextureId());
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, _myOffset[0],_myOffset[1], getWidth(),getHeight());
            CHECK_OGL_ERROR;

            // generate mipmap levels
            if (IS_SUPPORTED(glGenerateMipmapEXT) && _myImage->get<ImageMipmapTag>()) {
                AC_TRACE << "BufferToImage::performAction: generating mipmap levels";
                glGenerateMipmapEXT(GL_TEXTURE_2D);
                CHECK_OGL_ERROR;
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}
