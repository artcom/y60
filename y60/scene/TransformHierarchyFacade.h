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

#ifndef _xml_TransformHierarchyFacades_h_
#define _xml_TransformHierarchyFacades_h_

#include "y60_scene_settings.h"

#include <y60/base/CommonTags.h>
#include <y60/base/NodeNames.h>
#include <y60/base/DataTypes.h>

#include <asl/dom/Facade.h>
#include <asl/dom/AttributePlug.h>

#include <asl/math/Matrix4.h>
#include <asl/math/Box.h>
#include <asl/math/Vector234.h>

namespace y60 {
    //                  theTagName           theType           theAttributeName               theDefault
    DEFINE_ATTRIBUTE_TAG(PositionTag,         asl::Vector3f,    POSITION_ATTRIB,          asl::Vector3f(0,0,0), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(OrientationTag,      asl::Quaternionf, ORIENTATION_ATTRIB,       asl::Quaternionf(0,0,0,1), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ScaleTag,            asl::Vector3f,    SCALE_ATTRIB,             asl::Vector3f(1,1,1), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(PivotTag,            asl::Vector3f,    PIVOT_ATTRIB,             asl::Vector3f(0,0,0), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(InsensibleTag,       bool,             INSENSIBLE_ATTRIB,        false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(BillboardTag,        std::string,      BILLBOARD_ATTRIB,         "", Y60_SCENE_DECL);
    
    DEFINE_ATTRIBUTE_TAG(BillboardLookatTag,  std::string,      BILLBOARD_LOOKAT_ATTRIB,  "", Y60_SCENE_DECL);
    
    DEFINE_ATTRIBUTE_TAG(RenderOrderTag,      int,              RENDERORDER_ATTRIB,       0, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(CullableTag,         bool,             CULLABLE_ATTRIB,          true, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ClippingPlanesTag,   VectorOfString,   CLIPPING_PLANES_ATTRIB,   VectorOfString(), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ScissorTag,          std::string,      SCISSOR_ATTRIB,           "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(StickyTag,           bool,             STICKY_ATTRIB,            false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(EventsTag,           VectorOfString,   "events",                 VectorOfString(), Y60_SCENE_DECL);

    DEFINE_FACADE_ATTRIBUTE_TAG(LocalMatrixTag,      asl::Matrix4f,    LOCAL_MATRIX_ATTRIB,      asl::Matrix4f::Identity(), Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(GlobalMatrixTag,     asl::Matrix4f,    GLOBAL_MATRIX_ATTRIB,     asl::Matrix4f::Identity(), Y60_SCENE_DECL);
    DEFINE_FACADE_ATTRIBUTE_TAG(InverseGlobalMatrixTag, asl::Matrix4f, INVERSE_GLOBAL_MATRIX_ATTRIB,     asl::Matrix4f::Identity(), Y60_SCENE_DECL);


    class Y60_SCENE_DECL TransformHierarchyFacade :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public VisibleTag::Plug,
        public InsensibleTag::Plug,
        public PositionTag::Plug,
        public OrientationTag::Plug,
        public ScaleTag::Plug,
        public PivotTag::Plug,
        public BillboardTag::Plug,
        public BillboardLookatTag::Plug,
        public RenderOrderTag::Plug,
        public CullableTag::Plug,
        public ClippingPlanesTag::Plug,
        public ScissorTag::Plug,
        public StickyTag::Plug,
        public EventsTag::Plug,
        public dom::FacadeAttributePlug<BoundingBoxTag>,
        public dom::FacadeAttributePlug<GlobalMatrixTag>,
        public dom::FacadeAttributePlug<LocalMatrixTag>,
        public dom::FacadeAttributePlug<InverseGlobalMatrixTag>,
        public dom::FacadeAttributePlug<LastActiveFrameTag>
    {
        public:
            TransformHierarchyFacade(dom::Node & theNode);

            IMPLEMENT_FACADE(TransformHierarchyFacade);

            dom::NodePtr getWorld() const {
            	const dom::Node& myOwnNode = getNode();
            	dom::NodePtr myNode = myOwnNode.self().lock();
            	while(myNode) {
            		myNode = myNode->parentNode()->self().lock();
            		if(myNode->nodeName() == "world") {
            			return myNode;
            		}
            	}
            	return dom::NodePtr();
            }

            asl::Vector3f getViewVector() const;
            asl::Vector3f getUpVector() const;
            asl::Vector3f getRightVector() const;
            virtual void registerDependenciesRegistrators();
        protected:
            void registerDependenciesForMaterialupdate();
            void registerDependenciesForLocalMatrix();
            void registerDependenciesForGlobalMatrix();
            void registerDependenciesForBoundingBox();
            void registerDependenciesForInverseGlobalMatrix();

            void recalculateLocalMatrix();
            void recalculateGlobalMatrix();
            void copyLocalToGlobalMatrix();
            void recalculateBoundingBox();
            void recalculateInverseGlobalMatrix();
            void registerDependenciesForEvents();
            void updateEventTrigger();
            void triggerRenderEvent();
        private:
            void getLocalMatrix(asl::Matrix4f & theLocalMatrix) const;
            bool _onRenderEventEnabled;
            bool _onFirstRenderEventEnabled;
            //TODO: onProximity, onUnused, onRemoval, onInsert

    };

    typedef asl::Ptr<TransformHierarchyFacade, dom::ThreadingModel> TransformHierarchyFacadePtr;
}

#endif
