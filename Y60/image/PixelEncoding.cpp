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
#include <paintlib/plpixelformat.h>

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

}
