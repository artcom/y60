//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "MaterialBuilder.h"
#include "SceneBuilder.h"
#include "ImageBuilder.h"
#include "MovieBuilder.h"
#include "Texture.h"
#include <y60/NodeNames.h>
#include <y60/iostream_functions.h>
#include <y60/property_functions.h>

#include <dom/Nodes.h>
#include <dom/Value.h>
#include <asl/Block.h>
#include <asl/Dashboard.h>
#include <asl/file_functions.h>

#include <algorithm>

#include <paintlib/planybmp.h>
#include <paintlib/planydec.h>
#include <paintlib/Filter/plfilterresizebilinear.h>

using namespace std;
using namespace asl;

namespace y60 {
    MaterialBuilder::MaterialBuilder(const std::string & theName, bool theInlineTextureFlag)
        : BuilderBase(MATERIAL_NODE_NAME), _myInlineTextureFlag(theInlineTextureFlag),
          _myRequirementsAdded(false), _needTextureFallback(false)
    {
        dom::NodePtr myNode = getNode();
        myNode->appendAttribute(NAME_ATTRIB, theName);
        //myNode->getFacade<Material>()->set<NameTag>(theName);

        // Create all neccessary container nodes
        (*myNode)(PROPERTY_LIST_NAME);
        (*myNode)(TEXTURE_LIST_NAME);
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
    MaterialBuilder::isBumpMap(int theTextureIndex) const {
        return (*getNode())(TEXTURE_LIST_NAME)(TEXTURE_NODE_NAME,theTextureIndex)
            [TEXTURE_APPLYMODE_ATTRIB].nodeValue() == TEXTURE_USAGE_BUMP;
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
    MaterialBuilder::createTextureNode(const std::string & theImageId,
                                       const TextureApplyMode & theApplyMode,
                                       const TextureUsage & theUsage,
                                       const std::string & theMappingMode,
                                       const asl::Matrix4f & theTextureMatrix,
                                       float theRanking,
                                       bool  isFallback,
                                       float theFallbackRanking,
                                       bool  theSpriteFlag)
    {
        dom::NodePtr myTextureListNode = getNode()->childNode(TEXTURE_LIST_NAME);

        dom::NodePtr myTextureNode = dom::NodePtr(new dom::Element(TEXTURE_NODE_NAME));
        myTextureListNode->appendChild(myTextureNode);
#if 0
        myTextureNode->appendAttribute(TEXTURE_IMAGE_ATTRIB, theImageId);

        if (!theApplyMode.empty()) {
            myTextureNode->appendAttribute(TEXTURE_APPLYMODE_ATTRIB, theApplyMode);
        }

        myTextureNode->appendAttribute(TEXTURE_WRAPMODE_ATTRIB, theWrapMode);
        myTextureNode->appendAttribute(TEXTURE_SPRITE_ATTRIB, asl::as_string(theSpriteFlag));
        myTextureNode->appendAttribute(TEXTURE_MATRIX_ATTRIB, theTextureMatrix);
#else
        //TODO: activate this when Texture is a Facade
        TexturePtr myTexture = myTextureNode->getFacade<Texture>();
        myTexture->set<TextureImageIdTag>(theImageId);
        myTexture->set<TextureApplyModeTag>(theApplyMode);
        myTexture->set<TextureSpriteTag>(theSpriteFlag);
        myTexture->set<TextureMatrixTag>(theTextureMatrix);
#endif
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

        return myTextureNode;
    }

    const std::string &
    MaterialBuilder::createImage(SceneBuilder & theSceneBuilder,
                                 const std::string & theName,
                                 const std::string & theFileName,
                                 const TextureUsage & theUsage,
                                 bool  theCreateMipmapsFlag,
                                 asl::Vector4f theColorScale,
                                 asl::Vector4f theColorBias,
                                 ImageType theType,
                                 const TextureWrapMode & theWrapMode,
                                 const std::string & theInternalFormat,
                                 const std::string & theResizeMode,
                                 unsigned theDepth,
                                 bool allowSharing)
    {
        std::string myFileName(theFileName);
        asl::findAndReplace(myFileName, "\\","/");

        float myAlpha = theColorScale[3];

        if (allowSharing) {
            //TODO: consider also depth when looking for image; however, there is no use case for
            // using the same image as 3D-Texture with different depth values
            
            dom::NodePtr myImage = theSceneBuilder.findImageByFilename_ColorScale_ColorBias(myFileName,
                                                                                            theColorScale,
                                                                                            theColorBias);
            if(myImage)
                return myImage->getAttributeString(ID_ATTRIB);
        }

        ImageBuilder myImageBuilder(theName, theCreateMipmapsFlag);
        const string & myId = theSceneBuilder.appendImage(myImageBuilder);
        
        myImageBuilder.setColorScale(theColorScale);
        myImageBuilder.setColorBias(theColorBias);
        myImageBuilder.setType(theType);
        myImageBuilder.setDepth(theDepth);
        myImageBuilder.setWrapMode(theWrapMode);
        
        if (theType == CUBEMAP) {
            myImageBuilder.setTiling(Vector2i(1,6));
        }

        ImageFilter myFilter = lookupFilter(theUsage);
        
        if (_myInlineTextureFlag || myFilter != NO_FILTER) {
            myImageBuilder.inlineImage(myFileName, myFilter, theResizeMode);
        } else {
            myImageBuilder.createFileReference(myFileName, theResizeMode);
        }
        myImageBuilder.setInternalFormat(theInternalFormat);
        
        return myId;
    }


    const std::string &
    MaterialBuilder::createMovie(SceneBuilder & theSceneBuilder,
                                 const std::string & theName,
                                 const std::string & theFileName,
                                 unsigned theLoopCount,
                                 const asl::Vector4f theColorScale,
                                 const asl::Vector4f theColorBias,
                                 const std::string & theInternalFormat)
    {
        std::string myFileName(theFileName);
        asl::findAndReplace(myFileName, "\\","/");

        dom::NodePtr myMovie = theSceneBuilder.findMovieByFilename(myFileName);

        if (myMovie) {
            return myMovie->getAttributeString(ID_ATTRIB);
        } else {
            MovieBuilder myMovieBuilder(theName, myFileName);
            const string & myId = theSceneBuilder.appendMovie(myMovieBuilder);
            myMovieBuilder.setLoopCount(theLoopCount);
            myMovieBuilder.setColorScale(theColorScale);
            myMovieBuilder.setColorBias(theColorBias);
            myMovieBuilder.setInternalFormat(theInternalFormat);
            return myId;
        }
    }

    bool
    MaterialBuilder::isMovie(const std::string & theFileName) {
        const std::string myExtension = asl::getExtension(theFileName);
        const char * myMovieExtensions[] = {
            "m60", "mpg", "m2v", "avi", "mov", "mpeg", "wmv", 0
        };
        for (unsigned int i = 0; true; ++i) {
            if (myMovieExtensions[i]) {
                if (myExtension == myMovieExtensions[i]) {
                    return true;
                }
            } else {
                break;
            }
        }

        return false;
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
                                   const TextureApplyMode & theApplyMode,
                                   const asl::Vector4f theColorScale)
    {
        checkState();
        string myFileName = theFrontFileName + "|" + theRightFileName + "|"
            + theBackFileName + "|" + theLeftFileName + "|"
            + theTopFileName + "|" + theBottomFileName;

        const string & myId = createImage(theSceneBuilder, theName, myFileName,
                theUsage, false,
                theColorScale, asl::Vector4f(0.0f,0.0f,0.0f,0.0f),
                CUBEMAP, CLAMP, "");
        createTextureNode(myId, theApplyMode, theUsage,
                TEXCOORD_REFLECTIVE,
                Matrix4f::Identity(), 10.0f, false, 0.0f, false);
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
