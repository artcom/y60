//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_RendererState_h_
#define _ac_y60_RendererState_h_

#include <asl/Plane.h>
#include <asl/Ptr.h>
#include <asl/Box.h>

#ifdef WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif
    #include <windows.h>
    #undef max
#endif

#include <GL/gl.h>

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

    class RenderState {
        public:
            RenderState() :
                _myWireframeFlag(false),
                _myLightingFlag(true),
                _myBackfaceCullingFlag(false),
                _myFlatShadingFlag(false),
                _myTexturingFlag(true),
                _myIgnoreDepthFlag(false),
                _myDepthWritesFlag(true),
                _myPolygonOffsetFlag(false),
                _myBlendFlag(false),
                _myAlphaTestFlag(true),
                _myScissorTestFlag(false),
                _myFrontFaceCCWFlag(true),
                _myCullFaces(GL_BACK),
                _myEnabledClippingPlanes(0),
                _myScissorParams(0,0,1,1)
            {
                init();
            }

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
