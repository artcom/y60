//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: GLBufferAdapter.cpp,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/03/09 17:10:35 $
//
//  Description: This class performs buffer magic.
//
//=============================================================================

#include "GLBufferAdapter.h"

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

    BufferAdaptor::BufferAdaptor (unsigned theWidth, unsigned theHeight, unsigned theComponents) :
                _myWidth(theWidth), _myHeight(theHeight), _myComponents(theComponents) {
        alloc(_myWidth * _myHeight * _myComponents);
    }
    BufferAdaptor::~BufferAdaptor() {
    }

    void 
    BufferAdaptor::alloc(const unsigned myMemorySize) {
        _myData.resize(myMemorySize);
    }
        
    asl::Block &
    BufferAdaptor::getBlock(){
        return _myData;
    }

    unsigned 
    BufferAdaptor::getWidth() const {
        return _myWidth;
    }

    unsigned 
    BufferAdaptor::getHeight() const {
        return _myHeight;
    }

    unsigned 
    BufferAdaptor::getComponents() const {
        return _myComponents;
    }
    
    void
    BufferAdaptor::performAction(GLSourceBuffer theSourceBuffer) {
        GLenum myFormat;
        switch (theSourceBuffer) {
        case FRAME_BUFFER:
            glReadBuffer(GL_BACK);
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
            }
            break;
        case DEPTH_BUFFER:
            myFormat = GL_DEPTH_COMPONENT;
            break;
        default:
            throw GLBufferAdapterException(std::string("Unknown Sourcebuffer: " ) + asl::as_string(theSourceBuffer), PLUS_FILE_LINE);
        }

        glReadPixels(0, 0, _myWidth, _myHeight, myFormat, GL_UNSIGNED_BYTE, _myData.begin());
    }

    // ----------------------------------------------------------------------------------------    
    BufferToFile::BufferToFile (unsigned theWidth, unsigned theHeight, unsigned theComponents) : 
                BufferAdaptor(theWidth, theHeight, theComponents) {
    }
    
    BufferToFile::~BufferToFile() {
    }    
    
    bool 
    BufferToFile::saveBufferAsImage(unsigned theFormat, const std::string & theFileName) {
        Path myPath(theFileName, UTF8);
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
              myEncoding = RGBA;
              break;
        }
        PLPixelFormat pf;
        if (!mapPixelEncodingToFormat(myEncoding, pf)) {
            throw GLBufferAdapterException(std::string("unsupported pixel format"), PLUS_FILE_LINE); 
        }
        myBmp.Create( getWidth(), getHeight(), pf, 
                      getBlock().begin() + getWidth() * (getHeight()-1) * getComponents(), -1 * getWidth() * getComponents());
        switch(theFormat) {
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
                throw GLBufferAdapterException(std::string("Unknown target image format: " ) + asl::as_string(theFormat),
                                               PLUS_FILE_LINE);
        }
        return true;
    }        
}
