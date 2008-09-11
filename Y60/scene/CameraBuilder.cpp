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
#include <y60/base/NodeNames.h>
#include <asl/dom/Nodes.h>

using namespace asl;

namespace y60 {
    CameraBuilder::CameraBuilder(const std::string & theName) :
        TransformBuilderBase(CAMERA_NODE_NAME, theName)
    {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            Frustum myFrustum(54.0f, 54.0f, 0.1f, 10000.0f);
            myNode->appendAttribute(FRUSTUM_ATTRIB, asl::as_string( myFrustum ) );
        }
    }

    CameraBuilder::~CameraBuilder() {
    }

    void
    CameraBuilder::setHFov(const float theViewingAngle) {
        Frustum myFrustum;
        getFrustum( myFrustum );

        myFrustum.setHFov( theViewingAngle );

        setFrustum( myFrustum );
    }

    void
    CameraBuilder::setShift(float theHShift, float theVShift) {
        Frustum myFrustum;
        getFrustum( myFrustum );

        myFrustum.setHShift( theHShift );
        myFrustum.setVShift( theVShift );

        setFrustum( myFrustum );
    }

    void
    CameraBuilder::getFrustum( Frustum & theFrustum ) {
        if (getNode()->hasFacade()) {
            theFrustum = getNode()->getFacade<Camera>()->get<FrustumTag>();
        } else {
            std::string myStr = getNode()->getAttribute(FRUSTUM_ATTRIB)->nodeValue();
            theFrustum = asl::as<Frustum>( myStr );
        }
    }

    void
    CameraBuilder::setFrustum(const Frustum & theFrustum) {
        if (getNode()->hasFacade()) {
            getNode()->getFacade<Camera>()->set<FrustumTag>( theFrustum );
        } else {
            getNode()->getAttribute(FRUSTUM_ATTRIB)->nodeValue(asl::as_string( theFrustum ));
        }
    
    }
}
