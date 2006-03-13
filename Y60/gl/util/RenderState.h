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
//   $RCSfile: RenderState.h,v $
//   $Author: danielk $
//   $Revision: 1.2 $
//   $Date: 2005/03/23 15:08:20 $
//
//  Description:
//
//=============================================================================

#ifndef _ac_y60_RendererState_h_
#define _ac_y60_RendererState_h_

#include <asl/Plane.h>
#include <asl/Ptr.h>

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
                _myCullFaces(GL_BACK),
                _myDrawNormalsFlag(false),
                _myEnabledClippingPlanes(0)
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

            void setCullFaces(GLenum theFaces) {
                if (theFaces != _myCullFaces) {
                    commitCullFaces(theFaces);
                }
            }
            GLenum getCullFaces() const { return _myCullFaces; }
            void setClippingPlanes(const std::vector<asl::Planef> & thePlanes);
            void setDrawNormals(bool theFlag) { _myDrawNormalsFlag = theFlag; }
            bool getDrawNormals() const { return _myDrawNormalsFlag; }
        private:
            void commitCullFaces(GLenum theFaces);

            bool _myDrawNormalsFlag;
            int  _myEnabledClippingPlanes;
            GLenum _myCullFaces;

    };

    typedef asl::Ptr<RenderState> RenderStatePtr;
}

#endif

