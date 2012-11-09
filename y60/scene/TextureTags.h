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

#ifndef Y60_TEXTURETAGS_INCLUDED
#define Y60_TEXTURETAGS_INCLUDED

#include "y60_scene_settings.h"

#include <y60/image/Image.h>
#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/DataTypes.h>
#include <y60/base/CommonTags.h>

#include <asl/math/Matrix4.h>


namespace dom {

    // This is bogus, a WeakPtr is not a SimpleValue, it can not be copied using
    // just memcopy, same goes for other WeakPtr-Wrappers in TextureUnit.h
    // We must either define a special PtrValue type to wrap pointer values,
    // however this might not be easy, or we wrap it as ComplexValue
    template <>
    struct ValueWrapper<y60::ImageWeakPtr> {
        typedef dom::SimpleValue<y60::ImageWeakPtr> Type;
    };

    inline
    std::ostream& operator<<(std::ostream& os, const y60::ImageWeakPtr& i) {
        return os << "[ImageWeakPtr]";
    }

    inline
    std::istream& operator>>(std::istream& is,y60::ImageWeakPtr &i) {
        return is;
    }
}


namespace y60 {
    //                  theTagName                theType              theAttributeName                theDefault
    DEFINE_ATTRIBUTE_TAG(TextureImageIdTag,        std::string,         TEXTURE_IMAGE_ATTRIB,           "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TexturePixelFormatTag,    std::string,         TEXTURE_PIXELFORMAT_ATTRIB,     "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureImageIndexTag,     unsigned,            TEXTURE_IMAGE_INDEX,            0, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureMatrixTag,         asl::Matrix4f,       MATRIX_ATTRIB,                  asl::Matrix4f::Identity(), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureMipmapTag,         bool,                TEXTURE_MIPMAP_ATTRIB,          false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureAnisotropyTag,     float,               TEXTURE_ANISOTROPY_ATTRIB,      1.0f, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureColorBiasTag,      asl::Vector4f,       TEXTURE_COLOR_BIAS_ATTRIB,      asl::Vector4f(0, 0, 0, 0), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureColorScaleTag,     asl::Vector4f,       TEXTURE_COLOR_SCALE_ATTRIB,     asl::Vector4f(1, 1, 1, 1), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureWrapModeTag,       TextureWrapMode,     TEXTURE_WRAPMODE_ATTRIB,        REPEAT, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureMinFilterTag,      TextureSampleFilter, TEXTURE_MIN_FILTER_ATTRIB,      LINEAR, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(TextureMagFilterTag,      TextureSampleFilter, TEXTURE_MAG_FILTER_ATTRIB,      LINEAR, Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureTypeTag,           TextureType,         TEXTURE_TYPE_ATTRIB,            TEXTURE_2D, Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureIdTag,             unsigned,            TEXTURE_ID_ATTRIB,              0, Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureInternalFormatTag, std::string,         TEXTURE_INTERNAL_FORMAT_ATTRIB, "", Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureWidthTag,          unsigned,            WIDTH_ATTRIB,                   0, Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureHeightTag,         unsigned,            HEIGHT_ATTRIB,                  0, Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureDepthTag,          unsigned,            DEPTH_ATTRIB,                   1, Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureParamChangedTag,   bool,                "textureParamChanged",          false, Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureNPOTMatrixTag,     asl::Matrix4f,       "npotmatrix",                    asl::Matrix4f::Identity(), Y60_SCENE_DECL);
}

#endif
