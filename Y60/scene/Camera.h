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

#include "TransformHierarchyFacade.h"

namespace y60 {

    DEFINE_ATTRIBUT_TAG(FrustumTag,       asl::Frustum, FRUSTUM_ATTRIB, asl::Frustum());
    //                  theTagName        theType  theAttributeName      theDefault
    // XXX [DS] these attributes are here for backward compatibility only ...
    // please use the frustum tag instead
    /*
    DEFINE_ATTRIBUT_TAG(HfovTag,          float,   HORIZONTAL_FOV_ATTRIB, 54.0f);
    DEFINE_ATTRIBUT_TAG(OrthoWidthTag,    float,   ORTHO_WIDTH_ATTRIB,    0.0f);
    DEFINE_ATTRIBUT_TAG(NearPlaneTag,     float,   NEAR_PLANE_ATTRIB,     0.1f);
    DEFINE_ATTRIBUT_TAG(FarPlaneTag,      float,   FAR_PLANE_ATTRIB,      10000.0f);
*/

    class ProjectiveNode : 
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

    class Camera : public ProjectiveNode {
        public:
            Camera(dom::Node & theNode); 

            IMPLEMENT_FACADE(Camera);
    };

    typedef asl::Ptr<Camera, dom::ThreadingModel> CameraPtr;

    class Projector : public ProjectiveNode {
        public:
            Projector(dom::Node & theNode); 

            IMPLEMENT_FACADE(Projector);
    };

    typedef asl::Ptr<Projector, dom::ThreadingModel> ProjectorPtr;
} 

#endif 
