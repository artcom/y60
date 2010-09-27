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

// own header
#include "MaterialBuilder.h"

#include <algorithm>

#include <asl/dom/Nodes.h>
#include <asl/dom/Value.h>
#include <asl/base/Block.h>
#include <asl/base/Dashboard.h>
#include <asl/base/file_functions.h>

#include "TextureUnit.h"
#include <y60/base/NodeNames.h>
#include <y60/base/iostream_functions.h>
#include <y60/base/property_functions.h>

#include "SceneBuilder.h"
#include "ImageBuilder.h"
#include "MovieBuilder.h"
#include "TextureBuilder.h"

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)


using namespace std;
using namespace asl;

namespace y60 {

    MaterialBuilder::MaterialBuilder(const std::string & theName, bool theInlineTextureFlag)
        : BuilderBase(MATERIAL_NODE_NAME),
        _myRequirementsAdded(false),
        _needTextureFallback(false),
        _myInlineTextureFlag(theInlineTextureFlag)
    {
        dom::NodePtr myNode = getNode();
        myNode->appendAttribute(NAME_ATTRIB, theName);

        // Create all necessary container nodes
        (*myNode)(PROPERTY_LIST_NAME);
        (*myNode)(TEXTUREUNIT_LIST_NAME);
        (*myNode)(REQUIRES_LIST_NAME);
    }

    MaterialBuilder::~MaterialBuilder() {
        computeRequirements();
    }

    const std::string &
    MaterialBuilder::getName() const {
        return getNode()->getAttribute(NAME_ATTRIB)->nodeValue();
    }

    unsigned
    MaterialBuilder::getTextureCount() {
        return getNode()->childNode(TEXTURE_LIST_NAME)->childNodesLength();
    }

    bool
    MaterialBuilder::isBumpMap(unsigned theTextureIndex) const {
        // no wonder bump mapping doesn't work... APPLYMODE compared against USAGE
        return (*getNode())(TEXTURE_LIST_NAME)(TEXTURE_NODE_NAME,theTextureIndex)
            [TEXTUREUNIT_APPLYMODE_ATTRIB].nodeValue() == TEXTURE_USAGE_BUMP;
    }

    bool
    MaterialBuilder::isMovie(const std::string & theFileName) const {
        const std::string myExtension = asl::getExtension(theFileName);

        static const char * myMovieExtensions[] = {
            "m60", "mpg", "m2v", "avi", "mov", "mpeg", "wmv", 0
        };
        for (unsigned int i = 0; myMovieExtensions[i]; ++i) {
            if (myExtension == myMovieExtensions[i]) {
                return true;
            }
        }

        return false;
    }

    void
    MaterialBuilder::needTextureFallback(bool needTextureFallback) {
        _needTextureFallback = needTextureFallback;
    }

    void
    MaterialBuilder::computeRequirements() {
        if (_myRequirementsAdded) {
            return;
        }

        dom::NodePtr myFeatureListNode = getNode()->childNode(REQUIRES_LIST_NAME);

        // texture feature
        if (!_myTextureRequirements.empty()) {
            if (!_needTextureFallback && (_myTextureRequirements.size() > 1)) {
                VectorOfRankedFeature::iterator it = _myTextureRequirements.begin() + 1;
                _myTextureRequirements.erase(it);
            }
            if (_needTextureFallback && (_myTextureRequirements.size() == 1)) {
                RankedFeature myFallbackFeature;
                myFallbackFeature._myFeature.push_back(FEATURE_DROPPED);
                myFallbackFeature._myRanking = 0.0f;
                _myTextureRequirements.push_back(myFallbackFeature);
            }
            setPropertyValue<VectorOfRankedFeature>(getNode(), FEATURE_NODE_NAME, TEXTURE_FEATURE, _myTextureRequirements, REQUIRES_LIST_NAME);
        }

        // Mapping feature
        if (!_myMappingRequirements.empty()) {
            if (!_needTextureFallback && (_myMappingRequirements.size() > 1)) {
                VectorOfRankedFeature::iterator it = _myMappingRequirements.begin() + 1;
                _myMappingRequirements.erase(it);
            }
            if (_needTextureFallback && (_myMappingRequirements.size() == 1)) {
                RankedFeature myFallbackFeature;
                myFallbackFeature._myFeature.push_back(FEATURE_DROPPED);
                myFallbackFeature._myRanking = 0.0f;
                _myMappingRequirements.push_back(myFallbackFeature);
            }
            setPropertyValue<VectorOfRankedFeature>(getNode(), FEATURE_NODE_NAME, MAPPING_FEATURE, _myMappingRequirements, REQUIRES_LIST_NAME);
        }


        // lighting feature
        if (!_myLightingRequirements.empty()) {
            setPropertyValue<VectorOfRankedFeature>(getNode(), FEATURE_NODE_NAME, LIGHTING_FEATURE, _myLightingRequirements, REQUIRES_LIST_NAME);
        }

        _myRequirementsAdded = true;
        _needTextureFallback = false;
    }

    void
    MaterialBuilder::addFeature(const std::string & theClass, const VectorOfRankedFeature & theValue) {
        setPropertyValue<VectorOfRankedFeature>(getNode(), FEATURE_NODE_NAME, theClass.c_str(), theValue, REQUIRES_LIST_NAME);
    }
/*
        dom::NodePtr myFeatureListNode = getNode()->childNode(REQUIRES_LIST_NAME);
        dom::NodePtr myFeatureNode = dom::NodePtr(new dom::Element(FEATURE_NODE_NAME));
        myFeatureNode->appendAttribute(NAME_ATTRIB, theClass);
        myFeatureNode->appendAttribute(VALUES_ATTRIB, theValue);
        myFeatureListNode->appendChild(myFeatureNode);*/

    void
    MaterialBuilder::checkState() {
        if (_myRequirementsAdded) {
            throw MaterialBuilderException("Material is already added to dom. Further additions not allowed",
                                           PLUS_FILE_LINE);
        }
    }

    void
    MaterialBuilder::setType(const VectorOfRankedFeature & theType) {
        checkState();
        _myLightingRequirements = theType;
    }

    dom::NodePtr
    MaterialBuilder::createTextureUnitNode(const std::string & theTextureId,
            const TextureApplyMode & theApplyMode,
            const TextureUsage & theUsage,
            const std::string & theMappingMode,
            const asl::Matrix4f & theMatrix,
            bool theSpriteFlag, float theRanking,
            bool isFallback, float theFallbackRanking)
    {
        dom::NodePtr myTextureUnitListNode = getNode()->childNode(TEXTUREUNIT_LIST_NAME);
        dom::NodePtr myTextureUnitNode = dom::NodePtr(new dom::Element(TEXTUREUNIT_NODE_NAME));
        myTextureUnitListNode->appendChild(myTextureUnitNode);

        TextureUnitPtr myTexture = myTextureUnitNode->getFacade<TextureUnit>();
        myTexture->set<TextureUnitTextureIdTag>(theTextureId);
        myTexture->set<TextureUnitApplyModeTag>(theApplyMode);
        myTexture->set<TextureUnitSpriteTag>(theSpriteFlag);
        myTexture->set<TextureUnitMatrixTag>(theMatrix);

        if (_myTextureRequirements.empty()) {
            RankedFeature myFirstTextureSet;
            _myTextureRequirements.push_back(myFirstTextureSet);

            RankedFeature myFirstMappingSet;
            _myMappingRequirements.push_back(myFirstMappingSet);
        }

        if (isFallback) {
            if (_myTextureRequirements.size() == 1) {
                RankedFeature myFirstTextureSet;
                _myTextureRequirements.push_back(myFirstTextureSet);

                RankedFeature myFirstMappingSet;
                _myMappingRequirements.push_back(myFirstMappingSet);
            }

            _myTextureRequirements[1]._myFeature.push_back(theUsage.asString());
            _myTextureRequirements[1]._myRanking = theFallbackRanking;

            _myMappingRequirements[1]._myFeature.push_back(theMappingMode);
            _myMappingRequirements[1]._myRanking = theFallbackRanking;
        }

        _myTextureRequirements[0]._myFeature.push_back(theUsage.asString());
        _myTextureRequirements[0]._myRanking = theRanking;

        _myMappingRequirements[0]._myFeature.push_back(theMappingMode);
        _myMappingRequirements[0]._myRanking = theRanking;

        return myTextureUnitNode;
    }

    dom::NodePtr
    MaterialBuilder::createTextureNode(SceneBuilder & theSceneBuilder,
            const std::string & theName,
            const std::string & theImageId,
            const TextureWrapMode & theWrapMode,
            bool  theCreateMipmapsFlag,
            const asl::Matrix4f & theMatrix,
            const std::string & theInternalFormat,
            const asl::Vector4f & theColorScale,
            const asl::Vector4f & theColorBias,
            bool allowSharing)
    {
        AC_DEBUG << "MaterialBuilder::createTextureNode name=" << theName;

        if (allowSharing) {
            // share texture if attributes match
            dom::NodePtr myTextures = theSceneBuilder.getNode()->childNode(TEXTURE_LIST_NAME);

            std::vector<dom::NodePtr> myResults;
            myTextures->getNodesByAttribute(TEXTURE_NODE_NAME, TEXTURE_IMAGE_ATTRIB, theImageId, true, myResults);
            for (unsigned i = 0; i < myResults.size(); ++i) {

                dom::NodePtr myTextureNode = myResults[i];
                TexturePtr myTexture = myTextureNode->getFacade<Texture>();
                if (myTexture->get<TextureWrapModeTag>() != theWrapMode) {
                    continue;
                }
                if (myTexture->get<TextureMipmapTag>() != theCreateMipmapsFlag) {
                    continue;
                }
                if (myTexture->get<TextureMatrixTag>() != theMatrix) {
                    continue;
                }
                if (!almostEqual(myTexture->get<TextureColorScaleTag>(), theColorScale)) {
                    continue;
                }
                if (!almostEqual(myTexture->get<TextureColorBiasTag>(), theColorBias)) {
                    continue;
                }
                AC_DEBUG << "Sharing texture '" << myTexture->get<NameTag>() << "' id=" << myTexture->get<IdTag>();
                return myTextureNode;
            }
        }

        TextureBuilder myTextureBuilder(theName, theImageId);
        theSceneBuilder.appendTexture(myTextureBuilder);

        // determine texture type from image parameters
        dom::NodePtr myImageNode = myTextureBuilder.getNode()->getElementById(theImageId);
        ImagePtr myImage = myImageNode->getFacade<Image>();
        TextureType myType = TEXTURE_2D;
        if (myImage->get<ImageDepthTag>() > 1) {
            myType = TEXTURE_3D;
        } else {
            asl::Vector2i myTiles = myImage->get<ImageTileTag>();
            unsigned myNumTiles = myTiles[0] * myTiles[1];
            if (myNumTiles == 6) {
                myType = TEXTURE_CUBEMAP;
            } else if (myNumTiles != 1) {
                AC_ERROR << "Number of texture tiles is not six (cubemap) and not one (texture_2D), assuming texture_2D";
            }
        }

        myTextureBuilder.setType(myType);
        myTextureBuilder.setMipmapFlag(theCreateMipmapsFlag);
        myTextureBuilder.setWrapMode(theWrapMode);
        myTextureBuilder.setColorScale(theColorScale);
        myTextureBuilder.setColorBias(theColorBias);
        myTextureBuilder.setMatrix(theMatrix);
        myTextureBuilder.setTextureFormat(theInternalFormat);

        return myTextureBuilder.getNode();
    }

    dom::NodePtr
    MaterialBuilder::createImageNode(SceneBuilder & theSceneBuilder,
            const std::string & theName, const std::string & theFileName,
            const TextureUsage & theUsage,
            ImageType theType,
            const std::string & theResizeMode,
            unsigned theDepth,
            bool allowSharing)
    {
        std::string myFileName(theFileName);
        asl::findAndReplace(myFileName, "\\","/");

        //TODO: consider also depth when looking for image; however, there is no use case for
        // using the same image as 3D-Texture with different depth values
        if (allowSharing) {
            dom::NodePtr myImage = theSceneBuilder.findImageByFilename(myFileName);
            if (myImage) {
                return myImage;
            }
        }

        ImageBuilder myImageBuilder(theName);
        /*const string & myId =*/ theSceneBuilder.appendImage(myImageBuilder);
        myImageBuilder.setDepth(theDepth);

        if (theType == CUBEMAP) {
            myImageBuilder.setTiling(Vector2i(1,6));
        }

        ImageFilter myFilter = lookupFilter(theUsage);
        if (_myInlineTextureFlag || myFilter != NO_FILTER) {
            myImageBuilder.inlineImage(myFileName, myFilter, theResizeMode);
        } else {
            myImageBuilder.createFileReference(myFileName, theResizeMode);
        }

        return myImageBuilder.getNode();
    }


    dom::NodePtr
    MaterialBuilder::createMovieNode(SceneBuilder & theSceneBuilder, const std::string & theName,
            const std::string & theFileName,
            unsigned int theLoopCount)
    {
        std::string myFileName(theFileName);
        asl::findAndReplace(myFileName, "\\","/");

        dom::NodePtr myMovie = theSceneBuilder.findMovieByFilename(myFileName);
        if (myMovie) {
            return myMovie;
        }

        MovieBuilder myMovieBuilder(theName, myFileName);
        /*const string & myId =*/ theSceneBuilder.appendMovie(myMovieBuilder);
        myMovieBuilder.setLoopCount(theLoopCount);

        return myMovieBuilder.getNode();
    }

    void
    MaterialBuilder::appendCubemap(SceneBuilder & theSceneBuilder,
                                   const std::string & theName,
                                   const TextureUsage & theUsage,
                                   const std::string & theFrontFileName,
                                   const std::string & theRightFileName,
                                   const std::string & theBackFileName,
                                   const std::string & theLeftFileName,
                                   const std::string & theTopFileName,
                                   const std::string & theBottomFileName,
                                   const TextureApplyMode & theApplyMode)
    {
        checkState();
        string myFileName = theFrontFileName + "|" + theRightFileName + "|"
            + theBackFileName + "|" + theLeftFileName + "|"
            + theTopFileName + "|" + theBottomFileName;

        dom::NodePtr myImageNode = createImageNode(theSceneBuilder, theName, myFileName, theUsage, CUBEMAP);
        dom::NodePtr myTextureNode = createTextureNode(theSceneBuilder, theName, myImageNode->getAttributeString(ID_ATTRIB), TextureWrapMode(CLAMP), false, Matrix4f::Identity());
        createTextureUnitNode(myTextureNode->getAttributeString(ID_ATTRIB), theApplyMode, theUsage, TEXCOORD_REFLECTIVE, Matrix4f::Identity());
    }

    void
    createLightingFeature(VectorOfRankedFeature & theRequirement,
                          LightingModel theLightingModelType)
    {
        // build material requirements
        VectorOfRankedFeature myLightingRequirements;
        for (int i = theLightingModelType; i < MAX_LIGHTING_MODEL; i++) {
            RankedFeature myLighting;
            myLighting._myFeature.push_back(getStringFromEnum(i, LightingModelString));
            myLighting._myRanking = (10 * MAX_LIGHTING_MODEL) - i * 10.f;
            // check whether the new feature exists

            VectorOfRankedFeature::iterator pos;
            for (pos=theRequirement.begin(); pos < theRequirement.end(); ++pos) {
                if ((*pos)._myFeature == myLighting._myFeature) {
                    break;
                }
            }
            if (pos == theRequirement.end()) {
                theRequirement.push_back(myLighting);
            } else {
                (*pos)._myRanking = myLighting._myRanking;
            }
        }
    }

    void
    MaterialBuilder::setTransparencyFlag(bool theFlag) {
        if (getNode()->getAttribute(TRANSPARENCY_ATTRIB)) {
            (*getNode())[TRANSPARENCY_ATTRIB].nodeValueAssign<bool>(theFlag);
        } else {
            getNode()->appendAttribute(TRANSPARENCY_ATTRIB, theFlag);
        }
    }

    bool
    MaterialBuilder::getTransparencyFlag() const {
        return getNode()->getAttributeValue(TRANSPARENCY_ATTRIB, false);
    }
}
