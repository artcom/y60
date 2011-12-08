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

#ifndef _ac_y60_PixelEncoding_h_
#define _ac_y60_PixelEncoding_h_

#include "y60_image_settings.h"

#include <asl/base/Logger.h>
#include <y60/base/DataTypes.h>
#include <asl/base/string_functions.h>
#include <math.h>

class PLPixelFormat;

namespace y60 {
    // When creating new pixel encoding format,
    // append them to the END of the following enums
    // or else you will invalidate all existing
    // i60 files
    enum PixelEncoding {
        RGB,
        BGR,
        RGBA,
        BGRA,
        GRAY,
        GRAY16,
        GENERIC_RGB,
        GENERIC_RGBA,
        GENERIC_ALPHA,
        GENERIC_LUMINANCE,
        GENERIC_LUMINANCE_ALPHA,
        GENERIC_INTENSITY,
        S3TC_DXT1,
        S3TC_DXT1A,
        S3TC_DXT3,
        S3TC_DXT5,
        GRAYS16,
        ALPHA,
        DEPTH,
        GRAY32,
        RGBA_HALF,
        RGB_HALF,
        RGBA_FLOAT,
        RGB_FLOAT,
        YUV420,
        YUV422,
        YUV444,
        YUVA420
    };

    const char * const PixelEncodingString[] = {
        "RGB",
        "BGR",
        "RGBA",
        "BGRA",
        "GRAY",
        "GRAY16",
        "GENERIC_RGB",
        "GENERIC_RGBA",
        "GENERIC_ALPHA",
        "GENERIC_LUMINANCE",
        "GENERIC_LUMINANCE_ALPHA",
        "GENERIC_INTENSITY",
        "S3TC_DXT1",
        "S3TC_DXT1A",
        "S3TC_DXT3",
        "S3TC_DXT5",
        "GRAYS16",
        "ALPHA",
        "DEPTH",
        "GRAY32",
        "RGBA_HALF",
        "RGB_HALF",
        "RGBA_FLOAT",
        "RGB_FLOAT",
        "YUV420",
        "YUV422",
        "YUV444",
        "YUVA420",
        0
    };
    const char * const RasterEncodingString[] = {
        "RasterOfRGB",
        "RasterOfBGR",
        "RasterOfRGBA",
        "RasterOfBGRA",
        "RasterOfGRAY",
        "RasterOfGRAY16",
        "RasterOfGENERIC_RGB",
        "RasterOfGENERIC_RGBA",
        "RasterOfGENERIC_ALPHA",
        "RasterOfGENERIC_LUMINANCE",
        "RasterOfGENERIC_LUMINANCE_ALPHA",
        "RasterOfGENERIC_INTENSITY",
        "RasterOfDXT1",
        "RasterOfDXT1A",
        "RasterOfDXT3",
        "RasterOfDXT5",
        "RasterOfGRAYS16",
        "RasterOfGRAY",
        "RasterOfGRAY32",
        "RasterOfGRAY32",
        "RasterOfRGBA_HALF",
        "RasterOfRGB_HALF",
        "RasterOfRGBA_FLOAT",
        "RasterOfRGB_FLOAT",
        0
    };
   const char * const RasterElementNames[] = {
        "rasterofrgb",
        "rasterofbgr",
        "rasterofrgba",
        "rasterofbgra",
        "rasterofgray",
        "rasterofgray16",
        "rasterofgeneric_rgb",
        "rasterofgeneric_rgba",
        "rasterofgeneric_alpha",
        "rasterofgeneric_luminance",
        "rasterofgeneric_luminance_alpha",
        "rasterofgeneric_intensity",
        "rasterofdxt1",
        "rasterofdxt1a",
        "rasterofdxt3",
        "rasterofdxt5",
        "rasterofgrays16",
        "rasterofgray",
        "rasterofgray32",
        "rasterofgray32",
        "rasterofrgba_half",
        "rasterofrgb_half",
        "rasterofrgba_float",
        "rasterofrgb_float",
        0
    };

    inline
    dom::ValuePtr createRasterValue(PixelEncoding theEncoding,
                                    unsigned theWidth,unsigned theHeight,
                                    const asl::ReadableBlock & thePixels)
    {
        static asl::Ptr<dom::ValueFactory> ourImageFactory;
        if (!ourImageFactory) {
            ourImageFactory = asl::Ptr<dom::ValueFactory>(new dom::ValueFactory);
            registerSomImageTypes(*ourImageFactory);
        }
        dom::ValuePtr myReturnValue = dom::ValuePtr(ourImageFactory->createValue(RasterEncodingString[theEncoding], 0));
        dom::ResizeableRasterPtr myRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(myReturnValue);
        if (myRaster) {
            if (thePixels.size()) {
                myRaster->assign(theWidth, theHeight, thePixels);
            } else {
                myRaster->resize(theWidth, theHeight);
            }
        } else {
            AC_WARNING << "No raster found in factory for type "<<RasterEncodingString[theEncoding];
            return dom::ValuePtr();
        }
        return myReturnValue;
    };

    inline
    dom::ValuePtr createRasterValue(PixelEncoding theEncoding,
                                    unsigned theWidth,unsigned theHeight)
    {
        asl::ReadableBlockAdapter noPixels;
        return createRasterValue(theEncoding, theWidth, theHeight, noPixels);
    }

    inline
    unsigned getBytesRequired(unsigned thePixelCount, PixelEncoding theEncoding) {
        float myBytesPerPixel;
        switch (theEncoding) {
            case ALPHA:
            case GRAY:
                myBytesPerPixel = 1;
                break;
            case GRAY16:
            case GRAYS16:
                myBytesPerPixel = 2;
                break;
            case RGB:
            case BGR:
                myBytesPerPixel = 3;
                break;
            case GRAY32:
            case DEPTH:
            case RGBA:
            case BGRA:
                myBytesPerPixel = 4;
                break;
            case GENERIC_RGB:
            case GENERIC_RGBA:
            case GENERIC_ALPHA:
            case GENERIC_LUMINANCE:
            case GENERIC_LUMINANCE_ALPHA:
            case GENERIC_INTENSITY:
                throw asl::Exception(std::string("Encoding ratio for: ") +
                    asl::getStringFromEnum(theEncoding, PixelEncodingString) +
                    " is unknown. If you know please add here:",
                    PLUS_FILE_LINE);
            case S3TC_DXT1:
            case S3TC_DXT1A:
                myBytesPerPixel = 0.5;
                break;
            case S3TC_DXT3:
            case S3TC_DXT5:
                myBytesPerPixel = 1;
                break;
            case RGBA_HALF:
                myBytesPerPixel = 8;
                break;
            case RGB_HALF:
                myBytesPerPixel = 6;
                break;
            case RGBA_FLOAT:
                myBytesPerPixel = 16;
                break;
            case RGB_FLOAT:
                myBytesPerPixel = 12;
                break;
            default:
                throw asl::Exception(std::string("getBytesRequired(): Unsupported Encoding: ") +
                    asl::getStringFromEnum(theEncoding, PixelEncodingString));
        }

        float myBytesUsed = myBytesPerPixel * thePixelCount;
        if (ceil(myBytesUsed) != floor(myBytesUsed)) {
            throw asl::Exception(std::string("Size of image with pixelcount: ") +
                asl::as_string(thePixelCount) +
                " lead to non integral compressed data size: " +
                asl::as_string(myBytesUsed) + " Comprende?", PLUS_FILE_LINE);
        }

        return unsigned(myBytesUsed);
    }

    Y60_IMAGE_DECL bool mapPixelEncodingToFormat(const PixelEncoding & theEncoding,
            PLPixelFormat & theFormat);
    Y60_IMAGE_DECL bool mapFormatToPixelEncoding(const PLPixelFormat & theFormat,
            PixelEncoding & theEncoding);
    Y60_IMAGE_DECL TextureInternalFormat getInternalPixelFormat(PixelEncoding theEncoding);
}

#endif

