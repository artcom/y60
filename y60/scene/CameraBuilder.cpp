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
//   $Id: CameraBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: CameraBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//=============================================================================

// own header
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
