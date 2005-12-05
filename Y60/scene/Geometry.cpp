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
//   $RCSfile: Geometry.cpp,v $
//   $Author: pavel $
//   $Revision: 1.3 $
//   $Date: 2005/04/18 19:59:30 $
//
//  Description: Geometry class.
//
//=============================================================================

#include "Geometry.h"
#include "Scene.h"

#include <asl/Logger.h>
#include <asl/Matrix4.h>
#include <asl/GeometryUtils.h>
#include <asl/linearAlgebra.h>

using namespace std;
using namespace asl;

namespace y60 {

    Geometry::Geometry(dom::Node & theNode) : TransformHierarchyFacade(theNode),
        GeometryPlaneTag::Plug(theNode),
        dom::FacadeAttributePlug<GeometryGlobalPlaneTag>(this)
    {}

    void
    Geometry::registerDependenciesRegistrators() {
        TransformHierarchyFacade::registerDependenciesRegistrators();
        BoundingBoxTag::Plug::setReconnectFunction(&Geometry::registerDependenciesForBoundingBox);
        GeometryGlobalPlaneTag::Plug::setReconnectFunction( & Geometry::registerDependenciesForGlobalPlane);
    }

    void
    Geometry::registerDependenciesForBoundingBox() {        
        if (getNode()) {
            TransformHierarchyFacade::registerDependenciesForBoundingBox();

            /* TODO: add boundingbox modifying stuff here
            BoundingBoxTag::Plug::dependsOn<ShapeTag>(*this);

            ShapePtr myShape = getShape();
            if (myShape) {                
                BoundingBoxTag::Plug::dependsOn<BoundingBoxTag>(*myShape);
            }
            */

            GeometryPtr mySelf = dynamic_cast_Ptr<Geometry>(getSelf());
            BoundingBoxTag::Plug::getValuePtr()->setCalculatorFunction(mySelf, &Geometry::recalculateBoundingBox);
        }
    }

    void
    Geometry::registerDependenciesForGlobalPlane() {        
        if (getNode()) {
            GeometryGlobalPlaneTag::Plug::dependsOn<GeometryPlaneTag>(*this);
            GeometryGlobalPlaneTag::Plug::dependsOn<GlobalMatrixTag>(*this);
            GeometryPtr mySelf = dynamic_cast_Ptr<Geometry>(getSelf());
            GeometryGlobalPlaneTag::Plug::getValuePtr()->setCalculatorFunction( mySelf, & Geometry::recalculateGlobalPlane);
        }
    }

    Geometry::~Geometry() {
        AC_TRACE << "Geometry DTOR " << this << endl;
    }
    
    void
    Geometry::recalculateBoundingBox() {  
        if (get<VisibleTag>()) {
            // TODO: extend box by geometric primitives that have a valid box 
            //       ... like sphere, box, point ... plane doesn't have one ...
            Box3f myBoundingBox;
            myBoundingBox.makeEmpty();
            set<BoundingBoxTag>(myBoundingBox);
        }
    }

    void
    Geometry::recalculateGlobalPlane() {
        set<GeometryGlobalPlaneTag>(get<GeometryPlaneTag>() * get<GlobalMatrixTag>());
    }
    
    GeometryPtr 
    Geometry::create(dom::NodePtr theParent, const asl::Planef & thePlane) {
        dom::NodePtr myNode = dom::NodePtr(new dom::Element(GEOMETRY_NODE_NAME));
        myNode->appendAttribute(GEOMETRY_PLANE_ATTRIB, thePlane);
        myNode = theParent->appendChild(myNode);
        return myNode->getFacade<Geometry>();
    }
}




