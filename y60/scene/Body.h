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
//   $RCSfile: Body.h,v $
//   $Author: david $
//   $Revision: 1.12 $
//   $Date: 2005/04/04 14:40:15 $
//
//=============================================================================

#ifndef _ac_scene_Body_h_
#define _ac_scene_Body_h_

#include "y60_scene_settings.h"

#include "TransformHierarchyFacade.h"
#include "Shape.h"
#include <asl/dom/Nodes.h>

namespace y60 {
    class Scene;

    //                  theTagName   theType                   theAttributeName        theDefault
    DEFINE_ATTRIBUTE_TAG(ShapeTag,    std::string,              BODY_SHAPE_ATTRIB, "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(SkeletonTag, std::vector<std::string>, SKELETON_ATTRIB,   TYPE(), Y60_SCENE_DECL);

    class Body;
    typedef asl::Ptr<Body, dom::ThreadingModel> BodyPtr;

    class Y60_SCENE_DECL Body :
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
