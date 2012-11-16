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
//   $RCSfile: Geometry.h,v $
//   $Author: david $
//   $Revision: 1.12 $
//   $Date: 2005/04/04 14:40:15 $
//
//=============================================================================

#ifndef _ac_scene_Geometry_h_
#define _ac_scene_Geometry_h_

#include "y60_scene_settings.h"

#include "TransformHierarchyFacade.h"
#include "Shape.h"
#include <asl/dom/Nodes.h>

namespace y60 {
    class Scene;

    DEFINE_ATTRIBUTE_TAG(ColorTag, asl::Vector4f, GEOMETRY_COLOR_ATTRIB, asl::Vector4f(0.0, 0.0, 0.0, 1.0), Y60_SCENE_DECL);
    /* [DS] The following tags conflict with material properties, which are of a diffrent type. That's why
     * these are put in a namespace. AG stands for analytic geometry.
     */
    namespace AG {
        DEFINE_ATTRIBUTE_TAG(LineWidthTag, float, GEOMETRY_LINE_WIDTH_ATTRIB, 1.0, Y60_SCENE_DECL);
        DEFINE_ATTRIBUTE_TAG(PointSizeTag, float, GEOMETRY_POINT_SIZE_ATTRIB, 3.0, Y60_SCENE_DECL);
    }

    /* [DS] I think this needs some explanation: This template class uses two of the stranger template
     * idioms:
     *      1. It dervies itself from two of it's template arguments. (The *_TAG parameters)
     *      2. It knows it's derived class (curious template idiom)
     * This way more stuff can be handled in the base class (Geometry)
     */
    template <class PRIMITIVE, class PRIMITIVE_TAG, class GLOBAL_PRIMITIVE_TAG, class DERIVED_FACADE>
    class Geometry :
        public TransformHierarchyFacade,
        public PRIMITIVE_TAG::Plug,
        public dom::FacadeAttributePlug<GLOBAL_PRIMITIVE_TAG>,
        public ColorTag::Plug,
        public AG::LineWidthTag::Plug,
        public AG::PointSizeTag::Plug
    {
        public:
            Geometry( dom::Node & theNode);
            virtual ~Geometry();

            static asl::Ptr<DERIVED_FACADE, dom::ThreadingModel>
            create(dom::NodePtr theParent, const PRIMITIVE & thePrimitive);

            void registerDependenciesRegistrators();

        protected:
            virtual void registerDependenciesForGlobalPrimitive();
            virtual void registerDependenciesForBoundingBox();
    };



#define IMPLEMENT_GEOMETRY_FACADE( CLASS, NODE_NAME ) \
    IMPLEMENT_FACADE( CLASS ); \
    static const char * getNodeName() { \
        return #NODE_NAME; \
    }


    //=== Plane ============================================================
    DEFINE_ATTRIBUTE_TAG(PlaneTag,       asl::Planef,   GEOMETRY_VALUE_ATTRIB,    asl::Planef(asl::Vector3f(0,0,1), 0), Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalPlaneTag, asl::Planef,   GEOMETRY_GLOBAL_VALUE_ATTRIB, asl::Planef(asl::Vector3f(0,0,1), 0), Y60_SCENE_DECL);
    class Y60_SCENE_DECL Plane :
        public Geometry<asl::Planef, PlaneTag, GlobalPlaneTag, Plane>
    {
            typedef Geometry<asl::Planef, PlaneTag, GlobalPlaneTag, Plane> GeometryBase;
            friend class Geometry<asl::Planef, PlaneTag, GlobalPlaneTag, Plane>;
        public:
            Plane(dom::Node & theNode);
            virtual ~Plane();

            IMPLEMENT_GEOMETRY_FACADE(Plane, PLANE_NODE_NAME);

        protected:
            virtual void recalculateBoundingBox();
            // [DS] would be nice to implement this in the base class, because it's the same for all primitives
            virtual void recalculateGlobalPrimitive();
    };
    typedef asl::Ptr<Plane, dom::ThreadingModel> PlanePtr;


    //=== Point ============================================================
    DEFINE_ATTRIBUTE_TAG(PointTag,       asl::Point3f,  GEOMETRY_VALUE_ATTRIB,        asl::Point3f(0,0,0), Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalPointTag, asl::Point3f,  GEOMETRY_GLOBAL_VALUE_ATTRIB, asl::Point3f(0,0,0), Y60_SCENE_DECL);
    class Y60_SCENE_DECL Point :
        public Geometry<asl::Point3f, PointTag, GlobalPointTag, Point>
    {
            typedef Geometry<asl::Point3f, PointTag, GlobalPointTag, Point> GeometryBase;
            friend class Geometry<asl::Point3f, PointTag, GlobalPointTag, Point>;
        public:
            Point(dom::Node & theNode);
            virtual ~Point();

            IMPLEMENT_GEOMETRY_FACADE(Point, POINT_NODE_NAME);

        protected:
            virtual void recalculateBoundingBox();
            virtual void recalculateGlobalPrimitive();

    };
    typedef asl::Ptr<Point, dom::ThreadingModel> PointPtr;

    //=== Vector ============================================================
    DEFINE_ATTRIBUTE_TAG(VectorTag,       asl::Vector3f,  GEOMETRY_VALUE_ATTRIB,        asl::Vector3f(1,0,0), Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalVectorTag, asl::Vector3f,  GEOMETRY_GLOBAL_VALUE_ATTRIB, asl::Vector3f(1,0,0), Y60_SCENE_DECL);
    class Y60_SCENE_DECL Vector :
        public Geometry<asl::Vector3f, VectorTag, GlobalVectorTag, Vector>
    {
            typedef Geometry<asl::Vector3f, VectorTag, GlobalVectorTag, Vector> GeometryBase;
            friend class Geometry<asl::Vector3f, VectorTag, GlobalVectorTag, Vector>;
        public:
            Vector(dom::Node & theNode);
            virtual ~Vector();

            IMPLEMENT_GEOMETRY_FACADE(Vector, POINT_NODE_NAME);

        protected:
            virtual void recalculateBoundingBox();
            virtual void recalculateGlobalPrimitive();

    };
    typedef asl::Ptr<Vector, dom::ThreadingModel> VectorPtr;

    //=== LineSegment ============================================================
    DEFINE_ATTRIBUTE_TAG(LineSegmentTag,       asl::LineSegment<float>,  GEOMETRY_VALUE_ATTRIB,
            asl::LineSegment<float>(asl::Vector3f(0,0,0), asl::Vector3f(1,0,0)), Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalLineSegmentTag, asl::LineSegment<float>,  GEOMETRY_GLOBAL_VALUE_ATTRIB,
            asl::LineSegment<float>(asl::Vector3f(0,0,0), asl::Vector3f(1,0,0)), Y60_SCENE_DECL);
    class LineSegment :
        public Geometry<asl::LineSegment<float>, LineSegmentTag, GlobalLineSegmentTag, LineSegment>
    {
            typedef Geometry<asl::LineSegment<float>, LineSegmentTag, GlobalLineSegmentTag, LineSegment> GeometryBase;
            friend class Geometry<asl::LineSegment<float>, LineSegmentTag, GlobalLineSegmentTag, LineSegment>;
        public:
            LineSegment(dom::Node & theNode);
            virtual ~LineSegment();

            IMPLEMENT_GEOMETRY_FACADE(LineSegment, LINE_SEGMENT_NODE_NAME);

        protected:
            virtual void recalculateBoundingBox();
            virtual void recalculateGlobalPrimitive();

    };
    typedef asl::Ptr<LineSegment, dom::ThreadingModel> LineSegmentPtr;

}

#endif
