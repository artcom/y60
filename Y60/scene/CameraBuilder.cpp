//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//   $Id: CameraBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: CameraBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//=============================================================================

#include "CameraBuilder.h"
#include "Camera.h"
#include <y60/NodeNames.h>
#include <dom/Nodes.h>

using namespace asl;

namespace y60 {
    CameraBuilder::CameraBuilder(const std::string & theName) :
        TransformBuilderBase(CAMERA_NODE_NAME, theName)
    {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            /*
            myNode->appendAttribute(HORIZONTAL_FOV_ATTRIB, "54.0");
            myNode->appendAttribute(NEAR_PLANE_ATTRIB, "0.1");
            myNode->appendAttribute(FAR_PLANE_ATTRIB, "10000");
            */
            Frustum myFrustum(54.0, 54.0, 0.1, 10000);
            myNode->appendAttribute(FRUSTUM_ATTRIB, asl::as_string( myFrustum ) );
        }
    }

    CameraBuilder::~CameraBuilder() {
    }

    void
    CameraBuilder::setHFov(const float theViewingAngle) {
        Frustum myFrustum;

        if (getNode()->hasFacade()) {
            myFrustum = getNode()->getFacade<Camera>()->get<FrustumTag>();
        } else {
            std::string myStr = getNode()->getAttribute(FRUSTUM_ATTRIB)->nodeValue();
            myFrustum = asl::as<Frustum>( myStr );
        }

        myFrustum.setHFov( theViewingAngle );

        if (getNode()->hasFacade()) {
            getNode()->getFacade<Camera>()->set<FrustumTag>( myFrustum );
        } else {
            getNode()->getAttribute(FRUSTUM_ATTRIB)->nodeValue(asl::as_string( myFrustum ));
        }
    }
}
