//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or dupli:mcated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: TextureCompressor.cpp,v $
//     $Author: pavel $
//   $Revision: 1.15 $
//
// Description: OpenGL texture compression utilities
//
//=============================================================================

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

#include "TextureCompressor.h"
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
                    if (PL_RGBA_RED == 0) {
                        myPixelFormat = GL_RGBA;
                    } else {
                        myPixelFormat = GL_BGRA;
                    }
                    break;
                case 24:
                    if (PL_RGBA_RED == 0) {
                        myPixelFormat = GL_RGB;
                    } else {
                        myPixelFormat = GL_BGR;
                    }
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

