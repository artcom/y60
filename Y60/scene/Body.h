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
//   $RCSfile: Body.h,v $
//   $Author: david $
//   $Revision: 1.12 $
//   $Date: 2005/04/04 14:40:15 $
//
//=============================================================================

#ifndef _ac_scene_Body_h_
#define _ac_scene_Body_h_

#include "TransformHierarchyFacade.h"
#include "Shape.h"
#include <asl/dom/Nodes.h>

namespace y60 {
    class Scene;

    //                  theTagName   theType                   theAttributeName        theDefault
    DEFINE_ATTRIBUT_TAG(ShapeTag,    std::string,              BODY_SHAPE_ATTRIB, "");
    DEFINE_ATTRIBUT_TAG(SkeletonTag, std::vector<std::string>, SKELETON_ATTRIB,   TYPE());

    class Body;
    typedef asl::Ptr<Body, dom::ThreadingModel> BodyPtr;

    class Body :
        public TransformHierarchyFacade,
        public ShapeTag::Plug,
        public SkeletonTag::Plug
    {
        public:
            Body(dom::Node & theNode);
            virtual ~Body();

            IMPLEMENT_FACADE(Body);

            static BodyPtr create(dom::NodePtr theParent, const std::string & theShapeId);

            void registerDependenciesRegistrators();
            Shape & getShape();           
            const Shape & getShape() const;

			ShapePtr getShapePtr();           
            const ShapePtr getShapePtr() const;           
            double calculateVolume(const Scene & theScene) const;

        protected:
            virtual void registerDependenciesForBoundingBox();
            virtual void recalculateBoundingBox();            
    };        
}

#endif
