//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_TEXTURETAGS_INCLUDED
#define Y60_TEXTURETAGS_INCLUDED

#include <y60/Image.h>
#include <y60/NodeNames.h>
#include <y60/NodeValueNames.h>
#include <y60/DataTypes.h>
#include <y60/CommonTags.h>

#include <asl/Matrix4.h>


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
    DEFINE_ATTRIBUT_TAG(TextureImageIdTag,        std::string,         TEXTURE_IMAGE_ATTRIB,           "");
    DEFINE_ATTRIBUT_TAG(TexturePixelFormatTag,    std::string,         TEXTURE_PIXELFORMAT_ATTRIB,     "");
    DEFINE_ATTRIBUT_TAG(TextureImageIndexTag,     unsigned,            TEXTURE_IMAGE_INDEX,            0);
    DEFINE_ATTRIBUT_TAG(TextureMatrixTag,         asl::Matrix4f,       MATRIX_ATTRIB,                  asl::Matrix4f::Identity());
    DEFINE_ATTRIBUT_TAG(TextureMipmapTag,         bool,                TEXTURE_MIPMAP_ATTRIB,          false);
    DEFINE_ATTRIBUT_TAG(TextureAnisotropyTag,     float,               TEXTURE_ANISOTROPY_ATTRIB,      1.0f);
    DEFINE_ATTRIBUT_TAG(TextureColorBiasTag,      asl::Vector4f,       TEXTURE_COLOR_BIAS_ATTRIB,      asl::Vector4f(0, 0, 0, 0));
    DEFINE_ATTRIBUT_TAG(TextureColorScaleTag,     asl::Vector4f,       TEXTURE_COLOR_SCALE_ATTRIB,     asl::Vector4f(1, 1, 1, 1));
    DEFINE_ATTRIBUT_TAG(TextureWrapModeTag,       TextureWrapMode,     TEXTURE_WRAPMODE_ATTRIB,        REPEAT);
    DEFINE_ATTRIBUT_TAG(TextureMinFilterTag,      TextureSampleFilter, TEXTURE_MIN_FILTER_ATTRIB,      LINEAR);
    DEFINE_ATTRIBUT_TAG(TextureMagFilterTag,      TextureSampleFilter, TEXTURE_MAG_FILTER_ATTRIB,      LINEAR);
#ifdef BAD_TX
    DEFINE_ATTRIBUT_TAG(TextureImageTag,          ImageWeakPtr,        "textureImage",                 ImagePtr(0));
#endif
    
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureTypeTag,           TextureType,         TEXTURE_TYPE_ATTRIB,            TEXTURE_2D);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureIdTag,             unsigned,            TEXTURE_ID_ATTRIB,              0);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureInternalFormatTag, std::string,         TEXTURE_INTERNAL_FORMAT_ATTRIB, "");
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureWidthTag,          unsigned,            WIDTH_ATTRIB,                   0);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureHeightTag,         unsigned,            HEIGHT_ATTRIB,                  0);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureDepthTag,          unsigned,            DEPTH_ATTRIB,                   1);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureParamChangedTag,   bool,                "textureParamChanged",          false);
    DEFINE_FACADE_ATTRIBUTE_TAG(TextureNPOTMatrixTag,     asl::Matrix4f,       "npotmatrix",                    asl::Matrix4f::Identity());
}

#endif
