//============================================================================
//
// Copyright (C) 2002-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef INCL_TEXTURE_COMPRESSION_TYPE
#define INCL_TEXTURE_COMPRESSION_TYPE

#include <asl/string_functions.h>
#include <asl/Exception.h>
#include <y60/PixelEncoding.h>


#include <string>
#include <stdio.h>

#include "GLUtils.h"

namespace y60 {

    DEFINE_EXCEPTION(GLUtilException, asl::Exception);

    struct PixelEncodingInfo {
        PixelEncodingInfo(GLenum theInternalFormat,  float theBytesPerPixel,
                          bool theCompressedFlag, GLenum theExternalFormat, 
                          GLenum thePixelType=GL_UNSIGNED_BYTE) :
            externalformat(theExternalFormat),
            bytesPerPixel(theBytesPerPixel),
            compressedFlag(theCompressedFlag),
            internalformat(theInternalFormat),
            pixeltype(thePixelType)
        {}

        GLenum   externalformat;
        GLenum   internalformat;
        GLenum   pixeltype;
        float    bytesPerPixel;
        bool     compressedFlag;
    };
    PixelEncodingInfo getDefaultGLTextureParams(PixelEncoding theEncoding);
}
#endif
