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
*/

// own header
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
