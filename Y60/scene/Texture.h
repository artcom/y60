//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
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

#include <y60/Image.h>
#include <y60/NodeValueNames.h>

#include <asl/Exception.h>
#include <asl/Matrix4.h>

#include <dom/AttributePlug.h>
#include <dom/Facade.h>

#include <string>

namespace y60 {
    class TextureManager;

// TODO: we are working on IT : make this a facade ...

    //                  theTagName           theType        theAttributeName      theDefault
    DEFINE_ATTRIBUT_TAG(TextureImageTag,     std::string,   TEXTURE_IMAGE_ATTRIB,      "");
    DEFINE_ATTRIBUT_TAG(TextureApplyModeTag, std::string,   TEXTURE_APPLYMODE_ATTRIB,  "modulate");
    DEFINE_ATTRIBUT_TAG(TextureWrapModeTag,  std::string,   TEXTURE_WRAPMODE_ATTRIB,   "clamp");
    DEFINE_ATTRIBUT_TAG(TextureMinFilterTag, std::string,   TEXTURE_MIN_FILTER_ATTRIB, "linear");
    DEFINE_ATTRIBUT_TAG(TextureMagFilterTag, std::string,   TEXTURE_MAG_FILTER_ATTRIB, "linear");
    DEFINE_ATTRIBUT_TAG(TextureSpriteTag,    bool,          TEXTURE_SPRITE_ATTRIB,     false);
    DEFINE_ATTRIBUT_TAG(TextureMatrixTag,    asl::Matrix4f, TEXTURE_MATRIX_ATTRIB,     asl::Matrix4f::Identity());

    DEFINE_EXCEPTION(TextureException, asl::Exception);
	class Texture :
        public dom::Facade,
        /*public IdTag::Plug,*/
        public TextureImageTag::Plug,
        public TextureApplyModeTag::Plug,
        public TextureWrapModeTag::Plug,
        public TextureSpriteTag::Plug,
		public TextureMinFilterTag::Plug, 
		public TextureMagFilterTag::Plug, 
		public TextureMatrixTag::Plug 
	{
        public:
			//Texture(dom::Node & theNode, int theFoo) {}
			Texture(dom::Node & theNode);
            IMPLEMENT_FACADE(Texture);
            virtual ~Texture();

            //you have to call update after construction !
            bool update(const TextureManager & theTextureManager);

			void setUsage(TextureUsage theUsage) {}

			TextureApplyMode  getApplyMode() const;
            TextureWrapMode   getWrapMode() const;
            TextureSampleFilter  getMinFilter() const;
            TextureSampleFilter  getMagFilter() const;
            bool getSpriteMode() const;

			unsigned      getId() const;
            ImagePtr getImage() const;

        private:
            Texture();
            ImageWeakPtr               _myImage;
    };

    typedef asl::Ptr<Texture, dom::ThreadingModel> TexturePtr;
}

#endif // AC_Y60_TEXTURE_INCLUDED
