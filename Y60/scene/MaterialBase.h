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
//   $RCSfile: MaterialBase.h,v $
//   $Author: christian $
//   $Revision: 1.32 $
//   $Date: 2005/03/24 23:36:03 $
//
//   Description:
//
//=============================================================================

#ifndef _ac_scene_MaterialBase_h_
#define _ac_scene_MaterialBase_h_

#include "Texture.h"
#include "MaterialParameter.h"
#include "IShader.h"
#include "MaterialRequirementList.h"

#include <asl/Exception.h>
#include <y60/DataTypes.h>
#include <dom/Nodes.h>
#include <string>
#include <vector>

namespace y60 {
    class TextureManager;
   
    DEFINE_EXCEPTION(MaterialBaseException, asl::Exception);

    class MaterialBase {
        public:
            typedef std::vector<TexCoordMode> TexGenMode;
            typedef std::vector<TexGenMode>   TexGenModeList;

            typedef std::vector<asl::Vector4f> TexGenParams;
            typedef std::vector<TexGenParams>  TexGenParamsList;

            MaterialBase(MaterialRequirementList & theMaterialRequirement);
            virtual ~MaterialBase();

            const std::string & getId() const;
            const std::string & getName() const;

            virtual unsigned getTextureCount() const;
            virtual const Texture & getTexture(unsigned myIndex) const;

            virtual void load(const dom::NodePtr theMaterialNode,
                    TextureManager & theTextureMananger);

            virtual void setup(dom::NodePtr theSceneNode) {
            }

            virtual void update(TextureManager & theTextureManager, const dom::NodePtr theImages);

            const MaterialParameterVectorPtr getVertexParameters() const;
            MaterialRequirementList & getMaterialRequirements();

            void setShader(IShaderPtr theShader);
            const IShaderPtr getShader() const { return _myShader; };
            IShaderPtr getShader() { return _myShader; };

            void mergeProperties(const dom::NodePtr & thePropertyNode);
            const dom::NodePtr & getProperties() const;
            const LightingModel getLightingModel() const { return _myLightingModel; }

            template <class T>
            T
            getPropertyValue(const dom::NodePtr & thePropertyNode) const {
                if (!thePropertyNode) {
                    throw MaterialBaseException(std::string("getPropertyValue: invalid null propertynode") +
                                                " in material:\n"+asl::as_string(*_myMaterialNode),
                                                PLUS_FILE_LINE);
                }
                return (*thePropertyNode)("#text").dom::Node::nodeValueAs<T>();
            }

            TextureUsage 
                MaterialBase::getTextureUsage(unsigned theTextureSlot) const;
        
            template <class T>
            T
            getPropertyValue(const std::string & theName) const {
                dom::NodePtr myPropertyNode = findPropertyNode(theName);
                if (!myPropertyNode) {
                    throw MaterialBaseException(std::string("getPropertyValue: Could not find property with name'") + theName +
                    "' in material:\n"+asl::as_string(*_myMaterialNode),
                    PLUS_FILE_LINE);
               }
                return getPropertyValue<T>(myPropertyNode);
            }

            dom::NodePtr findPropertyNode(const std::string & thePropertyName) const {
                unsigned myChildCount = _myProperties->childNodesLength();
                for (unsigned i = 0; i < myChildCount; ++i) {
                    if (_myProperties->childNode(i)->getAttributeString("name") == thePropertyName) {
                        return _myProperties->childNode(i);
                    }
                }
                return dom::NodePtr(0);
            }

            bool hasProperty(const std::string & thePropertyName) const {
                return (findPropertyNode(thePropertyName) != 0);
            }

            bool hasTransparency() const;
            bool writesDepthBuffer() const;

            // texgen parameters
            void updateParams();

            bool hasTexGen() const {
                return _myTexGenFlag;
            }

            TexGenModeList getTexGenModes() const {
                return _myTexGenModes;
            }

			TexGenParamsList getTexGenParams() const {
                return _myTexGenParams;
            }

            // for updating a texture, that has changed via the texturemanager, not the dom
            // if you need another version, change the name as well ;-)
            // void updateTextureFromTextureManager(const std::string theTexturename, TextureManager & theTextureMananger);

        protected:
            MaterialRequirementList    _myRequirements;
            IShaderPtr                 _myShader;

            dom::NodePtr getNode() {
                return _myMaterialNode;
            }

        private:
            void addTextures(const dom::NodePtr theTextureListNode,
                              TextureManager & theTextureMananger);
            void addTexture(dom::NodePtr theTextureNode, TextureManager & theTextureManager);

            std::string             _myId;
            std::string             _myName;
            std::vector<TexturePtr> _myTextures;
            dom::NodePtr            _myProperties;
            dom::NodePtr            _myMaterialNode;
            LightingModel           _myLightingModel;
            MaterialRequirementList _myMaterialRequirement;

            unsigned long     _myMaterialVersion;
            bool              _myTexGenFlag;
			TexGenModeList    _myTexGenModes;
			TexGenParamsList  _myTexGenParams;
     };

     typedef asl::Ptr<MaterialBase>       MaterialBasePtr;
     typedef std::vector<MaterialBasePtr> MaterialBasePtrVector;
}

#endif
