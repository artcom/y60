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

// own header
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
