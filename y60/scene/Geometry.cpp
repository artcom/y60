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
//   $RCSfile: Geometry.cpp,v $
//   $Author: pavel $
//   $Revision: 1.3 $
//   $Date: 2005/04/18 19:59:30 $
//
//  Description: Geometry class.
//
//=============================================================================

// own header
#include "Geometry.h"

#include "Scene.h"

#include <asl/base/Logger.h>
#include <asl/math/Matrix4.h>
#include <asl/math/GeometryUtils.h>
#include <asl/math/linearAlgebra.h>

using namespace std;
using namespace asl;

namespace y60 {


    //=== Plane ============================================================
    Plane::Plane(dom::Node & theNode) : GeometryBase( theNode ) {}
    Plane::~Plane() { AC_TRACE << "Plane DTOR " << this << endl; }

    void
    Plane::recalculateBoundingBox() {
        if (get<VisibleTag>()) {

            Box3f myBoundingBox;
            myBoundingBox.makeEmpty(); // makeFull would be more correct but would break alot of other things,
                                       // e.g. world size would always be infinity
            TransformHierarchyFacade::recalculateBoundingBox();
            myBoundingBox.extendBy(get<BoundingBoxTag>());

            set<BoundingBoxTag>(myBoundingBox);

            // planes are not cullable
            set<CullableTag>(false);
        }
    }

    void
    Plane::recalculateGlobalPrimitive() {
        set<GlobalPlaneTag>(this->get<PlaneTag>() * this->get<GlobalMatrixTag>());
    }

    //=== Point ============================================================
    Point::Point(dom::Node & theNode) : GeometryBase( theNode ) {};
    Point::~Point() {
        AC_TRACE << "Point DTOR " << this << endl;
    }

    void
    Point::recalculateBoundingBox() {
        if (get<VisibleTag>()) {
            // TODO: extend box by geometric primitives that have a valid box
            //       ... like sphere, box, point ... plane doesn't have one ...
            Box3f myBoundingBox;
            myBoundingBox.makeEmpty();

            TransformHierarchyFacade::recalculateBoundingBox();
            myBoundingBox.extendBy(get<BoundingBoxTag>());

            myBoundingBox.extendBy( get<GlobalPointTag>() );
            set<BoundingBoxTag>(myBoundingBox);
        }
    }

    void
    Point::recalculateGlobalPrimitive() {
        set<GlobalPointTag>(get<PointTag>() * get<GlobalMatrixTag>());
    }

    //=== Vector ============================================================
    Vector::Vector(dom::Node & theNode) : GeometryBase( theNode ) {};
    Vector::~Vector() {
        AC_TRACE << "Vector DTOR " << this << endl;
    }

    void
    Vector::recalculateBoundingBox() {
        if (get<VisibleTag>()) {
            // TODO: extend box by geometric primitives that have a valid box
            //       ... like sphere, box, point ... plane doesn't have one ...
            Box3f myBoundingBox;
            myBoundingBox.makeEmpty();

            TransformHierarchyFacade::recalculateBoundingBox();
            myBoundingBox.extendBy(get<BoundingBoxTag>());

            Vector3f myTranslation = get<GlobalMatrixTag>().getTranslation();
            myBoundingBox.extendBy( myTranslation );
            myBoundingBox.extendBy( myTranslation + get<GlobalVectorTag>() );
            set<BoundingBoxTag>(myBoundingBox);
        }
    }

    void
    Vector::recalculateGlobalPrimitive() {
        set<GlobalVectorTag>( asVector( asPoint( get<VectorTag>()) *  get<GlobalMatrixTag>() ) );
    }

    //=== LineSegment ============================================================
    LineSegment::LineSegment(dom::Node & theNode) : GeometryBase( theNode ) {};
    LineSegment::~LineSegment() {
        AC_TRACE << "LineSegment DTOR " << this << endl;
    }

    void
    LineSegment::recalculateBoundingBox() {
        if (get<VisibleTag>()) {
            // TODO: extend box by geometric primitives that have a valid box
            //       ... like sphere, box, point ... plane doesn't have one ...
            Box3f myBoundingBox;
            myBoundingBox.makeEmpty();

            TransformHierarchyFacade::recalculateBoundingBox();
            myBoundingBox.extendBy(get<BoundingBoxTag>());

            const asl::LineSegment<float> & myLineSegment = get<GlobalLineSegmentTag>();
            myBoundingBox.extendBy( myLineSegment.origin );
            myBoundingBox.extendBy( myLineSegment.end );
            set<BoundingBoxTag>(myBoundingBox);
        }
    }

    void
    LineSegment::recalculateGlobalPrimitive() {
        set<GlobalLineSegmentTag>( get<LineSegmentTag>() *  get<GlobalMatrixTag>() );
    }

    //=== Geometry Baseclass ===============================================
    template <class PRIMITIVE, class PRIMITIVE_TAG, class GLOBAL_PRIMITIVE_TAG, class DERIVED_FACADE>
    Geometry<PRIMITIVE, PRIMITIVE_TAG, GLOBAL_PRIMITIVE_TAG, DERIVED_FACADE>::
    Geometry( dom::Node & theNode) :
            TransformHierarchyFacade( theNode ),
            PRIMITIVE_TAG::Plug( theNode ),
            dom::FacadeAttributePlug<GLOBAL_PRIMITIVE_TAG>( this ),
            ColorTag::Plug( theNode ),
            AG::LineWidthTag::Plug( theNode ),
            AG::PointSizeTag::Plug( theNode )
    {}

    template <class PRIMITIVE, class PRIMITIVE_TAG, class GLOBAL_PRIMITIVE_TAG, class DERIVED_FACADE>
    Geometry<PRIMITIVE, PRIMITIVE_TAG, GLOBAL_PRIMITIVE_TAG, DERIVED_FACADE>::
    ~Geometry() {}

    template <class PRIMITIVE, class PRIMITIVE_TAG, class GLOBAL_PRIMITIVE_TAG, class DERIVED_FACADE>
    asl::Ptr<DERIVED_FACADE, dom::ThreadingModel>
    Geometry<PRIMITIVE, PRIMITIVE_TAG, GLOBAL_PRIMITIVE_TAG, DERIVED_FACADE>::
    create(dom::NodePtr theParent, const PRIMITIVE & thePrimitive) {
        dom::NodePtr myNode = dom::NodePtr(new dom::Element( DERIVED_FACADE::getNodeName() ));
        myNode->appendAttribute(GEOMETRY_VALUE_ATTRIB, thePrimitive);
        myNode = theParent->appendChild(myNode);
        return myNode->getFacade<DERIVED_FACADE>();
    }

    template <class PRIMITIVE, class PRIMITIVE_TAG, class GLOBAL_PRIMITIVE_TAG, class DERIVED_FACADE>
    void
    Geometry<PRIMITIVE, PRIMITIVE_TAG, GLOBAL_PRIMITIVE_TAG, DERIVED_FACADE>::
    registerDependenciesRegistrators() {
        TransformHierarchyFacade::registerDependenciesRegistrators();
        BoundingBoxTag::Plug::setReconnectFunction( & DERIVED_FACADE::registerDependenciesForBoundingBox);
        GLOBAL_PRIMITIVE_TAG::Plug::setReconnectFunction( & DERIVED_FACADE::registerDependenciesForGlobalPrimitive);
    }

    template <class PRIMITIVE, class PRIMITIVE_TAG, class GLOBAL_PRIMITIVE_TAG, class DERIVED_FACADE>
    void
    Geometry<PRIMITIVE, PRIMITIVE_TAG, GLOBAL_PRIMITIVE_TAG, DERIVED_FACADE>::
    registerDependenciesForGlobalPrimitive() {
        if (getNode()) {
            GLOBAL_PRIMITIVE_TAG::Plug::template dependsOn<PRIMITIVE_TAG>(*this);
            GLOBAL_PRIMITIVE_TAG::Plug::template dependsOn<GlobalMatrixTag>(*this);

            asl::Ptr<DERIVED_FACADE, dom::ThreadingModel> mySelf = dynamic_cast_Ptr<DERIVED_FACADE>(getSelf());
            GLOBAL_PRIMITIVE_TAG::Plug::getValuePtr()->dom::Field::template setCalculatorFunction(
                    mySelf, & DERIVED_FACADE::recalculateGlobalPrimitive);
        }
    }

    template <class PRIMITIVE, class PRIMITIVE_TAG, class GLOBAL_PRIMITIVE_TAG, class DERIVED_FACADE>
    void
    Geometry<PRIMITIVE, PRIMITIVE_TAG, GLOBAL_PRIMITIVE_TAG, DERIVED_FACADE>::
    registerDependenciesForBoundingBox() {
        if (getNode()) {
            TransformHierarchyFacade::registerDependenciesForBoundingBox();

            BoundingBoxTag::Plug::dependsOn<GLOBAL_PRIMITIVE_TAG>(*this);

            asl::Ptr<DERIVED_FACADE, dom::ThreadingModel> mySelf = dynamic_cast_Ptr<DERIVED_FACADE>(getSelf());
            BoundingBoxTag::Plug::getValuePtr()->setCalculatorFunction(mySelf, & DERIVED_FACADE::recalculateBoundingBox);
        }
    }
}




