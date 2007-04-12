//=============================================================================
//
// Copyright (C) 2000-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

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

}
