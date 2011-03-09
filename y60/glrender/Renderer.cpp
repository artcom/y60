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

//own header
#include "Renderer.h"


#ifndef _AC_NO_CG
#include "CgShader.h"
#endif

#include <y60/scene/TransformHierarchyFacade.h>
#include <y60/scene/LodFacade.h>
#include <y60/scene/Facades.h>
#include <y60/gltext/BitmapTextRenderer.h>
#include <y60/glutil/GLUtils.h>
#include <y60/glutil/TextureCompressor.h>
#include <y60/scene/Shape.h>
#include <y60/scene/Viewport.h>
#include <y60/scene/Overlay.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/NodeNames.h>
#include <y60/base/PropertyNames.h>
#include <y60/base/DataTypes.h>
#include <y60/scene/Primitive.h>

#include <y60/scene/Viewport.h>
#include <y60/base/VertexDataRoles.h>

#include "GLResourceManager.h"

#include <asl/base/Assure.h>
#include <asl/base/begin_end.h>
#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/math/Matrix4.h>
#include <asl/base/Logger.h>
#include <asl/base/Time.h>

#include <asl/dom/Nodes.h>
#include <asl/dom/Schema.h>

#include <iostream>
#include <string>
#include <algorithm>

#define DB(x)   // x
#define DB2(x)  //x

// profiling
//#define PROFILING_LEVEL_FULL
//#define PROFILING_LEVEL_NORMAL

#ifdef PROFILING_LEVEL_NORMAL
#define DBP(x)  x
#else
#define DBP(x) // x
#endif

#ifdef PROFILING_LEVEL_FULL
#define DBP2(x)  x
#else
#define DBP2(x) // x
#endif

using namespace std;
using namespace asl;

namespace y60 {

    Renderer::Renderer(GLContextPtr theGLContext, unsigned int theRenderingCaps) :
        _myRenderingCaps(theRenderingCaps),
        _myScene(),
        _myContext(theGLContext),
        _myBoundingVolumeMode(BV_NONE),
        _myPreviousMaterial(0),
        _myPreviousBody(0),
        _myRenderedUnderlays(true),
        _myFrameNumber(0)
    {
        _myState = _myContext->getStateCache();
        _myLastVertexRegisterFlags.reset();
        initGL();
    }

    Renderer::~Renderer() {
        // UH: _myContext->isActive does not reflect the true status of a valid OpenGL context
        // the problem is that the destructor is called after the window is destroyed and therefore no
        // valid OpenGL context is available... maybe circumvent this by setting the scene to 0?
        if (_myScene) { // && _myContext && _myContext->isActive()) {
            _myScene->deregisterResourceManager();
        }
    }

    void
    Renderer::initGL() {
        // initialize the GL-Vertex Buffer/Object, initialize its Factories
        GLResourceManager::get().initVertexDataFactories();

        _myState->init();

        glColor3f(1.0, 1.0, 1.0);

        glEnable(GL_NORMALIZE);
        glEnable(GL_DEPTH_TEST);

        // enable separate calculation of specular color
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

        // If vertex colors are specified, use them in place of the ambient color value
        glColorMaterial(GL_FRONT, GL_AMBIENT);

        // enable blending
        // TODO: move this to the material handling
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // enable vertex arrays
        glEnableClientState(GL_VERTEX_ARRAY);

        glEnable(GL_MULTISAMPLE_ARB);
    }

    IShaderLibraryPtr
    Renderer::getShaderLibrary() {
        return GLResourceManager::get().getShaderLibrary();
    }

    void
    Renderer::setCurrentScene(y60::ScenePtr theScene) {
        if (_myScene) {
            // Deregister Resourcemanager
            _myScene->deregisterResourceManager();
        }
        _myScene = theScene;
        if (_myScene) {
            // Give the TextureManager an instance of our ResourceManager
            _myScene->registerResourceManager(&GLResourceManager::get());
        }
    }

    void
    Renderer::deactivatePreviousMaterial() const  {
        // clean up previous material
        if (_myPreviousMaterial) {
            DBP(MAKE_GL_SCOPE_TIMER(deactivatePreviousMaterial));

            IShaderPtr myPreviousShader = _myPreviousMaterial->getShader();
            myPreviousShader->disableTextures(*_myPreviousMaterial);
            CHECK_OGL_ERROR;

            myPreviousShader->deactivate(*_myPreviousMaterial);
            CHECK_OGL_ERROR;
        }
    }

    bool
    Renderer::switchMaterial(const Viewport & theViewport,
                             const MaterialBase & theMaterial,
                             bool isOverlay) {
        if (_myPreviousMaterial == &theMaterial) {
            return false;
        } else if (_myPreviousMaterial == 0) {
            _myLastVertexRegisterFlags.reset();
        }
        DBP(MAKE_GL_SCOPE_TIMER(switchMaterial));
        COUNT(materialChange);

        IShaderPtr myShader = theMaterial.getShader();
        deactivatePreviousMaterial();
        CHECK_OGL_ERROR;

        {
            // activate new material
            DBP(MAKE_GL_SCOPE_TIMER(activateShader));

            _myState->setLighting(theViewport.get<ViewportLightingTag>() && (theMaterial.getLightingModel() != UNLIT));
            CHECK_OGL_ERROR;

            // [CH] TODO: Material should be const.
            // The renderer should just take the scene information and render it as it is.
            // Right now in Shader.activate() the material representation is updated.
            // The scene should be responsible for that.
            if (myShader) {
                myShader->activate(const_cast<MaterialBase &>(theMaterial), theViewport, _myPreviousMaterial);
            } else {
                AC_ERROR << "Material has no shader, material id = " << theMaterial.get<IdTag>();
            }
            CHECK_OGL_ERROR;
        }
        DBP2(START_TIMER(renderBodyPart_switchMaterial_vertexRegistry));
        VertexRegisterFlags myVertexRegisterFlags;
        if (isOverlay) {
            myVertexRegisterFlags.reset();
        } else {
            myVertexRegisterFlags = myShader->getVertexRegisterFlags();
        }
        DBP2(STOP_TIMER(renderBodyPart_switchMaterial_vertexRegistry));
        for (unsigned myRegister = 0; myRegister < MAX_REGISTER; ++myRegister) {
            bool myEnable = false;
#if 1
            if (myVertexRegisterFlags[myRegister] && !_myLastVertexRegisterFlags[myRegister]) {
                myEnable = true;
            } else if (!myVertexRegisterFlags[myRegister] && _myLastVertexRegisterFlags[myRegister]) {
                myEnable = false;
            } else {
                continue;
            }
#else
            myEnable = myVertexRegisterFlags[myRegister];
#endif
            DBP2(START_TIMER(renderBodyPart_switchMaterial_register));
            switch (myRegister) {
                case POSITION_REGISTER:
                    break;
                case NORMAL_REGISTER:
                    if (myEnable) {
                        DB2(AC_TRACE <<"Enable GL_NORMAL_ARRAY"<<endl);
                        glEnableClientState(GL_NORMAL_ARRAY);
                    } else {
                        DB2(AC_TRACE <<"Disable GL_NORMAL_ARRAY"<<endl);
                        glDisableClientState(GL_NORMAL_ARRAY);
                    }
                    break;
                case COLORS_REGISTER:
                    if (myEnable) {
                        DB2(AC_TRACE <<"Enable GL_COLOR_ARRAY"<<endl);
                        glEnableClientState(GL_COLOR_ARRAY);
                        glEnable(GL_COLOR_MATERIAL);
                    } else {
                        DB2(AC_TRACE <<"Disable GL_COLOR_ARRAY"<<endl);
                        glDisableClientState(GL_COLOR_ARRAY);
                        glDisable(GL_COLOR_MATERIAL);
                    }
                    break;
                default:
                    glActiveTexture(asGLTextureRegister(GLRegister(myRegister)));
                    glClientActiveTexture(asGLTextureRegister(GLRegister(myRegister)));
                    DB2(AC_TRACE <<"Active Texture Register "<<asGLTextureRegister(GLRegister(myRegister))<<endl);
                    if (myEnable) {
                        DB2(AC_TRACE <<"Enable GL_TEXTURE_COORD_ARRAY"<<endl);
                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    } else {
                        DB2(AC_TRACE <<"Disable GL_TEXTURE_COORD_ARRAY"<<endl);
                        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                    }
                    break;
            }
        }
        DBP2(STOP_TIMER(renderBodyPart_switchMaterial_register));
        CHECK_OGL_ERROR;
        DBP2(START_TIMER(renderBodyPart_switchMaterial_enableTextures));
        if (_myState->getTexturing()) {
           myShader->enableTextures(theMaterial);
        }
        DBP2(STOP_TIMER(renderBodyPart_switchMaterial_enableTextures));
        CHECK_OGL_ERROR;

        _myPreviousMaterial        = &theMaterial;
        _myLastVertexRegisterFlags = myVertexRegisterFlags;
        return true;
    }

    void
    Renderer::renderBodyPart(const BodyPart & theBodyPart, const Viewport & theViewport, const Camera & theCamera) {
        DBP(MAKE_GL_SCOPE_TIMER(renderBodyPart));
        DBP2(START_TIMER(renderBodyPart_pre));

        const y60::World & myWorld = theBodyPart.getWorld();
        const y60::Body & myBody = theBodyPart.getBody();
        const y60::Shape & myShape = theBodyPart.getShape();
        bool myBodyHasChanged = (_myPreviousBody != &myBody);
        DBP2(STOP_TIMER(renderBodyPart_pre));
        DBP(START_TIMER(renderBodyPart_bodyChanged));
        if (myBodyHasChanged) {
            glPopMatrix();
            CHECK_OGL_ERROR;

            _myState->setClippingPlanes(theBodyPart.getClippingPlanes());
            _myState->setScissorBox(theBodyPart.getScissorBox(), theViewport);

            glPushMatrix();

            // draw body bounding-box
            if (_myBoundingVolumeMode & BV_BODY) {
                const Box3f & myBoundingBox = myBody.get<BoundingBoxTag>();
                renderBoundingBox(myBoundingBox);
                CHECK_OGL_ERROR;
            }

            glMultMatrixf((myBody.get<GlobalMatrixTag>().getData()));

            // change face-winding when matrix is mirroring
            const asl::Matrix4f & myMatrix = myBody.get<GlobalMatrixTag>();
            const asl::Vector3f & myXVector = asVector3(myMatrix[0][0]);
            const asl::Vector3f & myYVector = asVector3(myMatrix[1][0]);
            const asl::Vector3f & myZVector = asVector3(myMatrix[2][0]);
            bool myNegativeScaleFlag = (dot(myXVector, cross(myYVector, myZVector)) < 0);
            _myState->setFrontFaceCCW( ! myNegativeScaleFlag );

            _myPreviousBody = &myBody;
            CHECK_OGL_ERROR;
        }
        DBP(STOP_TIMER(renderBodyPart_bodyChanged));

        DBP2(START_TIMER(renderBodyPart_getDrawNormals));
        const y60::Primitive & myPrimitive = theBodyPart.getPrimitive();
        if (theViewport.get<ViewportDrawNormalsTag>()) {
            const Box3f & myBoundingBox = myShape.get<BoundingBoxTag>();
            float myDiameter = magnitude(myBoundingBox[Box3f::MAX] - myBoundingBox[Box3f::MIN]);
            drawNormals(myPrimitive, myDiameter / 64.0f);
        }
        DBP2(STOP_TIMER(renderBodyPart_getDrawNormals));

        DBP2(START_TIMER(renderBodyPart_switchMaterial));
        const MaterialBase & myMaterial = myPrimitive.getMaterial();
        bool myMaterialHasChanged = switchMaterial(theViewport, myMaterial);
        DBP2(STOP_TIMER(renderBodyPart_switchMaterial));

        DBP2(START_TIMER(renderBodyPart_materialChanged));
        DBP2(START_TIMER(renderBodyPart_materialChanged_registerTexture));
        glMatrixMode( GL_TEXTURE );
        for (unsigned myTexUnit = 0; myTexUnit < myMaterial.getTextureUnitCount(); ++myTexUnit) {
            glActiveTexture(asGLTextureRegister(myTexUnit));
            glPushMatrix();
        }
        glMatrixMode( GL_MODELVIEW );
        DBP2(STOP_TIMER(renderBodyPart_materialChanged_registerTexture));

        IShaderPtr myShader = myMaterial.getShader();
        if (myShader) {
            if (myMaterial.hasTexGen()) {
            	DBP2(START_TIMER(renderBodyPart_materialChanged_enableTextureProjection));
                myShader->enableTextureProjection( myMaterial, theViewport, theCamera );
                DBP2(STOP_TIMER(renderBodyPart_materialChanged_enableTextureProjection));
            }
            if (myBodyHasChanged || myMaterialHasChanged) {
                DBP2(MAKE_GL_SCOPE_TIMER(renderBodyPart_bindBodyParams));
                myShader->bindBodyParams(myMaterial, theViewport, myWorld.getLights(), myBody, theCamera);
                CHECK_OGL_ERROR;
            }
        }
        DBP2(STOP_TIMER(renderBodyPart_materialChanged));

        // get renderstyles for this primitive
        DBP2(START_TIMER(renderBodyPart_getRenderStyles));
        const RenderStyles & myPrimitveStyle = myPrimitive.get<RenderStylesTag>();
        const RenderStyles & myShapeStyle    = myShape.get<RenderStylesTag>();

        const RenderStyles & myRenderStyles  = myPrimitveStyle.any() ? myPrimitveStyle : myShapeStyle;
        DBP2(STOP_TIMER(renderBodyPart_getRenderStyles));

        DBP2(START_TIMER(renderBodyPart_render));
        DBP2(START_TIMER(renderBodyPart_enableREnderStyles));
        enableRenderStyles(myRenderStyles, &myMaterial);
        DBP2(STOP_TIMER(renderBodyPart_enableREnderStyles));

        bool myRendererCullingEnabled = _myState->getBackfaceCulling();
        if (myRendererCullingEnabled) {
            if (myRenderStyles[FRONT] && myRenderStyles[BACK]) {
                // render back & front (two passes, supposedly faster
                DBP2(START_TIMER(renderBodyPart_renderPrimitives1));
                _myState->setCullFaces(GL_FRONT);
                renderPrimitives(theBodyPart, myMaterial);
                _myState->setCullFaces(GL_BACK);
                renderPrimitives(theBodyPart, myMaterial);
                DBP2(STOP_TIMER(renderBodyPart_renderPrimitives1));
            } else {  // render one or zero faces - single pass
                DBP2(START_TIMER(renderBodyPart_renderPrimitives2));
                if (!myRenderStyles[FRONT] && !myRenderStyles[BACK]) {
                    // UH: no facets are drawn but other primitives such as points and lines are
                    _myState->setCullFaces(GL_FRONT_AND_BACK);
                } else {
                    if (myRenderStyles[FRONT]) {
                        _myState->setCullFaces(GL_BACK); // render front
                    } else {
                        _myState->setCullFaces(GL_FRONT); // render back
                    }
                }
                renderPrimitives(theBodyPart, myMaterial);
                DBP2(START_TIMER(renderBodyPart_renderPrimitives2));
            }
        } else { // face culling is disabled - just render it.
            DBP2(START_TIMER(renderBodyPart_renderPrimitives3));
            renderPrimitives(theBodyPart, myMaterial);
            DBP2(START_TIMER(renderBodyPart_renderPrimitives3));
        }
        CHECK_OGL_ERROR;
        DBP2(STOP_TIMER(renderBodyPart_render));

        DBP2(START_TIMER(renderBodyPart_setupBoundingVolume));
        if (myRenderStyles[BOUNDING_VOLUME] || (_myBoundingVolumeMode & BV_SHAPE)) {
            const asl::Box3f & myBoundingBox = myShape.get<BoundingBoxTag>();
            renderBoundingBox(myBoundingBox);
            CHECK_OGL_ERROR;
        }
        DBP2(STOP_TIMER(renderBodyPart_setupBoundingVolume));

        DBP2(START_TIMER(renderBodyPart_registerTextures));
        glMatrixMode( GL_TEXTURE );
        for (unsigned myTexUnit = 0; myTexUnit < myMaterial.getTextureUnitCount(); ++myTexUnit) {
            glActiveTexture(asGLTextureRegister(myTexUnit));
            glPopMatrix();
        }
        DBP2(STOP_TIMER(renderBodyPart_registerTextures));
        glMatrixMode( GL_MODELVIEW );
        CHECK_OGL_ERROR;
    }

    void
    Renderer::drawNormals(const Primitive & thePrimitive, float theNormalScale) {
        if (thePrimitive.hasVertexData(NORMALS)) {
            glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
            glDisable(GL_LIGHTING);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
            glColor3f(1.0f, .5f, 0);

            asl::Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = thePrimitive.getConstLockingPositionsAccessor();
            asl::Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = thePrimitive.getConstLockingNormalsAccessor();
            const VertexData3f & myPositions = myPositionAccessor->get();
            const VertexData3f & myNormals = myNormalsAccessor->get();
            Vector3f myNormalTip;

            glBegin(GL_LINES);
            for (VertexData3f::size_type i = 0; i < myPositions.size(); ++i) {
                glVertex3fv(myPositions[i].begin());
                myNormalTip = myPositions[i] + (myNormals[i] * theNormalScale);
                glVertex3fv(myNormalTip.begin());
            }
            glEnd();
            glPopAttrib();
        }
    }

    void
    Renderer::renderPrimitives(const BodyPart & theBodyPart, const MaterialBase & theMaterial) {
        DBP(MAKE_GL_SCOPE_TIMER(renderPrimitives));
        DBP2(START_TIMER(renderBodyPart_renderPrimitives));
        DBP(START_TIMER(getPrimitive));
        const y60::Primitive & myPrimitive = theBodyPart.getPrimitive();
        DBP(STOP_TIMER(getPrimitive));
        COUNT_N(Vertices, myPrimitive.size());


        const MaterialParameterVector & myMaterialParameters = theMaterial.getVertexParameters();
        unsigned myVertexParamSize = myMaterialParameters.size();
        DBP2(START_TIMER(renderBodyPart_renderPrimitives_loop));
        for (unsigned i = 0; i < myVertexParamSize; ++i) {
            const MaterialParameter & myParameter = myMaterialParameters[i];
            if (myPrimitive.hasVertexData(myParameter.getRole())) {
                const VertexDataBase & myData = myPrimitive.getVertexData(myParameter.getRole());
                DB(AC_DEBUG << "-- Parameter " << i << " role " << getStringFromEnum(myParameter.getRole(), VertexDataRoleString) << ", data=" << myData.getDataPtr());
                GLRegister myRegister = myParameter.getRegister();
                switch (myRegister) {
                    case POSITION_REGISTER:
                        myData.useAsPosition();
                        break;
                    case NORMAL_REGISTER:
                        myData.useAsNormal();
                        break;
                    case COLORS_REGISTER:
                        myData.useAsColor();
                        break;
                    default:
                    	DBP2(START_TIMER(renderBodyPart_renderPrimitives_loopdefault));
                        GLenum myGlRegister = asGLTextureRegister(myRegister);
                        glActiveTexture(myGlRegister);
                        glClientActiveTexture(myGlRegister);
                        myData.useAsTexCoord();
                        DBP2(STOP_TIMER(renderBodyPart_renderPrimitives_loopdefault));
                        break;
                }
                CHECK_OGL_ERROR;
			} else {
				throw RendererException(string("Body Part of shape: ") + theBodyPart.getShape().get<IdTag>() +
					" does not contain required vertexdata of role: " +
					getStringFromEnum(myParameter.getRole(), GLRegisterString), PLUS_FILE_LINE);
			}
        }
        DBP2(STOP_TIMER(renderBodyPart_renderPrimitives_loop));
        /*
        DBP2(
            unsigned long myPrimitiveCount = myPrimitive.size();
            AC_TRACE << "glDrawArrays size=" << myPrimitive.size() << " primCount="
                     << myPrimitiveCount << " primType=" << myPrimitive.getType()
                     << " GLtype=" << getPrimitiveGLType(myPrimitive.getType());
            static unsigned long myMaxPrimitiveCount = 0;
            if (myPrimitiveCount > myMaxPrimitiveCount) {
                myMaxPrimitiveCount = myPrimitiveCount;
            }
            AC_TRACE << "Primitive maxsize: " << myMaxPrimitiveCount;
        )
        */

        DBP2(static asl::NanoTime lastTime;
                asl::NanoTime switchTime = asl::NanoTime() - lastTime;
                AC_TRACE << "switch time = " << switchTime.micros() << " us" << endl;
                asl::NanoTime startTime;
           );

        DBP(COUNT(VertexArrays));

        {
            DBP(MAKE_GL_SCOPE_TIMER(glDrawArrays));
            glDrawArrays(getPrimitiveGLType(myPrimitive.get<PrimitiveTypeTag>()), 0, myPrimitive.size());
            CHECK_OGL_ERROR;
        }
        DBP2(STOP_TIMER(renderBodyPart_renderPrimitives));
    }

    void
    Renderer::renderBoundingBoxHierarchy(dom::NodePtr theNode) {
        if (theNode->nodeType() == dom::Node::ELEMENT_NODE &&
            (theNode->nodeName() == TRANSFORM_NODE_NAME || theNode->nodeName() == JOINT_NODE_NAME))
        {
            const asl::Box3f & myBox = theNode->getFacade<TransformHierarchyFacade>()->get<BoundingBoxTag>();
            renderBoundingBox(myBox);
        }
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            renderBoundingBoxHierarchy(theNode->childNode(i));
        }
    }

    void
    Renderer::renderBoundingBox(const asl::Box3f & theBox) {
        Point3f myLTF, myRBF, myRTF, myLBF;
        Point3f myLTBK, myRBBK, myRTBK, myLBBK;

        theBox.getCorners(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK);
        renderBox(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK,
                  Vector4f(1.0, 0.5, 0.5, 1.0));
    }

    /*
    void
    Renderer::renderFrustum(const CameraPtr & theCamera) {
        Point3f myLTF, myRBF, myRTF, myLBF;
        Point3f myLTBK, myRBBK, myRTBK, myLBBK;

        theCamera->get<FrustumTag>().getCorners(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK);
        renderBox(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK,
                  Vector4f(0.5, 1.0, 0.5, 1.0), Vector4f(1.0, 0.5, 0.0, 1.0));
    }
    */

    void
    Renderer::renderFrustum( dom::NodePtr theProjectiveNode ) {
        ProjectiveNodePtr myProjector = theProjectiveNode->getFacade<ProjectiveNode>();
        Frustum myFrustum = myProjector->get<FrustumTag>(); // XXX superfluous copy

        Point3f myCamPos = myProjector->get<PositionTag>();
        Point3f myLTF, myRBF, myRTF, myLBF;
        Point3f myLTBK, myRBBK, myRTBK, myLBBK;

        myFrustum.updatePlanes( myProjector->get<GlobalMatrixTag>(), myProjector->get<InverseGlobalMatrixTag>() );
        myFrustum.getCorners(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK);

        renderBox(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK,
                  Vector4f(0.5, 1.0, 0.5, 1.0), Vector4f(1.0, 0.5, 0.0, 1.0));

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor4f(1.0, 1.0, 1.0, 0.5);
        glBegin(GL_LINES);
        {
            glVertex3fv(myCamPos.begin());
            glVertex3fv(myLTF.begin());

            glVertex3fv(myCamPos.begin());
            glVertex3fv(myLBF.begin());

            glVertex3fv(myCamPos.begin());
            glVertex3fv(myRTF.begin());

            glVertex3fv(myCamPos.begin());
            glVertex3fv(myRBF.begin());
        }
        glEnd();
        glPopAttrib();
    }

    void
    Renderer::renderBox(const Point3f & theLTF, const Point3f & theRBF,
                        const Point3f & theRTF, const Point3f & theLBF,
                        const Point3f & theLTBK, const Point3f & theRBBK,
                        const Point3f & theRTBK, const Point3f & theLBBK,
                        const Vector4f & theFrontColor, Vector4f theBackColor)

    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor4fv(theFrontColor.begin());
        if (theBackColor == Vector4f(0,0,0,0)) {
            theBackColor = theFrontColor;
        }

        // DS: Ok, let's play 'Das ist das haus vom ni-ko-laus' in 3D ;-)
        //     draw as much lines as possible in one pass ...
        glBegin(GL_LINE_STRIP);
            glVertex3fv(theLBF.begin());
            glVertex3fv(theLTF.begin());
            glColor4fv(theBackColor.begin());
            glVertex3fv(theLTBK.begin());
            glVertex3fv(theRTBK.begin());
            glVertex3fv(theRBBK.begin());
            glVertex3fv(theLBBK.begin());
            glColor4fv(theFrontColor.begin());
            glVertex3fv(theLBF.begin());
            glVertex3fv(theRBF.begin());
            glVertex3fv(theRTF.begin());
            glColor4fv(theBackColor.begin());
            glVertex3fv(theRTBK.begin());
        glEnd();

        // ... fill in the remaining lines
        glBegin(GL_LINES);
            glColor4fv(theFrontColor.begin());
            glVertex3fv(theLTF.begin());
            glVertex3fv(theRTF.begin());
            glVertex3fv(theRBF.begin());

            glColor4fv(theBackColor.begin());
            glVertex3fv(theRBBK.begin());
            glVertex3fv(theLBBK.begin());
            glVertex3fv(theLTBK.begin());
        glEnd();
        glPopAttrib();
    }

    void
    Renderer::enableRenderStyles(const RenderStyles & theRenderStyles, const MaterialBase * theMaterial) {
        MAKE_GL_SCOPE_TIMER(Renderer_enableRenderStyles);
        //AC_WARNING << "Renderstyle for " << theMaterial->get<IdTag>() << " is " << theRenderStyles;
        _myState->setIgnoreDepth(theRenderStyles[IGNORE_DEPTH]);
        _myState->setPolygonOffset( theRenderStyles[POLYGON_OFFSET]);

        if (theRenderStyles[NO_DEPTH_WRITES]) {
            _myState->setDepthWrites(false);
        } else {
            if (theMaterial) {
                MaterialPropertiesFacadePtr myMaterialPropFacade = theMaterial->getChild<MaterialPropertiesTag>();
                const TargetBuffers & myMasks = myMaterialPropFacade->get<TargetBuffersTag>();
                _myState->setDepthWrites(myMasks[DEPTH_MASK]);

                // depth test
                dom::NodePtr myDepthTestProp = myMaterialPropFacade->getProperty(DEPTHTEST_PROPERTY);
                if (myDepthTestProp) {
                    bool myDepthTest = myDepthTestProp->nodeValueAs<bool>();
                    if (myDepthTest) {
                        glEnable(GL_DEPTH_TEST);
                    } else {
                        glDisable(GL_DEPTH_TEST);
                    }
                } else {
                    if (_myState->getIgnoreDepth()) {
                        glDisable(GL_DEPTH_TEST);
                    } else {
                        glEnable(GL_DEPTH_TEST);
                    }
                }
                CHECK_OGL_ERROR;
            } else {
                _myState->setDepthWrites(true);
            }
        }
    }

    void Renderer::preDraw(const asl::Vector4f & theColor,
                           const asl::Matrix4f & theTransformation,
                           float theSize,
                           const std::string & theRenderStyles)
   {
        MAKE_GL_SCOPE_TIMER(Renderer_preDraw);
        std::istringstream myRenderStylesStream(theRenderStyles);
        RenderStyles myRenderStyles;
        myRenderStylesStream >> myRenderStyles;
        // don't use our _myState GL-State cache,
        // instead, bypass this cache and use
        // glPush/PopAttrib & glEnable directly
        // enableRenderStyles(myRenderStyles);


#if 1
        glPushAttrib(GL_COLOR_BUFFER_BIT | // color writemasks
                     GL_CURRENT_BIT |      // current RGBA color
                     GL_DEPTH_BUFFER_BIT | // depth mask
                     GL_ENABLE_BIT |       // depth test, lighting, texture_2d
                     GL_LINE_BIT |         // line width
                     GL_POLYGON_BIT);      // polygon offset
#else
        glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif
        // inside a glPushAttrib/glPopAttrib, don't use RenderState
        glDepthMask( myRenderStyles[NO_DEPTH_WRITES]);
        if (myRenderStyles[IGNORE_DEPTH]) {
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
        }
        if (myRenderStyles[POLYGON_OFFSET]) {
            glEnable(GL_POLYGON_OFFSET_POINT);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0, 1.0);
        } else {
            glDisable(GL_POLYGON_OFFSET_POINT);
            glDisable(GL_POLYGON_OFFSET_LINE);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.0, 0.0);
        }

        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        glColor4fv(theColor.begin());
        glLineWidth(theSize);
        glPointSize(theSize);

        glPushMatrix();
        glMultMatrixf(theTransformation.getData());
    }

    void Renderer::postDraw() {
        glPopMatrix();
        glPopAttrib();
        CHECK_OGL_ERROR;
    }

    template <>
    Y60_GLRENDER_DECL void Renderer::draw(const asl::Vector3f & thePoint,
                        const asl::Vector4f & theColor,
                        const asl::Matrix4f & theTransformation,
                        float theSize,
                        const std::string & theRenderStyles)
    {
        preDraw(theColor, theTransformation, theSize, theRenderStyles);

        glBegin(GL_POINTS);
            glVertex3fv(thePoint.begin());
        glEnd();

        postDraw();
    }

    template <>
    Y60_GLRENDER_DECL void Renderer::draw(const asl::LineSegment<float> & theLine,
                        const asl::Vector4f & theColor,
                        const asl::Matrix4f & theTransformation,
                        float theSize,
                        const std::string & theRenderStyles)
    {
        preDraw(theColor, theTransformation, theSize,theRenderStyles);

        glBegin(GL_LINES);
            glVertex3fv(theLine.origin.begin());
            glVertex3fv(theLine.end.begin());
        glEnd();

        postDraw();
    }

    template <>
    Y60_GLRENDER_DECL void Renderer::draw(const asl::Sphere<float> & theSphere,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation,
            float theSize,
            const std::string & theRenderStyles)
    {
        preDraw(theColor, theTransformation, theSize, theRenderStyles);

        const float TwoPI = float(asl::PI) * 2.0f;
        const unsigned mySegments = 32;
        const float myResolution = TwoPI / (float)mySegments;
        float myPos[mySegments][2];

        glTranslatef(theSphere.center[0], theSphere.center[1], theSphere.center[2]);
        glBegin(GL_LINE_LOOP);
        for (unsigned i = 0; i < mySegments; ++i) {
            float r = i * myResolution;
            myPos[i][0] = sin(r) * theSphere.radius;
            myPos[i][1] = cos(r) * theSphere.radius;
            glVertex3f(myPos[i][0], myPos[i][1], 0.0f);
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        for (unsigned i = 0; i < mySegments; ++i) {
            glVertex3f(0.0f, myPos[i][0], myPos[i][1]);
        }
        glEnd();
        glBegin(GL_LINE_LOOP);
        for (unsigned i = 0; i < mySegments; ++i) {
            glVertex3f(myPos[i][0], 0.0f, myPos[i][1]);
        }
        glEnd();

        postDraw();
    }

    template <>
    Y60_GLRENDER_DECL void Renderer::draw(const asl::Box3<float> & theBox,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation,
            float theSize,
            const std::string & theRenderStyles)
    {
        preDraw(theColor, theTransformation, theSize, theRenderStyles);

        Point3f myLTF, myRBF, myRTF, myLBF;
        Point3f myLTBK, myRBBK, myRTBK, myLBBK;

        theBox.getCorners(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK);
        renderBox(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK,
                  theColor);

        postDraw();
    }

    template <>
    Y60_GLRENDER_DECL void Renderer::draw(const asl::Triangle<float> & theTriangle,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation,
            float theSize,
            const std::string & theRenderStyles)
    {
        preDraw(theColor, theTransformation, theSize, theRenderStyles);

        glBegin(GL_TRIANGLES);
            glVertex3fv(theTriangle[0].begin());
            glVertex3fv(theTriangle[1].begin());
            glVertex3fv(theTriangle[2].begin());
        glEnd();

        postDraw();
    }

    template <>
    Y60_GLRENDER_DECL void Renderer::draw(const asl::BSpline<float> & theBSpline,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation,
            float theSize,
            const std::string & theRenderStyles)
    {
        preDraw(theColor, theTransformation, theSize, theRenderStyles);

        glBegin(GL_LINE_STRIP);
        unsigned myNumSegments = 16;
        for (unsigned i = 0; i <= myNumSegments; ++i) {
            const asl::Vector3f myPoint = theBSpline.evaluate(i / (float)myNumSegments);
            glVertex3fv(myPoint.begin());
        }
        glEnd();

        postDraw();
    }

    template <>
    Y60_GLRENDER_DECL void Renderer::draw(const asl::SvgPath & thePath,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation,
            float theSize,
            const std::string & theRenderStyles)
    {
        preDraw(theColor, theTransformation, theSize, theRenderStyles);

        glBegin(GL_LINE_STRIP);
        asl::Vector3f myLastPos;
        for (unsigned i = 0; i < thePath.getNumElements(); ++i) {
            const LineSegment3fPtr mySegment = thePath.getElement(i);
            if (i == 0 || !asl::almostEqual(mySegment->origin, myLastPos)) {
                // discontinuous
                if (i > 0) {
                    glEnd();
                    glBegin(GL_LINE_STRIP);
                }
                glVertex3fv(mySegment->origin.begin());
            }

            myLastPos = mySegment->end;
            glVertex3fv(myLastPos.begin());
        }
        glEnd();

        postDraw();
    }

    dom::NodePtr
    Renderer::getActiveLodChild(dom::NodePtr theNode, const CameraPtr theCamera) {
        MAKE_GL_SCOPE_TIMER(Renderer_getActiveLodChild);

        // get lod facade of given node
        const LodFacadePtr myLodFacade = theNode->getFacade<LodFacade>();
        if (!myLodFacade) {
            throw RendererException(string("Node with id: ") + theNode->getAttributeString(ID_ATTRIB)
                + " is not a lod node", PLUS_FILE_LINE);
        }

        // we always need the frustum
        const asl::Frustum & myFrustum = theCamera->get<FrustumTag>();

        // distance decision based on bounding box center
        const asl::Box3f & myBoundingBox = myLodFacade->get<BoundingBoxTag>();
        asl::Point3f myCenterPoint = myBoundingBox.getCenter();

        // compute distance to camera depending on projection type
        float myDistance;
        if(myFrustum.getType() == PERSPECTIVE) {
            // distance between bbox-center and camera
            asl::Vector3f myCenter(myCenterPoint[0], myCenterPoint[1], myCenterPoint[2]);
            myDistance = length(theCamera->get<PositionTag>() - myCenter);

            // compensate for field of view (90deg results in factor=1)
            float myZoomAdjustment = 0.0f;
            float myFOV = myFrustum.getHFov();
            if (myFOV < 180.0f) {
                myZoomAdjustment = float(tan(radFromDeg( myFOV / 2.0f)));
            }
            myDistance *= myZoomAdjustment;
        } else {
            // distance between bbox-center and nearest point on cam near plane
            const asl::Planef &myNearPlane = myFrustum.getNearPlane();
            asl::Point3f myNearestPoint = asl::nearest(myNearPlane, myCenterPoint);
            myDistance = asl::distance(myCenterPoint, myNearestPoint);
        }

        // apply global lod scale
        WorldPtr myWorld = myLodFacade->getWorld()->getFacade<World>();
        float myMetric = myDistance * myWorld->get<LodScaleTag>();

        // select child node to render
        const VectorOfFloat & myRanges = myLodFacade->get<RangesTag>();
        if (theNode->childNodesLength() == 1) {
            return theNode->childNode(0);
        } else {
            unsigned n = 0;
            for (unsigned i = 0 ; i < theNode->childNodesLength(); ++i) {
                if (theNode->childNode(i)->tryGetFacade<TransformHierarchyFacade>()) {
                    if (n >= myRanges.size() || myMetric < myRanges[n]) {
                        return theNode->childNode(i);
                    }
                    n++;
                }
            }
        }
        return dom::NodePtr();
    }
//#define DB(x) x
    void
    Renderer::createRenderList(const WorldPtr & theWorld,
    		                   const dom::NodePtr & theNode,
    		                   BodyPartMap & theBodyParts,
                               const CameraPtr theCamera,
                               const Matrix4f & theEyeSpaceTransform,
                               ViewportPtr theViewport,
                               bool theOverlapFrustumFlag,
                               std::vector<asl::Planef> theClippingPlanes,
                               asl::Box2f theScissorBox)
    {
        DB(AC_TRACE << "createRenderList: " << theNode->nodeName());
        TransformHierarchyFacadePtr myFacade;
        {
            DBP(MAKE_GL_SCOPE_TIMER(createRenderList_prologue));
            // Skip undefined nodes
            if (!theNode) {
                DB(AC_TRACE << "createRenderList: no node return");
                return;
            }

            // Skip non element nodes
            if (!(theNode->nodeType() == dom::Node::ELEMENT_NODE)) {
                DB(AC_TRACE << "createRenderList: no element return");
                return;
            }

            // Skip comments
            if (theNode->nodeName() == "#comment") {
                DB(AC_TRACE << "createRenderList: comment return");
                return;
            }

            myFacade = theNode->tryGetFacade<TransformHierarchyFacade>();

            // skip non-hierarchy nodes
            if (!myFacade) {
                DB(AC_TRACE << "createRenderList: no transform return");
                return;
            }

            // Skip invisible nodes
            if (!(myFacade->get<VisibleTag>())) {
                DB(AC_TRACE << "createRenderList: visible flag return";)
                return;
            }
        }
        // Once we collect a body we want to do billboarding, so every following step has the
        // right globalmatrix

        if (theNode->nodeName() == BODY_NODE_NAME) {
            Body & myBody = *(dynamic_cast_Ptr<Body>(myFacade));
            DBP(MAKE_GL_SCOPE_TIMER(realREnderlisting));
            // do the billboarding here
            if (myBody.get<BillboardTag>() == AXIS_BILLBOARD) {
                DBP(MAKE_GL_SCOPE_TIMER(update_billboards));
                Matrix4f myBillboardTransform = myBody.get<GlobalMatrixTag>();

                // calculate absolute scale of transform hierarchy:
                Vector3f myCurScale = myBody.get<ScaleTag>();
                const dom::Node* myCurNode = myBody.getNode().parentNode();
                while(myCurNode->nodeName() != WORLD_NODE_NAME) {
                    myCurScale *= myCurNode->getFacade<TransformHierarchyFacade>()->get<ScaleTag>();
                    myCurNode = myCurNode->parentNode();
                }
                // get rid of scale in billboards global matrix
                Vector3f myFinalInverseScale(1.0f/myCurScale[0], 1.0f/myCurScale[1], 1.0f/myCurScale[2]);
                Matrix4f myBillboardMatrix = myBody.get<GlobalMatrixTag>();
                myBillboardMatrix.scale(myFinalInverseScale);

                float myRotation = static_cast<float>(getBillboardRotationY(myBillboardMatrix,
                        theCamera->get<GlobalMatrixTag>()));
                Matrix4f myLocalRotation;
                myLocalRotation.makeRotating(Vector3f(0,1,0),myRotation);
                myLocalRotation.postMultiply(myBillboardTransform);
                myBody.set<GlobalMatrixTag>(myLocalRotation);
            } else if (myBody.get<BillboardTag>() == POINT_BILLBOARD) {
                // calculate absolute scale of transform hierarchy:
                Vector3f myCurScale = myBody.get<ScaleTag>();
                const dom::Node* myCurNode = myBody.getNode().parentNode();
                while(myCurNode->nodeName() != WORLD_NODE_NAME) {
                    myCurScale *= myCurNode->getFacade<TransformHierarchyFacade>()->get<ScaleTag>();
                    myCurNode = myCurNode->parentNode();
                }
                // get rid of scale in billboards global matrix
                Vector3f myFinalInverseScale(1.0f/myCurScale[0], 1.0f/myCurScale[1], 1.0f/myCurScale[2]);
                Matrix4f myBillboardMatrix = myBody.get<GlobalMatrixTag>();
                myBillboardMatrix.scale(myFinalInverseScale);

                // transform camera into billboard space
                myBillboardMatrix.invert();
                Matrix4f myCameraMatrix = theCamera->get<GlobalMatrixTag>();
                myCameraMatrix.postMultiply(myBillboardMatrix);

                // calculate billboard transform matrix
                Vector4f myCamUpVector    = myCameraMatrix.getRow(1);
                Vector3f myCamUpVector3(myCamUpVector[0], myCamUpVector[1], myCamUpVector[2]);
                Vector4f myCamViewVector  = myCameraMatrix.getRow(2);
                Vector3f myCamViewVector3(myCamViewVector[0], myCamViewVector[1], myCamViewVector[2]);
                Vector3f myRightVec       = cross(myCamUpVector3, myCamViewVector3);

                Matrix4f myScreenAlignedMatrix;
                myScreenAlignedMatrix.assign(myRightVec[0], myRightVec[1], myRightVec[2], 0,
                        myCamUpVector[0],myCamUpVector[1],myCamUpVector[2], 0,
                        myCamViewVector[0],myCamViewVector[1],myCamViewVector[2], 0,
                        0,0,0,1, ROTATING);

                Matrix4f myBillboardTransform = myBody.get<GlobalMatrixTag>();
                myScreenAlignedMatrix.postMultiply(myBillboardTransform);

                myBody.set<GlobalMatrixTag>(myScreenAlignedMatrix);
            }

        }

        // Check culling
        bool myOverlapFrustumFlag = true;
        const Frustum & myFrustum = theCamera->get<FrustumTag>();
        {

            DBP(MAKE_GL_SCOPE_TIMER(createRenderList_cull));
            if (theOverlapFrustumFlag && theViewport->get<ViewportCullingTag>() && myFacade->get<CullableTag>()) {
                if (!intersection(myFacade->get<BoundingBoxTag>(), myFrustum, myOverlapFrustumFlag)) {
                    DB(AC_TRACE << "createRenderList: bbox=" << myFacade->get<BoundingBoxTag>();)
                    DB(AC_TRACE << "createRenderList: Frustum=" << myFrustum;)
                    DB(AC_TRACE << "createRenderList: cull return";)
                    return;
                }
            }
        }

        // Collect clipping planes and scissoring
        {
            DBP(MAKE_GL_SCOPE_TIMER(createRenderList_collectClippingScissor));
            collectClippingPlanes(theNode, theClippingPlanes);
            collectScissorBox(theNode, theScissorBox);
        }

        myFacade->set<LastActiveFrameTag>(_myFrameNumber);

        // Check for lodding
        if (theNode->nodeName() == LOD_NODE_NAME) {
            DBP(MAKE_GL_SCOPE_TIMER(createRenderList_lod));
            createRenderList(theWorld, getActiveLodChild(theNode, theCamera), theBodyParts,
                    theCamera, theEyeSpaceTransform, theViewport, theOverlapFrustumFlag,
                    theClippingPlanes, theScissorBox);
            DB(AC_TRACE << "createRenderList: lod return";)
            return;
        }

        // Add remaining bodies to render list
        if (theNode->nodeName() == BODY_NODE_NAME) {
            DB(AC_TRACE << "createRenderList: body processing";)
            DBP(MAKE_GL_SCOPE_TIMER(createRenderList_insertBody));
            DBP2(START_TIMER(createRenderList_remainingBodies));
            const Body & myBody = *(dynamic_cast_Ptr<Body>(myFacade));

            // Split the body in bodyparts to make material sorted rendering possible
            const Shape & myShape = myBody.getShape();
            const_cast<Shape&>(myShape).set<LastActiveFrameTag>(_myFrameNumber);
            const y60::PrimitiveVector & myPrimitives = myShape.getPrimitives();

            Matrix4f myTransform = myBody.get<GlobalMatrixTag>();
            myTransform.postMultiply(theEyeSpaceTransform);
            double myFarPlane = myFrustum.getFar();
            double myNearPlane = myFrustum.getNear();

            unsigned mySize = myPrimitives.size();
            DB(AC_TRACE << "createRenderList: primitives="<<mySize;)
            for (unsigned i = 0; i < mySize; ++i) {
                const Primitive & myPrimitive = (*myPrimitives[i]);
                const MaterialBase & myMaterial = myPrimitive.getMaterial();
                const_cast<MaterialBase&>(myMaterial).set<LastActiveFrameTag>(_myFrameNumber);
                asl::Unsigned16 myBodyKey = makeBodyKey(myMaterial,
                                                        myShape,
                                                        myTransform,
                                                        myNearPlane,
                                                        myFarPlane);
                BodyPart::Key myKey(&myMaterial, &myBody, myBodyKey);

                if (myMaterial.get<TransparencyTag>()) {
                    COUNT(TransparentPrimitives);
                } else {
                    COUNT(OpaquePrimitives);
                }

                theBodyParts.insert(std::make_pair(myKey, BodyPart(*(theWorld),myBody, myShape, myPrimitive, theClippingPlanes, theScissorBox)));
            }
            DBP2(STOP_TIMER(createRenderList_remainingBodies));

            COUNT(RenderedBodies);
        }

        {
            DBP2(START_TIMER(createRenderList_nowCREATING));
            DBP(MAKE_GL_SCOPE_TIMER(createRenderList_recurse));
            for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
                createRenderList(theWorld, theNode->childNode(i), theBodyParts, theCamera,
                        theEyeSpaceTransform, theViewport, myOverlapFrustumFlag,
                        theClippingPlanes, theScissorBox);
            }
            DBP2(STOP_TIMER(createRenderList_nowCREATING));
        }
        DB(AC_TRACE << "createRenderList: end of function return";)
    }
//#undef DB
//#define DB(x) // x

    void
    Renderer::collectClippingPlanes(dom::NodePtr theNode,
                                    std::vector<asl::Planef> & theClippingPlanes)
    {
        TransformHierarchyFacadePtr myFacade = theNode->getFacade<TransformHierarchyFacade>();

        const VectorOfString & myPlaneIds = myFacade->get<ClippingPlanesTag>();
        if (myPlaneIds.size() == 0) {
            return;
        }

        dom::NodePtr myPlaneNode;
        for (unsigned i = 0; i < myPlaneIds.size(); ++i) {
            myPlaneNode = theNode->getElementById( myPlaneIds[i] );
            if (!myPlaneNode) {
                throw RendererException(string("Can not find geometry '")+myPlaneIds[i]+
                    "' for node " + theNode->nodeName() + " with id '" +
                    theNode->getAttributeString(ID_ATTRIB)+ "' named '" + theNode->getAttributeString(NAME_ATTRIB) + "'!", PLUS_FILE_LINE);
            }
            PlanePtr myPlane = myPlaneNode->getFacade<Plane>();
            myPlane->set<LastActiveFrameTag>(_myFrameNumber);
            theClippingPlanes.push_back( myPlane->get<GlobalPlaneTag>());
        }
    }

    void
    Renderer::collectScissorBox(dom::NodePtr theNode,
                                asl::Box2f & theScissorBox)
    {
        TransformHierarchyFacadePtr myFacade = theNode->getFacade<TransformHierarchyFacade>();

        const string & myScissorId = myFacade->get<ScissorTag>();
        if (myScissorId == "") {
            return;
        }

        dom::NodePtr myBoxNode = theNode->getElementById(myScissorId);
        if (myBoxNode) {
            const asl::Box2f & myBox = myBoxNode->childNode("#text")->nodeValueAs<asl::Box2f>();
            theScissorBox.intersect(myBox);
        }
    }
    void
    Renderer::clearBuffers(const CanvasPtr & theCanvas, unsigned int theBuffersMask) {
        MAKE_GL_SCOPE_TIMER(Renderer_clearBuffers);
        // called once per canvas per frame
        const asl::Vector4f & backgroundColor = theCanvas->get<CanvasBackgroundColorTag>();
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glClear( theBuffersMask );
    }

    void
    Renderer::setupRenderState(ViewportPtr theViewport) {
        MAKE_GL_SCOPE_TIMER(Renderer_setupRenderState);
        _myState->setWireframe(theViewport->get<ViewportWireframeTag>());
        _myState->setFlatShading(theViewport->get<ViewportFlatshadingTag>());
        _myState->setLighting(theViewport->get<ViewportLightingTag>());
        _myState->setBackfaceCulling(theViewport->get<ViewportBackfaceCullingTag>());
        _myState->setTexturing(theViewport->get<ViewportTexturingTag>());
        _myState->setDepthWrites(true);
        _myState->setFrontFaceCCW(true);

        // This prevents translucent pixels from being drawn. This way the
        // background can shine through.
        if (!theViewport->get<ViewportAlphaTestTag>()
            || theViewport->get<ViewportDrawGlowTag>())
        {
            glDisable(GL_ALPHA_TEST);
        } else {
            glAlphaFunc(GL_GREATER, 0.0);
            glEnable(GL_ALPHA_TEST);
        }

        //glColor4f(1,1,1,1);
        CHECK_OGL_ERROR;
    }

    // called once per Canvas per Frame
    void
    Renderer::render(ViewportPtr theViewport) {
        AC_TRACE << "Rendering:" << theViewport->getNode();
        MAKE_GL_SCOPE_TIMER(render);
        _myRenderedUnderlays = false;
        ++_myFrameNumber;

        // Setup viewport, parameters are in screen space
        glViewport(theViewport->get<ViewportLeftTag>(), theViewport->getLower(),
                   theViewport->get<ViewportWidthTag>(), theViewport->get<ViewportHeightTag>());
        CHECK_OGL_ERROR;

        setupRenderState(theViewport);

        // We need to push all thouse bits, that need to be reset after the main render pass (before
        // text and overlay rendering). But not thouse that are managed by the renderstate class.
        glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);
        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

        // Render underlays
        {
            MAKE_GL_SCOPE_TIMER(renderUnderlays);
            renderOverlays(*theViewport, UNDERLAY_LIST_NAME);
        }

        // if we have a camera, render the world
        dom::NodePtr myWorld; // XXX: referenced by bounding box render
        const std::string & myCameraId =theViewport->get<CameraTag>();
        if ( myCameraId.length() ) {
            dom::NodePtr myCameraNode = theViewport->getNode().getElementById(myCameraId);
            if (!myCameraNode) {
                throw RendererException(string("Can not find camera '")+theViewport->get<CameraTag>()+
                        "' for viewport '"+theViewport->get<NameTag>()+"' id="+theViewport->get<IdTag>(), PLUS_FILE_LINE);
            }
            CameraPtr myCamera = myCameraNode->getFacade<Camera>();
            bindViewMatrix(myCamera);
            CHECK_OGL_ERROR;

            myWorld = myCamera->getWorld();
            WorldPtr myWorldFacade = myWorld->getFacade<World>();

            if (theViewport->get<ViewportCullingTag>()) {
                if (theViewport->get<ViewportDebugCullingTag>()) {
                    renderFrustum( myCameraNode );
                } else {
                    //theViewport->updateClippingPlanes();
                    // XXX Good place to do the aspect correction
                    theViewport->applyAspectToCamera();
                }
            }

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            CHECK_OGL_ERROR;

            setProjection(theViewport, myCamera);
            CHECK_OGL_ERROR;

            // don't render anything if world isn't visible
            if (myWorld->getFacade<TransformHierarchyFacade>()->get<VisibleTag>()) {

                // (2) Create lists of render objects
                BodyPartMap myBodyParts;
                {
                    MAKE_GL_SCOPE_TIMER(createRenderList);
                    Matrix4f myEyeSpaceTransform = myCamera->get<InverseGlobalMatrixTag>();
                    asl::Box2f myScissorBox;
                    myScissorBox.makeFull();
                    createRenderList(myWorldFacade, myWorld, myBodyParts, myCamera,
                            myEyeSpaceTransform, theViewport, true, std::vector<asl::Planef>(),
                            myScissorBox);
                    DB(AC_TRACE << "created Renderlist, size = "<< myBodyParts.size());
                }

                // (3) render skybox
                    renderSkyBox(*theViewport, myCamera);
                    CHECK_OGL_ERROR;

                // (4) Setup camera
                bindViewMatrix(myCamera);

                // (5) activate all visible lights
                    enableVisibleLights(myWorldFacade);
                    CHECK_OGL_ERROR;

                // (6) enable fog

                    enableFog(myWorldFacade);
                    CHECK_OGL_ERROR;

                // (7) render bodies
                if (! myBodyParts.empty()) {

                	DBP2(START_TIMER(render_renderBodyParts));
                    MAKE_GL_SCOPE_TIMER(renderBodyParts);
                    _myPreviousBody = 0;

                    glPushMatrix();
                    glDisable(GL_ALPHA_TEST);
                    CHECK_OGL_ERROR;

                    //int i = 0;
                    bool currentMaterialHasAlpha = false;
                    DBP2(START_TIMER(render_renderBodyParts_Iterator));

                    for (BodyPartMap::const_iterator it = myBodyParts.begin(); it != myBodyParts.end(); ++it) {
                        if (theViewport->get<ViewportAlphaTestTag>()
                            && !currentMaterialHasAlpha && it->first.getTransparencyFlag())
                        {
                        	DBP2(START_TIMER(render_renderBodyParts_enableAlpha));
                            glEnable(GL_ALPHA_TEST);
                            currentMaterialHasAlpha = true;
                            DBP2(STOP_TIMER(render_renderBodyParts_enableAlpha));
                        }
                        DBP2(START_TIMER(render_renderBodyParts_renderBodyPart));
                        renderBodyPart(it->second, *theViewport, *myCamera);
                        DBP2(STOP_TIMER(render_renderBodyParts_renderBodyPart));

                    }
                    DBP2(STOP_TIMER(render_renderBodyParts_Iterator));
                    glPopMatrix();
                    CHECK_OGL_ERROR;

                    _myState->setScissorTest(false);
                    _myState->setClippingPlanes(std::vector<asl::Planef>());
                    _myState->setFrontFaceCCW(true);
                    DBP2(STOP_TIMER(render_renderBodyParts));
                }

                // turn fog off again
                glDisable(GL_FOG);

                // disable lights again
                disableAllLights();

                // (8) render analytic geometry
                renderAnalyticGeometry( theViewport, myCamera );
            }
        }


        {
            MAKE_GL_SCOPE_TIMER(renderOverlays);
            renderOverlays(*theViewport, OVERLAY_LIST_NAME);
        }


        glPopClientAttrib();
        glPopAttrib();  // GL_TEXTURE_BIT + GL_COLOR_BUFFER_BIT

        // Set renderer into known state for drawing calls from js
        deactivatePreviousMaterial();
        _myPreviousMaterial = 0;
        _myState->setDepthWrites(true);
        _myState->setIgnoreDepth(false);

        if (_myBoundingVolumeMode & BV_HIERARCHY) {
            renderBoundingBoxHierarchy(myWorld);
        }

        _myTextRendererManager.render(theViewport);
    }

    void
    Renderer::setProjection(ViewportPtr theViewport, CameraPtr theCamera) {
        glMatrixMode(GL_PROJECTION);
        const Frustum & myFrustum = theCamera->get<FrustumTag>();
        Matrix4f myProjectionMatrix = myFrustum.getProjectionMatrix();
        DB(AC_TRACE << "setting proj matrix to " << myProjectionMatrix << endl);
        //AC_PRINT << "Renderer::setProjection(): setting frustum: " << myFrustum << endl;
        //AC_PRINT << "Renderer::setProjection(): setting proj matrix to " << myProjectionMatrix << endl;
        glLoadMatrixf(static_cast<const GLfloat *>(myProjectionMatrix.getData()));
        if (theViewport->get<ViewportOrientationTag>()
            == PORTRAIT_ORIENTATION)
        {
            asl::Matrix4f myRotationMatrix;
            myRotationMatrix.makeZRotating(-float(asl::PI_2));
            glMultMatrixf(static_cast<const GLfloat *>(myRotationMatrix.getData()));
        }
        glMatrixMode(GL_MODELVIEW);
    }

    void
    Renderer::setBoundingVolumeMode(unsigned short theMode) {
        _myBoundingVolumeMode = theMode;
    }

    unsigned short
    Renderer::getBoundingVolumeMode() const {
        return _myBoundingVolumeMode;
    }

    void
    Renderer::bindViewMatrix(CameraPtr theCamera) {
#if 0
        if (AC_TRACE_ON) {
            theCamera->debug<InverseGlobalMatrixTag>();
            theCamera->debug<PositionTag>();
        }
#endif
        glLoadMatrixf(static_cast<const GLfloat *>(theCamera->get<InverseGlobalMatrixTag>().getData()));
    }

    GLenum
    Renderer::getPrimitiveGLType(y60::PrimitiveType theType) const {
        return GLenum(theType);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Light handling and Fog
    ///////////////////////////////////////////////////////////////////////////////////////////

    void
    Renderer::disableAllLights() {
        MAKE_GL_SCOPE_TIMER(disableAllLights);
        static GLint myMaxLights = 0;
        if (myMaxLights == 0) {
            glGetIntegerv(GL_MAX_LIGHTS, &myMaxLights);
        }

        // Disable remaining lights
        for (GLint i = 0; i < myMaxLights; ++i) {
            glDisable(asGLLightEnum(i));
        }
    }

    void
    Renderer::enableVisibleLights(WorldPtr & theWorld) {
        MAKE_GL_SCOPE_TIMER(enableVisibleLights);
        static GLint myMaxLights = 0;
        if (myMaxLights == 0) {
            glGetIntegerv(GL_MAX_LIGHTS, &myMaxLights);
        }

        int myActiveLightCount = 0;
        const LightVector & myLights = theWorld->getLights();
        DB(AC_TRACE << "enableVisibleLights: numlights=" << myLights.size());
        for (unsigned i = 0; i < myLights.size(); ++i) {
            DB(AC_TRACE << myLights[i]->getNode());
            if (myLights[i]->get<VisibleTag>()) {
                if (myActiveLightCount >= myMaxLights) {
                    static bool myAlreadyWarned = false;
                    if (!myAlreadyWarned) {
                        AC_WARNING << "Max. " << int(myMaxLights) << " hardware light sources supported by OpenGL; active=" << myLights.size();
                        myAlreadyWarned = true;
                    }
                    break;
                }
                enableLight(myLights[i], myActiveLightCount);
                DB(AC_TRACE << "enableVisibleLights: " << myLights[i]->getNode().getAttributeString("name"));
                myActiveLightCount++;
            }
        }

        // Disable remaining lights
        for (GLint i = myActiveLightCount; i < myMaxLights; ++i) {
            glDisable(asGLLightEnum(i));
        }

        COUNT_N(ActiveLights, myActiveLightCount);
    }

    void
    Renderer::enableLight(const y60::LightPtr & theLight, int theActiveLightIndex) {
        LightSourcePtr myLightSource = theLight->getLightSource();
		LightPropertiesFacadePtr myLightPropFacade = myLightSource->getChild<LightPropertiesTag>();

        LightSourceType myType = myLightSource->getType();
        switch (myType) {
            case DIRECTIONAL:
            case POSITIONAL:
            case SPOT:
                break;
	        case AMBIENT:
		        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, myLightPropFacade->get<LightAmbientTag>().begin());
                return;
            case UNSUPPORTED:
                return;
            case MAX_LIGHTSOURCE_TYPE:
                AC_ERROR << "internal error: MAX_LIGHTSOURCE_TYPE encountered as enumeration value";
                break;
        }

        GLenum gl_lightid = asGLLightEnum(theActiveLightIndex);
        asl::Vector4f myGLLightPos;

        float mySpotCutoff = 180.0f;
        float mySpotExponent = 0.0f;
        float myConstantAttenuation = 1.0f;
        float myQuadraticAttenuation = 0.0f;
        float myLinearAttenuation = 0.0f;
        asl::Vector3f myDirection = Vector3f(0.0, 0.0, -1.0);

        switch (myType) {
            case DIRECTIONAL: {
                // GL directional lights: xyz is (0,0,1) rotated by orientation, w=0.0
                asl::QuadrupleOf<float> myLightDirection = theLight->get<GlobalMatrixTag>().getRow(2);
                myGLLightPos = asl::Vector4f(myLightDirection[0], myLightDirection[1], myLightDirection[2], 0.0f);
                DB(AC_TRACE << "enabled direction light: " << theLight->getNode().getAttributeString("name"));
                break;
            }
            case POSITIONAL:
            {
                // GL positional lights: xyz = light position, w=1.0
                asl::Vector3f myLightTranslation = theLight->get<GlobalMatrixTag>().getTranslation();
                myGLLightPos = asl::Vector4f(myLightTranslation[0], myLightTranslation[1], myLightTranslation[2],1.0f);
                myLinearAttenuation = myLightPropFacade->get<AttenuationTag>();
                DB(AC_TRACE << "enabled positional light: " << theLight->getNode().getAttributeString("name"));
                break;
            }
            case SPOT:
            {
                mySpotCutoff = myLightPropFacade->get<CutOffTag>();
                mySpotExponent = myLightPropFacade->get<ExponentTag>();

                // get the Z-Axis to set the light direction
                asl::QuadrupleOf<float> myTmpVec = theLight->get<GlobalMatrixTag>().getRow(2); // Z-axis
                asl::Vector3f myLightDirection(myTmpVec.begin());
                myDirection = asl::product(myLightDirection, -1.0f);

                asl::Vector3f myLightTranslation = theLight->get<GlobalMatrixTag>().getTranslation();
                myGLLightPos = asl::Vector4f(myLightTranslation[0], myLightTranslation[1], myLightTranslation[2],1.0f);
                DB(AC_TRACE << "enabled spotlight: " << theLight->getNode().getAttributeString("name"));
                break;
            }
            // avoid warnings
            case AMBIENT:
            case UNSUPPORTED:
            case MAX_LIGHTSOURCE_TYPE:
                break;
        }

        glLightf(gl_lightid, GL_SPOT_CUTOFF, mySpotCutoff); // 180
        glLightf(gl_lightid, GL_SPOT_EXPONENT, mySpotExponent); // 0
        glLightf(gl_lightid, GL_CONSTANT_ATTENUATION, myConstantAttenuation); // 1
        glLightf(gl_lightid, GL_QUADRATIC_ATTENUATION, myQuadraticAttenuation); // 0
        glLightf(gl_lightid, GL_LINEAR_ATTENUATION, myLinearAttenuation); // 0
        glLightfv(gl_lightid, GL_SPOT_DIRECTION, &(myDirection[0])); // 0.0, 0.0, -1.0

        glLightfv(gl_lightid, GL_POSITION, asl::begin_ptr(myGLLightPos));
        glLightfv(gl_lightid, GL_AMBIENT,  myLightPropFacade->get<LightAmbientTag>().begin());
        glLightfv(gl_lightid, GL_DIFFUSE,  myLightPropFacade->get<LightDiffuseTag>().begin());
        glLightfv(gl_lightid, GL_SPECULAR, myLightPropFacade->get<LightSpecularTag>().begin());
        glEnable(gl_lightid);
    }

    void
    Renderer::enableFog(WorldPtr & theWorld) {
        MAKE_GL_SCOPE_TIMER(Renderer_enableFog);
        const string & myFogModeString = theWorld->get<FogModeTag>();
        if (myFogModeString.size() == 0) {
            return;
        }
        FogMode myFogMode = static_cast<FogMode>(getEnumFromString(myFogModeString, FogModeStrings));
        switch (myFogMode) {
            case FOG_OFF :
                glDisable(GL_FOG);
                return;
            case FOG_LINEAR :
                glFogi(GL_FOG_MODE, GL_LINEAR);
                glFogf(GL_FOG_START, theWorld->get<FogRangeTag>()[0]);
                glFogf(GL_FOG_END, theWorld->get<FogRangeTag>()[1]);
                break;
            case FOG_EXP :
                glFogi(GL_FOG_MODE, GL_EXP);
                glFogf(GL_FOG_DENSITY, theWorld->get<FogDensityTag>());
                break;
            case FOG_EXP2 :
                glFogi(GL_FOG_MODE, GL_EXP2);
                glFogf(GL_FOG_DENSITY, theWorld->get<FogDensityTag>());
                break;
            default :
                throw RendererException(string("World Fog Mode :'") + theWorld->get<FogModeTag>()+
                            "' unknown", PLUS_FILE_LINE);
        }
        glFogfv(GL_FOG_COLOR, &(theWorld->get<FogColorTag>()[0]));
        glHint(GL_FOG_HINT, GL_DONT_CARE);
        glEnable(GL_FOG);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Billboard handling
    ///////////////////////////////////////////////////////////////////////////////////////////

    double
    Renderer::getBillboardRotationY(const asl::Matrix4f & theBillboardMatrix,
                                   const asl::Matrix4f & theCameraMatrix)
    {
        asl::Matrix4f myBillboardMatrix = theBillboardMatrix;
        myBillboardMatrix.invert();

        // Transform the camera matrix into local billboard coordinates
        asl::Matrix4f myCameraMatrix = theCameraMatrix;
        myCameraMatrix.postMultiply(myBillboardMatrix);

        asl::Vector4f myViewVector4 =  myCameraMatrix.getRow(2);
        asl::Vector3f myViewVector(myViewVector4[0], myViewVector4[1], myViewVector4[2]);

        // calc y-axis rotation
        if (myViewVector[2] > 0.000001) {
            return atan(myViewVector[0] / myViewVector[2]);
        } else if (myViewVector[2] < -0.000001) {
            return atan(myViewVector[0] / myViewVector[2]) + asl::PI;
        } else {
            if (myViewVector[0] > 0) {
                return asl::PI/2;
            } else if (myViewVector[0] < 0) {
                return -asl::PI / 2;
            }
        }

        return 0.0;
    }

    void
    Renderer::renderSkyBox(const Viewport & theViewport, CameraPtr theCamera) {
        MAKE_GL_SCOPE_TIMER(renderSkyBox);
        if (_myRenderedUnderlays) {
            return;
        }

        // Get Material
        const dom::NodePtr myWorldNode = theCamera->getWorld();
        WorldPtr myWorld = myWorldNode->getFacade<World>();

        if (myWorld->get<SkyBoxMaterialTag>().size() == 0) {
            return;
        }

        std::string myMaterialId = myWorld->get<SkyBoxMaterialTag>();
        dom::NodePtr myMaterialNode = _myScene->getMaterialsRoot()->getElementById(myMaterialId);
        if (!myMaterialNode) {
            AC_ERROR << "Could not find SkyBox material: " << myMaterialId << endl;
            return;
        }

        MaterialBasePtr myMaterial = myMaterialNode->getFacade<MaterialBase>();
        //MaterialBasePtr myMaterial = _myScene->getMaterial(myMaterialId);
        if (!myMaterial) {
            AC_ERROR << "Could not find SkyBox material: " << myMaterialId << endl;
            return;
        }

        // Clear everything but the rotational part
        asl::Matrix4f myModelView = theCamera->get<InverseGlobalMatrixTag>();
        myModelView[0][3] = 0; myModelView[1][3] = 0; myModelView[2][3] = 0;
        myModelView[3][0] = 0; myModelView[3][1] = 0; myModelView[3][2] = 0; myModelView[3][3] = 1;

        glPushMatrix();
        glLoadMatrixf(static_cast<const GLfloat *>(myModelView.getData()));

        switchMaterial(theViewport, *myMaterial);

        // We'll just index into the cube map directly to render the cubic panorama
        GLfloat rgba[4] = { 1.0, 1.0, 1.0, 1.0 };
        glColor4fv(rgba);

        const float mySize = theCamera->get<FrustumTag>().getFar() / 2;
        DB(AC_TRACE << "drawing skybox. Size=" << mySize << endl;);
        //glDepthMask(GL_FALSE);
        _myState->setDepthWrites(false);
        //glDisable(GL_DEPTH_TEST);
        _myState->setIgnoreDepth(true);

        const int zDir = -1;

        glBegin(GL_QUAD_STRIP);
            //-z:
            glTexCoord3f(-1, 1, -1 * zDir);
            glVertex3f  (-mySize, mySize, -mySize);

            glTexCoord3f(-1, -1, -1 * zDir);
            glVertex3f  (-mySize, -mySize, -mySize);

            glTexCoord3f(1, 1, -1 * zDir);
            glVertex3f  (mySize, mySize, -mySize);

            glTexCoord3f(1, -1, -1 * zDir);
            glVertex3f  (mySize, -mySize, -mySize);

            //+x:
            glTexCoord3f(1, 1, 1 * zDir);
            glVertex3f  (mySize, mySize, mySize);

            glTexCoord3f(1, -1, 1 * zDir);
            glVertex3f  (mySize, -mySize, mySize);

            //+z:
            glTexCoord3f(-1, 1, 1 * zDir);
            glVertex3f  (-mySize, mySize, mySize);

            glTexCoord3f(-1, -1, 1 * zDir);
            glVertex3f  (-mySize, -mySize, mySize);

            //-x:
            glTexCoord3f(-1, 1, -1 * zDir);
            glVertex3f  (-mySize, mySize, -mySize);

            glTexCoord3f(-1, -1, -1 * zDir);
            glVertex3f  (-mySize, -mySize, -mySize);
        glEnd();

        glBegin(GL_QUADS);
            //+y:
            glTexCoord3f(-1, 1, -1 * zDir);
            glVertex3f  (-mySize, mySize, -mySize);

            glTexCoord3f(1, 1, -1 * zDir);
            glVertex3f  (mySize, mySize, -mySize);

            glTexCoord3f(1, 1, 1 * zDir);
            glVertex3f  (mySize, mySize, mySize);

            glTexCoord3f(-1, 1, 1 * zDir);
            glVertex3f  (-mySize, mySize, mySize);

            //-y:
            glTexCoord3f(-1, -1, 1 * zDir);
            glVertex3f  (-mySize, -mySize, mySize);

            glTexCoord3f(1, -1, 1 * zDir);
            glVertex3f  (mySize, -mySize, mySize);

            glTexCoord3f(1, -1, -1 * zDir);
            glVertex3f  (mySize, -mySize, -mySize);

            glTexCoord3f(-1, -1, -1 * zDir);
            glVertex3f  (-mySize, -mySize, -mySize);
        glEnd();

        glPopMatrix();
        //glDepthMask(GL_TRUE);
        //glEnable(GL_DEPTH_TEST);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Overlay handling
    ///////////////////////////////////////////////////////////////////////////////////////////

    void
    Renderer::renderOverlays(const Viewport & theViewport, const std::string & theRootNodeName)
    {
        dom::NodePtr myOverlays = theViewport.getNode().childNode(theRootNodeName);
        if (!myOverlays) {
            return;
        }

        unsigned myOverlayCount = myOverlays->childNodesLength(OVERLAY_NODE_NAME);
        if (myOverlayCount == 0) {
            return;
        }

        glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
        //glPushAttrib(GL_ALL_ATTRIB_BITS);

        _myState->setBackfaceCulling( false );
        CHECK_OGL_ERROR;

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        CHECK_OGL_ERROR;

        // UH: projection matrix is relative to viewport so no translation is necessary
        // (already done in glViewport)
        gluOrtho2D(0.0, theViewport.get<ViewportWidthTag>(),
                   theViewport.get<ViewportHeightTag>(), 0.0);
        CHECK_OGL_ERROR;

        if (theViewport.get<ViewportOrientationTag>() == PORTRAIT_ORIENTATION) {
            asl::Matrix4f myRotationMatrix;
            myRotationMatrix.makeZRotating(float(asl::PI_2));
            myRotationMatrix.translate(asl::Vector3f(float(theViewport.get<ViewportWidthTag>()), 0.0f, 0.0f));
            glMultMatrixf(static_cast<const GLfloat *>(myRotationMatrix.getData()));
            CHECK_OGL_ERROR;
        }

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        CHECK_OGL_ERROR;

        // don't force depth-buffer deactivation here - it will be done by the material
        //_myState->setIgnoreDepth(true);
        //_myState->setDepthWrites(false);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECK_OGL_ERROR;

        for (unsigned i = 0; i < myOverlayCount; ++i) {
            renderOverlay(theViewport, myOverlays->childNode(OVERLAY_NODE_NAME, i));
        }

        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);

        glPopAttrib();

        _myState->setBackfaceCulling(theViewport.get<ViewportBackfaceCullingTag>());

        _myRenderedUnderlays = true;
    }

    void
    Renderer::renderOverlay(const Viewport & theViewport, dom::NodePtr theOverlayNode, float theAlpha) {
        MAKE_GL_SCOPE_TIMER(renderOverlay);

        const y60::Overlay & myOverlay = *(theOverlayNode->getFacade<y60::Overlay>());
        if (myOverlay.get<VisibleTag>() == false) {
            return;
        }

        float myAlpha  = theAlpha * myOverlay.get<AlphaTag>();
        if (myAlpha <= 0.0f) {
            return;
        }

        // do the transformation
        glPushMatrix();

        const asl::Vector2f & myPosition = myOverlay.get<Position2DTag>();
        glTranslatef(myPosition[0], myPosition[1], 0);

        float myOverlayRotation2d = myOverlay.get<Rotation2DTag>();
        if (!asl::almostEqual(myOverlayRotation2d, 0.0) ) {
            // rotate around pivot
            const asl::Vector2f & myPivot = myOverlay.get<Pivot2DTag>();
            glTranslatef(myPivot[0], myPivot[1], 0.0f);
            glRotatef(float(degFromRad(myOverlayRotation2d)), 0.0f,0.0f,1.0f);
            glTranslatef(-myPivot[0], -myPivot[1], 0.0f);
        }

        const asl::Vector2f & myScale = myOverlay.get<Scale2DTag>();
        glScalef(myScale[0], myScale[1], 1.0f);

        float myWidth  = myOverlay.get<WidthTag>();
        float myHeight = myOverlay.get<HeightTag>();

        if (myWidth > 0.0f && myHeight > 0.0f) {

            const std::string & myMaterialId = myOverlay.get<MaterialTag>();
            if (myMaterialId.empty() == false) {
                MaterialBasePtr myMaterial = _myScene->getMaterialsRoot()->getElementById(myMaterialId)->getFacade<MaterialBase>();
                if (!myMaterial) {
                    AC_WARNING << "renderOverlay() material:" << myMaterialId << " not found." << endl;
                    // UH: missing glPopMatrix()
                    return;
                }

                COUNT(Overlays);

                bool myMaterialHasChanged = switchMaterial(theViewport, *myMaterial, true);
                if (myMaterialHasChanged) {
                    IShaderPtr myShader = myMaterial->getShader();
                    if (myShader) {
                        MAKE_GL_SCOPE_TIMER(renderOverlay_bindOverlayParams);
                        myShader->bindOverlayParams(*myMaterial);
                        CHECK_OGL_ERROR;
                    }
                }

                // force depth buffer deactivation
                RenderStyles myOverlayRenderStyle(BIT(IGNORE_DEPTH) | BIT(NO_DEPTH_WRITES));

                enableRenderStyles(myOverlayRenderStyle, myMaterial.get());
                MaterialPropertiesFacadePtr myPropFacade = myMaterial->getChild<MaterialPropertiesTag>();

                const asl::Vector4f & myColor = myPropFacade->get<SurfaceColorTag>();
                glColor4f(myColor[0], myColor[1], myColor[2], myColor[3]*myAlpha);

                const asl::Vector2f & mySourceOrigin = myOverlay.get<SrcOriginTag>();
                const asl::Vector2f & mySourceSize   = myOverlay.get<SrcSizeTag>();

                unsigned myTextureCount = myMaterial->getTextureUnitCount();
                if (myTextureCount == 1) {
                    MAKE_GL_SCOPE_TIMER(renderOverlay_single_texture);
                    glBegin(GL_QUADS);
                    glTexCoord2f(mySourceOrigin[0],mySourceOrigin[1]);
                    glVertex2f(0, 0);

                    glTexCoord2f(mySourceOrigin[0],mySourceOrigin[1]+mySourceSize[1]);
                    glVertex2f(0, myHeight);

                    glTexCoord2f(mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]+mySourceSize[1]);
                    glVertex2f(myWidth, myHeight);

                    glTexCoord2f(mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]);
                    glVertex2f(myWidth, 0);
                    glEnd();
                } else {
                    MAKE_GL_SCOPE_TIMER(renderOverlay_multi_texture);
                    glBegin(GL_QUADS);
                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0],mySourceOrigin[1]);
                    }
                    glVertex2f(0,0);

                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0],mySourceOrigin[1]+mySourceSize[1]);
                    }
                    glVertex2f(0, myHeight);

                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]+mySourceSize[1]);
                    }
                    glVertex2f(myWidth, myHeight);

                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]);
                    }
                    glVertex2f(myWidth, 0);
                    glEnd();
                }
            }
            MAKE_GL_SCOPE_TIMER(renderOverlay_border);

            // Render borders
            const bool & hasTopBorder    = myOverlay.get<TopBorderTag>();
            const bool & hasBottomBorder = myOverlay.get<BottomBorderTag>();
            const bool & hasLeftBorder   = myOverlay.get<LeftBorderTag>();
            const bool & hasRightBorder  = myOverlay.get<RightBorderTag>();

            if (hasTopBorder || hasBottomBorder || hasLeftBorder || hasRightBorder) {
                Vector4f myBorderColor = myOverlay.get<BorderColorTag>();
                myBorderColor[3] *= myAlpha;

                _myState->setTexturing(false);

                glColor4fv(myBorderColor.begin()); // border color
                glLineWidth(myOverlay.get<BorderWidthTag>());
                glBegin(GL_LINES);
                if (hasTopBorder) {
                    glVertex2f(0,0);
                    glVertex2f(myWidth, 0);
                }
                if (hasBottomBorder) {
                    glVertex2f(0, myHeight);
                    glVertex2f(myWidth, myHeight);
                }
                if (hasLeftBorder) {
                    glVertex2f(0,0);
                    glVertex2f(0, myHeight);
                }
                if (hasRightBorder) {
                    glVertex2f(myWidth,0);
                    glVertex2f(myWidth, myHeight);
                }
                glEnd();

                _myState->setTexturing(theViewport.get<ViewportTexturingTag>());
            }
        }

        // Render child overlays
        unsigned myOverlayCount = theOverlayNode->childNodesLength(OVERLAY_NODE_NAME);
        for (unsigned i = 0; i < myOverlayCount; ++i) {
             renderOverlay(theViewport, theOverlayNode->childNode(OVERLAY_NODE_NAME, i), myAlpha);
        }
        glPopMatrix();

        CHECK_OGL_ERROR;
    }
#if 0
    void
    Renderer::renderAnalyticGeometry( ViewportPtr theViewport, CameraPtr theCamera) {
        const Frustum & myFrustum = theCamera->get<FrustumTag>();
        bool myOverlapFrustumFlag = true;

        MAKE_GL_SCOPE_TIMER(renderAnalyticGeometry);
        Matrix4f myMatrix;
        myMatrix.makeIdentity();

        std::vector<dom::NodePtr> & myAnalyticGeometry = _myScene->getAnalyticGeometry();
        //AC_PRINT << "draw " << myAnalyticGeometry.size() << " primitives";
        for (unsigned i = 0; i < myAnalyticGeometry.size(); ++i) {
            dom::NodePtr myNode = myAnalyticGeometry[i];
            TransformHierarchyFacadePtr myFacade( myNode->getFacade<TransformHierarchyFacade>());

            // [DS] perform culling: Some primitives can't be culled with bounding boxes, e.g
            // planes, lines, rays. They would result in infinite large boxes, which breaks stuff like
            // automatic near-far-plane adjustment.
            if (theViewport->get<ViewportCullingTag>() && myFacade->get<CullableTag>()) {
                if (!intersection(myFacade->get<BoundingBoxTag>(), myFrustum, myOverlapFrustumFlag)) {
                    continue;
                }
            }

            if (myAnalyticGeometry[i]->nodeName() == PLANE_NODE_NAME) {
                PlanePtr myFacade( myNode->getFacade<Plane>());
                Planef myPlane = myFacade->get<GlobalPlaneTag>();
                // TODO: intersect with frustum and visualize something ...
            } else if (myAnalyticGeometry[i]->nodeName() == POINT_NODE_NAME) {
                PointPtr myFacade( myNode->getFacade<Point>());
                draw( asVector(myFacade->get<GlobalPointTag>()), myFacade->get<ColorTag>(),
                      myMatrix, myFacade->get<AG::PointSizeTag>(), "");
            } else if (myAnalyticGeometry[i]->nodeName() == VECTOR_NODE_NAME) {
                VectorPtr myFacade( myNode->getFacade<Vector>());
                asl::LineSegment<float> myLineSegment( myFacade->get<GlobalMatrixTag>().getTranslation(),
                                           myFacade->get<GlobalVectorTag>() );
                draw( myLineSegment, myFacade->get<ColorTag>(),
                      myMatrix, myFacade->get<AG::LineWidthTag>(), "");
                // TODO: draw arrow head
            }
        }
    }
#else
    void
    Renderer::renderAnalyticGeometry(ViewportPtr theViewport, CameraPtr theCamera) {
        const Frustum & myFrustum = theCamera->get<FrustumTag>();
        bool myOverlapFrustumFlag = true;

        MAKE_GL_SCOPE_TIMER(renderAnalyticGeometry);
        Matrix4f myMatrix;
        myMatrix.makeIdentity();

        dom::Node & mySceneNode = _myScene->getNode();
#if 1
        std::vector<PlanePtr> myPlanes = mySceneNode.getAllFacades<Plane>(PLANE_NODE_NAME);
        for (unsigned i = 0; i < myPlanes.size();++i) {
            PlanePtr & myFacade = myPlanes[i];
            Planef myPlane = myFacade->get<GlobalPlaneTag>();
            // TODO: intersect with frustum and visualize something ...
        }
#endif
        std::vector<PointPtr> myPoints = mySceneNode.getAllFacades<Point>(POINT_NODE_NAME);
        for (unsigned i = 0; i < myPoints.size();++i) {
            PointPtr & myFacade = myPoints[i];
            if (!theViewport->get<ViewportCullingTag>() || !myFacade->get<CullableTag>() ||
                    intersection(myFacade->get<BoundingBoxTag>(), myFrustum, myOverlapFrustumFlag)) {
                draw( asVector(myFacade->get<GlobalPointTag>()), myFacade->get<ColorTag>(), myMatrix, myFacade->get<AG::PointSizeTag>(), "");
            }
        }

        std::vector<VectorPtr> myVectors = mySceneNode.getAllFacades<Vector>(VECTOR_NODE_NAME);
        for (unsigned i = 0; i < myVectors.size();++i) {
            VectorPtr & myFacade = myVectors[i];
            if (!theViewport->get<ViewportCullingTag>() || !myFacade->get<CullableTag>() ||
                    intersection(myFacade->get<BoundingBoxTag>(), myFrustum, myOverlapFrustumFlag)) {
                asl::LineSegment<float> myLineSegment( myFacade->get<GlobalMatrixTag>().getTranslation(), myFacade->get<GlobalVectorTag>() );
                draw( myLineSegment, myFacade->get<ColorTag>(), myMatrix, myFacade->get<AG::LineWidthTag>(), "");
                // TODO: draw arrow head
            }
        }
    }

#endif


}
