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
#include "MaterialBase.h"

#include "TextureManager.h"
#include "TextureUnit.h"
#include "Scene.h"

#include <y60/base/NodeValueNames.h>
#include <y60/base/NodeNames.h>

#include <asl/base/Logger.h>
#include <asl/base/string_functions.h>
#include <asl/base/checksum.h>


#include <algorithm>

#define DB(x)  x

using namespace asl;
using namespace std;
using namespace dom;

namespace y60 {

    MaterialBase::MaterialBase(dom::Node & theNode):
        Facade(theNode),
        IdTag::Plug(theNode),
        MaterialPropertiesTag::Plug(this),
        MaterialRequirementTag::Plug(this),
        NameTag::Plug(theNode),
        TransparencyTag::Plug(theNode),
        EnabledTag::Plug(theNode),
        dom::FacadeAttributePlug<LastActiveFrameTag>(this),
        _myShader(),
        _myLightingModel(LAMBERT),
        _myMaterialVersion(0),
        _myRequiresVersion(0),
        _myTexGenFlag(false),
        _ensuring(false)
    {}

    MaterialBase::~MaterialBase() {
        //AC_TRACE << "~MaterialBase() id=" << get<IdTag>() <<", name="<<get<NameTag>();
        AC_TRACE << "~MaterialBase() @"<<(void*)this;
    }

    Scene &
    MaterialBase::getScene() {
        Node * myRoot = getNode().getRootElement();
        return *myRoot->getFacade<Scene>();
    }
    const Scene &
    MaterialBase::getScene() const {
        const Node * myRoot = getNode().getRootElement();
        return *myRoot->getFacade<Scene>();
    }

    void
    MaterialBase::registerDependenciesRegistrators() {
        Facade::registerDependenciesRegistrators();
        getChild<MaterialPropertiesTag>()->registerDependenciesRegistrators();
    }

    void
    MaterialBase::registerDependenciesForMaterialupdate() {
        AC_TRACE << "MaterialBase::registerDependenciesForMaterialupdate()";
        Node & myNode = getNode();
        if (myNode) {
            AC_TRACE << "MaterialBase::registerDependenciesForMaterialupdate() - callback registered";
            EnabledTag::Plug::getValuePtr()->setImmediateCallBack(dynamic_cast_Ptr<MaterialBase>(getSelf()), &MaterialBase::updateLock);
        }
     }
    void
    MaterialBase::updateLock() {
        AC_TRACE << "MaterialBase::updateLock()";
        Node & myNode = getNode();
        if (myNode) {
            AC_TRACE << "MaterialBase::updateLock() - callback registered";
            if (get<EnabledTag>()) {
                ensureShader();
            }
        }
     }


    bool
    MaterialBase::reloadRequired() const {
        const MaterialRequirementFacadePtr myReqFacade = getChild<MaterialRequirementTag>();
        if (_myShader && myReqFacade && myReqFacade->getNode().nodeVersion() == _myRequiresVersion) {
            AC_TRACE << "Not reloading params for material " << get<NameTag>() << " last requiresVersion:" << _myRequiresVersion;
            AC_TRACE << "Not reloading params for material " << get<NameTag>() << " cur. requiresVersion:" << myReqFacade->getNode().nodeVersion();
            return false;
        }
        AC_DEBUG << "Reloading params for material " << get<NameTag>() << " last requiresVersion:" << _myRequiresVersion;
        AC_DEBUG << "Reloading params for material " << get<NameTag>() << " cur. requiresVersion:" << myReqFacade->getNode().nodeVersion();
        return true;
    }

    int
    MaterialBase::getGroup1Hash() const {
        return getChild<MaterialPropertiesTag>()->get<MaterialPropGroup1HashTag>();
    }
/*
    bool
    MaterialBase::rebindRequired() {
        if (getNode().getAttribute(ID_ATTRIB)->nodeVersion() != _myIdTagVersion) {
            _myIdTagVersion = getNode().getAttribute(ID_ATTRIB)->nodeVersion();
            return true;
        } else {
            return false;
        }
    }
*/
    const IShaderPtr
    MaterialBase::getShader() const {
        if (!get<EnabledTag>()) {
            throw MaterialLocked(std::string("material id=")+get<IdTag>()+" is locked and must be unlocked to be rendered", PLUS_FILE_LINE);
        }
        const_cast<MaterialBase*>(this)->ensureShader();
        return _myShader;
    };
    IShaderPtr MaterialBase::getShader() {
        if (!get<EnabledTag>()) {
            throw MaterialLocked(std::string("material id=")+get<IdTag>()+" is locked and must be unlocked to be rendered", PLUS_FILE_LINE);
        }
        ensureShader();
        return _myShader;
    };
    void MaterialBase::ensureShader() {
        AC_TRACE << "MaterialBase::ensureShader() - load?";
        if (reloadRequired()) {
            AC_TRACE << "MaterialBase::ensureShader() - load!";
            load();
        }
    }
#if 0
    //TODO: we should not load every time a property has changed, only when the requirements change
   void MaterialBase::ensureProperties() const {
       if (!_ensuring && getNode().nodeVersion() != _myMaterialVersion) {
           _ensuring = true;
           const_cast<MaterialBase*>(this)->load();
           _ensuring = false;
           _myMaterialVersion = getNode().nodeVersion();
       }
   }
#else
    // this is an improved version, but it does not work properly
   void MaterialBase::ensureProperties() const {
       if(!get<EnabledTag>()) {
           return;
       }
       if (_ensuring || getNode().nodeVersion() == _myMaterialVersion) {
           return;
       }
       _ensuring = true;
       if (!reloadRequired()) {
           _ensuring = false;
           return;
       }
       const_cast<MaterialBase*>(this)->load();
       _ensuring = false;
   }
 #endif

    static bool isSampler(const DOMString & theName) {
        return theName == "sampler1d" || theName == "sampler2d" || theName == "sampler3d" || theName == "samplerCUBE";
    }

    void
    MaterialBase::load() {
        if (!get<EnabledTag>()) {
            AC_TRACE << "MaterialBase::load not performed because material is not enabled, material id" << get<IdTag>();
            return;
        }
        DB(AC_TRACE << "MaterialBase::load " << getNode());
        MaterialBasePtr myMaterial = getNode().getFacade<MaterialBase>();

        Scene & myScene = getScene();

        IShaderPtr myShader;
        if (myScene.getShaderLibrary()) {
            // 2. ask shaderlib for shaderdefinition
            myShader = myScene.getShaderLibrary()->findShader(myMaterial);
            if (!myShader) {
                throw SceneException(string("No shader defintion found for Material: ") + get<NameTag>(), PLUS_FILE_LINE);
            }
            DB(AC_TRACE << "load shader");
            myShader->load(*myScene.getShaderLibrary());
            // 3. decide which material to build
            // 4. load material from node
            // 5. give material the found shaderdefinition
        }
        _myShader = myShader;
        DB(AC_TRACE << "Material::load(): Load material " << endl << getNode() <<
            endl << " with shader: " << (myShader ? myShader->getName() : "NULL"));

        if (myShader) {
            typedef map<string,bool> PropertyUsedMap;
            PropertyUsedMap myPropertyNames;

            NodePtr myPropertiesNode = getNode().childNode(PROPERTY_LIST_NAME);
            if (myPropertiesNode) {
                // remove sampler properties since they may change with the new shader
                for (int i = myPropertiesNode->childNodesLength()-1; i >= 0; --i) {
                    NodePtr myPropertyNode = myPropertiesNode->childNode(i);
                    const std::string & myPropTagName = myPropertyNode->nodeName();
                    const std::string & myPropName = myPropertyNode->getAttributeString("name");
                    if (isSampler(myPropTagName)) {
                        AC_DEBUG << "Removing sampler property '" << myPropName << "'";
                        myPropertiesNode->removeChild(myPropertyNode);
                    } else {
                        myPropertyNames[myPropName] = false;
                    }
                }
            }

            for (unsigned i = 0; i < myShader->getPropertyNodeCount(); ++i) {
                // default property with the shader default, unless the material already has that property
                myPropertiesNode = myShader->getPropertyNode(i);
                for (unsigned j = 0; j < myPropertiesNode->childNodesLength(); ++j) {
                    dom::NodePtr myPropertyNode  = myPropertiesNode->childNode(j);
                    if (myPropertyNode->nodeType() == dom::Node::ELEMENT_NODE && myPropertyNode->nodeName() != "#comment") {
                        const std::string & myPropName = myPropertyNode->getAttributeString("name");
                        if (myPropertyNames.find(myPropName) == myPropertyNames.end()) {
                            mergeProperties(myPropertyNode);
                        }
                    }
                }
            }
        }

        addTextures(getNode().childNode(TEXTUREUNIT_LIST_NAME), myScene.getTextureManager());

        if (myShader) {
            const VectorOfString * myLightingFeature = myShader->getFeatures(LIGHTING_FEATURE);
            if (myLightingFeature && myLightingFeature->size() == 1) {
                _myLightingModel = LightingModel(getEnumFromString((*myLightingFeature)[0],LightingModelString));
            } else {
                throw ShaderException("Shader has none or more than one lightingmodels.I do not know how to light this buddy.",
                    PLUS_FILE_LINE);
            }
            myShader->setup(*this);
        } else {
            _myLightingModel = UNLIT;
        }

        updateParams();
        DB(AC_TRACE << "Material::load() - id: " << myMaterial->get<IdTag>()
                    << ", name: " << myMaterial->get<NameTag>());
    }
/*
    void
    MaterialBase::setShader(IShaderPtr theShader) {
        _myShader = theShader;
    }
*/
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
            // the shader has properties not in the material list, add'em
            if (theShaderPropertyNode->nodeType() == dom::Node::ELEMENT_NODE &&
                theShaderPropertyNode->nodeName() != "#comment")
            {
                AC_TRACE << "MaterialBase::mergeProperties(): adding property "<<*theShaderPropertyNode<<" from shader to material:" << getNode();
                getNode().childNode(PROPERTY_LIST_NAME)->appendChild(
                                theShaderPropertyNode->cloneNode(Node::DEEP));
                AC_TRACE << "MaterialBase::mergeProperties(): done: adding property from shader to material:" << getNode();
            }
        } else {
            // set the value of the material property with the shader's property
            // in case the property was created by the facade using a default value
            // because the property did not exist in the material node

            AC_TRACE << "MaterialBase::mergeProperties(): setting material property to property from shader:"<< *theShaderPropertyNode<<" into material "<< getNode();

            myMaterialProperty->nodeValue((*theShaderPropertyNode)("#text").nodeValue());
            AC_TRACE << "MaterialBase::mergeProperties(): done: setting material property to property from shader:"<< *theShaderPropertyNode<<" into material "<< getNode();
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
                        // Q: What about the other samplers (sampler3d, samplerCube, etc)? [DS]
                        dom::NodePtr mySamplerNode = curPropList->childNode(j);
                        if (isSampler(mySamplerNode->nodeName())) {
                            unsigned myTextureIndex = (*mySamplerNode)("#text").dom::Node::nodeValueAs<unsigned>();
                            if (myTextureIndex >= _myTextureUnits.size()) {
                                string myTextureName = mySamplerNode->getAttributeString("name");
                                dom::NodePtr myTextureNode = myShaderTextures->childNodeByAttribute("texture","name",myTextureName);
                                if (myTextureNode) {
                                    AC_DEBUG << "adding textureunit " << myTextureIndex << " " << myTextureName << endl;
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
            AC_TRACE << "Not updating params for material " << get<NameTag>() << " last materialVersion:" << _myMaterialVersion << " last requiresVersion:" << _myRequiresVersion;
            AC_TRACE << "Not updating params for material " << get<NameTag>() << " cur. materialVersion::" << getNode().nodeVersion() << " cur. requiresVersion:" << myReqFacade->getNode().nodeVersion();
            return;
        }

        _myMaterialVersion = getNode().nodeVersion();
        _myRequiresVersion = myReqFacade->getNode().nodeVersion();
        //_myIdTagVersion    = getNode().getAttribute(ID_ATTRIB)->nodeVersion();
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
                AC_TRACE << " MaterialBase::updateParams(): myTexCoordFeature[="<<myTexUnit<<"]='"<<myTexCoordFeature[myTexUnit]<<"'";
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
        /*int myHash =*/ get<MaterialPropGroup1HashTag>();
    }

    void
    MaterialPropertiesFacade::registerDependenciesForMaterialPropGroup1HashTag() {
        MaterialPropGroup1HashTag::Plug::dependsOn<TargetBuffersTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<BlendFunctionTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<BlendEquationTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<BlendColorTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<GlowTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<LineWidthTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<PointSizeTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<LineSmoothTag,MaterialPropertiesFacade>(*this);
        MaterialPropGroup1HashTag::Plug::dependsOn<PointSmoothTag,MaterialPropertiesFacade>(*this);

        MaterialPropertiesFacadePtr mySelf = dynamic_cast_Ptr<MaterialPropertiesFacade>(getSelf());
        MaterialPropGroup1HashTag::Plug::getValuePtr()->setCalculatorFunction(mySelf, &MaterialPropertiesFacade::updateGroup1Hash);

    }

    void
    MaterialPropertiesFacade::updateGroup1Hash() {
        unsigned long myCRC32 = crc32(0L, Z_NULL, 0);
        const TargetBuffers & myMasks = get<TargetBuffersTag>();
        appendCRC32(myCRC32, (bool)myMasks[RED_MASK]);
        appendCRC32(myCRC32, (bool)myMasks[GREEN_MASK]);
        appendCRC32(myCRC32, (bool)myMasks[BLUE_MASK]);
        appendCRC32(myCRC32, (bool)myMasks[ALPHA_MASK]);
        const VectorOfBlendFunction & myBlendFunction = get<BlendFunctionTag>();
        if(myBlendFunction.size() >= 2) {
            appendCRC32(myCRC32, myBlendFunction[0]);
            appendCRC32(myCRC32, myBlendFunction[1]);
        }
        if(myBlendFunction.size() >= 4) {
            appendCRC32(myCRC32, myBlendFunction[2]);
            appendCRC32(myCRC32, myBlendFunction[3]);
        }
        const VectorOfBlendEquation & myBlendEquation = get<BlendEquationTag>();
        if(myBlendEquation.size() >= 1) {
            appendCRC32(myCRC32, myBlendEquation[0]);
        }
        if(myBlendEquation.size() >= 2) {
            appendCRC32(myCRC32, myBlendEquation[1]);
        }
        appendCRC32(myCRC32, get<BlendColorTag>());
        appendCRC32(myCRC32, get<GlowTag>());
        appendCRC32(myCRC32, get<LineWidthTag>());
        appendCRC32(myCRC32, get<PointSizeTag>());
        appendCRC32(myCRC32, get<LineSmoothTag>());
        appendCRC32(myCRC32, get<PointSmoothTag>());
 
        string myMaterialName = getNode().parentNode()->getFacade<MaterialBase>()->get<NameTag>();
        set<MaterialPropGroup1HashTag>(myCRC32);
    }

}
