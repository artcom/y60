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

#include "Camera.h"

namespace y60 {
    
    ProjectiveNode::ProjectiveNode(dom::Node & theNode) 
        : TransformHierarchyFacade(theNode),
        FrustumTag::Plug(theNode)

/*
        dom::DynamicAttributePlug<HfovTag, ProjectiveNode>(this, & ProjectiveNode::getHfov),
        dom::DynamicAttributePlug<OrthoWidthTag, ProjectiveNode>(this, & ProjectiveNode::getOrthoWidth),
        dom::DynamicAttributePlug<NearPlaneTag, ProjectiveNode>(this, & ProjectiveNode::getNearPlane),
        dom::DynamicAttributePlug<FarPlaneTag, ProjectiveNode>(this, & ProjectiveNode::getFarPlane)
    */
    {}
    ProjectiveNode::~ProjectiveNode() {}

    bool 
    ProjectiveNode::getHFov(float & theValue) const {
        float myVal = get<FrustumTag>().getHFov();
        if (myVal != theValue) {
            theValue = myVal;
            return true;
        }
        return false;
    }
    bool
    ProjectiveNode::getWidth(float & theValue) const {
        float myVal = get<FrustumTag>().getWidth();
        if (myVal != theValue) {
            theValue = myVal;
            return true;
        }
        return false;
    }
    bool
    ProjectiveNode::getNearPlane(float & theValue) const {
        float myVal = get<FrustumTag>().getNear();
        if (myVal != theValue) {
            theValue = myVal;
            return true;
        }
        return false;
    }
    bool
    ProjectiveNode::getFarPlane(float & theValue) const {
        float myVal = get<FrustumTag>().getFar();
        if (myVal != theValue) {
            theValue = myVal;
            return true;
        }
        return false;
    }

    void 
    ProjectiveNode::updateFrustum(asl::ResizePolicy thePolicy, float theNewAspect ) {
        asl::Frustum myFrustum = get<FrustumTag>();
        myFrustum.changeAspectRatio( thePolicy, theNewAspect );
        myFrustum.updatePlanes(get<GlobalMatrixTag>(), get<InverseGlobalMatrixTag>());
        // avoid bumping version numbers when nothing has changed
        if (memcmp(&myFrustum, &get<FrustumTag>(), sizeof(asl::Frustum)) !=0) { 
            set<FrustumTag>( myFrustum ); 
        }
    }

    Camera::Camera(dom::Node & theNode) : ProjectiveNode( theNode ) {}

    Projector::Projector(dom::Node & theNode) : ProjectiveNode( theNode ) {}
} 
