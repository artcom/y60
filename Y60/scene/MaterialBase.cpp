//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "MaterialBase.h"
#include "TextureManager.h"
#include "TextureUnit.h"

#include <y60/NodeValueNames.h>
#include <y60/NodeNames.h>

#include <asl/Logger.h> 
#include <asl/string_functions.h>
#include <asl/checksum.h>


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
        _myTexGenFlag(false), _myMaterialVersion(0), _myRequiresVersion(0), _myIdTagVersion(0)
    {}

    MaterialBase::~MaterialBase() {
    }


    void
    MaterialBase::registerDependenciesRegistrators() {
        Facade::registerDependenciesRegistrators();        
        getChild<MaterialPropertiesTag>()->registerDependenciesRegistrators();
        
    }

    void
    MaterialBase::registerDependenciesForMaterialupdate() {}
    

    bool
    MaterialBase::reloadRequired(){
        // force childnodes to reconnect
        forceRebindChild<MaterialPropertiesTag>();
        forceRebindChild<MaterialRequirementTag>();

        MaterialRequirementFacadePtr myReqFacade = getChild<MaterialRequirementTag>();
        if (myReqFacade && myReqFacade->getNode().nodeVersion() == _myRequiresVersion) {
            return false;
        }
        return true;
    }

    int
    MaterialBase::getGroup1Hash() const {
        return getChild<MaterialPropertiesTag>()->get<MaterialPropGroup1HashTag>();
    }

    bool 
    MaterialBase::rebindRequired() {
		if (getNode().getAttribute(ID_ATTRIB)->nodeVersion() != _myIdTagVersion) {
            _myIdTagVersion = getNode().getAttribute(ID_ATTRIB)->nodeVersion();
            return true;
        } else {
            return false;
        }
    }

    void
    MaterialBase::load(TextureManagerPtr theTextureManager) {

        addTextures(getNode().childNode(TEXTUREUNIT_LIST_NAME), theTextureManager);

        if (_myShader) {
            const VectorOfString * myLightingFeature = _myShader->getFeatures(LIGHTING_FEATURE);
            if (myLightingFeature && myLightingFeature->size() == 1) {
                _myLightingModel = LightingModel(getEnumFromString((*myLightingFeature)[0],LightingModelString));
            } else {
                throw ShaderException("Shader has none or more than one lightingmodels.I do not know how to light this buddy.",
                    PLUS_FILE_LINE);
            } 
            _myShader->setup(*this);
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
        //const Facade::PropertyMap & myPropertyMap = myReqFacade->getProperties();
        
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
        VectorOfTextureUsage myFeatures = _myShader->getTextureFeature();
        if (theTextureSlot >= myFeatures.size()) {
            throw ShaderException(string("Shader '" + _myShader->getName() 
                        + "' cannot handle " + asl::as_string(theTextureSlot) + " textures\n")
                    + as_string(myFeatures), PLUS_FILE_LINE);
        }
        return myFeatures[theTextureSlot];
    }
    
    void
    MaterialBase::addTextures(const dom::NodePtr theTextureUnitListNode,
                               TextureManagerPtr theTextureManager)
    {
        if (theTextureUnitListNode) {
            AC_DEBUG << "MaterialBase::addTextures " << *theTextureUnitListNode;
        }

        _myTextureUnits.clear();
        // first thing is to check if the chosen shader can handle textures of this mode
        // if not, do not load the texture
        if (_myShader && _myShader->hasFeature(TEXTURE_FEATURE)) {

            // add textureunits
            if (theTextureUnitListNode) {
                unsigned myTextureCount = theTextureUnitListNode->childNodesLength(TEXTUREUNIT_NODE_NAME);
                for (unsigned i = 0; i < myTextureCount; ++i) {
                    dom::NodePtr myTextureNode = theTextureUnitListNode->childNode(TEXTUREUNIT_NODE_NAME,i);
                    addTexture(myTextureNode, theTextureManager);
                }
            }

            // does the shader have all necessary textures? look for default textures too.
            dom::NodePtr myShaderTextures = _myShader->getDefaultTextures();
            if (myShaderTextures) {
                for (unsigned i = 0; i < _myShader->getPropertyNodeCount(); ++i) {
                    dom::NodePtr curPropList = _myShader->getPropertyNode(i);

                    for (unsigned j = 0; j < curPropList->childNodesLength(); ++j) {
                        // Q: What about the other samplers (3d, Cube, etc)? [DS]
                        dom::NodePtr mySamplerNode = curPropList->childNode(j);
                        if (mySamplerNode->nodeName() == "sampler2d") { 
                            unsigned myTextureIndex = (*mySamplerNode)("#text").dom::Node::nodeValueAs<unsigned>();
                            if (myTextureIndex >= _myTextureUnits.size()) {
                                string myTextureName = mySamplerNode->getAttributeString("name");
                                dom::NodePtr myTextureNode = myShaderTextures->childNodeByAttribute("texture","name",myTextureName);
                                if (myTextureNode) {
                                    AC_DEBUG << "Loading default texture " << myTextureIndex << " " << myTextureName << endl;
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
    MaterialBase::addTexture(dom::NodePtr theTextureUnitNode, TextureManagerPtr theTextureManager) {

        AC_DEBUG << "MaterialBase::addTexture " << *theTextureUnitNode;
        unsigned myMaxUnits = _myShader->getMaxTextureUnits();
        if (_myTextureUnits.size() < myMaxUnits) {            
            TextureUnitPtr myTextureUnit = theTextureUnitNode->getFacade<TextureUnit>();
            myTextureUnit->setTextureManager(theTextureManager);
            _myTextureUnits.push_back(myTextureUnit);
            TexturePtr myTexture = myTextureUnit->getTexture();
            if (myTexture && myTexture->getTextureId() == 0) {
                //AC_DEBUG << "addTexture triggering texture upload id=" << myTexture->get<IdTag>() << " texId=" << myTexture->getTextureId();
                myTexture->triggerUpload();
            }
        } else {
            AC_WARNING << "Your OpenGL implementation only supports " 
                 << asl::as_string(myMaxUnits) << " texture units, "
                 << endl << "              ignoring: "
                 << theTextureUnitNode->getAttributeString(TEXTUREUNIT_TEXTURE_ATTRIB)
                 << " of material " << get<NameTag>() << endl;
        }
    }

    unsigned
    MaterialBase::getTextureUnitCount() const {
        return _myTextureUnits.size();
    }

    const TextureUnit &
    MaterialBase::getTextureUnit(unsigned theIndex) const {
        return *(_myTextureUnits[theIndex]);
    }

    const MaterialParameterVector &
    MaterialBase::getVertexParameters() const {
        return _myShader->getVertexParameters();
    }

    bool
    MaterialBase::getDepthBufferWrite() const {
        MaterialPropertiesFacadePtr myPropFacade = getChild<MaterialPropertiesTag>();
        return myPropFacade->get<TargetBuffersTag>()[DEPTH_MASK];
    }

    void
    MaterialBase::setDepthBufferWrite(bool theFlag) {
        MaterialPropertiesFacadePtr myPropFacade = getChild<MaterialPropertiesTag>();
        TargetBuffers mySet = myPropFacade->get<TargetBuffersTag>();
        mySet.set(DEPTH_MASK, theFlag);
        myPropFacade->set<TargetBuffersTag>(mySet);
    }

    void
    MaterialBase::updateParams() {

        // check node version if update is necessary
        MaterialRequirementFacadePtr myReqFacade = getChild<MaterialRequirementTag>();
        if (!getNode() ||
            (getNode().nodeVersion() == _myMaterialVersion && myReqFacade->getNode().nodeVersion() == _myRequiresVersion)) {
            return;
        }

        // force childnodes to reconnect
        forceRebindChild<MaterialPropertiesTag>();
        forceRebindChild<MaterialRequirementTag>();

        _myMaterialVersion = getNode().nodeVersion();
        _myRequiresVersion = myReqFacade->getNode().nodeVersion();
        _myIdTagVersion    = getNode().getAttribute(ID_ATTRIB)->nodeVersion();
        AC_DEBUG << "Updating params for material " << get<NameTag>() << " materialVersion:" << _myMaterialVersion << " requiresVersion:" << _myRequiresVersion;

        _myTexGenModes.clear();
        _myTexGenParams.clear();
        _myTexGenFlag = false;

        NodePtr myMappingRequirement = myReqFacade->getProperty(MAPPING_FEATURE);
        if (myMappingRequirement) {

            VectorOfRankedFeature myTexCoordFeatures = myMappingRequirement->nodeValueAs<VectorOfRankedFeature>();
            if (myTexCoordFeatures.size() > 0) {
 
                const VectorOfString & myTexCoordFeature = myTexCoordFeatures[0]._myFeature;
                for (unsigned myTexUnit = 0 ; myTexUnit < myTexCoordFeature.size(); ++myTexUnit) {
                    TexGenMode myTexGenModes;
                    TexGenParams myTexGenParams;

                    TexCoordMapping myTexCoordMode = TexCoordMapping( 
                            asl::getEnumFromString(myTexCoordFeature[myTexUnit], TexCoordMappingStrings));

                    // if any texunit has non-uvmap then texgen is on for the material
                    if (myTexCoordMode != UV_MAP) {
                        _myTexGenFlag = true;
                    }

                    switch (myTexCoordMode) {
                        case PLANAR_PROJECTION:
                        case CUBE_PROJECTION:
                            //AC_DEBUG << "CUBE or PLANAR_PROJECTION";
                            myTexGenModes.push_back(OBJECT_LINEAR); // generate s
                            myTexGenModes.push_back(OBJECT_LINEAR); // and t
                            myTexGenModes.push_back(OBJECT_LINEAR); // and r
                            break;
                        case FRONTAL_PROJECTION:
                            //AC_DEBUG << "FRONTAL_PROJECTION";
                            myTexGenModes.push_back(EYE_LINEAR); // generate s
                            myTexGenModes.push_back(EYE_LINEAR); // and t
                            myTexGenModes.push_back(EYE_LINEAR); // and r
                            break;
                        case SPHERICAL_PROJECTION:
                            //AC_DEBUG << "SPHERICAL_PROJECTION";
                            myTexGenModes.push_back(SPHERE_MAP);
                            myTexGenModes.push_back(SPHERE_MAP);
                            break;
                        case CYLINDRICAL_PROJECTION:
                            //AC_DEBUG << "CYLINDRICAL_PROJECTION";
                            myTexGenModes.push_back(SPHERE_MAP);
                            myTexGenModes.push_back(OBJECT_LINEAR);
                            break;
                        case UV_MAP:
                            // do not use any texgen mode at all
                            // do not generate uvcoords at all
                            myTexGenModes.push_back(NONE);
                            break;
                        case REFLECTIVE:
                            //AC_DEBUG << "REFLECTIVE";
                            myTexGenModes.push_back(REFLECTION); // generate s
                            myTexGenModes.push_back(REFLECTION); // and t
                            myTexGenModes.push_back(REFLECTION); // and r
                            break;
                        case PERSPECTIVE_PROJECTION:
                        default:
                            throw ShaderException(string("Invalid texgenmode '") + myTexCoordFeature[myTexUnit] + 
                                    "' in material " + get<NameTag>(), PLUS_FILE_LINE);
                    }

                    // the following modes do not use texgenparams
                    if (myTexCoordMode != UV_MAP &&
                        myTexCoordMode != SPHERICAL_PROJECTION &&
                        myTexCoordMode != REFLECTIVE) {
 
                        // fetch texgen params
                        MaterialPropertiesFacadePtr myPropertyFacade = getChild<MaterialPropertiesTag>();
                        string myTexGenParamName = string("texgenparam") + asl::as_string(myTexUnit);
                        NodePtr myTexGenProperty = myPropertyFacade->getProperty(myTexGenParamName);
                        if (myTexGenProperty) {
                            VectorOfVector4f myTexGenParamsVec = myTexGenProperty->nodeValueAs<VectorOfVector4f>();
                            for (unsigned i = 0; i < myTexGenParamsVec.size(); ++i) {
                                myTexGenParams.push_back(myTexGenParamsVec[i]);
                            }
                        } else {
                            AC_WARNING << "No such property: " << myTexGenParamName <<" in material " << get<NameTag>();
                        }
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
        AC_DEBUG << "updateParams done";
    }

    void
    MaterialPropertiesFacade::registerDependenciesRegistrators() {
        MaterialPropGroup1HashTag::Plug::setReconnectFunction(&MaterialPropertiesFacade::registerDependenciesForMaterialPropGroup1HashTag);
        int myHash = get<MaterialPropGroup1HashTag>();
    }

    void
    MaterialPropertiesFacade::registerDependenciesForMaterialPropGroup1HashTag() {
        MaterialPropGroup1HashTag::Plug::dependsOn<TargetBuffersTag,MaterialPropertiesFacade>(*this);  
        MaterialPropGroup1HashTag::Plug::dependsOn<BlendFunctionTag,MaterialPropertiesFacade>(*this);  
        MaterialPropGroup1HashTag::Plug::dependsOn<BlendEquationTag,MaterialPropertiesFacade>(*this);  
        MaterialPropGroup1HashTag::Plug::dependsOn<GlowTag,MaterialPropertiesFacade>(*this);  
        MaterialPropGroup1HashTag::Plug::dependsOn<LineWidthTag,MaterialPropertiesFacade>(*this);  
        MaterialPropGroup1HashTag::Plug::dependsOn<PointSizeTag,MaterialPropertiesFacade>(*this);  
        MaterialPropGroup1HashTag::Plug::dependsOn<LineSmoothTag,MaterialPropertiesFacade>(*this);  

        MaterialPropertiesFacadePtr mySelf = dynamic_cast_Ptr<MaterialPropertiesFacade>(getSelf());
        MaterialPropGroup1HashTag::Plug::getValuePtr()->setCalculatorFunction(mySelf, &MaterialPropertiesFacade::updateGroup1Hash);
        
    }

    void
    MaterialPropertiesFacade::updateGroup1Hash() {
        unsigned long myCRC32 = crc32(0L, Z_NULL, 0);
        const TargetBuffers & myMasks = get<TargetBuffersTag>();
        appendCRC32(myCRC32, myMasks[RED_MASK]);
        appendCRC32(myCRC32, myMasks[GREEN_MASK]);
        appendCRC32(myCRC32, myMasks[BLUE_MASK]);
        appendCRC32(myCRC32, myMasks[ALPHA_MASK]);
        const VectorOfBlendFunction & myBlendFunction = get<BlendFunctionTag>();
        appendCRC32(myCRC32, myBlendFunction[0]);
        appendCRC32(myCRC32, myBlendFunction[1]);
        //appendCRC32(myCRC32, get<BlendFunctionTag>());
        appendCRC32(myCRC32, get<BlendEquationTag>());
        appendCRC32(myCRC32, get<GlowTag>());
        appendCRC32(myCRC32, get<LineWidthTag>());
        appendCRC32(myCRC32, get<PointSizeTag>());
        appendCRC32(myCRC32, get<LineSmoothTag>());        

        string myMaterialName = getNode().parentNode()->getFacade<MaterialBase>()->get<NameTag>();
        set<MaterialPropGroup1HashTag>(myCRC32);
    }

}
