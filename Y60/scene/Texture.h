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
//   $RCSfile: Texture.h,v $
//   $Author: david $
//   $Revision: 1.25 $
//   $Date: 2005/04/04 14:40:15 $
//
//  Description: This class performs texture loading and management.
//
//=============================================================================

#ifndef AC_Y60_TEXTURE_INCLUDED
#define AC_Y60_TEXTURE_INCLUDED

#include "TextureManager.h"
#include <y60/Image.h>
#include <y60/NodeValueNames.h>
#include <y60/ITextureManager.h>

#include <asl/Exception.h>
#include <asl/Matrix4.h>
#include <asl/Ptr.h>

#include <dom/AttributePlug.h>
#include <dom/Facade.h>

#include <string>
namespace dom {
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

    //                  theTagName           theType        theAttributeName           theDefault
    DEFINE_ATTRIBUT_TAG(TextureImageTag,       ImageWeakPtr,  "texture_image_tag",       ImagePtr(0));
    DEFINE_ATTRIBUT_TAG(TextureImageIdTag,     std::string,   TEXTURE_IMAGE_ATTRIB,      "");
    DEFINE_ATTRIBUT_TAG(TextureProjectorIdTag, std::string,   TEXTURE_PROJECTOR_ATTRIB,      "");
    DEFINE_ATTRIBUT_TAG(TextureApplyModeTag,   TextureApplyMode,   TEXTURE_APPLYMODE_ATTRIB,  MODULATE);
    DEFINE_ATTRIBUT_TAG(TextureSpriteTag,      bool,          TEXTURE_SPRITE_ATTRIB,     false);
    DEFINE_ATTRIBUT_TAG(TextureMatrixTag,      asl::Matrix4f, TEXTURE_MATRIX_ATTRIB,     asl::Matrix4f::Identity());

    DEFINE_EXCEPTION(TextureException, asl::Exception);
    class Texture :
        public dom::Facade,
        public TextureImageIdTag::Plug,
        public TextureProjectorIdTag::Plug,
        public TextureApplyModeTag::Plug,
        public TextureSpriteTag::Plug,
        public TextureMatrixTag::Plug,
        public dom::FacadeAttributePlug<TextureImageTag>,
        public ResizePolicyTag::Plug
    {
        public:
            Texture(dom::Node & theNode);
            IMPLEMENT_FACADE(Texture);
            virtual ~Texture();

            void setTextureManager(const asl::Ptr<TextureManager> theTextureManager);

            void setUsage(TextureUsage theUsage) {}

            TextureApplyMode  getApplyMode() const;
            bool getSpriteMode() const;

            unsigned      getId() const;
            ImagePtr  getImage() const;

            virtual void registerDependenciesRegistrators();
    
        protected:
            virtual void registerDependenciesForImageTag();
        private:
            void update();

            Texture();
            asl::Ptr<TextureManager>      _myTextureManager;
    };

    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;
}

#endif // AC_Y60_TEXTURE_INCLUDED
