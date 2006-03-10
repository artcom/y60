//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_renderer_Renderer_h_
#define _ac_renderer_Renderer_h_

#include "BodyPart.h"
#include "RenderState.h"

#include <y60/TextRendererManager.h>

#include <y60/Body.h>
#include <y60/Canvas.h>
#include <y60/Viewport.h>
#include <y60/Scene.h>
#include <y60/MaterialBase.h>
#include <y60/Light.h>

#include <asl/Frustum.h>
#include <asl/Dashboard.h>
#include <asl/linearAlgebra.h>
#include <asl/intersection.h>
#include <asl/Exception.h>
#include <asl/SvgPath.h>

#ifdef WIN32
#   include <windows.h>
#endif

#include <GL/gl.h>

// CG support
#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <string>
#include <vector>
#include <list>

namespace y60 {
    class Overlay;
}

namespace y60 {

    DEFINE_EXCEPTION(RendererException, asl::Exception);

    class Renderer {
        public:
            Renderer(unsigned int theRenderingCaps = -1 /*get em all*/);
            virtual ~Renderer();

            void preRender(const y60::CanvasPtr & theCanvas);
            void render(y60::ViewportPtr theViewport);
            void postRender();

            void preloadShader();
            IShaderLibraryPtr getShaderLibrary();

            bool getStatisticsOn() const;
            void setStatisticsOn(bool theOn);
            bool getCulling() const;
            void setCulling(bool theOn);

            TextRendererManager & getTextManager() {
                return _myTextRendererManager;
            }

            enum {
                BV_NONE      = 0,
                BV_SHAPE     = 1,
                BV_BODY      = 2,
                BV_HIERARCHY = 4
            };
            void setBoundingVolumeMode(unsigned short theMode);
            unsigned short getBoundingVolumeMode() const;

            void setCurrentScene(y60::ScenePtr theScene);

            y60::ScenePtr getCurrentScene() const {
                return _myScene;
            }

            void initGL();

            template <class T>
            void draw(const T & theDrawable,
                      const asl::Vector4f & theColor,
                      const asl::Matrix4f & theTransformation,
                      float theWidth = 1.0f,
                      const std::string & theRenderStyles = "");

       private:
            void setupRenderState(ViewportPtr theViewport);

            void bindViewMatrix(y60::CameraPtr theCamera);
            void setProjection(ViewportPtr theViewport);
            void rotateBillboard(const Body & theBody, const Camera & theCamera);

            GLenum getPrimitiveGLType(PrimitiveType theType) const;

            void enableRenderStyles(const std::vector<RenderStyleType> & theRenderStyles);
            bool switchMaterial(const MaterialBase & theMaterial, bool isOverlay = false);
            void deactivatePreviousMaterial() const;

            dom::NodePtr getActiveLodChild(dom::NodePtr theNode, const y60::CameraPtr theCamera);

            void createRenderList(dom::NodePtr theNode, BodyPartMap & theBodyParts,
                                  const y60::CameraPtr theCamera,
                                  const asl::Matrix4f & theEyeSpaceTransform,
                                  ViewportPtr theViewport,
                                  bool theOverlapFrustumFlag,
                                  std::vector<asl::Planef> theClippingPlanes);
            void renderBodyPart(const BodyPart & theBodyPart,
                                const Viewport & theViewport,
                                const Camera & theCamera);
            void renderPrimitives(const BodyPart & theBodyPart,
                                  const MaterialBase & theMaterial);

            void renderBoundingBox(const asl::Box3f & theBox);
            void renderBoundingBoxHierarchy(dom::NodePtr theNode);

            void renderFrustum(const ViewportPtr & theViewport);
            void renderOverlays(const ViewportPtr & theViewport,
                                const std::string & theRootNodeName);
            void renderOverlay(dom::NodePtr theOverlayNode, float theAlpha = 1);
            void renderTextSnippets(ViewportPtr theViewport);

            void renderBox(const asl::Point3f & theLTF, const asl::Point3f & theRBF,
                           const asl::Point3f & theRTF, const asl::Point3f & theLBF,
                           const asl::Point3f & theLTBK, const asl::Point3f & theRBBK,
                           const asl::Point3f & theRTBK, const asl::Point3f & theLBBK,
                           const asl::Vector4f & theColor, asl::Vector4f theBackColor = asl::Vector4f(0,0,0,0));

            /// setup immediate draw
            void preDraw(const asl::Vector4f & theColor,
                         const asl::Matrix4f & theTransformation,
                         float theWidth = 1.0f,
                         const std::string & theRenderStyle = "");

            /// restore from immediate draw
            void postDraw();

            void enableFog();
            void enableVisibleLights();
            void enableLight(y60::LightPtr & theLight, int theActiveLightIndex);
            void renderSkyBox(y60::CameraPtr theCamera);
            void drawNormals(const Primitive & thePrimitive, float theNormalScale);

            double getBillboardRotation(const asl::Matrix4f & theBillboardMatrix,
                                        const asl::Matrix4f & theCameraMatrix);

            void collectClippingPlanes(dom::NodePtr theNode, std::vector<asl::Planef> & theClippingPlanes);
            // renderer members
            TextRendererManager     _myTextRendererManager;
            unsigned int            _myRenderingCaps;
            y60::ScenePtr           _myScene;

            // OpenGl state chache
            RenderState             _myState;
            unsigned short          _myBoundingVolumeMode;

            // transient state (valid during one render pass)
            VertexRegisterFlags  _myLastVertexRegisterFlags;
            const MaterialBase * _myPreviousMaterial;
            Body const *         _myPreviousBody;
};

    typedef asl::Ptr<Renderer> RendererPtr;
}

#endif // _ac_renderer_Renderer_h_
