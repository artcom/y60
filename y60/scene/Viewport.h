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

#ifndef _scene_Viewport_h_
#define _scene_Viewport_h_

#include "y60_scene_settings.h"

#include <y60/base/CommonTags.h>
#include <y60/base/NodeNames.h>
#include <y60/base/DataTypes.h>
#include <asl/dom/Facade.h>
#include <asl/dom/AttributePlug.h>
#include <asl/math/Frustum.h>

#include <y60/jslib/Picking.h>

namespace y60 {

    //                  theTagName                  theType        theAttributeName                      theDefault
    DEFINE_ATTRIBUTE_TAG(ViewportOrientationTag,     std::string,   ORIENTATION_ATTRIB,              LANDSCAPE_ORIENTATION, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(CameraTag,                  std::string,   CAMERA_ATTRIB,                   "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(Size2DTag,                  asl::Vector2f, VIEWPORT_SIZE_ATTRIB,            asl::Vector2f(1,1), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportLeftTag,            int,           VIEWPORT_LEFT_ATTRIB,            0, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportTopTag,             int,           VIEWPORT_TOP_ATTRIB,             0, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportHeightTag,          unsigned,      VIEWPORT_HEIGHT_ATTRIB,          0, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportWidthTag,           unsigned,      VIEWPORT_WIDTH_ATTRIB,           0, Y60_SCENE_DECL);

    DEFINE_ATTRIBUTE_TAG(ViewportBackfaceCullingTag, bool,          VIEWPORT_BACKFACECULLING_ATTRIB, true, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportWireframeTag,       bool,          VIEWPORT_WIREFRAME_ATTRIB,       false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportLightingTag,        bool,          VIEWPORT_LIGHTING_ATTRIB,        true, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportFlatshadingTag,     bool,          VIEWPORT_FLATSHADING_ATTRIB,     false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportCullingTag,         bool,          VIEWPORT_CULLING_ATTRIB,         true, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportDebugCullingTag,    bool,          VIEWPORT_DEBUGCULLING_ATTRIB,    false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportTexturingTag,       bool,          VIEWPORT_TEXTURING_ATTRIB,       true, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportAlphaTestTag,       bool,          VIEWPORT_ALPHATEST_ATTRIB,
    true, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportDrawGlowTag,        bool,          VIEWPORT_GLOW_ATTRIB,            false, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(ViewportDrawNormalsTag,     bool,          VIEWPORT_DRAWNORMALS_ATTRIB,     false, Y60_SCENE_DECL);

    typedef asl::Ptr<Viewport, dom::ThreadingModel> ViewportPtr;

    class Y60_SCENE_DECL Viewport :
        public dom::Facade,
        public IdTag::Plug,
		public NameTag::Plug,
		public CameraTag::Plug,
        public Position2DTag::Plug,
        public Size2DTag::Plug,
		public ViewportOrientationTag::Plug,
		public ViewportBackfaceCullingTag::Plug,
		public ViewportWireframeTag::Plug,
		public ViewportLightingTag::Plug,
		public ViewportTexturingTag::Plug,
		public ViewportFlatshadingTag::Plug,
		public ViewportCullingTag::Plug,
		public ViewportDebugCullingTag::Plug,
        public ViewportAlphaTestTag::Plug,
        public ViewportDrawNormalsTag::Plug,
        public ViewportDrawGlowTag::Plug,
        public dom::DynamicAttributePlug<ViewportTopTag, Viewport>,
        public dom::DynamicAttributePlug<ViewportLeftTag, Viewport>,
        public dom::DynamicAttributePlug<ViewportHeightTag, Viewport>,
        public dom::DynamicAttributePlug<ViewportWidthTag, Viewport>,
        public ResizePolicyTag::Plug
    {
        public:
            Viewport(dom::Node & theNode)
                : dom::Facade(theNode),
                  IdTag::Plug(theNode),
                  NameTag::Plug(theNode),
		          CameraTag::Plug(theNode),
                  Position2DTag::Plug(theNode),
                  Size2DTag::Plug(theNode),
                  ViewportOrientationTag::Plug(theNode),
                  ViewportBackfaceCullingTag::Plug(theNode),
                  ViewportWireframeTag::Plug(theNode),
                  ViewportLightingTag::Plug(theNode),
                  ViewportTexturingTag::Plug(theNode),
                  ViewportFlatshadingTag::Plug(theNode),
                  ViewportCullingTag::Plug(theNode),
                  ViewportDebugCullingTag::Plug(theNode),
                  ViewportAlphaTestTag::Plug(theNode),
                  ViewportDrawNormalsTag::Plug(theNode),
                  ViewportDrawGlowTag::Plug(theNode),
                  dom::DynamicAttributePlug<ViewportTopTag, Viewport>(this, &Viewport::getTop),
                  dom::DynamicAttributePlug<ViewportLeftTag, Viewport>(this, &Viewport::getLeft),
                  dom::DynamicAttributePlug<ViewportHeightTag, Viewport>(this, &Viewport::getHeight),
                  dom::DynamicAttributePlug<ViewportWidthTag, Viewport>(this, &Viewport::getWidth),
                  ResizePolicyTag::Plug( theNode ),
                  _myPicking()
            {}

            IMPLEMENT_DYNAMIC_FACADE(Viewport);
            /// returns the distance between bottom of viewport and bottom of
            //  canvas. Suitable for glViewport.
            int getLower() const;
            inline dom::NodePtr pickBody(const unsigned int theX, const unsigned int theY) const {
                return _myPicking.pickBody(this->getNode(), theX, theY);
            };
            inline dom::NodePtr pickBodyBySweepingSphereFromBodies(const unsigned int theX, const unsigned int theY, const float theSphereRadius) const {
                return _myPicking.pickBodyBySweepingSphereFromBodies(this->getNode(), theX, theY, theSphereRadius);
            };
            void applyAspectToCamera();
        private:
            bool getTop(int & theTop) const;
            bool getLeft(int & theLeft) const;
            bool getHeight(unsigned & theHeight) const;
            bool getWidth(unsigned & theWidth) const;

            jslib::Picking _myPicking;
    };

}

#endif
