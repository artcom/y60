//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef Y60_TEXTUREUNIT_INCLUDED
#define Y60_TEXTUREUNIT_INCLUDED

#include "TextureManager.h"
#include "IShader.h"
#include "Texture.h"

#include <y60/base/NodeValueNames.h>
#include <y60/image/ITextureManager.h>

#include <asl/base/Exception.h>
#include <asl/math/Matrix4.h>
#include <asl/base/Ptr.h>

#include <asl/dom/AttributePlug.h>
#include <asl/dom/Facade.h>

#include <string>

//#define BAD_TX
#ifdef BAD_TX
namespace dom {
    template <>
    struct ValueWrapper<y60::TextureWeakPtr> {
        typedef dom::SimpleValue<y60::TextureWeakPtr> Type;
    };
    inline
    std::ostream& operator<<(std::ostream& os, const y60::TextureWeakPtr& i) {
        return os << "[TextureWeakPtr]";
    }
    inline
    std::istream& operator>>(std::istream& is, y60::TextureWeakPtr& i) {
        return is;
    }

    template <>
    struct ValueWrapper<y60::ICombinerWeakPtr> {
        typedef dom::SimpleValue<y60::ICombinerWeakPtr> Type;
    };
}
#endif

namespace y60 {

#ifdef BAD_TX
    inline
    std::ostream& operator<<(std::ostream& os, const y60::ICombinerWeakPtr& i) {
        return os << "[ICombinerWeakPtr]";
    }
    inline
    std::istream& operator>>(std::istream& is, y60::ICombinerWeakPtr& i) {
        return is;
    }
#endif

    //                  theTagName                 theType           theAttributeName              theDefault
    DEFINE_ATTRIBUT_TAG(TextureUnitTextureIdTag,   std::string,      TEXTUREUNIT_TEXTURE_ATTRIB,   "");
    DEFINE_ATTRIBUT_TAG(TextureUnitApplyModeTag,   TextureApplyMode, TEXTUREUNIT_APPLYMODE_ATTRIB, MODULATE);
    DEFINE_ATTRIBUT_TAG(TextureUnitCombinerTag,    std::string,      TEXTUREUNIT_COMBINER_ATTRIB,  "");
    DEFINE_ATTRIBUT_TAG(TextureUnitEnvColorTag,    asl::Vector4f,    TEXTUREUNIT_ENVCOLOR_ATTRIB,  asl::Vector4f(1,1,1,1));
    DEFINE_ATTRIBUT_TAG(TextureUnitSpriteTag,      bool,             TEXTUREUNIT_SPRITE_ATTRIB,    false);
    DEFINE_ATTRIBUT_TAG(TextureUnitProjectorIdTag, std::string,      TEXTUREUNIT_PROJECTOR_ATTRIB, "");
    DEFINE_ATTRIBUT_TAG(TextureUnitMatrixTag,      asl::Matrix4f,    MATRIX_ATTRIB,                asl::Matrix4f::Identity());
#ifdef BAD_TX
    DEFINE_ATTRIBUT_TAG(TextureUnitTexturePtrTag,  TextureWeakPtr,   "TextureUnitTexturePtrTag",   TexturePtr(0));
    DEFINE_ATTRIBUT_TAG(TextureUnitCombinerPtrTag, ICombinerWeakPtr, "TextureUnitCombinerPtrTag",  ICombinerPtr(0));
#endif
    DEFINE_EXCEPTION(TextureUnitException, asl::Exception);

    class TextureUnit :
        public dom::Facade,
        public TextureUnitTextureIdTag::Plug,
        public TextureUnitProjectorIdTag::Plug,
        public TextureUnitApplyModeTag::Plug,
        public TextureUnitCombinerTag::Plug,
        public TextureUnitEnvColorTag::Plug,
        public TextureUnitSpriteTag::Plug,
        public TextureUnitMatrixTag::Plug,
        public ResizePolicyTag::Plug
#ifdef BAD_TX
,
        public dom::FacadeAttributePlug<TextureUnitTexturePtrTag>,
        public dom::FacadeAttributePlug<TextureUnitCombinerPtrTag>
#endif
    {
        public:
            TextureUnit(dom::Node & theNode);
            IMPLEMENT_FACADE(TextureUnit);
            virtual ~TextureUnit();

            void setTextureManager(const TextureManagerPtr theTextureManager);

            TexturePtr getTexture() const;
            ICombinerPtr getCombiner() const;

            virtual void registerDependenciesRegistrators();
    
        protected:
#ifdef BAD_TX
            virtual void registerDependenciesForTextureTag();
            virtual void registerDependenciesForCombinerTag();
#endif

        private:
            TextureUnit();

            void updateTexture();
            void updateCombiner();

            TextureManagerPtr _myTextureManager;
    };

    typedef asl::Ptr<TextureUnit, dom::ThreadingModel> TextureUnitPtr;
}

#endif // Y60_TEXTUREUNIT_INCLUDED
