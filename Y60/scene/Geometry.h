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
#include <dom/Nodes.h>

namespace y60 {
    class Scene;

    //                  theTagName   theType                   theAttributeName        theDefault
    DEFINE_ATTRIBUT_TAG(GeometryPlaneTag,       asl::Planef,   GEOMETRY_PLANE_ATTRIB,    asl::Planef(asl::Vector3f(0,0,1), 0));
    DEFINE_ATTRIBUT_TAG(GeometryGlobalPlaneTag, asl::Planef,   GEOMETRY_GLOBAL_PLANE_ATTRIB, asl::Planef(asl::Vector3f(0,0,1), 0));

    class Geometry;
    typedef asl::Ptr<Geometry, dom::ThreadingModel> GeometryPtr;

    class Geometry :
        public TransformHierarchyFacade,
        public GeometryPlaneTag::Plug,
        public dom::FacadeAttributePlug<GeometryGlobalPlaneTag>
    {
        public:
            Geometry(dom::Node & theNode);
            virtual ~Geometry();

            IMPLEMENT_FACADE(Geometry);

            static GeometryPtr create(dom::NodePtr theParent, const asl::Planef & thePlane);

            void registerDependenciesRegistrators();
        protected:
            virtual void registerDependenciesForBoundingBox();
            virtual void registerDependenciesForGlobalPlane();

            virtual void recalculateBoundingBox();            
            virtual void recalculateGlobalPlane();            
    };        
}

#endif
