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

#ifndef _ac_scene_MaterialBase_h_
#define _ac_scene_MaterialBase_h_

#include "y60_scene_settings.h"

#include "TextureUnit.h"
#include "MaterialParameter.h"
#include "PropertyListFacade.h"
#include "IShader.h"

#include <y60/base/CommonTags.h>
#include <y60/base/PropertyNames.h>
#include <asl/dom/Facade.h>
#include <asl/dom/PropertyPlug.h>
#include <asl/dom/AttributePlug.h>
#include <asl/dom/ChildNodePlug.h>

#include <asl/base/Exception.h>
#include <y60/base/DataTypes.h>
#include <asl/dom/Nodes.h>
#include <string>
#include <vector>

namespace y60 {
    class TextureManager;

    DEFINE_EXCEPTION(MaterialBaseException, asl::Exception);
    DEFINE_EXCEPTION(MaterialLocked, MaterialBaseException);

    static y60::VectorOfBlendFunction getDefaultBlendFunction() {
        y60::VectorOfBlendFunction myResult;
        myResult.push_back(SRC_ALPHA);
        myResult.push_back(ONE_MINUS_SRC_ALPHA);
        return myResult;
    }
    static y60::VectorOfBlendEquation getDefaultBlendEquation() {
        y60::VectorOfBlendEquation myResult;
        myResult.push_back(EQUATION_ADD);
        return myResult;
    }

#define DEFINE_MATERIAL_PROPERTY_TAG(theTagName, theType, thePropertyName, theDefault) \
    DEFINE_PROPERTY_TAG(theTagName,  MaterialPropertiesFacade, theType, y60::getTypeName<theType>(), thePropertyName,  PROPERTY_LIST_NAME, theDefault);

    DEFINE_MATERIAL_PROPERTY_TAG(MaterialAmbientTag, asl::Vector4f, AMBIENT_PROPERTY, asl::Vector4f(0.0f,0.0f,0.0f,1));
    DEFINE_MATERIAL_PROPERTY_TAG(MaterialDiffuseTag, asl::Vector4f, DIFFUSE_PROPERTY, asl::Vector4f(1.0f,1.0f,1.0f,1));
    DEFINE_MATERIAL_PROPERTY_TAG(MaterialSpecularTag, asl::Vector4f, SPECULAR_PROPERTY, asl::Vector4f(0,0,0,1));
    DEFINE_MATERIAL_PROPERTY_TAG(MaterialEmissiveTag, asl::Vector4f, EMISSIVE_PROPERTY, asl::Vector4f(0,0,0,1));
    DEFINE_MATERIAL_PROPERTY_TAG(SurfaceColorTag, asl::Vector4f, SURFACE_COLOR_PROPERTY, asl::Vector4f(0.0f,0.0f,0.0f,1));
    DEFINE_MATERIAL_PROPERTY_TAG(ShininessTag, float, SHININESS_PROPERTY, float(1.0));
    DEFINE_MATERIAL_PROPERTY_TAG(TargetBuffersTag, y60::TargetBuffers, TARGETBUFFERS_PROPERTY, TargetBuffers((1<<RED_MASK)|(1<<GREEN_MASK)|(1<<BLUE_MASK)|(1<<ALPHA_MASK)|(1<<DEPTH_MASK)));
    DEFINE_MATERIAL_PROPERTY_TAG(BlendFunctionTag, y60::VectorOfBlendFunction, BLENDFUNCTION_PROPERTY, getDefaultBlendFunction());
    DEFINE_MATERIAL_PROPERTY_TAG(BlendEquationTag, y60::VectorOfBlendEquation, BLENDEQUATION_PROPERTY, getDefaultBlendEquation());
    DEFINE_MATERIAL_PROPERTY_TAG(BlendColorTag, asl::Vector4f, BLENDCOLOR_PROPERTY, asl::Vector4f(0,0,0,0));
    DEFINE_MATERIAL_PROPERTY_TAG(GlowTag, float, GLOW_PROPERTY, 0.0f);
    DEFINE_MATERIAL_PROPERTY_TAG(LineWidthTag, float, LINEWIDTH_PROPERTY, float(1.0));
    DEFINE_MATERIAL_PROPERTY_TAG(PointSizeTag, asl::Vector3f, POINTSIZE_PROPERTY, asl::Vector3f(1,1,1));
    DEFINE_MATERIAL_PROPERTY_TAG(LineSmoothTag, bool, LINESMOOTH_PROPERTY, bool(false));
    DEFINE_MATERIAL_PROPERTY_TAG(PointSmoothTag, bool, POINTSMOOTH_PROPERTY, bool(false));
    DEFINE_FACADE_ATTRIBUTE_TAG(MaterialPropGroup1HashTag, int, "material_prop_group1_hash_tag", 0, Y60_SCENE_DECL);

    DEFINE_PROPERTY_TAG(ReqLightingTag, MaterialRequirementFacade, y60::VectorOfRankedFeature, FEATURE_NODE_NAME,
            LIGHTING_FEATURE, REQUIRES_LIST_NAME, y60::VectorOfRankedFeature(1, RankedFeature(100,"unlit")));

    class Y60_SCENE_DECL MaterialPropertiesFacade :
        public PropertyListFacade,
        public MaterialAmbientTag::Plug,
        public MaterialDiffuseTag::Plug,
        public MaterialSpecularTag::Plug,
        public MaterialEmissiveTag::Plug,
        public SurfaceColorTag::Plug,
        public ShininessTag::Plug,
        public BlendFunctionTag::Plug,
        public BlendEquationTag::Plug,
        public BlendColorTag::Plug,
        public GlowTag::Plug,
        public TargetBuffersTag::Plug,
        public LineWidthTag::Plug,
        public PointSizeTag::Plug,
        public LineSmoothTag::Plug,
        public PointSmoothTag::Plug,
        public dom::FacadeAttributePlug<MaterialPropGroup1HashTag>
    {
        public:
            MaterialPropertiesFacade(dom::Node & theNode) :
                PropertyListFacade(theNode),
                MaterialAmbientTag::Plug(this),
                MaterialDiffuseTag::Plug(this),
                MaterialSpecularTag::Plug(this),
                MaterialEmissiveTag::Plug(this),
                SurfaceColorTag::Plug(this),
                ShininessTag::Plug(this),
                BlendFunctionTag::Plug(this),
                BlendEquationTag::Plug(this),
                BlendColorTag::Plug(this),
                GlowTag::Plug(this),
                TargetBuffersTag::Plug(this),
                LineWidthTag::Plug(this),
                PointSizeTag::Plug(this),
                LineSmoothTag::Plug(this),
                PointSmoothTag::Plug(this),
                dom::FacadeAttributePlug<MaterialPropGroup1HashTag>(this)
            {}
            IMPLEMENT_FACADE(MaterialPropertiesFacade);
            void registerDependenciesRegistrators();
            void updateGroup1Hash();

        protected:
            virtual void registerDependenciesForMaterialPropGroup1HashTag();

    };
    typedef asl::Ptr<MaterialPropertiesFacade, dom::ThreadingModel> MaterialPropertiesFacadePtr;

    DEFINE_CHILDNODE_TAG(MaterialPropertiesTag, MaterialBase, MaterialPropertiesFacade, PROPERTY_LIST_NAME);


    class MaterialRequirementFacade :
        public PropertyListFacade,
        public ReqLightingTag::Plug
    {
        public:
            MaterialRequirementFacade(dom::Node & theNode) :
                PropertyListFacade(theNode),
                ReqLightingTag::Plug(this)
            {}
            IMPLEMENT_FACADE(MaterialRequirementFacade);
    };
    typedef asl::Ptr<MaterialRequirementFacade, dom::ThreadingModel> MaterialRequirementFacadePtr;

    DEFINE_CHILDNODE_TAG(MaterialRequirementTag, MaterialBase, MaterialRequirementFacade, REQUIRES_LIST_NAME);


    DEFINE_ATTRIBUTE_TAG(TransparencyTag,   bool, TRANSPARENCY_ATTRIB, false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(EnabledTag,   bool, "enabled", true, Y60_SCENE_DECL);

    class Scene;

    class Y60_SCENE_DECL MaterialBase :
        public dom::Facade,
        public IdTag::Plug,
        public MaterialPropertiesTag::Plug,
        public MaterialRequirementTag::Plug,
        public NameTag::Plug,
        public TransparencyTag::Plug,
        public EnabledTag::Plug,
        public dom::FacadeAttributePlug<LastActiveFrameTag>
    {
        public:
            typedef std::vector<TexCoordMode> TexGenMode;
            typedef std::vector<TexGenMode>   TexGenModeList;

            typedef std::vector<asl::Vector4f> TexGenParams;
            typedef std::vector<TexGenParams>  TexGenParamsList;

            MaterialBase(dom::Node & theNode);
            IMPLEMENT_PARENT_FACADE(MaterialBase);

            void registerDependenciesRegistrators();
            virtual ~MaterialBase();

            MaterialBase(dom::Node & theNode, dom::Node & theDefaults);

            virtual unsigned getTextureUnitCount() const;
            virtual const TextureUnit & getTextureUnit(unsigned myIndex) const;

            virtual void load();

            const MaterialParameterVector & getVertexParameters() const;
            virtual bool reloadRequired() const;
            //virtual bool rebindRequired();
            int getGroup1Hash() const;

            //void setShader(IShaderPtr theShader);
            const IShaderPtr getShader() const;
            IShaderPtr getShader();

            void mergeProperties(const dom::NodePtr & thePropertyNode);
            const LightingModel getLightingModel() const { return _myLightingModel; }

            TextureUsage getTextureUsage(unsigned theTextureSlot) const;

            bool getDepthBufferWrite() const;
            void setDepthBufferWrite(bool theFlag);

            // texgen parameters
            void updateParams();

            bool hasTexGen() const {
                return _myTexGenFlag;
            }

            TexGenModeList getTexGenModes() const {
                ensureProperties();
                return _myTexGenModes;
            }

            TexGenParamsList getTexGenParams() const {
                ensureProperties();
                return _myTexGenParams;
            }
            Scene & getScene();
            const Scene & getScene() const;

        protected:
            virtual void registerDependenciesForMaterialupdate();
            void ensureShader();
            virtual void ensureProperties() const;
            void updateLock();
        private:
            void addTextures(const dom::NodePtr theTextureListNode, asl::Ptr<TextureManager> theTextureMananger);
            void addTexture(dom::NodePtr theTextureUnitNode, asl::Ptr<TextureManager> theTextureManager);
        private:
            IShaderPtr                 _myShader;

            std::vector<TextureUnitPtr> _myTextureUnits;
            LightingModel               _myLightingModel;

            mutable asl::Unsigned64   _myMaterialVersion;
            asl::Unsigned64   _myRequiresVersion;

            bool              _myTexGenFlag;
            TexGenModeList    _myTexGenModes;
            TexGenParamsList  _myTexGenParams;
            mutable bool      _ensuring;
    };

    typedef std::vector<MaterialBasePtr> MaterialBasePtrVector;
    typedef asl::Ptr<MaterialBase, dom::ThreadingModel> MaterialBaseFacadePtr;
    typedef asl::Ptr<MaterialBase, dom::ThreadingModel> MaterialBasePtr;
}

#endif
