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
//
//   $RCSfile: Camera.h,v $
//   $Author: danielk $
//   $Revision: 1.16 $
//   $Date: 2005/03/17 18:22:42 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _scene_Camera_h_
#define _scene_Camera_h_

#include "y60_scene_settings.h"

#include "TransformHierarchyFacade.h"
#include <y60/base/FrustumValue.h>

namespace y60 {

    DEFINE_ATTRIBUTE_TAG(FrustumTag,       asl::Frustum, FRUSTUM_ATTRIB, asl::Frustum(), Y60_SCENE_DECL);
    //                  theTagName        theType  theAttributeName      theDefault
    // XXX [DS] these attributes are here for backward compatibility only ...
    // please use the frustum tag instead
    /*
    DEFINE_ATTRIBUTE_TAG(HfovTag,          float,   HORIZONTAL_FOV_ATTRIB, 54.0f, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(OrthoWidthTag,    float,   ORTHO_WIDTH_ATTRIB,    0.0f, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(NearPlaneTag,     float,   NEAR_PLANE_ATTRIB,     0.1f, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(FarPlaneTag,      float,   FAR_PLANE_ATTRIB,      10000.0f, Y60_SCENE_DECL);
*/

    class Y60_SCENE_DECL ProjectiveNode :
        public TransformHierarchyFacade,
        public FrustumTag::Plug
        // [DS] backward compatibility
        /*
        public dom::DynamicAttributePlug<HfovTag, ProjectiveNode>,
        public dom::DynamicAttributePlug<OrthoWidthTag, ProjectiveNode>,
        public dom::DynamicAttributePlug<NearPlaneTag, ProjectiveNode>,
        public dom::DynamicAttributePlug<FarPlaneTag, ProjectiveNode>
        */
    {
        public:
            ProjectiveNode(dom::Node & theNode);
            ~ProjectiveNode();

            IMPLEMENT_FACADE(ProjectiveNode);

            bool getHFov(float & theValue) const;
            bool getWidth(float & theValue) const;
            bool getNearPlane(float & theValue) const;
            bool getFarPlane(float & theValue) const;

            //asl::Frustum & getFrustum();
            void updateFrustum(asl::ResizePolicy thePolicy, float theNewAspect );
    };

    typedef asl::Ptr<ProjectiveNode, dom::ThreadingModel> ProjectiveNodePtr;

    class Y60_SCENE_DECL Camera : public ProjectiveNode {
        public:
            Camera(dom::Node & theNode);

            IMPLEMENT_FACADE(Camera);
    };

    typedef asl::Ptr<Camera, dom::ThreadingModel> CameraPtr;

    class Y60_SCENE_DECL Projector : public ProjectiveNode {
        public:
            Projector(dom::Node & theNode);

            IMPLEMENT_FACADE(Projector);
    };

    typedef asl::Ptr<Projector, dom::ThreadingModel> ProjectorPtr;
}

#endif
