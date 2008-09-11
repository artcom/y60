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
//   $RCSfile: Geometry.h,v $
//   $Author: david $
//   $Revision: 1.12 $
//   $Date: 2005/04/04 14:40:15 $
//
//=============================================================================

#ifndef _ac_scene_Geometry_h_
#define _ac_scene_Geometry_h_

#include "TransformHierarchyFacade.h"
#include "Shape.h"
#include <asl/dom/Nodes.h>

namespace y60 {
    class Scene;

    DEFINE_ATTRIBUT_TAG(ColorTag, asl::Vector4f, GEOMETRY_COLOR_ATTRIB, asl::Vector4f(0.0, 0.0, 0.0, 1.0));
    /* [DS] The following tags conflict with material properties, which are of a diffrent type. That's why
     * these are put in a namespace. AG stands for analytic geometry.
     */
    namespace AG {
        DEFINE_ATTRIBUT_TAG(LineWidthTag, float, GEOMETRY_LINE_WIDTH_ATTRIB, 1.0);
        DEFINE_ATTRIBUT_TAG(PointSizeTag, float, GEOMETRY_POINT_SIZE_ATTRIB, 3.0);
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
    DEFINE_ATTRIBUT_TAG(PlaneTag,       asl::Planef,   GEOMETRY_VALUE_ATTRIB,    asl::Planef(asl::Vector3f(0,0,1), 0));
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalPlaneTag, asl::Planef,   GEOMETRY_GLOBAL_VALUE_ATTRIB, asl::Planef(asl::Vector3f(0,0,1), 0));
    class Plane :
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
    DEFINE_ATTRIBUT_TAG(PointTag,       asl::Point3f,  GEOMETRY_VALUE_ATTRIB,        asl::Point3f(0,0,0));
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalPointTag, asl::Point3f,  GEOMETRY_GLOBAL_VALUE_ATTRIB, asl::Point3f(0,0,0));
    class Point :
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
    DEFINE_ATTRIBUT_TAG(VectorTag,       asl::Vector3f,  GEOMETRY_VALUE_ATTRIB,        asl::Vector3f(1,0,0));
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalVectorTag, asl::Vector3f,  GEOMETRY_GLOBAL_VALUE_ATTRIB, asl::Vector3f(1,0,0));
    class Vector :
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
    DEFINE_ATTRIBUT_TAG(LineSegmentTag,       asl::LineSegment<float>,  GEOMETRY_VALUE_ATTRIB,
            asl::LineSegment<float>(asl::Vector3f(0,0,0), asl::Vector3f(1,0,0)));
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalLineSegmentTag, asl::LineSegment<float>,  GEOMETRY_GLOBAL_VALUE_ATTRIB,
            asl::LineSegment<float>(asl::Vector3f(0,0,0), asl::Vector3f(1,0,0)));
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
