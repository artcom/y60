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

// own header
#include "PixelEncodingInfo.h"


using namespace std;

namespace y60 {

    PixelEncodingInfo
    getDefaultGLTextureParams(PixelEncoding theEncoding) {
        switch (theEncoding) {
            case y60::DEPTH: return PixelEncodingInfo(GL_DEPTH_COMPONENT, 4, false, GL_DEPTH_COMPONENT, GL_FLOAT);
            case y60::ALPHA: return PixelEncodingInfo(GL_ALPHA, 1, false, GL_ALPHA);
            case y60::GRAY: return PixelEncodingInfo(GL_LUMINANCE8, 1, false, GL_LUMINANCE); // maybe we have to put GL_LUMINANCE or GL_ALPHA here
            case y60::GRAY16: return PixelEncodingInfo(GL_LUMINANCE16, 2, false, GL_LUMINANCE, GL_UNSIGNED_SHORT);
            case y60::GRAYS16:return PixelEncodingInfo(GL_LUMINANCE16, 2, false, GL_LUMINANCE, GL_SHORT);
            case y60::GRAY32: return PixelEncodingInfo(GL_INTENSITY, 4, false, GL_INTENSITY, GL_UNSIGNED_INT);
            case y60::RGB:  return PixelEncodingInfo(GL_RGB8, 3, false, GL_RGB);
            case y60::BGR:  return PixelEncodingInfo(GL_RGB8, 3, false, GL_BGR);
            case y60::RGBA: return PixelEncodingInfo(GL_RGBA8, 4, false, GL_RGBA);
            case y60::BGRA: return PixelEncodingInfo(GL_RGBA8, 4, false, GL_BGRA);
            case y60::GENERIC_RGB:             return PixelEncodingInfo(GL_COMPRESSED_RGB_ARB, 4, true, GL_COMPRESSED_RGB_ARB);
            case y60::GENERIC_RGBA:            return PixelEncodingInfo(GL_COMPRESSED_RGBA_ARB, 4, true, GL_COMPRESSED_RGBA_ARB);
            case y60::GENERIC_ALPHA:           return PixelEncodingInfo(GL_COMPRESSED_ALPHA_ARB, 4, true, GL_COMPRESSED_ALPHA_ARB);
            case y60::GENERIC_LUMINANCE:       return PixelEncodingInfo(GL_COMPRESSED_LUMINANCE_ARB, 4, true, GL_COMPRESSED_LUMINANCE_ARB);
            case y60::GENERIC_LUMINANCE_ALPHA: return PixelEncodingInfo(GL_COMPRESSED_LUMINANCE_ALPHA_ARB, 1, true, GL_COMPRESSED_LUMINANCE_ALPHA_ARB);
            case y60::GENERIC_INTENSITY:       return PixelEncodingInfo(GL_COMPRESSED_INTENSITY_ARB, 1, true, GL_COMPRESSED_INTENSITY_ARB);
            case y60::S3TC_DXT1:               return PixelEncodingInfo(GL_COMPRESSED_RGB_S3TC_DXT1_EXT, 0.5, true, GL_COMPRESSED_RGB_S3TC_DXT1_EXT);
            case y60::S3TC_DXT1A:              return PixelEncodingInfo(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, 0.5, true, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT);
            case y60::S3TC_DXT3:               return PixelEncodingInfo(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, 1, true, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT);
            case y60::S3TC_DXT5:               return PixelEncodingInfo(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, 1, true, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT);
            case y60::RGBA_HALF:               return PixelEncodingInfo(GL_RGBA16F_ARB, 8, false, GL_RGBA, GL_HALF_FLOAT_ARB);
            case y60::RGB_HALF:                return PixelEncodingInfo(GL_RGB16F_ARB, 6, false, GL_RGB, GL_HALF_FLOAT_ARB);
            case y60::RGBA_FLOAT:              return PixelEncodingInfo(GL_RGBA_FLOAT32_ATI, 16, false, GL_RGBA, GL_FLOAT);
            case y60::RGB_FLOAT:               return PixelEncodingInfo(GL_RGB_FLOAT32_ATI, 12, false, GL_RGB, GL_FLOAT);
            default:
                throw GLUtilException(string("Unsupported texture pixelformat ") +
                        asl::getStringFromEnum(theEncoding, y60::PixelEncodingString), PLUS_FILE_LINE);
        }
    }
    std::string getGLEnumString(GLenum theFormat) {
        switch (theFormat) {
            case GL_DEPTH_COMPONENT: return "GL_DEPTH_COMPONENT";
            case GL_ALPHA: return "GL_ALPHA";
            case GL_LUMINANCE: return "GL_LUMINANCE";
            case GL_LUMINANCE8: return "GL_LUMINANCE8";
            case GL_LUMINANCE16: return "GL_LUMINANCE16";
            case GL_INTENSITY: return "GL_INTENSITY";
            case GL_RGB8: return "GL_RGB8";
            case GL_RGB: return "GL_RGB";
            case GL_BGR: return "GL_BGR";
            case GL_RGBA8: return "GL_RGBA8";
            case GL_BGRA: return "GL_BGRA";
            case GL_RGBA: return "GL_RGBA";
            case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
            case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
            case GL_SHORT: return "GL_SHORT";
            case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";
            case GL_COMPRESSED_RGB_ARB: return "GL_COMPRESSED_RGB_ARB";
            case GL_COMPRESSED_RGBA_ARB: return "GL_COMPRESSED_RGBA_ARB";
            case GL_COMPRESSED_ALPHA_ARB: return "GL_COMPRESSED_ALPHA_ARB";
            case GL_COMPRESSED_LUMINANCE_ARB: return "GL_COMPRESSED_LUMINANCE_ARB";
            case GL_COMPRESSED_LUMINANCE_ALPHA_ARB: return "GL_COMPRESSED_LUMINANCE_ALPHA_ARB";
            case GL_COMPRESSED_INTENSITY_ARB: return "GL_COMPRESSED_INTENSITY_ARB";
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: return "GL_COMPRESSED_RGB_S3TC_DXT1_EXT";
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: return "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT";
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: return "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT";
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: return "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT";
            case GL_RGBA16F_ARB: return "GL_RGBA16F_ARB";
            case GL_RGB16F_ARB: return "GL_RGB16F_ARB";
            case GL_RGBA_FLOAT32_ATI: return "GL_RGBA_FLOAT32_ATI";
            case GL_RGB_FLOAT32_ATI: return "GL_RGB_FLOAT32_ATI";
            case GL_HALF_FLOAT_ARB: return "GL_HALF_FLOAT_ARB";
            case GL_FLOAT: return "GL_FLOAT";
            default:return std::string("UNKNOWN_GL_ENUM=0x")+asl::as_string(asl::as_hex(theFormat));
        }
    }
}
