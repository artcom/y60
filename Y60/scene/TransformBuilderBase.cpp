//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: TransformBuilderBase.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: TransformBuilderBase.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//=============================================================================

#include "TransformBuilderBase.h"
#include "TransformHierarchyFacade.h"
#include "Shape.h"
#include <y60/NodeNames.h>
#include <asl/Quaternion.h>
#include <dom/Nodes.h>

namespace y60 {
    TransformBuilderBase::TransformBuilderBase(const std::string & theNodeName,
            const std::string & theName) : WorldBuilderBase(theNodeName)
    {
        if (!getNode()->hasFacade()) {
            getNode()->appendAttribute(NAME_ATTRIB, theName);
            getNode()->appendAttribute(POSITION_ATTRIB, asl::Vector3f(0,0,0));
            getNode()->appendAttribute(SCALE_ATTRIB, asl::Vector3f(1,1,1));
            getNode()->appendAttribute(ORIENTATION_ATTRIB, asl::Quaternionf(0,0,0,1));
            getNode()->appendAttribute(SHEAR_ATTRIB, asl::Vector3f(0,0,0));
            getNode()->appendAttribute(PIVOT_ATTRIB, asl::Vector3f(0,0,0));
            getNode()->appendAttribute(PIVOT_TRANSLATION_ATTRIB, asl::Vector3f(0,0,0));
            getNode()->appendAttribute(BILLBOARD_ATTRIB, "");
            getNode()->appendAttribute(SENSOR_ATTRIB, "");
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
    TransformBuilderBase::setSensor(const SensorType theSensorType, float theSensorRadius) {
        std::string myType = "";
        if (theSensorType == SENSOR_APPROXIMATION) {
            myType = "approximation";
//            getNode()->appendAttribute(SENSOR_RADIUS_ATTRIB, theSensorRadius);
            getNode()->getAttribute(SENSOR_RADIUS_ATTRIB)->nodeValue(asl::as_string(theSensorRadius));
        } else {
            return;
        }
//        getNode()->appendAttribute(SENSOR_ATTRIB, myType);
        getNode()->getAttribute(SENSOR_ATTRIB)->nodeValue(myType);

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
    TransformBuilderBase::setShear(const asl::Vector3f & theShear) {
        getNode()->getAttribute(SHEAR_ATTRIB)->nodeValueAssign(theShear);
    }

    void
    TransformBuilderBase::setPivot(const asl::Vector3f & thePivot) {
        getNode()->getAttribute(PIVOT_ATTRIB)->nodeValueAssign(thePivot);
    }

	void
	TransformBuilderBase::setPivotTranslation(const asl::Vector3f & thePivotTranslation) {
		getNode()->getAttribute(PIVOT_TRANSLATION_ATTRIB)->nodeValueAssign(thePivotTranslation);
	}
}

