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

#ifndef _ac_y60_RendererState_h_
#define _ac_y60_RendererState_h_

#include "y60_glutil_settings.h"

#include <asl/math/Plane.h>
#include <asl/base/Ptr.h>
#include <asl/math/Box.h>

#include <GL/glew.h>

#define RENDERSTATE_GETTER_SETTER(THE_NAME) \
    public: \
        void set ## THE_NAME (bool theFlag) { \
            if (theFlag != _my ## THE_NAME ## Flag) { \
                commit ## THE_NAME (theFlag); \
            } \
        } \
        bool get ## THE_NAME () const { \
        return _my ## THE_NAME ## Flag; \
        } \
    private: \
        void commit ## THE_NAME (bool theFlag); \
        bool _my ## THE_NAME ## Flag; \
    public:


namespace y60 {

    class Viewport;

    class Y60_GLUTIL_DECL RenderState {
        public:
            RenderState();
            void init();

            RENDERSTATE_GETTER_SETTER(Wireframe);
            RENDERSTATE_GETTER_SETTER(Lighting);
            RENDERSTATE_GETTER_SETTER(BackfaceCulling);
            RENDERSTATE_GETTER_SETTER(FlatShading);
            RENDERSTATE_GETTER_SETTER(Texturing);
            RENDERSTATE_GETTER_SETTER(IgnoreDepth);
            RENDERSTATE_GETTER_SETTER(DepthWrites);
            RENDERSTATE_GETTER_SETTER(PolygonOffset);
            RENDERSTATE_GETTER_SETTER(Blend);
            RENDERSTATE_GETTER_SETTER(AlphaTest);
            RENDERSTATE_GETTER_SETTER(ScissorTest);
            RENDERSTATE_GETTER_SETTER(FrontFaceCCW);

            void setCullFaces(GLenum theFaces) {
                if (theFaces != _myCullFaces) {
                    commitCullFaces(theFaces);
                }
            }
            GLenum getCullFaces() const { return _myCullFaces; }

            void setClippingPlanes(const std::vector<asl::Planef> & thePlanes);
            void setScissorBox(const asl::Box2f & theBox, const Viewport & theViewport);

        private:
            void commitCullFaces(GLenum theFaces);

            int  _myEnabledClippingPlanes;
            asl::Vector4i _myScissorParams;
            GLenum _myCullFaces;
    };

    typedef asl::Ptr<RenderState> RenderStatePtr;
}

#endif
