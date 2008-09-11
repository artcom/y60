//============================================================================
// Copyright (C) 2000-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "TextureBuilder.h"

#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>

#include <asl/dom/Nodes.h>
#include <asl/base/Block.h>


namespace y60 {
    TextureBuilder::TextureBuilder(const std::string & theName, const std::string & theImageId)
        : BuilderBase(TEXTURE_NODE_NAME)
    {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            //TODO: the follwoing attributes are all appended as text and not as type
            // because there is no schema available at this time
            myNode->appendAttribute(NAME_ATTRIB, theName);
            myNode->appendAttribute(TEXTURE_IMAGE_ATTRIB, theImageId);
		} else {
            TexturePtr myTexture = myNode->getFacade<Texture>();
            myTexture->set<NameTag>(theName);
            myTexture->set<TextureImageIdTag>(theImageId);
		}
    }

    TextureBuilder::~TextureBuilder() {
    }

    void
    TextureBuilder::setType(TextureType theType) {
        getNode()->getFacade<Texture>()->set<TextureTypeTag>(theType);
    }

    void
    TextureBuilder::setMipmapFlag(bool theFlag) {
        getNode()->getFacade<Texture>()->set<TextureMipmapTag>(theFlag);
    }
    
    void 
    TextureBuilder::setTextureFormat(const std::string & theInternalFormat) {
        getNode()->getFacade<Texture>()->set<TexturePixelFormatTag>(theInternalFormat);
    }

    void
    TextureBuilder::setWrapMode(const TextureWrapMode & theWrapMode) {
        getNode()->getFacade<Texture>()->set<TextureWrapModeTag>(theWrapMode);
    }

    void
    TextureBuilder::setColorScale(const asl::Vector4f & theColorScale) {
        getNode()->getFacade<Texture>()->set<TextureColorScaleTag>(theColorScale);
    }

    void
    TextureBuilder::setColorBias(const asl::Vector4f & theColorBias) {
        getNode()->getFacade<Texture>()->set<TextureColorBiasTag>(theColorBias);
    }

    void
    TextureBuilder::setMatrix(const asl::Matrix4f & theMatrix) {
        getNode()->getFacade<Texture>()->set<TextureMatrixTag>(theMatrix);
    }
}
