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


#include <dom/Nodes.h>
#include <asl/Matrix4.h>

#include <vector>

namespace y60 {
    class TextureManager;

    class SkinAndBones : public MaterialBase {
        public:
            SkinAndBones(MaterialRequirementList theMaterialRequirement) : 
			  MaterialBase(theMaterialRequirement), _myBoundingBoxNode(0), _myBoneMatrixPropertyNode(0) {}

            virtual ~SkinAndBones() {}

            void setup(dom::NodePtr theSceneNode);

            void update(TextureManager & theTextureManager, const dom::NodePtr theImages);

        private:
            dom::NodePtr                 _myBoneMatrixPropertyNode;
            std::vector<const asl::Matrix4f *> _myJointMatrices;
            std::vector<asl::Matrix4f>   _myJointSpaceTransforms;    
            dom::NodePtr                 _myBoundingBoxNode;
     };
}   

#endif

