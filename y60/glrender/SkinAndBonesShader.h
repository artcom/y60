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

#ifndef _ac_scene_SkinAndBonesShader_h_
#define _ac_scene_SkinAndBonesShader_h_

#include "y60_glrender_settings.h"

#include "CgShader.h"

#include <asl/dom/Nodes.h>
#include <y60/scene/Facades.h>
#include <asl/math/Matrix4.h>

#include <vector>

namespace y60 {
    class SkinAndBonesShader : public CgShader {
        public:
            SkinAndBonesShader(const dom::NodePtr theShaderLibrary,
                               const std::string & theVertexProfileName,
                               const std::string & theFragmentProfileName)
            : CgShader(theShaderLibrary, theVertexProfileName, theFragmentProfileName), _myBoundingBoxNode() {}

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

