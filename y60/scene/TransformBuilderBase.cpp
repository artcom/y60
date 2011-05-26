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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $Id: TransformBuilderBase.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: TransformBuilderBase.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//=============================================================================

// own header
#include "TransformBuilderBase.h"

#include "TransformHierarchyFacade.h"
#include "Shape.h"
#include <y60/base/NodeNames.h>
#include <asl/math/Quaternion.h>
#include <asl/dom/Nodes.h>

namespace y60 {
    TransformBuilderBase::TransformBuilderBase(const std::string & theNodeName,
            const std::string & theName) : WorldBuilderBase(theNodeName)
    {
        if (!getNode()->hasFacade()) {
            getNode()->appendAttribute(NAME_ATTRIB, theName);
            getNode()->appendAttribute(POSITION_ATTRIB, asl::Vector3f(0,0,0));
            getNode()->appendAttribute(SCALE_ATTRIB, asl::Vector3f(1,1,1));
            getNode()->appendAttribute(ORIENTATION_ATTRIB, asl::Quaternionf(0,0,0,1));
            getNode()->appendAttribute(PIVOT_ATTRIB, asl::Vector3f(0,0,0));
            getNode()->appendAttribute(BILLBOARD_ATTRIB, "");
            getNode()->appendAttribute(BILLBOARD_LOOKAT_ATTRIB, "");
            getNode()->appendAttribute<int>(RENDERORDER_ATTRIB, 0);
            
        } else {
            TransformHierarchyFacadePtr myTransform = getNode()->getFacade<TransformHierarchyFacade>();
            myTransform->set<NameTag>(theName);
        }
    }

    TransformBuilderBase::~TransformBuilderBase() {
    }

    void
    TransformBuilderBase::setBillboard(const BillboardType theBillboardType) {
        std::string myType = "";
        if (theBillboardType == BB_POINT) {
            myType = POINT_BILLBOARD;
        } else if (theBillboardType == BB_AXIS) {
            myType =  AXIS_BILLBOARD;
        } else {
            return;
        }
//        getNode()->appendAttribute(BILLBOARD_ATTRIB, myType);
        getNode()->getAttribute(BILLBOARD_ATTRIB)->nodeValue(myType);

    }

    void
    TransformBuilderBase::setPosition(const asl::Vector3f & thePosition) {
        getNode()->getAttribute(POSITION_ATTRIB)->nodeValueAssign(thePosition);
    }

    void
    TransformBuilderBase::setOrientation(const asl::Quaternionf & theOrientation) {
        getNode()->getAttribute(ORIENTATION_ATTRIB)->nodeValueAssign(theOrientation);
    }

    void
    TransformBuilderBase::setOrientation(const asl::Vector3f & theOrientation) {
        setOrientation(asl::Quaternionf::createFromEuler(theOrientation));
    }

    void
    TransformBuilderBase::setScale(const asl::Vector3f & theScale) {
        getNode()->getAttribute(SCALE_ATTRIB)->nodeValueAssign(theScale);
    }

    void
    TransformBuilderBase::setPivot(const asl::Vector3f & thePivot) {
        getNode()->getAttribute(PIVOT_ATTRIB)->nodeValueAssign(thePivot);
    }
}

