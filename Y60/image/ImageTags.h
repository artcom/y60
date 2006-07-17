//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ImageTags.h,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2005/04/29 13:37:56 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _ac_y60_ImageTags_h_
#define _ac_y60_ImageTags_h_

#include <y60/NodeNames.h>
#include <y60/NodeValueNames.h>
#include <y60/DataTypes.h>
#include <y60/CommonTags.h>

#include <asl/Matrix4.h>

namespace y60 {

    //                  theTagName              theType             theAttributeName                   theDefault
    DEFINE_ATTRIBUT_TAG(ImageSourceTag,         std::string,        IMAGE_SRC_ATTRIB,             "");
    DEFINE_ATTRIBUT_TAG(RasterPixelFormatTag,   std::string,        IMAGE_RASTER_PIXELFORMAT_ATTRIB,  "RGB");
    DEFINE_ATTRIBUT_TAG(TexturePixelFormatTag,  std::string,        IMAGE_TEXTURE_PIXELFORMAT_ATTRIB, "");
    DEFINE_ATTRIBUT_TAG(ImageInternalFormatTag, std::string,        IMAGE_INTERNAL_FORMAT_ATTRIB, "");
    DEFINE_ATTRIBUT_TAG(ImageWidthTag,          unsigned,           WIDTH_ATTRIB,                 0);
    DEFINE_ATTRIBUT_TAG(ImageHeightTag,         unsigned,           HEIGHT_ATTRIB,                0);
    DEFINE_ATTRIBUT_TAG(ImageDepthTag,          unsigned,           DEPTH_ATTRIB,                 1);
    DEFINE_ATTRIBUT_TAG(ImageMatrixTag,         asl::Matrix4f,      IMAGE_MATRIX_ATTRIB,          asl::Matrix4f::Identity());
    DEFINE_ATTRIBUT_TAG(ImageTypeTag,           std::string,        IMAGE_TYPE_ATTRIB,            IMAGE_TYPE_SINGLE);
    DEFINE_ATTRIBUT_TAG(ImageMipmapTag,         bool,               IMAGE_MIPMAP_ATTRIB,          false);
    DEFINE_ATTRIBUT_TAG(ImageResizeTag,         std::string,        IMAGE_RESIZE_ATTRIB,          IMAGE_RESIZE_PAD);
    DEFINE_ATTRIBUT_TAG(ImageFilterTag,         std::string,        IMAGE_FILTER_ATTRIB,          IMAGE_FILTER_NONE);
    DEFINE_ATTRIBUT_TAG(ImageFilterParamsTag,   VectorOfFloat,      IMAGE_FILTER_PARAMS_ATTRIB,   TYPE());
    DEFINE_ATTRIBUT_TAG(ImageColorBiasTag,      asl::Vector4f,      IMAGE_COLOR_BIAS_ATTRIB,      asl::Vector4f(0, 0, 0, 0));
    DEFINE_ATTRIBUT_TAG(ImageColorScaleTag,     asl::Vector4f,      IMAGE_COLOR_SCALE_ATTRIB,     asl::Vector4f(1, 1, 1, 1));
    DEFINE_ATTRIBUT_TAG(ImageTileTag,           asl::Vector2i,      IMAGE_TILE_ATTRIB,            asl::Vector2i(1,1));
    DEFINE_ATTRIBUT_TAG(ImageBytesPerPixelTag,  float,              IMAGE_BYTESPERPIXEL_ATTRIB,   0);
    DEFINE_ATTRIBUT_TAG(TextureIdTag,           unsigned,           IMAGE_TEXTURE_ID_ATTRIB,      0);
    DEFINE_ATTRIBUT_TAG(LoadCountTag,           unsigned,           IMAGE_LOAD_COUNT_ATTRIB,      0);    
}
#endif
