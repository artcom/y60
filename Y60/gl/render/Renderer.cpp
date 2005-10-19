//=============================================================================
// Copyright (C) 2003 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: Renderer.cpp,v $
//     $Author: jens $
//   $Revision: 1.143 $
//       $Date: 2005/04/26 19:55:58 $
//
//=============================================================================

#ifdef WIN32
// Note: GLH_EXT_SINGLE_FILE must be defined only in one object file
// it makes the header file to define the function pointer variables
// never set it in a .h file or any other file that shall be linked
// with this object file
// If you use GLH_EXT_SINGLE_FILE 1 make sure that glh_extensions.h
// and glh_genext.h has not been included from another include file
// already without GLH_EXT_SINGLE_FILE set
//
#   define GLH_EXT_SINGLE_FILE
#   include <GL/glh_extensions.h>
#   include <GL/glh_genext.h>

#endif

#include <GL/glu.h>

#include "Renderer.h"
#include "CGShader.h"

#include <y60/TransformHierarchyFacade.h>
#include <y60/LodFacade.h>
#include <y60/Facades.h>
#include <y60/BitmapTextRenderer.h>
#include <y60/GLUtils.h>
#include <y60/TextureCompressor.h>
#include <y60/Shape.h>
#include <y60/Viewport.h>
#include <y60/Overlay.h>
#include <y60/NodeValueNames.h>
#include <y60/NodeNames.h>
#include <y60/PropertyNames.h>
#include <y60/DataTypes.h>
#include <y60/Primitive.h>

#include <y60/Viewport.h>
#include <y60/VertexDataRoles.h>

#include "GLResourceManager.h"

#include <asl/Assure.h>
#include <asl/file_functions.h>
#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Matrix4.h>
#include <asl/Logger.h>
#include <asl/Time.h>

#include <dom/Nodes.h>
#include <dom/Schema.h>

#include <GL/glu.h>
#include <iostream>
#include <string>
#include <algorithm>

#define DB(x)  //x
#define DB2(x) //x
#define DBP(x) // x

using namespace std;
using namespace asl;

namespace y60 {

    Renderer::Renderer(unsigned int theRenderingCaps) :
        _myScene(0),
        _myBoundingVolumeMode(BV_NONE),
        _myPreviousMaterialIndex(UINT_MAX),
        _myRenderingCaps(theRenderingCaps)
    {
        _myLastVertexRegisterFlags.reset();
        initGL();
    }

    Renderer::~Renderer() {
        if (_myScene) {
            _myScene->deregisterResourceManager();
        }
    }

    void
    Renderer::initGL() {

        // initialize the GL-Vertex Buffer/Object, initialize its Factories
        GLResourceManager::get().initVertexDataFactories();

        static bool myVerboseFlag = true;
        initGLExtensions(_myRenderingCaps, myVerboseFlag);
        myVerboseFlag = false;

        _myState.init();

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

        // This prevents translucent pixels from beeing drawn. This way the
        // background can shine through.
        glAlphaFunc(GL_GREATER, 0.0);
        glEnable(GL_ALPHA_TEST);

        // setup initial camera position
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // enable vertex arrays
        glEnableClientState(GL_VERTEX_ARRAY);
    }

    void
    Renderer::preloadShader() {
        unsigned int myMaterialCount = _myScene->getMaterialCount();
        AC_INFO << "Renderer::preloadShader(): preloading " << myMaterialCount << " shaders" << endl;
        for (int i = 0; i < myMaterialCount; i++) {
            MaterialBasePtr myMaterial = _myScene->getMaterial(i);
            GLShaderPtr myShader = dynamic_cast_Ptr<GLShader>(myMaterial->getShader());
            if (myShader) {
                myShader->load(*getShaderLibrary());
                AC_INFO << "Renderer::preloadShader(): using shader '" << myShader->getName() << "' for material '"<<myMaterial->get<NameTag>()<<"'" << endl;
            }
        }
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
        if (_myPreviousMaterialIndex != UINT_MAX) {
            MAKE_SCOPE_TIMER(deactivatePreviousMaterial);

            const MaterialBasePtr myPrevMaterial = _myScene->getMaterial(_myPreviousMaterialIndex);

            IShaderPtr prevShader = myPrevMaterial->getShader();
            prevShader->disableTextures(*myPrevMaterial);
            CHECK_OGL_ERROR;
            prevShader->deactivate(*myPrevMaterial);
            CHECK_OGL_ERROR;
        }

    }

    void
    Renderer::switchMaterial(int theMaterialIndex) {

        if (_myPreviousMaterialIndex == theMaterialIndex) {
            return;
        } else if (_myPreviousMaterialIndex == UINT_MAX) {
            _myLastVertexRegisterFlags.reset();
        }

        MAKE_SCOPE_TIMER(switchMaterial);
        COUNT(materialChange);

        MaterialBasePtr myMaterial = _myScene->getMaterial(theMaterialIndex);
        IShaderPtr myShader = myMaterial->getShader();
        deactivatePreviousMaterial();

        {
            // activate new material
            MAKE_SCOPE_TIMER(activateShader);

            if (myMaterial->getLightingModel() == UNLIT) {
                glDisable(GL_LIGHTING);
            } else if (_myState.getLighting()) {
                glEnable(GL_LIGHTING);
            }
            CHECK_OGL_ERROR;

            if (myMaterial->writesDepthBuffer()) {
                glDepthMask(GL_TRUE);
            } else {
                glDepthMask(GL_FALSE);
            }

            myShader->activate(*myMaterial);
            CHECK_OGL_ERROR;
        }

        const VertexRegisterFlags & myVertexRegisterFlags = myShader->getVertexRegisterFlags();
        for (unsigned myRegister = 0; myRegister < MAX_REGISTER; ++myRegister) {
            bool myEnable = false;

            if (myVertexRegisterFlags[myRegister] && !_myLastVertexRegisterFlags[myRegister]) {
                myEnable = true;
            } else if (!myVertexRegisterFlags[myRegister] && _myLastVertexRegisterFlags[myRegister]) {
                myEnable = false;
            } else {
                continue;
            }

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
                    glActiveTextureARB(asGLTextureRegister(GLRegister(myRegister)));
                    glClientActiveTextureARB(asGLTextureRegister(GLRegister(myRegister)));
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
        CHECK_OGL_ERROR;

        if (_myState.getTexturing()) {
            myShader->enableTextures(*myMaterial);
        }
        CHECK_OGL_ERROR;

        _myPreviousMaterialIndex   = theMaterialIndex;
        _myLastVertexRegisterFlags = myVertexRegisterFlags;
    }

    void
    Renderer::rotateBillboard(BodyPtr theBody, const Camera & theCamera) {
        const Vector3f & myPivot = theBody->get<PivotTag>();
        glTranslatef(myPivot[0], myPivot[1], myPivot[2]);
        if (theBody->get<BillboardTag>() == AXIS_BILLBOARD) {
            MAKE_SCOPE_TIMER(update_billboards);
            Matrix4f myBillboardTransform = theBody->get<GlobalMatrixTag>();
            myBillboardTransform.translate( theBody->get<PivotTag>());
            double myRotation = getBillboardRotation(myBillboardTransform,
                    theCamera.get<GlobalMatrixTag>());
            glRotated((myRotation * 180 / asl::PI), 0, 1, 0);

        } else if (theBody->get<BillboardTag>() == POINT_BILLBOARD) {

            asl::Vector4f myCamUpVector    = theCamera.get<GlobalMatrixTag>().getRow(1);
            asl::Vector3f myCamUpVector3(myCamUpVector[0], myCamUpVector[1], myCamUpVector[2]);
            asl::Vector4f myCamViewVector  = theCamera.get<GlobalMatrixTag>().getRow(2);
            asl::Vector3f myCamViewVector3(myCamViewVector[0], myCamViewVector[1], myCamViewVector[2]);
            asl::Vector3f myRightVec       = cross(myCamUpVector3, myCamViewVector3);

            asl::Matrix4f myScreenAlignedMatrix;
            myScreenAlignedMatrix.assign(myRightVec[0], myRightVec[1], myRightVec[2], 0,
                    myCamUpVector[0],myCamUpVector[1],myCamUpVector[2],0,
                    myCamViewVector[0],myCamViewVector[1],myCamViewVector[2],0,
                    0,0,0,1, Matrix4<float>::ROTATING);
            glMultMatrixf(myScreenAlignedMatrix.getData());
        }
        glTranslatef( - myPivot[0], - myPivot[1], - myPivot[2]);
    }

    void
    Renderer::renderBodyPart(const BodyPart & theBodyPart, const Viewport & theViewport, const Camera & theCamera) {
        MAKE_SCOPE_TIMER(renderBodyPart);
        CHECK_OGL_ERROR;
        y60::BodyPtr myBody = theBodyPart.getBody();
        y60::ShapePtr myShape = myBody->getShape();
        bool myBodyHasChanged = _myPreviousBody != &(*myBody);
        if (myBodyHasChanged) {
            MAKE_SCOPE_TIMER(update_bodymatrix);
            glPopMatrix();

            _myState.setClippingPlanes(theBodyPart.getClippingPlanes());
            
            glPushMatrix();

            // draw body bounding-box
            if (_myBoundingVolumeMode & BV_BODY) {
                const Box3f & myBoundingBox = myBody->get<BoundingBoxTag>();
                renderBoundingBox(myBoundingBox);
                CHECK_OGL_ERROR;
            }

            glMultMatrixf((myBody->get<GlobalMatrixTag>().getData()));

            // XXX update TexGen here
            if (myBody->get<BillboardTag>() != "") {
                rotateBillboard(myBody, theCamera);
            }

            _myPreviousBody = &(*myBody);
            CHECK_OGL_ERROR;
        }

        const y60::Primitive & myPrimitive = theBodyPart.getPrimitive();
        if (_myState.getDrawNormals()) {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glDisable(GL_LIGHTING);
            const Box3f & myBoundingBox = myShape->get<BoundingBoxTag>();
            float myDiameter = magnitude(myBoundingBox[Box3f::MAX] - myBoundingBox[Box3f::MIN]);
            drawNormals(myPrimitive, myDiameter / 64.0f);
            glPopAttrib();
        }

        unsigned myMaterialIndex = myPrimitive.getMaterialIndex();
        bool     myMaterialHasChanged = (myMaterialIndex != _myPreviousMaterialIndex);

        switchMaterial(myMaterialIndex);

        MaterialBasePtr myMaterial = _myScene->getMaterial(myMaterialIndex);
        IShaderPtr myShader = myMaterial->getShader();

        if (myShader && (myBodyHasChanged || myMaterialHasChanged)) {
            myShader->bindBodyParams(*myMaterial, theViewport, _myScene->getLights(), *myBody, theCamera);
            CHECK_OGL_ERROR;
        }

        bool isRendered = false;

        const std::vector<RenderStyleType> & myRenderStyles = myShape->getRenderStyles();

        bool myIgnoreDepthFlag = (std::find(myRenderStyles.begin(), myRenderStyles.end(),
                                            IGNORE_DEPTH) !=  myRenderStyles.end());
        if (myIgnoreDepthFlag) {
            glDepthFunc(GL_ALWAYS);
            glEnable(GL_POLYGON_OFFSET_POINT);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.0, -1.0);
        }
        if (std::find(myRenderStyles.begin(), myRenderStyles.end(), BACK) !=  myRenderStyles.end()) {
            glCullFace(GL_FRONT);
            renderPrimitives(theBodyPart, myMaterial);
            isRendered = true;
            CHECK_OGL_ERROR;
        }

        if (!isRendered || std::find(myRenderStyles.begin(), myRenderStyles.end(), FRONT) !=  myRenderStyles.end()) {
            glCullFace(GL_BACK);
            renderPrimitives(theBodyPart, myMaterial);
            CHECK_OGL_ERROR;
        }

        if (myIgnoreDepthFlag) {
            glDepthFunc(GL_LESS);
            glDisable(GL_POLYGON_OFFSET_POINT);
            glDisable(GL_POLYGON_OFFSET_LINE);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        if (std::find(myRenderStyles.begin(), myRenderStyles.end(), BOUNDING_VOLUME) !=  myRenderStyles.end() ||
            (_myBoundingVolumeMode & BV_SHAPE))
        {
            dom::NodePtr myShapeNode = myBody->getShape()->getXmlNode();
            const asl::Box3f & myBoundingBox = myShapeNode->getFacade<Shape>()->get<BoundingBoxTag>();
            renderBoundingBox(myBoundingBox);
            CHECK_OGL_ERROR;
        }
    }

    void
    Renderer::drawNormals(const Primitive & thePrimitive, float theNormalScale) {
        if (thePrimitive.hasVertexData(NORMALS)) {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glColor3f(1.0f, .5f, 0);

            Ptr<ConstVertexDataAccessor<Vector3f> > myPositionAccessor = thePrimitive.getConstLockingPositionsAccessor();
            Ptr<ConstVertexDataAccessor<Vector3f> > myNormalsAccessor = thePrimitive.getConstLockingNormalsAccessor();
            const VertexData3f & myPositions = myPositionAccessor->get();
            const VertexData3f & myNormals = myNormalsAccessor->get();
            Vector3f myNormalTip;
            glBegin(GL_LINES);
            for (int i = 0; i < myPositions.size(); ++i) {
                glVertex3fv(myPositions[i].begin());
                myNormalTip = myPositions[i] + (myNormals[i] * theNormalScale);
                glVertex3fv(myNormalTip.begin());
            }
            glEnd();
            glPopAttrib();
        }
    }

    void
    Renderer::renderPrimitives(const BodyPart & theBodyPart, MaterialBasePtr theMaterial) {
        MAKE_SCOPE_TIMER(renderPrimitives);
        const y60::Primitive & myPrimitive = theBodyPart.getPrimitive();
        AC_TRACE << "------ renderPrimitives for BodyPart of Body id='"
                << theBodyPart.getBody()->get<IdTag>()<<"'"
                << ", name='"<<theBodyPart.getBody()->get<NameTag>()<<"'";

        COUNT_N(Vertices, myPrimitive.size());
        const MaterialParameterVectorPtr myMaterialParameters = theMaterial->getVertexParameters();

        for (unsigned i = 0; i < myMaterialParameters->size(); ++i) {

            const MaterialParameter & myParameter = myMaterialParameters->at(i);
            GLRegister myRegister = myParameter.getRegister();

            if ( myPrimitive.hasVertexData(myParameter.getRole())) {
                const VertexDataBase & myData = myPrimitive.getVertexData(myParameter.getRole());
                AC_TRACE << "-- Parameter " << i << " role " << getStringFromEnum(myParameter.getRole(), VertexDataRoleString) << ", data=" << myData.getDataPtr();
                switch (myRegister) {
                    case POSITION_REGISTER:
                        myData.useAsPosition();
                        break;
                    case NORMAL_REGISTER:
                        myData.useAsNormal();
                        break;
                    case COLORS_REGISTER:
                        myData.useAsColor();
                        CHECK_OGL_ERROR;
                        break;
                    default:
                        GLenum myGlRegister = asGLTextureRegister(myRegister);
                        glActiveTextureARB(myGlRegister);
                        CHECK_OGL_ERROR;
                        glClientActiveTextureARB(myGlRegister);
                        CHECK_OGL_ERROR;
                        myData.useAsTexCoord();
                        break;
                }
            }
        }
        CHECK_OGL_ERROR;

#ifdef DBP
        unsigned long myPrimitiveCount = myPrimitive.size();
        AC_TRACE << "glDrawArrays size=" << myPrimitive.size() << " primCount=" << myPrimitiveCount << " primType=" << myPrimitive.getType() << " GLtype=" << getPrimitiveGLType(myPrimitive.getType());
        static unsigned long myMaxPrimitiveCount = 0;
        if (myPrimitiveCount > myMaxPrimitiveCount) {
            myMaxPrimitiveCount = myPrimitiveCount;
        }
        AC_TRACE << "Primitive maxsize: " << myMaxPrimitiveCount;
#endif

        DBP(static asl::NanoTime lastTime;
                asl::NanoTime switchTime = asl::NanoTime() - lastTime;
                AC_TRACE << "switch time = " << switchTime.micros() << " us" << endl;
                asl::NanoTime startTime;
           );

        COUNT(VertexArrays);
        MAKE_SCOPE_TIMER(glDrawArrays);

        glDrawArrays(getPrimitiveGLType(myPrimitive.getType()), 0, myPrimitive.size());
        CHECK_OGL_ERROR;
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

    void
    Renderer::renderFrustum(const ViewportPtr & theViewport) {
        Point3f myLTF, myRBF, myRTF, myLBF;
        Point3f myLTBK, myRBBK, myRTBK, myLBBK;

        theViewport->get<ViewportFrustumTag>().getCorners(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK);
        renderBox(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK,
                  Vector4f(0.5, 1.0, 0.5, 1.0), Vector4f(1.0, 0.5, 0.0, 1.0));
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

    void Renderer::draw(const asl::LineSegment<float> & theLine,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation)
    {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glMultMatrixf(theTransformation.getData());
        glColor4fv(theColor.begin());

        glBegin(GL_LINES);
            glVertex3fv(theLine.origin.begin());
            glVertex3fv(theLine.end.begin());
        glEnd();

        glPopMatrix();
        CHECK_OGL_ERROR;
    }

    void Renderer::draw(const asl::Sphere<float> & theSphere,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation)
    {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glMultMatrixf(theTransformation.getData());
        glColor4fv(theColor.begin());

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

        glPopMatrix();
        CHECK_OGL_ERROR;
    }

    void Renderer::draw(const asl::Box3<float> & theBox,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation)
    {
        glPushMatrix();
        glMultMatrixf(theTransformation.getData());

        Point3f myLTF, myRBF, myRTF, myLBF;
        Point3f myLTBK, myRBBK, myRTBK, myLBBK;

        theBox.getCorners(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK);

        renderBox(myLTF, myRBF, myRTF, myLBF, myLTBK, myRBBK, myRTBK, myLBBK,
                  theColor);

        glPopMatrix();
        CHECK_OGL_ERROR;
    }

    void Renderer::draw(const asl::Triangle<float> & theTriangle,
            const asl::Vector4f & theColor,
            const asl::Matrix4f & theTransformation)
    {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glMultMatrixf(theTransformation.getData());
        glColor4fv(theColor.begin());

        glBegin(GL_TRIANGLES);
            glVertex3fv(theTriangle[0].begin());
            glVertex3fv(theTriangle[1].begin());
            glVertex3fv(theTriangle[2].begin());
        glEnd();

        glPopMatrix();
        CHECK_OGL_ERROR;
    }

    dom::NodePtr
    Renderer::getActiveLodChild(dom::NodePtr theNode, const CameraPtr theCamera) {
        const LodFacadePtr myLodFacade = theNode->getFacade<LodFacade>();
        if (!myLodFacade) {
            throw RendererException(string("Node with id: ") + theNode->getAttributeString(ID_ATTRIB)
                + " is not a lod node", PLUS_FILE_LINE);
        }

        // Distance from bounding-box-center to camera
        const asl::Box3f & myBoundingBox = myLodFacade->get<BoundingBoxTag>();
        asl::Point3f myCenterPoint = myBoundingBox.getCenter();
        asl::Vector3f myCenter(myCenterPoint[0], myCenterPoint[1], myCenterPoint[2]);
        float myDistance = length(theCamera->get<PositionTag>() - myCenter);

        // compensate for field of view (90° results in factor=1)
        float myZoomAdjustment = 0.0f;
        float myFOV = theCamera->get<HfovTag>();
        if (myFOV < 180.0f) {
            myZoomAdjustment = float(tan(radFromDeg(theCamera->get<HfovTag>()/2.0f)));
        }

        // apply lodscale
        WorldFacadePtr myWorldFacade = _myScene->getWorldRoot()->getFacade<WorldFacade>();
        float myMetric = myDistance * myWorldFacade->get<LodScaleTag>() * myZoomAdjustment;

        const VectorOfFloat & myRanges = myLodFacade->get<RangesTag>();
        if (theNode->childNodesLength() == 1) {
            return theNode->childNode(0);
        } else {
            for (unsigned i = 0 ; i < theNode->childNodesLength(); ++i) {
                if (i >= myRanges.size() || myMetric < myRanges[i]) {
                    return theNode->childNode(i);
                }
            }
        }

        return dom::NodePtr(0);
    }

    void
    Renderer::createRenderList(dom::NodePtr theNode, BodyPartMap & theBodyParts,
                               const CameraPtr theCamera,
                               const Matrix4f & theEyeSpaceTransform,
                               ViewportPtr theViewport,
                               bool theOverlapFrustumFlag,
                               std::vector<asl::Planef> theClippingPlanes)
    {
        // Skip undefined nodes
        if (!theNode) {
            return;
        }

        // Skip non element nodes
        if (!(theNode->nodeType() == dom::Node::ELEMENT_NODE)) {
            return;
        }

        // Skip comments
        if (theNode->nodeName() == "#comment") {
            return;
        }

        TransformHierarchyFacadePtr myFacade = theNode->getFacade<TransformHierarchyFacade>();

        // Skip invisible nodes
        if (!(myFacade->get<VisibleTag>())) {
            return;
        }

        collectClippingPlanes(theNode, theClippingPlanes);

        // Check for lodding
        if (theNode->nodeName() == LOD_NODE_NAME) {
            createRenderList(getActiveLodChild(theNode, theCamera), theBodyParts, theCamera, theEyeSpaceTransform,
                    theViewport, theOverlapFrustumFlag, theClippingPlanes);
            return;
        }

        // Check culling
        bool myOverlapFrustumFlag = true;
        const Frustum & myFrustum = theViewport->get<ViewportFrustumTag>();
        if (theViewport->get<ViewportCullingTag>() && theOverlapFrustumFlag && myFacade->get<CullableTag>()) {
            if (!intersection(myFacade->get<BoundingBoxTag>(), myFrustum, myOverlapFrustumFlag)) {
                return;
            }
        }

        // Add remaining bodies to render list
        if (theNode->nodeName() == BODY_NODE_NAME) {
            BodyPtr myBody = dynamic_cast_Ptr<Body>(myFacade);
            // Split the body in bodyparts to make material sorted rendering possible
            const y60::PrimitiveVector & myPrimitives = myBody->getShape()->getPrimitives();
            Matrix4f myTransform = myBody->get<GlobalMatrixTag>();
            myTransform.postMultiply(theEyeSpaceTransform);

             // AC_TRACE << "Adding body: " << myBody->get<NameTag>() << " with " << myPrimitives.size() <<
             //       " primitives to renderlist" << endl;

            double myFarPlane = myFrustum.getFar();
            double myNearPlane = myFrustum.getNear();
            for (unsigned i = 0; i < myPrimitives.size(); ++i) {

                const y60::Primitive & myPrimitive = (*myPrimitives[i]);
				unsigned myMaterialIndex = myPrimitive.getMaterialIndex();
                y60::MaterialBasePtr myMaterial = _myScene->getMaterial(myMaterialIndex);
                unsigned short myBodyKey = makeBodyKey(myMaterial,
                                                       myBody,
                                                       myTransform,
                                                       myNearPlane,
                                                       myFarPlane,
                                                       &*_myScene);
                BodyPart::Key myKey = makeBodyPartKey(myMaterialIndex, & ( * myBody), myBodyKey);

                if (myMaterial->get<TransparencyTag>()) {
                    COUNT(TransparentPrimitives);
                } else {
                    COUNT(OpaquePrimitives);
                }

                theBodyParts.insert(std::make_pair(myKey, BodyPart(myBody, myPrimitive, theClippingPlanes)));
            }

            COUNT(RenderedBodies);
        }

        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            createRenderList(theNode->childNode(i), theBodyParts, theCamera, theEyeSpaceTransform,
                    theViewport, myOverlapFrustumFlag, theClippingPlanes);
        }
    }

    void 
    Renderer::collectClippingPlanes(dom::NodePtr theNode,
                                    std::vector<asl::Planef> & theClippingPlanes)
    {
        TransformHierarchyFacadePtr myFacade = theNode->getFacade<TransformHierarchyFacade>();

        const VectorOfString & myPlaneIds = myFacade->get<ClippingPlanesTag>();
        dom::NodePtr myGeometryNode;
        for (unsigned i = 0; i < myPlaneIds.size(); ++i) {
            if ( ! myPlaneIds[i].empty()) {
                myGeometryNode = theNode->getElementById( myPlaneIds[i] );
                if (!myGeometryNode) {
                    throw RendererException(string("Can not find geometry '")+myPlaneIds[i]+
                        "' for node " + theNode->nodeName() + " with id '" + 
                        theNode->getAttributeString(ID_ATTRIB)+"'!", PLUS_FILE_LINE);
                }
                GeometryPtr myGeometry = myGeometryNode->getFacade<Geometry>();
                theClippingPlanes.push_back( myGeometry->get<GeometryGlobalPlaneTag>());
            }
        }
    }

    void
    Renderer::preRender(const CanvasPtr & theCanvas) {
        MAKE_SCOPE_TIMER(Renderer_preRender);
        // called once per canvas per frame
        const asl::Vector4f & backgroundColor = theCanvas->get<CanvasBackgroundColorTag>();
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
        glColor4f(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void
    Renderer::setupViewport(ViewportPtr theViewport) {
        MAKE_SCOPE_TIMER(setupViewport);

        activateViewport(theViewport);
        CHECK_OGL_ERROR;

        _myState.setWireframe(theViewport->get<ViewportWireframeTag>());
        _myState.setFlatShading(theViewport->get<ViewportFlatshadingTag>());
        _myState.setLighting(theViewport->get<ViewportLightingTag>());
        _myState.setBackfaceCulling(theViewport->get<ViewportBackfaceCullingTag>());
        _myState.setTexturing(theViewport->get<ViewportTexturingTag>());
        _myState.setDrawNormals(theViewport->get<ViewportDrawNormalsTag>());
        CHECK_OGL_ERROR;
    }

    // called once per Canvas per Frame
    void
    Renderer::render(ViewportPtr theViewport) {
        MAKE_SCOPE_TIMER(render);
        setupViewport(theViewport);

        glPushAttrib(GL_ALL_ATTRIB_BITS);
        // we need to push the client state too
        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

        dom::NodePtr myCameraNode = theViewport->getNode().getElementById(
                theViewport->get<CameraTag>());
        if (!myCameraNode) {
            throw RendererException(string("Can not find camera '")+theViewport->get<CameraTag>()+
                    "' for viewport '"+theViewport->get<IdTag>()+"'!", PLUS_FILE_LINE);
        }
        CameraPtr myCamera = myCameraNode->getFacade<Camera>();
        bindViewMatrix(myCamera);
        CHECK_OGL_ERROR;

        if (theViewport->get<ViewportCullingTag>()) {
            if (theViewport->get<ViewportDebugCullingTag>()) {
                renderFrustum(theViewport);
            } else {
                theViewport->updateClippingPlanes();
            }
        }

        // (2) Create lists of render objects
        BodyPartMap myBodyParts;
        {
            MAKE_SCOPE_TIMER(createRenderList_lod_cull);
            Matrix4f myEyeSpaceTransform = myCamera->get<InverseGlobalMatrixTag>();
            createRenderList(_myScene->getWorldRoot(), myBodyParts, myCamera, myEyeSpaceTransform,
                    theViewport, true, std::vector<asl::Planef>());
            resetModelView();
            CHECK_OGL_ERROR;
        }
        setProjection(theViewport);
        CHECK_OGL_ERROR;
        {
            MAKE_SCOPE_TIMER(renderSkyBox);
            renderSkyBox(myCamera);
            CHECK_OGL_ERROR;
        }

        {
            MAKE_SCOPE_TIMER(enableVisibleLights);
            // (4) Setup camera
            bindViewMatrix(myCamera);

            // (5) activate all visible lights
            enableVisibleLights();
            CHECK_OGL_ERROR;
        }

        {
            enableFog();
            CHECK_OGL_ERROR;
        }

        // (7) render bodies
        {
            MAKE_SCOPE_TIMER(renderBodyParts);
            BodyPartMap::const_iterator it = myBodyParts.begin();
            _myPreviousBody = 0;
            glPushMatrix();
            glDisable(GL_ALPHA_TEST);
            bool currentMaterialHasAlpha = false;

            CHECK_OGL_ERROR;
            while (it != myBodyParts.end()) {
                if ( ! currentMaterialHasAlpha && getTransparencyBitFromKey(it->first)) {
                    glEnable(GL_ALPHA_TEST);
                    currentMaterialHasAlpha = true;
                }
                renderBodyPart(it->second, *theViewport, *myCamera);
                it++;
            }
            glPopMatrix();
        }
        glPopAttrib();
        // we need to pop the client state too
        glPopClientAttrib();

        if (_myBoundingVolumeMode & BV_HIERARCHY) {
            renderBoundingBoxHierarchy(_myScene->getWorldRoot());
        }

        {
            MAKE_SCOPE_TIMER(renderOverlays);
            renderOverlays(theViewport);
        }
        {
            MAKE_SCOPE_TIMER(renderTextSnippets);
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            renderTextSnippets(theViewport);
            glPopAttrib();
        }
        // Set renderer into known state for drawing calls from js
        deactivatePreviousMaterial();
        _myPreviousMaterialIndex = UINT_MAX;
    }

    void
    Renderer::postRender() {
    }

    void
    Renderer::setProjection(ViewportPtr theViewport) {
        glMatrixMode(GL_PROJECTION);
        DB(AC_TRACE << "setting proj matrix to " << theViewport->get<ProjectionMatrixTag>() << endl);
        glLoadMatrixf(static_cast<const GLfloat *>(theViewport->get<ProjectionMatrixTag>().getData()));
        glMatrixMode(GL_MODELVIEW);
    }

    bool
    Renderer::activateViewport(const ViewportPtr & theViewport) {
        AC_TRACE << "setting viewport to " << theViewport->get<ViewportWidthTag>() << "x" << theViewport->get<ViewportHeightTag>() << endl;
        glViewport(theViewport->get<ViewportLeftTag>(), theViewport->getLower(),
                   theViewport->get<ViewportWidthTag>(), theViewport->get<ViewportHeightTag>());
    	return true;
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
    Renderer::resetModelView() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void
    Renderer::bindViewMatrix(CameraPtr theCamera) {
#if 0
        if (AC_TRACE_ON) {
            //AC_DEBUG << "binding view matrix to inverse global matrix of camera "<< theCamera->getNode();
            theCamera->debug<InverseGlobalMatrixTag>();
            theCamera->debug<PositionTag>();
        }
#endif
        glLoadMatrixf(static_cast<const GLfloat *>(theCamera->get<InverseGlobalMatrixTag>().getData()));
    }

    GLenum
    Renderer::getPrimitiveGLType(y60::PrimitiveType theType) {
        return GLenum(theType);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Light handling and Fog
    ///////////////////////////////////////////////////////////////////////////////////////////

    void
    Renderer::enableVisibleLights() {
        int myActiveLightCount = 0;
        LightVector & myLights = _myScene->getLights();
        for (unsigned i = 0; i < myLights.size(); ++i) {
            if (myLights[i]->get<VisibleTag>()) {
                enableLight(myLights[i], myActiveLightCount);
                myActiveLightCount++;
            }
        }
        AC_TRACE << "enabled " << myActiveLightCount << " lights" << endl;
    }

    void
    Renderer::enableLight(y60::LightPtr & theLight, int theActiveLightIndex) {
        LightSourcePtr myLightSource = theLight->getLightSource();
		LightPropertiesFacadePtr myLightPropFacade = myLightSource->getFacade<LightPropertiesTag>();

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
        }
        GLint myMaxLights = 8;
        glGetIntegerv(GL_MAX_LIGHTS, &myMaxLights);
        if (theActiveLightIndex >= myMaxLights) {
            static bool myAlreadyWarned = false;
            if (!myAlreadyWarned) {
                AC_WARNING << "Only " << int(myMaxLights) << " hardware light sources supported by open gl" << endl;
                myAlreadyWarned = true;
            }
            return;
        }
        GLenum gl_lightid = asGLLightEnum(theActiveLightIndex);
        asl::Vector4f myGLLightPos;

        switch (myType) {
            case DIRECTIONAL: {
                // GL directional lights: xyz is direction, w=0.0
                asl::QuadrupleOf<float> myLightDirection = theLight->get<GlobalMatrixTag>().getRow(2);
                myGLLightPos = asl::Vector4f(myLightDirection[0], myLightDirection[1], myLightDirection[2], 0.0f);
                break;
            }
            case POSITIONAL:
            {
                // GL positional lights: xyz = light position, w=1.0
                asl::Vector3f myLightTranslation = theLight->get<GlobalMatrixTag>().getTranslation();
                myGLLightPos = asl::Vector4f(myLightTranslation[0], myLightTranslation[1], myLightTranslation[2],1.0f);
                glLightf(gl_lightid, GL_LINEAR_ATTENUATION, myLightPropFacade->get<AttenuationTag>());
                break;
            }
            case SPOT:
            {
                glLightf(gl_lightid, GL_SPOT_CUTOFF, myLightPropFacade->get<CutOffTag>());
                glLightf(gl_lightid, GL_SPOT_EXPONENT, myLightPropFacade->get<ExponentTag>());

                // get the Z-Axis to set the light direction
                asl::QuadrupleOf<float> myTmpVec = theLight->get<GlobalMatrixTag>().getRow(2); // Z-axis
                asl::Vector3f myLightDirection(myTmpVec.begin());
                glLightfv(gl_lightid, GL_SPOT_DIRECTION, &(myLightDirection[0]));
                break;
            }
        }

        glLightfv(gl_lightid, GL_POSITION, &(*myGLLightPos.begin()));
        glLightfv(gl_lightid, GL_AMBIENT,  myLightPropFacade->get<LightAmbientTag>().begin());
        glLightfv(gl_lightid, GL_DIFFUSE,  myLightPropFacade->get<LightDiffuseTag>().begin());
		glLightfv(gl_lightid, GL_SPECULAR, myLightPropFacade->get<LightSpecularTag>().begin());

        glEnable(gl_lightid);
    }

    void
    Renderer::enableFog() {
        WorldFacadePtr myWorldFacade = _myScene->getWorldRoot()->getFacade<WorldFacade>();
        const string & myFogModeString = myWorldFacade->get<FogModeTag>();
        if (myFogModeString == "") {
            return;
        }
        FogMode myFogMode = static_cast<FogMode>(getEnumFromString(myFogModeString, FogModeStrings));
        switch (myFogMode) {
            case FOG_OFF :
                glDisable(GL_FOG);
                return;
            case FOG_LINEAR :
                glFogi(GL_FOG_MODE, GL_LINEAR);
                glFogf(GL_FOG_START, myWorldFacade->get<FogRangeTag>()[0]);
                glFogf(GL_FOG_END, myWorldFacade->get<FogRangeTag>()[1]);
                break;
            case FOG_EXP :
                glFogi(GL_FOG_MODE, GL_EXP);
                glFogf(GL_FOG_DENSITY, myWorldFacade->get<FogDensityTag>());
                break;
            case FOG_EXP2 :
                glFogi(GL_FOG_MODE, GL_EXP2);
                glFogf(GL_FOG_DENSITY, myWorldFacade->get<FogDensityTag>());
                break;
            default :
                throw RendererException(string("World Fog Mode :'") + myWorldFacade->get<FogModeTag>()+
                            "' unknown", PLUS_FILE_LINE);
        }
        glFogfv(GL_FOG_COLOR, &(myWorldFacade->get<FogColorTag>()[0]));
        glHint(GL_FOG_HINT, GL_DONT_CARE);
        glEnable(GL_FOG);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Billboard handling
    ///////////////////////////////////////////////////////////////////////////////////////////

    double
    Renderer::getBillboardRotation(const asl::Matrix4f & theBillboardMatrix,
                                   const asl::Matrix4f & theCameraMatrix)
    {
        asl::Matrix4f myBillboardMatrix = theBillboardMatrix;
        myBillboardMatrix.invert();

        // Transform the camera matrix into local billboard coordinates
        asl::Matrix4f myCameraMatrix = theCameraMatrix;
        myCameraMatrix.postMultiply(myBillboardMatrix);

        asl::Vector3f myViewVector =  myCameraMatrix.getTranslation();

        // calc y-axis rotation
        if (myViewVector[2] > 0.001) {
            return atan(myViewVector[0] / myViewVector[2]);
        } else if (myViewVector[2] < -0.001) {
            return atan(myViewVector[0] / myViewVector[2]) + asl::PI;
        } else {
            if (myViewVector[0] > 0) {
                return asl::PI / 2;
            } else if (myViewVector[0] < 0) {
                return - asl::PI / 2;
            }
        }

        return 0.0;
    }

    void
    Renderer::renderTextSnippets(ViewportPtr theViewport) {
        glActiveTextureARB(asGLTextureRegister(0));
        glClientActiveTextureARB(asGLTextureRegister(0));
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        CHECK_OGL_ERROR;

        _myTextRendererManager.updateWindow(theViewport->get<ViewportWidthTag>(),
                theViewport->get<ViewportHeightTag>());

        Matrix4f myRotationMatrix;
        if (theViewport->get<ViewportOrientationTag>() == PORTRAIT_ORIENTATION) {
            myRotationMatrix.makeZRotating(float(asl::PI_2));
        } else {
            myRotationMatrix.makeIdentity();
        }

        _myTextRendererManager.render(myRotationMatrix);
    }

    void
    Renderer::renderSkyBox(CameraPtr theCamera) {
        // get Material

        const dom::NodePtr & myWorldNode = _myScene->getWorldRoot();
        WorldFacadePtr myWorld = myWorldNode->getFacade<WorldFacade>();
        if (myWorld->get<SkyBoxMaterialTag>().size() == 0) {
            return;
        }

        std::string myMaterialId = myWorld->get<SkyBoxMaterialTag>();
        unsigned myMaterialIndex = 0;
        if (!_myScene->getMaterialIndex(myMaterialId, myMaterialIndex)) {
            AC_ERROR << "Could not find SkyBox material: " << myMaterialId << endl;
            return;
        }

        // clear everything but the rotational part
        asl::Matrix4f myModelView = theCamera->get<InverseGlobalMatrixTag>();
        myModelView[0][3] = 0; myModelView[1][3] = 0; myModelView[2][3] = 0;
        myModelView[3][0] = 0; myModelView[3][1] = 0; myModelView[3][2] = 0; myModelView[3][3] = 1;

        glPushMatrix();
        glLoadMatrixf(static_cast<const GLfloat *>(myModelView.getData()));

        switchMaterial(myMaterialIndex);

        // We'll just index into the cube map directly to render the cubic panorama
        GLfloat rgba[4] = { 1.0, 1.0, 1.0, 1.0 };
        glColor4fv(rgba);

        const float mySize = theCamera->get<FarPlaneTag>() / 2;
        DB(AC_TRACE << "drawing skybox. Size=" << mySize << endl;);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

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
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Overlay handling
    ///////////////////////////////////////////////////////////////////////////////////////////

    void
    Renderer::renderOverlays(ViewportPtr theViewport) {
        dom::NodePtr myOverlays = theViewport->getNode().childNode(OVERLAY_LIST_NAME);
        if (!myOverlays) {
            return;
        }
        unsigned myOverlayCount = myOverlays->childNodesLength();
        if (myOverlayCount == 0) {
            return;
        }

        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(theViewport->get<ViewportLeftTag>(),
                   theViewport->get<ViewportLeftTag>() + theViewport->get<ViewportWidthTag>(),
                   theViewport->getLower() + theViewport->get<ViewportHeightTag>(),
                   theViewport->getLower());
        if (theViewport->get<ViewportOrientationTag>() == PORTRAIT_ORIENTATION) {
            asl::Matrix4f myRotationMatrix;
            myRotationMatrix.makeZRotating(float(asl::PI_2));
            myRotationMatrix.translate(asl::Vector3f(float(theViewport->get<ViewportWidthTag>()), 0.0f, 0.0f));
            glMultMatrixf(static_cast<const GLfloat *>(myRotationMatrix.getData()));
        }

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        for (unsigned i = 0; i < myOverlayCount; ++i) {
             renderOverlay(myOverlays->childNode(i));
        }

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glPopAttrib();
    }

    void
    Renderer::renderOverlay(dom::NodePtr theOverlayNode, const asl::Vector2f thePosition, float theAlpha) {
        if (theOverlayNode->nodeType() != dom::Node::ELEMENT_NODE) {
            return;
        }
        const y60::Overlay & myOverlay = *(theOverlayNode->getFacade<y60::Overlay>());
        if (!myOverlay.get<VisibleTag>()) {
            return;
        }
        MAKE_SCOPE_TIMER(renderOverlay);

        const std::string & myMaterialId = myOverlay.get<MaterialTag>();
        const asl::Vector2f myPosition = thePosition + myOverlay.get<Position2DTag>();
        float myWidth  = myOverlay.get<WidthTag>();
        float myHeight = myOverlay.get<HeightTag>();

        if (myHeight == 0.0f || myWidth == 0.0f) {
            return;
        }

        float myAlpha  = theAlpha * myOverlay.get<AlphaTag>();

        if (!myMaterialId.empty()) {
            unsigned myMaterialIndex = 0;
            DB(AC_TRACE << "Rendering Overlay " << myOverlay.get<NameTag>() << " with material " << myMaterialId << endl);
            if (!_myScene->getMaterialIndex(myMaterialId, myMaterialIndex)) {
                AC_WARNING << "renderOverlay() material:" << myMaterialId << " not found." << endl;
                return;
            }

            COUNT(Overlays);

            switchMaterial(myMaterialIndex);
            MaterialBasePtr myMaterial = _myScene->getMaterial(myMaterialIndex);
			MaterialPropertiesFacadePtr myPropFacade = myMaterial->getFacade<MaterialPropertiesTag>();

            const asl::Vector4f & myColor = myPropFacade->get<SurfaceColorTag>();
            glColor4f(myColor[0], myColor[1], myColor[2], myColor[3]*myAlpha);

            const asl::Vector2f & mySourceOrigin = myOverlay.get<SrcOriginTag>();
            const asl::Vector2f & mySourceSize   = myOverlay.get<SrcSizeTag>();

            unsigned myTextureCount = myMaterial->getTextureCount();

            // TODO: Workaround: switchMaterial enables vertex arrays for all vertex registers
            // but overlays do not use arrays
            for (unsigned i = 0; i < myTextureCount; ++i) {
                glActiveTextureARB(asGLTextureRegister(i));
                glClientActiveTextureARB(asGLTextureRegister(i));
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                CHECK_OGL_ERROR;
            }

            if (myTextureCount == 1) {
                glBegin(GL_QUADS);
                    glTexCoord2f(mySourceOrigin[0],mySourceOrigin[1]);
                    glVertex2f(myPosition[0], myPosition[1]);

                    glTexCoord2f(mySourceOrigin[0],mySourceOrigin[1]+mySourceSize[1]);
                    glVertex2f(myPosition[0], myPosition[1] + myHeight);

                    glTexCoord2f(mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]+mySourceSize[1]);
                    glVertex2f(myPosition[0] + myWidth, myPosition[1] + myHeight);

                    glTexCoord2f(mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]);
                    glVertex2f(myPosition[0] + myWidth, myPosition[1]);
                glEnd();
            } else {
                glBegin(GL_QUADS);
                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0],mySourceOrigin[1]);
                    }
                    glVertex2f(myPosition[0], myPosition[1]);

                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0],mySourceOrigin[1]+mySourceSize[1]);
                    }
                    glVertex2f(myPosition[0], myPosition[1] + myHeight);

                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]+mySourceSize[1]);
                    }
                    glVertex2f(myPosition[0] + myWidth, myPosition[1] + myHeight);

                    for (unsigned i = 0; i < myTextureCount; ++i) {
                        glMultiTexCoord2fARB(asGLTextureRegister(i), mySourceOrigin[0]+mySourceSize[0],mySourceOrigin[1]);
                    }
                    glVertex2f(myPosition[0] + myWidth, myPosition[1]);
                glEnd();
            }
        }

        // Render borders
        const bool & hasTopBorder    = myOverlay.get<TopBorderTag>();
        const bool & hasBottomBorder = myOverlay.get<BottomBorderTag>();
        const bool & hasLeftBorder   = myOverlay.get<LeftBorderTag>();
        const bool & hasRightBorder  = myOverlay.get<RightBorderTag>();

        if (hasTopBorder || hasBottomBorder || hasLeftBorder || hasRightBorder) {
            Vector4f myBorderColor = myOverlay.get<BorderColorTag>();
            myBorderColor[3] *= myAlpha;

            glDisable(GL_TEXTURE_2D);
            glColor4fv(myBorderColor.begin()); // border color
            glLineWidth(myOverlay.get<BorderWidthTag>());
            glBegin(GL_LINES);
            if (hasTopBorder) {
                glVertex2f(myPosition[0], myPosition[1]);
                glVertex2f(myPosition[0] + myWidth, myPosition[1]);
            }
            if (hasBottomBorder) {
                glVertex2f(myPosition[0], myPosition[1] + myHeight);
                glVertex2f(myPosition[0] + myWidth, myPosition[1] + myHeight);
            }
            if (hasLeftBorder) {
                glVertex2f(myPosition[0], myPosition[1]);
                glVertex2f(myPosition[0], myPosition[1] + myHeight);
            }
            if (hasRightBorder) {
                glVertex2f(myPosition[0] + myWidth, myPosition[1]);
                glVertex2f(myPosition[0] + myWidth, myPosition[1] + myHeight);
            }
            glEnd();

            if (_myState.getTexturing()) {
                glEnable(GL_TEXTURE_2D);
            }
        }

        // Render child overlays
        unsigned myOverlayCount = theOverlayNode->childNodesLength();
        for (unsigned i = 0; i < myOverlayCount; ++i) {
             renderOverlay(theOverlayNode->childNode(i), myPosition, myAlpha);
        }
    }

}

