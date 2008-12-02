//============================================================================
// Copyright (C) 2000-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "PixelEncoding.h"

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixelformat.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace y60 {

bool mapPixelEncodingToFormat(const PixelEncoding & theEncoding,
            PLPixelFormat & theFormat)
{
    switch (theEncoding) {
        case GRAY:
            theFormat = PLPixelFormat::L8;
            break;
        case GRAY16:
            theFormat = PLPixelFormat::L16;
            break;
        case GRAYS16:
            theFormat = PLPixelFormat::L16S;
            break;
        case GRAY32:
            theFormat = PLPixelFormat::A8R8G8B8;
            break;
        case RGB:
		    theFormat = PLPixelFormat::B8G8R8; // PLPixelFormats are backwards
            break;
        case BGR:
			theFormat = PLPixelFormat::R8G8B8; // PLPixelFormats are backwards
            break;
        case RGBA:
			theFormat = PLPixelFormat::A8B8G8R8; // PLPixelFormats are backwards
            break;
        case BGRA:
			theFormat = PLPixelFormat::A8R8G8B8; // PLPixelFormats are backwards
            break;
        case RGBA_HALF:
			theFormat = PLPixelFormat::A16B16G16R16; // PLPixelFormats are backwards
            break;
        case RGB_HALF:
			theFormat = PLPixelFormat::B16G16R16; // PLPixelFormats are backwards
            break;
        case RGBA_FLOAT:
			theFormat = PLPixelFormat::A32B32G32R32; // PLPixelFormats are backwards
            break;
        case RGB_FLOAT:
			theFormat = PLPixelFormat::B32G32R32; // PLPixelFormats are backwards
            break;
        default:
            return false;
    }
    return true;
}

bool mapFormatToPixelEncoding(const PLPixelFormat & theFormat,
        PixelEncoding & theEncoding)
{
    if (theFormat == PLPixelFormat::L8) {
        theEncoding = GRAY;
    } else if (theFormat == PLPixelFormat::L16) {
        theEncoding = GRAY16;
    } else if (theFormat == PLPixelFormat::L16S) {
        theEncoding = GRAYS16;
    } else if (theFormat == PLPixelFormat::B8G8R8) {
        theEncoding = RGB;
    } else if (theFormat == PLPixelFormat::R8G8B8) {
        theEncoding = BGR;
    } else if (theFormat == PLPixelFormat::A8R8G8B8) {
        theEncoding = BGRA;
    } else if (theFormat == PLPixelFormat::X8R8G8B8) {
        theEncoding = BGRA;
    } else if (theFormat == PLPixelFormat::A8B8G8R8) {
        theEncoding = RGBA;
    } else if (theFormat == PLPixelFormat::X8B8G8R8) {
        theEncoding = RGBA;
    } else if (theFormat == PLPixelFormat::A16B16G16R16) {
        theEncoding = RGBA_HALF;
    } else if (theFormat == PLPixelFormat::B16G16R16) {
        theEncoding = RGB_HALF;
    } else if (theFormat == PLPixelFormat::A32B32G32R32) {
        theEncoding = RGBA_FLOAT;
    } else if (theFormat == PLPixelFormat::B32G32R32) {
        theEncoding = RGB_FLOAT;
    } else {
        return false;
    }
    return true;
}

TextureInternalFormat
getInternalPixelFormat(PixelEncoding theEncoding) {
    switch (theEncoding) {
        case y60::DEPTH: return TEXTURE_IFMT_DEPTH; 
        case y60::ALPHA: return TEXTURE_IFMT_ALPHA; 
        case y60::GRAY: return TEXTURE_IFMT_LUMINANCE8;
        case y60::GRAY16: return TEXTURE_IFMT_LUMINANCE16;
        case y60::GRAYS16:return TEXTURE_IFMT_LUMINANCE16;
        case y60::GRAY32: return TEXTURE_IFMT_INTENSITY;
        case y60::RGB:  return TEXTURE_IFMT_RGB8;
        case y60::BGR:  return TEXTURE_IFMT_RGB8;
        case y60::RGBA: return TEXTURE_IFMT_RGBA8;
        case y60::BGRA: return TEXTURE_IFMT_RGBA8;
        case y60::GENERIC_RGB:             return TEXTURE_IFMT_COMPRESSED_RGB_ARB;
        case y60::GENERIC_RGBA:            return TEXTURE_IFMT_COMPRESSED_RGBA_ARB;
        case y60::GENERIC_ALPHA:           return TEXTURE_IFMT_COMPRESSED_ALPHA_ARB;
        case y60::GENERIC_LUMINANCE:       return TEXTURE_IFMT_COMPRESSED_LUMINANCE_ARB;
        case y60::GENERIC_LUMINANCE_ALPHA: return TEXTURE_IFMT_COMPRESSED_LUMINANCE_ALPHA_ARB;
        case y60::GENERIC_INTENSITY:       return TEXTURE_IFMT_COMPRESSED_INTENSITY_ARB;
        case y60::S3TC_DXT1:               return TEXTURE_IFMT_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case y60::S3TC_DXT1A:              return TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case y60::S3TC_DXT3:               return TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case y60::S3TC_DXT5:               return TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case y60::RGBA_HALF:               return TEXTURE_IFMT_RGBA_HALF;
        case y60::RGB_HALF:                return TEXTURE_IFMT_RGB_HALF;
        case y60::RGBA_FLOAT:              return TEXTURE_IFMT_RGBA_FLOAT;
        case y60::RGB_FLOAT:               return TEXTURE_IFMT_RGB_FLOAT;
        default:
            throw asl::Exception(std::string("Unsupported texture pixelformat ") +
                    asl::getStringFromEnum(theEncoding, y60::PixelEncodingString), PLUS_FILE_LINE);
    }
}

}
