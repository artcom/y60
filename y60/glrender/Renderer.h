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
*/

#ifndef _ac_renderer_Renderer_h_
#define _ac_renderer_Renderer_h_

#include "y60_glrender_settings.h"

#include <y60/glutil/GLUtils.h>
#include "BodyPart.h"

#include <y60/glutil/RenderState.h>

#include <y60/gltext/TextRendererManager.h>

#include <y60/scene/Body.h>
#include <y60/scene/Canvas.h>
#include <y60/scene/Viewport.h>
#include <y60/scene/Scene.h>
#include <y60/scene/MaterialBase.h>
#include <y60/scene/Light.h>
#include <y60/scene/Facades.h>
#include <y60/glutil/GLContext.h>


#include <asl/math/Frustum.h>
#include <asl/base/Dashboard.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/intersection.h>
#include <asl/base/Exception.h>
#include <asl/math/SvgPath.h>

#ifdef _WIN32
#   include <windows.h>
#endif

// CG support
#ifndef _AC_NO_CG_
    #include <Cg/cg.h>
    #include <Cg/cgGL.h>
#endif

#include <string>
#include <vector>
#include <list>

namespace y60 {
    class Overlay;
}

namespace y60 {

    DEFINE_EXCEPTION(RendererException, asl::Exception);

    class Y60_GLRENDER_DECL Renderer {
        public:
            Renderer(GLContextPtr theGLContext, unsigned int theRenderingCaps = -1 /*get em all*/);
            virtual ~Renderer();

            void clearBuffers(const y60::CanvasPtr & theCanvas, unsigned int theBuffersMask);
            void render(y60::ViewportPtr theViewport);

            IShaderLibraryPtr getShaderLibrary();

            bool getStatisticsOn() const;
            void setStatisticsOn(bool theOn);
            bool getCulling() const;
            void setCulling(bool theOn);

            TextRendererManager & getTextManager() {
                return _myTextRendererManager;
            }

            const TextRendererManager & getTextManager() const {
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

            void renderFrustum(dom::NodePtr theProjectiveNode );
            asl::Unsigned64 getFrameNumber() const {
                return _myFrameNumber;
            }
       private:
            void setupRenderState(ViewportPtr theViewport);

            void bindViewMatrix(y60::CameraPtr theCamera);
            void setProjection(ViewportPtr theViewport, y60::CameraPtr theCamera);
            void rotateBillboard(Body & theBody, y60::CameraPtr theCamera);

            GLenum getPrimitiveGLType(PrimitiveType theType) const;

            void enableRenderStyles(const RenderStyles & theRenderStyles, const MaterialBase * theMaterial);
            bool switchMaterial(const Viewport & theViewport, const MaterialBase & theMaterial, bool isOverlay = false);
            void deactivatePreviousMaterial() const;

            dom::NodePtr getActiveLodChild(dom::NodePtr theNode, const y60::CameraPtr theCamera);

            void createRenderList(const WorldPtr & theWorld,
                                  const dom::NodePtr & theNode,
                                  BodyPartMap & theBodyParts,
                                  const y60::CameraPtr theCamera,
                                  const asl::Matrix4f & theEyeSpaceTransform,
                                  ViewportPtr theViewport,
                                  bool theOverlapFrustumFlag,
                                  std::vector<asl::Planef> theClippingPlanes,
                                  asl::Box2f theScissorBox);
            void renderBodyPart(const BodyPart & theBodyPart,
                                const Viewport & theViewport,
                                const Camera & theCamera);
            void renderPrimitives(const BodyPart & theBodyPart,
                                  const MaterialBase & theMaterial);

            void renderBoundingBox(const asl::Box3f & theBox);
            void renderBoundingBoxHierarchy(dom::NodePtr theNode);

            void renderOverlays(const Viewport & theViewport,
                                const std::string & theRootNodeName);
            void renderOverlay(const Viewport & theViewport, dom::NodePtr theOverlayNode, float theAlpha = 1);

            void renderBox(const asl::Point3f & theLTF, const asl::Point3f & theRBF,
                           const asl::Point3f & theRTF, const asl::Point3f & theLBF,
                           const asl::Point3f & theLTBK, const asl::Point3f & theRBBK,
                           const asl::Point3f & theRTBK, const asl::Point3f & theLBBK,
                           const asl::Vector4f & theColor, asl::Vector4f theBackColor = asl::Vector4f(0,0,0,0));

            void renderAnalyticGeometry( y60::ViewportPtr theViewport, CameraPtr theCamera);

            /// setup immediate draw
            void preDraw(const asl::Vector4f & theColor,
                         const asl::Matrix4f & theTransformation,
                         float theSize = 1.0f,
                         const std::string & theRenderStyle = "");

            /// restore from immediate draw
            void postDraw();

            void enableFog(WorldPtr & theWorld);
            void disableAllLights();
            void enableVisibleLights(WorldPtr & theWorld);
            void enableLight(const y60::LightPtr & theLight, int theActiveLightIndex);
            void renderSkyBox(const Viewport & theViewport, y60::CameraPtr theCamera);
            void drawNormals(const Primitive & thePrimitive, float theNormalScale);

            double getBillboardRotationY(const asl::Matrix4f & theBillboardMatrix,
                                        const asl::Matrix4f & theCameraMatrix);

            void collectClippingPlanes(dom::NodePtr theNode, std::vector<asl::Planef> & theClippingPlanes);
            void collectScissorBox(dom::NodePtr theNode, asl::Box2f & theScissorBox);
        private:
            // renderer members
            TextRendererManager     _myTextRendererManager;
            unsigned int            _myRenderingCaps;
            y60::ScenePtr           _myScene;

            // OpenGl state chache
            GLContextPtr            _myContext;
            RenderStatePtr          _myState;
            unsigned short          _myBoundingVolumeMode;

            // transient state (valid during one render pass)
            VertexRegisterFlags  _myLastVertexRegisterFlags;
            const MaterialBase * _myPreviousMaterial;
            Body const *         _myPreviousBody;

            // needed to avoid skybox/underlays interferences
            bool _myRenderedUnderlays;
            asl::Unsigned64 _myFrameNumber;
};

    typedef asl::Ptr<Renderer> RendererPtr;
}

#endif // _ac_renderer_Renderer_h_
