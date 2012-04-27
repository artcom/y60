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

#ifndef _ac_y60_ImageTags_h_
#define _ac_y60_ImageTags_h_

#include "y60_image_settings.h"

#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/DataTypes.h>
#include <y60/base/CommonTags.h>

#include <asl/math/Matrix4.h>

namespace y60 {
    //                  theTagName              theType         theAttributeName                 theDefault
    DEFINE_ATTRIBUTE_TAG(ImageSourceTag,         std::string,    IMAGE_SRC_ATTRIB,                "", Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(ImageAsyncFlagTag,      bool,           IMAGE_ASYNC_LOAD_ATTRIB,         false, Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(TargetPixelFormatTag,   std::string,    IMAGE_TARGET_PIXELFORMAT_ATTRIB, "", Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(ImageResizeTag,         std::string,    IMAGE_RESIZE_ATTRIB,             IMAGE_RESIZE_NONE, Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(ImageDepthTag,          unsigned,       DEPTH_ATTRIB,                    1, Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(ImageMatrixTag,         asl::Matrix4f,  MATRIX_ATTRIB,                   asl::Matrix4f::Identity(), Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(ImageTileTag,           asl::Vector2i,  IMAGE_TILE_ATTRIB,               asl::Vector2i(1,1), Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(ImageFilterTag,         std::string,    IMAGE_FILTER_ATTRIB,             IMAGE_FILTER_NONE, Y60_IMAGE_DECL);
    DEFINE_ATTRIBUTE_TAG(ImageFilterParamsTag,   VectorOfFloat,  IMAGE_FILTER_PARAMS_ATTRIB,      TYPE(), Y60_IMAGE_DECL);    
    DEFINE_ATTRIBUTE_TAG(LoadStateTag,            bool,          IMAGE_LOADSTATE_ATTRIB,          false, Y60_IMAGE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(ImageBytesPerPixelTag,  float,          IMAGE_BYTESPERPIXEL_ATTRIB,      0, Y60_IMAGE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(RasterPixelFormatTag,   std::string,    IMAGE_RASTER_PIXELFORMAT_ATTRIB, "RGB", Y60_IMAGE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(ImageWidthTag,          unsigned,       WIDTH_ATTRIB,                    0, Y60_IMAGE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(ImageHeightTag,         unsigned,       HEIGHT_ATTRIB,                   0, Y60_IMAGE_DECL);
}

#endif
