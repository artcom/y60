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
#include "TextureCompressor.h"

#include <iostream>
#include <cstring>
#include <errno.h>
#include <stdexcept>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include <asl/base/settings.h>
#include <asl/base/file_functions.h>
#include <asl/base/Logger.h>
#include <asl/math/numeric_functions.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/pltiffenc.h>
#include <paintlib/Filter/plfilterflip.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "GLUtils.h"
#include "PixelEncodingInfo.h"

namespace y60 {

    using namespace std;


    TextureCompressor::TextureCompressor (PixelEncoding myFormat)
        : _myFormat(myFormat)
    {
        unsigned int myGlewError = glewInit();
        if (GLEW_OK != myGlewError) {
            AC_ERROR << "Glew Initialization Error: " << glewGetErrorString(myGlewError);
            throw;
        }
    }

    TextureCompressor::~TextureCompressor() {}

    int
    TextureCompressor::compress(const PLAnyBmp& AnyBmp, asl::VariableCapacityBlock & theDestBlock) {
        int compressedSize = 0;
        PixelEncodingInfo myPixelEncoding = getDefaultGLTextureParams(_myFormat);

        int myInternalFormat = myPixelEncoding.internalformat;
        if (myInternalFormat > 0) {
            int myPixelFormat;
            switch (AnyBmp.GetBitsPerPixel()) {
                case 32:
#if defined(PL_PIXEL_RGBA_ORDER)                    
                    myPixelFormat = GL_RGBA;
#elif defined(PL_PIXEL_BGRA_ORDER)                   
                    myPixelFormat = GL_BGRA;
#else
  #error Pixel RGB bytes order not selected
#endif                   
                    break;
                case 24:
#if defined(PL_PIXEL_RGBA_ORDER)                    
                    myPixelFormat = GL_RGB;
#elif defined(PL_PIXEL_BGRA_ORDER)                   
                    myPixelFormat = GL_BGR;
#else
  #error Pixel RGB bytes order not selected
#endif                   
                    break;
                default:
                    throw GLTextureCompressionException(string("Cannot compress texture with ") +
                                                        asl::as_string(AnyBmp.GetBitsPerPixel()) +
                                                        " bits per pixel.", PLUS_FILE_LINE);
            }

            cerr << "using gl internal format:" << myInternalFormat << endl;
            glTexImage2D(GL_TEXTURE_2D, 0, myInternalFormat,
                    AnyBmp.GetWidth(), AnyBmp.GetHeight(),
                    0, myPixelFormat, GL_UNSIGNED_BYTE, (GLubyte *)(AnyBmp.GetPixels()));

            int isCompressed = 0;

#ifdef GL_ARB_texture_compression

            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_ARB, (GLint*)&isCompressed);
            if (isCompressed == GL_TRUE) {
    #ifdef GL_TEXTURE_IMAGE_SIZE_ARB
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_IMAGE_SIZE_ARB,
                        &compressedSize);
                AC_DEBUG << "Retrieved size (a): " << compressedSize <<endl;
    #endif

    #ifdef GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB,
                        (GLint*)&compressedSize);
                AC_DEBUG << "Retrieved size (b): " << compressedSize <<endl;
    #endif
                unsigned myCurrentBufferSize = theDestBlock.size();
                theDestBlock.resize(myCurrentBufferSize + compressedSize);
                glGetCompressedTexImageARB(GL_TEXTURE_2D, 0, theDestBlock.begin() + myCurrentBufferSize);
            }

        }
#endif
        return compressedSize;
    }
}

