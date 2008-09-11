//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_ImageTags_h_
#define _ac_y60_ImageTags_h_

#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/DataTypes.h>
#include <y60/base/CommonTags.h>

#include <asl/math/Matrix4.h>

namespace y60 {
    //                  theTagName              theType         theAttributeName                 theDefault
    DEFINE_ATTRIBUT_TAG        (ImageSourceTag,         std::string,    IMAGE_SRC_ATTRIB,                "");
    DEFINE_ATTRIBUT_TAG(TargetPixelFormatTag,   std::string,    IMAGE_TARGET_PIXELFORMAT_ATTRIB, "");
    DEFINE_ATTRIBUT_TAG(ImageResizeTag,         std::string,    IMAGE_RESIZE_ATTRIB,             IMAGE_RESIZE_NONE);
    DEFINE_ATTRIBUT_TAG(ImageDepthTag,          unsigned,       DEPTH_ATTRIB,                    1);
    DEFINE_ATTRIBUT_TAG(ImageMatrixTag,         asl::Matrix4f,  MATRIX_ATTRIB,                   asl::Matrix4f::Identity());
    DEFINE_ATTRIBUT_TAG(ImageTileTag,           asl::Vector2i,  IMAGE_TILE_ATTRIB,               asl::Vector2i(1,1));
    DEFINE_ATTRIBUT_TAG(ImageFilterTag,         std::string,    IMAGE_FILTER_ATTRIB,             IMAGE_FILTER_NONE);
    DEFINE_ATTRIBUT_TAG(ImageFilterParamsTag,   VectorOfFloat,  IMAGE_FILTER_PARAMS_ATTRIB,      TYPE());
    DEFINE_FACADE_ATTRIBUTE_TAG(ImageBytesPerPixelTag,  float,          IMAGE_BYTESPERPIXEL_ATTRIB,      0);
    DEFINE_FACADE_ATTRIBUTE_TAG(RasterPixelFormatTag,   std::string,    IMAGE_RASTER_PIXELFORMAT_ATTRIB, "RGB");
    DEFINE_FACADE_ATTRIBUTE_TAG(ImageWidthTag,          unsigned,       WIDTH_ATTRIB,                    0);
    DEFINE_FACADE_ATTRIBUTE_TAG(ImageHeightTag,         unsigned,       HEIGHT_ATTRIB,                   0);
}

#endif
