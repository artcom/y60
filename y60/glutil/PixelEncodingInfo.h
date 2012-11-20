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
*/

#ifndef INCL_TEXTURE_COMPRESSION_TYPE
#define INCL_TEXTURE_COMPRESSION_TYPE

#include "y60_glutil_settings.h"

#include <asl/base/string_functions.h>
#include <asl/base/Exception.h>
#include <y60/image/PixelEncoding.h>


#include <string>
#include <stdio.h>
#include <iostream>

#include "GLUtils.h"

namespace y60 {

    DEFINE_EXCEPTION(GLUtilException, asl::Exception);

    struct PixelEncodingInfo {
        PixelEncodingInfo(GLenum theInternalFormat,
                          float theBytesPerPixel,
                          bool theCompressedFlag,
                          GLenum theExternalFormat,
                          GLenum thePixelType=GL_UNSIGNED_BYTE)
            : internalformat(theInternalFormat),
            bytesPerPixel(theBytesPerPixel),
            compressedFlag(theCompressedFlag),
            externalformat(theExternalFormat),
            pixeltype(thePixelType)
        {}

        GLenum   internalformat;
        float    bytesPerPixel;
        bool     compressedFlag;
        GLenum   externalformat;
        GLenum   pixeltype;
    };

    Y60_GLUTIL_DECL PixelEncodingInfo getDefaultGLTextureParams(PixelEncoding theEncoding);
    Y60_GLUTIL_DECL std::string getGLEnumString(GLenum theFormat);

    inline
    std::ostream & operator<<(std::ostream & os, const PixelEncodingInfo & theInfo) {
        os << "externalformat: " << getGLEnumString(theInfo.externalformat);
        os << ", internalformat: " << getGLEnumString(theInfo.internalformat);
        os << ", pixeltype: " << getGLEnumString(theInfo.pixeltype);
        os << ", bytesPerPixel: " << theInfo.bytesPerPixel;
        os << ", compressedFlag: " << theInfo.compressedFlag;
        return os;
    }
 }
#endif
