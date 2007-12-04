//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_scene_MaterialBase_h_
#define _ac_scene_MaterialBase_h_

#include "TextureUnit.h"
#include "MaterialParameter.h"
#include "PropertyListFacade.h"
#include "IShader.h"

#include <y60/CommonTags.h>
#include <y60/PropertyNames.h>
#include <dom/Facade.h>
#include <dom/PropertyPlug.h>
#include <dom/AttributePlug.h>
#include <dom/ChildNodePlug.h>

#include <asl/Exception.h>
#include <y60/DataTypes.h>
#include <dom/Nodes.h>
#include <string>
#include <vector>

namespace y60 {
    class TextureManager;

    DEFINE_EXCEPTION(MaterialBaseException, asl::Exception);
    static y60::VectorOfBlendFunction getDefaultBlendFunction() {
        y60::VectorOfBlendFunction myResult;
        myResult.push_back(SRC_ALPHA);
        myResult.push_back(ONE_MINUS_SRC_ALPHA);
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
    DEFINE_MATERIAL_PROPERTY_TAG(BlendEquationTag, BlendEquation, BLENDEQUATION_PROPERTY, EQUATION_ADD);
    DEFINE_MATERIAL_PROPERTY_TAG(GlowTag, float, GLOW_PROPERTY, 0.0f);
    DEFINE_MATERIAL_PROPERTY_TAG(LineWidthTag, float, LINEWIDTH_PROPERTY, float(1.0));
    DEFINE_MATERIAL_PROPERTY_TAG(PointSizeTag, asl::Vector3f, POINTSIZE_PROPERTY, asl::Vector3f(1,1,1));
    DEFINE_MATERIAL_PROPERTY_TAG(LineSmoothTag, bool, LINESMOOTH_PROPERTY, bool(false));
    DEFINE_ATTRIBUT_TAG(MaterialPropGroup1HashTag, int, "material_prop_group1_hash_tag", 0);

    DEFINE_PROPERTY_TAG(ReqLightingTag, MaterialRequirementFacade, y60::VectorOfRankedFeature, FEATURE_NODE_NAME,
            LIGHTING_FEATURE, REQUIRES_LIST_NAME, y60::VectorOfRankedFeature(1, RankedFeature(100,"unlit")));

    class MaterialPropertiesFacade :
        public PropertyListFacade,
        public MaterialAmbientTag::Plug,
        public MaterialDiffuseTag::Plug,
        public MaterialSpecularTag::Plug,
        public MaterialEmissiveTag::Plug,
        public SurfaceColorTag::Plug,
        public ShininessTag::Plug,
        public BlendFunctionTag::Plug,
        public GlowTag::Plug,
        public TargetBuffersTag::Plug,
        public BlendEquationTag::Plug,
        public LineWidthTag::Plug,
        public PointSizeTag::Plug,
        public LineSmoothTag::Plug,
        public dom::FacadeAttributePlug<MaterialPropGroup1HashTag>
    {
        public:
            MaterialPropertiesFacade(dom::Node & theNode) :
                PropertyListFacade(theNode),
                MaterialAmbientTag::Plug(this),
                MaterialDiffuseTag::Plug(this),
                MaterialSpecularTag::Plug(this),
                SurfaceColorTag::Plug(this),
                ShininessTag::Plug(this),
                MaterialEmissiveTag::Plug(this),
                BlendFunctionTag::Plug(this),
                GlowTag::Plug(this),
                TargetBuffersTag::Plug(this),
                BlendEquationTag::Plug(this),
                LineWidthTag::Plug(this),
                PointSizeTag::Plug(this),
                LineSmoothTag::Plug(this),
                dom::FacadeAttributePlug<MaterialPropGroup1HashTag>(this)
            {}
            IMPLEMENT_FACADE(MaterialPropertiesFacade);
            void registerDependenciesRegistrators();
            void updateGroup1Hash();
            
        protected:
            virtual void registerDependenciesForMaterialPropGroup1HashTag();
            
    };
    typedef asl::Ptr<MaterialPropertiesFacade, dom::ThreadingModel> MaterialPropertiesFacadePtr;

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

    DEFINE_CHILDNODE_TAG(MaterialPropertiesTag, MaterialBase, MaterialPropertiesFacade, PROPERTY_LIST_NAME);
    DEFINE_CHILDNODE_TAG(MaterialRequirementTag, MaterialBase, MaterialRequirementFacade, REQUIRES_LIST_NAME);

    DEFINE_ATTRIBUT_TAG(TransparencyTag,   bool, TRANSPARENCY_ATTRIB, false);

    class MaterialBase :
        public dom::Facade,
        public IdTag::Plug,
        public MaterialPropertiesTag::Plug,
        public MaterialRequirementTag::Plug,
        public NameTag::Plug,
        public TransparencyTag::Plug
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

            virtual void load(TextureManagerPtr theTextureMananger);

            const MaterialParameterVector & getVertexParameters() const;
            virtual bool reloadRequired();
            virtual bool rebindRequired();
            int getGroup1Hash() const;

            void setShader(IShaderPtr theShader);
            const IShaderPtr getShader() const { return _myShader; };
            IShaderPtr getShader() { return _myShader; };

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
                return _myTexGenModes;
            }

            TexGenParamsList getTexGenParams() const {
                return _myTexGenParams;
            }

        protected:
            virtual void registerDependenciesForMaterialupdate();    
            IShaderPtr                 _myShader;

        private:
            void addTextures(const dom::NodePtr theTextureListNode, asl::Ptr<TextureManager> theTextureMananger);
            void addTexture(dom::NodePtr theTextureUnitNode, asl::Ptr<TextureManager> theTextureManager);

            std::vector<TextureUnitPtr> _myTextureUnits;
            LightingModel               _myLightingModel;

            asl::Unsigned64   _myMaterialVersion;
            asl::Unsigned64   _myRequiresVersion;
            asl::Unsigned64   _myIdTagVersion;

            bool              _myTexGenFlag;
            TexGenModeList    _myTexGenModes;
            TexGenParamsList  _myTexGenParams;
    };

    typedef std::vector<MaterialBasePtr> MaterialBasePtrVector;
    typedef asl::Ptr<MaterialBase, dom::ThreadingModel> MaterialBaseFacadePtr;
    typedef asl::Ptr<MaterialBase, dom::ThreadingModel> MaterialBasePtr;
}

#endif
