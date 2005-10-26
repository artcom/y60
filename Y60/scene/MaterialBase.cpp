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
//   $RCSfile: MaterialBase.cpp,v $
//   $Author: pavel $
//   $Revision: 1.45 $
//   $Date: 2005/04/24 00:41:21 $
//
//  Description: A simple material class.
//
//=============================================================================

#include "MaterialBase.h"
#include "TextureManager.h"

#include <y60/Image.h>
#include <y60/NodeValueNames.h>
#include <y60/NodeNames.h>

#include <asl/Logger.h> 
#include <asl/string_functions.h>

#include <algorithm>

#define DB(x) // x

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    MaterialBase::MaterialBase(dom::Node & theNode): 
        
        IdTag::Plug(theNode),
        NameTag::Plug(theNode),
		TransparencyTag::Plug(theNode),
		MaterialPropertiesTag::Plug(this),
		MaterialRequirementTag::Plug(this),      
        Facade(theNode),
        _myShader(0),
        _myLightingModel(LAMBERT),
        _myTexGenFlag(false), _myMaterialVersion(0), _myRequiresVersion(0)
    {}

    MaterialBase::~MaterialBase() {
    }

    void
    MaterialBase::update(TextureManager & theTextureManager, const dom::NodePtr theImages) {
        for (unsigned i = 0 ; i <_myTextures.size(); ++i) {
            _myTextures[i]->update(theTextureManager);
        }
    }

    bool
    MaterialBase::reloadRequired() const {
		MaterialRequirementFacadePtr myReqFacade = getChild<MaterialRequirementTag>();
        if (myReqFacade->getNode().nodeVersion() != _myRequiresVersion) {
            return true;
        }
        return false;
    }

    void
    MaterialBase::load(TextureManager & theTextureManager) {
        addTextures(getNode().childNode(TEXTURE_LIST_NAME), theTextureManager);

        if (_myShader) {
            const VectorOfString * myLightingFeature = _myShader->getFeatures(LIGHTING_FEATURE);
            if (myLightingFeature && myLightingFeature->size() == 1) {
                _myLightingModel = LightingModel(getEnumFromString((*myLightingFeature)[0],LightingModelString));
            } else {
                throw ShaderException("Shader has none or more than one lightingmodels.I do not know how to light this buddy.",
                    PLUS_FILE_LINE);
            } 
        } else {
            _myLightingModel = UNLIT;
        }

        updateParams();
    }

    void
    MaterialBase::setShader(IShaderPtr theShader) {
        _myShader = theShader;
    }

    void
    MaterialBase::mergeProperties(const dom::NodePtr & theShaderPropertyNode) {
        if ( ! theShaderPropertyNode) {
            AC_WARNING << "No properties found!" << endl;
            return;
        }

		MaterialPropertiesFacadePtr myReqFacade = getChild<MaterialPropertiesTag>();
        const Facade::PropertyMap & myPropertyMap = myReqFacade->getProperties();
		
		dom::NodePtr myMaterialProperty = 
            myReqFacade->getProperty(theShaderPropertyNode->getAttributeString(NAME_ATTRIB));

		if (!myMaterialProperty) {
			// the shader has properties not in the material liste, add'em
			if (theShaderPropertyNode->nodeType() == dom::Node::ELEMENT_NODE &&
				theShaderPropertyNode->nodeName() != "#comment") 
            {
				getNode().childNode(PROPERTY_LIST_NAME)->appendChild(
						        theShaderPropertyNode->cloneNode(Node::DEEP));
			}
		} else {
			// set the value of the material with the shaders value
            myMaterialProperty->nodeValue((*theShaderPropertyNode)("#text").nodeValue());
		}
    }

    TextureUsage MaterialBase::getTextureUsage(unsigned theTextureSlot) const {
        const VectorOfString * myFeatures = _myShader->getFeatures(TEXTURE_FEATURE);
        if (!myFeatures || theTextureSlot >= myFeatures->size()) {
            throw ShaderException(string("Shader '" + _myShader->getName() 
                        + "' cannot handle " + asl::as_string(theTextureSlot) + " textures\n")
                    //+ "Material:\n" + as_string(*_myMaterialNode) + "\nShader Features: "
                    + as_string(*myFeatures), PLUS_FILE_LINE);
        }
        return TextureUsage(getEnumFromString((*myFeatures)[theTextureSlot], TextureUsageStrings));
    }
    
    void
    MaterialBase::addTextures(const dom::NodePtr theTextureListNode,
                               TextureManager & theTextureManager)
    {
        // first thing is to check if the chosen shader can handle textures of this mode
        // if not, do not load the texture
        if (_myShader && _myShader->hasFeature(TEXTURE_FEATURE)) {
            if (theTextureListNode) {
                unsigned myTextureCount = theTextureListNode->childNodesLength(TEXTURE_NODE_NAME);
                for (unsigned i = 0; i < myTextureCount; ++i) {
					dom::NodePtr myTextureNode = theTextureListNode->childNode(TEXTURE_NODE_NAME,i);
                    addTexture(myTextureNode, theTextureManager);
                }
            }

            // does the shader have all necessary textures? look for default textures too.
            dom::NodePtr myShaderTextures = _myShader->getDefaultTextures();
            if (myShaderTextures) {
                for (unsigned i = 0; i < _myShader->getPropertyNodeCount(); ++i) {
                    dom::NodePtr curPropList = _myShader->getPropertyNode(i);

                    for (unsigned j = 0; j < curPropList->childNodesLength(); ++j) {
                        if (curPropList->childNode(j)->nodeName() == "sampler2d") {
                            dom::NodePtr mySamplerNode = curPropList->childNode(j);
                            unsigned myTextureIndex = (*mySamplerNode)("#text").dom::Node::nodeValueAs<unsigned>();
                            if (myTextureIndex >= _myTextures.size()) {
                                string myTextureName = mySamplerNode->getAttributeString("name");
                                dom::NodePtr myTextureNode = myShaderTextures->childNodeByAttribute("texture","name",myTextureName);
                                if (myTextureNode) {
                                    AC_INFO << " loading default texture " << myTextureIndex << " " << myTextureName << endl;
                                    addTexture(myTextureNode, theTextureManager);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void
    MaterialBase::addTexture(dom::NodePtr theTextureNode, TextureManager & theTextureManager) {
        unsigned myMaxUnits = theTextureManager.getMaxTextureUnits();
        if (_myTextures.size() < myMaxUnits) {
            
			TexturePtr myTexture = TexturePtr(new Texture(*theTextureNode));
			myTexture->update(theTextureManager);
            _myTextures.push_back(myTexture);
            
        } else {
            AC_WARNING << "Your OpenGL implementation only supports " 
                 << asl::as_string(myMaxUnits) << " texture units, "
                 << endl << "              ignoring: "
                 << theTextureNode->getAttributeString(TEXTURE_IMAGE_ATTRIB)
                 << " of material " << get<NameTag>() << endl;
        }
    }

    unsigned
    MaterialBase::getTextureCount() const {
        return _myTextures.size();
    }

    const Texture &
    MaterialBase::getTexture(unsigned theIndex) const {
        return *(_myTextures[theIndex]);
    }

    const MaterialParameterVector &
    MaterialBase::getVertexParameters() const {
        return _myShader->getVertexParameters();
    }

    bool
    MaterialBase::writesDepthBuffer() const {
        if (getNode().getAttribute(WRITE_DEPTH_BUFFER_ATTRIB)) {
            return getNode().getAttributeValue<bool>(WRITE_DEPTH_BUFFER_ATTRIB);
        }
        return true;
    }

    void
    MaterialBase::updateParams() {
        // check node version if update is necessary
		MaterialRequirementFacadePtr myReqFacade = getChild<MaterialRequirementTag>();
        if (!getNode() ||
            (getNode().nodeVersion() == _myMaterialVersion && myReqFacade->getNode().nodeVersion() == _myRequiresVersion)) {
            return;
        }

        _myMaterialVersion = getNode().nodeVersion();
        _myRequiresVersion = myReqFacade->getNode().nodeVersion();
        AC_DEBUG << "Updating params for material " << get<NameTag>() << " materialVersion:" << _myMaterialVersion << " requiresVersion:" << _myRequiresVersion;

        _myTexGenModes.clear();
        _myTexGenParams.clear();
        _myTexGenFlag = false;

        NodePtr myMappingRequirement = myReqFacade->getProperty(MAPPING_FEATURE);
        if (myMappingRequirement) {

            VectorOfRankedFeature myTexCoordFeatures = myMappingRequirement->nodeValueAs<VectorOfRankedFeature>();
            if (myTexCoordFeatures.size() > 0) {
 
                // assume it's on until told otherwise
                _myTexGenFlag = true;

                const VectorOfString & myTexCoordFeature = myTexCoordFeatures[0]._myFeature;
                for (unsigned myTexUnit = 0 ; myTexUnit < myTexCoordFeature.size(); ++myTexUnit) {
                    TexGenMode myTexGenModes;
                    TexGenParams myTexGenParams;

                    TexCoordMapping myTexCoordMode = TexCoordMapping( 
                            asl::getEnumFromString(myTexCoordFeature[myTexUnit], TexCoordMappingStrings));
                    switch (myTexCoordMode) {
                        case PLANAR_PROJECTION:
                            //AC_DEBUG << "PLANAR_PROJECTION" << endl;
                            myTexGenModes.push_back(OBJECT_LINEAR); // generate s
                            myTexGenModes.push_back(OBJECT_LINEAR); // and t
                            myTexGenModes.push_back(OBJECT_LINEAR); // and r
                            break;
                        case CUBE_PROJECTION:
                            //AC_DEBUG << "CUBE_PROJECTION" << endl;
                            myTexGenModes.push_back(OBJECT_LINEAR); // generate s
                            myTexGenModes.push_back(OBJECT_LINEAR); // and t
                            myTexGenModes.push_back(OBJECT_LINEAR); // and r
                            break;
                        case FRONTAL_PROJECTION:
                            //AC_DEBUG << "FRONTAL_PROJECTION" << endl;
                            myTexGenModes.push_back(EYE_LINEAR); // generate s
                            myTexGenModes.push_back(EYE_LINEAR); // and t
                            myTexGenModes.push_back(EYE_LINEAR); // and r
                            break;
                        case SPHERICAL_PROJECTION:
                            //AC_DEBUG << "SPHERICAL_PROJECTION" << endl;
                            myTexGenModes.push_back(SPHERE_MAP); // generate s
                            myTexGenModes.push_back(SPHERE_MAP); // and t
                            break;
                        case CYLINDRICAL_PROJECTION:
                            //AC_DEBUG << "CYLINDRICAL_PROJECTION" << endl;
                            myTexGenModes.push_back(SPHERE_MAP); // generate s
                            myTexGenModes.push_back(OBJECT_LINEAR); // and t
                            break;
                        case UV_MAP:
                            // do not use any texgen mode at all
                            // do not generate uvcoords at all
                            myTexGenModes.push_back(NONE);
                            _myTexGenFlag = false;
                            break;
                        case PERSPECTIVE_PROJECTION:
                        default:
                            throw ShaderException(string("Invalid texgenmode '") + myTexCoordFeature[myTexUnit] + "' in material " + get<NameTag>(), PLUS_FILE_LINE);
                    }

                    // fetch texgen params
					MaterialPropertiesFacadePtr myPropertyFacade = getChild<MaterialPropertiesTag>();
                    string myTexGenParamName = string("texgenparam") + asl::as_string(myTexUnit);
					NodePtr myTexGenProperty = myPropertyFacade->getProperty(myTexGenParamName);
                    if (myTexGenProperty) {
						VectorOfVector4f myTexGenParamsVec = (*myTexGenProperty)("#text").dom::Node::nodeValueAs<VectorOfVector4f>();
                        for (unsigned i = 0; i < myTexGenParamsVec.size(); ++i) {
                            myTexGenParams.push_back(myTexGenParamsVec[i]);
                        }
                    } else {
                        AC_DEBUG << "No such property: " << myTexGenParamName <<" in material " << get<NameTag>();
                    }

                    _myTexGenModes.push_back(myTexGenModes);
                    _myTexGenParams.push_back(myTexGenParams);
                }
            }
            if (_myTexGenFlag) {
                AC_DEBUG << "TexGen enabled for material " << get<NameTag>();
            }
        } else {
            AC_DEBUG << "No such feature '" << MAPPING_FEATURE << "' for material " << get<NameTag>();
        }
    }
}
