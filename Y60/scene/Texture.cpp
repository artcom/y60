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
//   $RCSfile: Texture.cpp,v $
//   $Author: pavel $
//   $Revision: 1.25 $
//   $Date: 2005/04/24 00:41:21 $
//
//
//=============================================================================
#include "Texture.h"
#include "TextureManager.h"

#include <y60/NodeNames.h>
#include <y60/NodeValueNames.h>
#include <asl/string_functions.h>
#include <asl/Logger.h>

using namespace asl;

namespace y60 {

    Texture::Texture(dom::Node & theNode) :
        Facade(theNode),
        /*IdTag::Plug(theNode),*/
        TextureImageTag::Plug(theNode),
        TextureApplyModeTag::Plug(theNode),
        TextureWrapModeTag::Plug(theNode),
        TextureSpriteTag::Plug(theNode),
		TextureMatrixTag::Plug(theNode),
        TextureMinFilterTag::Plug(theNode),
        TextureMagFilterTag::Plug(theNode)
    {
    }

    Texture::~Texture() {
        AC_TRACE << "Texture destructor";
        ImagePtr myImage = _myImage.lock();
        if (myImage) {
            myImage->deregisterTexture();
        }
    }

    bool
    Texture::update(const TextureManager & theTextureManager) {
        AC_TRACE << "Texture::update";

        /*
        NodePtr myImageNode = getNode()->getElementById(get<TextureImageTag>());
        if (myImageNode) {            
        }
        */

        //DO THIS WITH DPEENDENCIES ON set/get TextureImageTag
        
		// TODO better dirty detection
        ImagePtr myImageInDom = theTextureManager.findImage(get<TextureImageTag>());

        if (!_myImage || getImage().getNativePtr() != myImageInDom.getNativePtr()) {
            AC_TRACE << "Textureimage has changed from ";

            ImagePtr myImage = _myImage.lock();
            if (myImage) {
                myImage->deregisterTexture();
            }

            myImageInDom->registerTexture();
            _myImage = ImageWeakPtr(myImageInDom);
            return true;
        } else {
            return false;
        }
    }

    TextureApplyMode
    Texture::getApplyMode() const {
        return TextureApplyMode(getEnumFromString( get<TextureApplyModeTag>(), TextureApplyModeStrings));
	}
	
    TextureWrapMode
    Texture::getWrapMode() const {
        return TextureWrapMode(getEnumFromString( get<TextureWrapModeTag>(), TextureWrapModeStrings));
    }

    TextureSampleFilter
    Texture::getMinFilter() const {
        return TextureSampleFilter(getEnumFromString( get<TextureMinFilterTag>(), TextureSampleFilterStrings));
    }
    
    TextureSampleFilter
    Texture::getMagFilter() const {
        return TextureSampleFilter(getEnumFromString( get<TextureMagFilterTag>(), TextureSampleFilterStrings));
    }
    
    ImagePtr
    Texture::getImage() const {
        AC_TRACE << "Texture::getImage";
        if (_myImage) {
            return _myImage.lock();
        } else {            
            throw TextureException(std::string("Invalid image in texture: ") + asl::as_string(getNode()), PLUS_FILE_LINE);
        }
    }

	unsigned
    Texture::getId() const {
        if (_myImage) {
            return _myImage.lock()->getGraphicsId();
        } else {
            throw TextureException(std::string("Invalid image in texture: ") + asl::as_string(getNode()), PLUS_FILE_LINE);
        }
    }
}
