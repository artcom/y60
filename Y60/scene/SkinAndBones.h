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
//   $RCSfile: SkinAndBones.h,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2005/03/24 23:36:03 $
//
//   Description:
//
//=============================================================================

#ifndef _ac_scene_SkinAndBones_h_
#define _ac_scene_SkinAndBones_h_

#include "MaterialBase.h"
#include "Facades.h"
#include <y60/PropertyNames.h>

#include <dom/Nodes.h>
#include <asl/Matrix4.h>

#include <vector>

namespace y60 {
    class TextureManager;

    DEFINE_PROPERTY_TAG(BoneMatrixTag, SkinAndBonesPropertiesFacade, y60::VectorOfVector4f, y60::VectorTypeNameTrait<asl::Vector4f>::name(), BONE_MATRIX_PROPERTY, "properties", VectorOfVector4f());
	class SkinAndBonesPropertiesFacade :
		public dom::Facade,
		public BoneMatrixTag::Plug
	{
		public:
			SkinAndBonesPropertiesFacade(dom::Node & theNode) :
				Facade(theNode),
				BoneMatrixTag::Plug(this)
			{}
			IMPLEMENT_FACADE(SkinAndBonesPropertiesFacade);

	};
	typedef asl::Ptr<SkinAndBonesPropertiesFacade, dom::ThreadingModel> SkinAndBonesPropertiesFacadePtr;

	DEFINE_CHILDNODE_TAG(SkinAndBonesPropertiesTag, SkinAndBones, SkinAndBonesPropertiesFacade, REQUIRES_LIST_NAME);
    class SkinAndBones : public MaterialBase
    {
        public:
            SkinAndBones(dom::Node & theNode) :
			MaterialBase(theNode), _myBoundingBoxNode(0) {}
            
            IMPLEMENT_PARENT_FACADE(SkinAndBones);

            virtual ~SkinAndBones() {}

            void setup(dom::NodePtr theSceneNode);

            void update(TextureManager & theTextureManager, const dom::NodePtr theImages);

        private:
			dom::NodePtr                 _myBoneMatrixPropertyNode;
            std::vector<JointFacadePtr>  _myJoints;
            std::vector<asl::Matrix4f>   _myJointSpaceTransforms;
            dom::NodePtr                 _myBoundingBoxNode;

     };
}

#endif

