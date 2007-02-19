//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
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

#ifndef _ac_scene_SkinAndBonesShader_h_
#define _ac_scene_SkinAndBonesShader_h_

#include "CGShader.h"

#include <dom/Nodes.h>
#include <y60/Facades.h>
#include <asl/Matrix4.h>

#include <vector>

namespace y60 {
    class SkinAndBonesShader : public CGShader {
        public:
            SkinAndBonesShader(const dom::NodePtr theShaderLibrary,
                               const std::string & theVertexProfileName,
                               const std::string & theFragmentProfileName) 
            : CGShader(theShaderLibrary, theVertexProfileName, theFragmentProfileName), _myBoundingBoxNode(0) {}

            virtual ~SkinAndBonesShader() {}

            void activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial);
            void setup(MaterialBase & theMaterial);

        private:
            dom::NodePtr                 _myBoneMatrixPropertyNode;
            std::vector<JointFacadePtr>  _myJoints;
            std::vector<asl::Matrix4f>   _myJointSpaceTransforms;
            dom::NodePtr                 _myBoundingBoxNode;

    };

    typedef asl::Ptr<SkinAndBonesShader, dom::ThreadingModel> SkinAndBonesShaderPtr;
}

#endif

