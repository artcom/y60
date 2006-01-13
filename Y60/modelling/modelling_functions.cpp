//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: modelling_functions.cpp,v 1.27 2005/04/24 00:41:19 pavel Exp $
//   $RCSfile: modelling_functions.cpp,v $
//   $Author: pavel $
//   $Revision: 1.27 $
//   $Date: 2005/04/24 00:41:19 $
//
//=============================================================================

#include "modelling_functions.h"

#include <y60/TransformBuilder.h>
#include <y60/SceneBuilder.h>
#include <y60/ShapeBuilder.h>
#include <y60/ElementBuilder.h>
#include <y60/MaterialBuilder.h>
#include <y60/CanvasBuilder.h>

#include <y60/property_functions.h>
#include <y60/PropertyNames.h>

#include <asl/Matrix4.h>

#define DB(x) // x
#define DB2(x) // x

using namespace std;
using namespace asl;

namespace y60 {

    dom::NodePtr
    createTransform(dom::NodePtr theParentNode) {
        if (!theParentNode) {
              throw asl::Exception("createTransform:: theParentNode is null!");
        }
        WorldBuilderBase myParent(theParentNode);
        TransformBuilder myTransform("Transform");
        myParent.appendObject(myTransform);
        return myTransform.getNode();
    }

    dom::NodePtr
    createBody(dom::NodePtr theParentNode, const std::string & theShapeId) {
        dom::NodePtr myNode = dom::NodePtr(new dom::Element(BODY_NODE_NAME));
        myNode->appendAttribute(BODY_SHAPE_ATTRIB, theShapeId);
        myNode = theParentNode->appendChild(myNode);
        return myNode;
    }

    dom::NodePtr
    createCanvas(y60::ScenePtr theScene, const std::string & theCanvasName) {
        CanvasBuilder myCanvas(theCanvasName);
        theScene->getSceneBuilder()->appendCanvas(myCanvas);
        return myCanvas.getNode();
    }

    dom::NodePtr
    createQuad(y60::ScenePtr theScene, const std::string & theMaterialId,
        asl::Vector3f theTopLeftCorner,
        asl::Vector3f theBottomRightCorner)
    {
        float myZ = (theBottomRightCorner[2] - theTopLeftCorner[2]) / 2;
        asl::Vector3f myXVector(theBottomRightCorner[0] - theTopLeftCorner[0], 0, myZ);
        asl::Vector3f myYVector(0, theBottomRightCorner[1] - theTopLeftCorner[1], myZ);

        return createPlane(theScene, 2, 2, "myQuad", theMaterialId,
            QuadBuilder(),
            PlanePosition(asPoint(theTopLeftCorner), myXVector, myYVector),
            ConstNormal(),
            PlaneUVCoord(),
            WhiteColor());
    }


    dom::NodePtr createCrosshair(y60::ScenePtr theScene, const std::string & theMaterialId,
                                 float theInnerRadius, float theHairLength,
                                 const std::string & theName)
    {
        ShapeBuilder myShapeBuilder(theName);
        theScene->getSceneBuilder()->appendShape(myShapeBuilder);

        const unsigned myCircleSubdivision = 16;
        unsigned myCount = myCircleSubdivision + 4;
        float myStep = 2 * float(PI) / myCircleSubdivision;

        CircularPosition myPositionFunction(Vector3f(0.0, 0.0, 0.0), theInnerRadius, myStep);
        ElementBuilder myCircleElementBuilder(PRIMITIVE_TYPE_LINE_LOOP, theMaterialId);

        myShapeBuilder.appendElements(myCircleElementBuilder);

        // UH: the ShapeBuilder:: prefix is necessary for gcc to work
        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE, myCount);
        myCircleElementBuilder.createIndex(POSITION_ROLE, POSITIONS, myCircleSubdivision);


        for (unsigned i = 0; i < myCircleSubdivision; ++i) {

            Vector3f myPos = myPositionFunction(i);
            myShapeBuilder.appendVertexData(POSITION_ROLE, myPos);
            myCircleElementBuilder.appendIndex(POSITIONS, i);
        }

        myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(0.0, theHairLength, 0.0));
        myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(theHairLength, 0.0, 0.0));
        myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(0.0, - theHairLength, 0.0));
        myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f( - theHairLength, 0.0, 0.0));

        ElementBuilder myHairElementBuilder(PRIMITIVE_TYPE_LINES, theMaterialId);
        myHairElementBuilder.createIndex(POSITION_ROLE, POSITIONS, 8);
        myShapeBuilder.appendElements(myHairElementBuilder);

        myHairElementBuilder.appendIndex(POSITIONS, 0);
        myHairElementBuilder.appendIndex(POSITIONS, myCircleSubdivision);
        myHairElementBuilder.appendIndex(POSITIONS, 4);
        myHairElementBuilder.appendIndex(POSITIONS, myCircleSubdivision  + 1);
        myHairElementBuilder.appendIndex(POSITIONS, 8);
        myHairElementBuilder.appendIndex(POSITIONS, myCircleSubdivision  + 2);
        myHairElementBuilder.appendIndex(POSITIONS, 12);
        myHairElementBuilder.appendIndex(POSITIONS, myCircleSubdivision + 3);

        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createAngleMarkup(y60::ScenePtr theScene, const std::string & theMaterialId,
                         const asl::Vector3f & theApex,
                         const asl::Vector3f & thePointA,  const asl::Vector3f & thePointB,
                         bool theOuterAngleFlag, const std::string & theName)
    {

        asl::Vector3f myV1 = normalized( thePointA - theApex );
        asl::Vector3f myV2 = normalized( thePointB - theApex );

        float myAngle = acos( dot( myV1, myV2));
        if (theOuterAngleFlag) {
            myAngle = float(2 * PI) - myAngle;
        }

        float myLength1 = magnitude( thePointA - theApex );
        float myLength2 = magnitude( thePointB - theApex );

        float myRadius = asl::minimum(myLength1, myLength2) * 0.5f;

        // myU and myV are an orthonormal basis on the plane defined
        // by our three input points. We use this basis to create the
        // circle segment that indicates the angles direction.
        asl::Vector3f myU = normalized(myV1);
        asl::Vector3f myV = normalized( myV2 - (dot( myV1, myV2) * myV1));

        const unsigned myCircleSubdivision = 36;
        unsigned mySteps = unsigned( (myAngle / (2 * PI)) * myCircleSubdivision) + 1;
        float myStepSize = myAngle / (mySteps - 1); // fence post problem


        DB(AC_TRACE << "U = " << myU << " V = " << myV << " dot(myU, myV) = " << dot(myU, myV)
                << " steps = " << mySteps << endl);

        ShapeBuilder myShapeBuilder(theName);
        theScene->getSceneBuilder()->appendShape(myShapeBuilder);
        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE, 3 + mySteps);
        myShapeBuilder.appendVertexData(POSITION_ROLE, theApex);
        myShapeBuilder.appendVertexData(POSITION_ROLE, thePointA);
        myShapeBuilder.appendVertexData(POSITION_ROLE, thePointB);

        float myDirectionFactor(1.0);
        if (theOuterAngleFlag) {
            myDirectionFactor = -1.0;
        }
        for (unsigned i = 0; i < mySteps; ++i) {
            float myPhi = i * myStepSize;
            asl::Vector3f myCirclePos( theApex +
                                       myRadius * (cos(myPhi) * myU +
                                       myDirectionFactor * sin(myPhi) * myV));
            DB2(AC_TRACE << "===> " << myCirclePos << endl);
            myShapeBuilder.appendVertexData(POSITION_ROLE, myCirclePos);
        }


        ElementBuilder myElementBuilder(PRIMITIVE_TYPE_LINE_STRIP, theMaterialId);
        myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, 3);

        myElementBuilder.appendIndex(POSITIONS, 1);
        myElementBuilder.appendIndex(POSITIONS, 0);
        myElementBuilder.appendIndex(POSITIONS, 2);

        myShapeBuilder.appendElements( myElementBuilder );

        ElementBuilder myCircleElementBuilder(PRIMITIVE_TYPE_LINE_STRIP, theMaterialId);
        myCircleElementBuilder.createIndex(POSITION_ROLE, POSITIONS, mySteps);

        for (unsigned i = 0; i < mySteps; ++i) {
            myCircleElementBuilder.appendIndex(POSITIONS, i + 3);
        }
        myShapeBuilder.appendElements( myCircleElementBuilder );

        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createDistanceMarkup(y60::ScenePtr theScene, const std::string & theMaterialId,
                         const std::vector<asl::Vector3f> & thePositions,
                         const std::string & theName)
    {
        ShapeBuilder myShapeBuilder(theName);
        theScene->getSceneBuilder()->appendShape(myShapeBuilder);
        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE, 2);

        ElementBuilder myElementBuilder(PRIMITIVE_TYPE_LINE_STRIP, theMaterialId);
        myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, thePositions.size());

        for(unsigned i = 0; i < thePositions.size(); ++i) {
            myShapeBuilder.appendVertexData(POSITION_ROLE, thePositions[i]);
            myElementBuilder.appendIndex(POSITIONS, i);
        }

        myShapeBuilder.appendElements( myElementBuilder );
        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createLineStrip(y60::ScenePtr theScene, const std::string & theLineMaterialId,
                         const std::vector<asl::Vector3f> & thePositions,
                         const std::string & theName)
    {
        ShapeBuilder myShapeBuilder(theName);
        ElementBuilder myLineElementBuilder(PRIMITIVE_TYPE_LINE_STRIP, theLineMaterialId);

        theScene->getSceneBuilder()->appendShape(myShapeBuilder);

        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE,
                    thePositions.size());

        myLineElementBuilder.createIndex(POSITION_ROLE, POSITIONS, thePositions.size());

        for(unsigned i = 0; i < thePositions.size(); ++i) {
            myShapeBuilder.appendVertexData(POSITION_ROLE, thePositions[i]);
            myLineElementBuilder.appendIndex(POSITIONS, i);
        }

        myShapeBuilder.appendElements( myLineElementBuilder );
        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createTriangleMeshMarkup(y60::ScenePtr theScene, const std::string & theLineMaterialId,
                             const std::string & theAreaMaterialId,
                             const std::vector<asl::Vector3f> & thePositions,
                             const std::string & theName)
    {
        ShapeBuilder myShapeBuilder(theName);
        ElementBuilder myLineElementBuilder(PRIMITIVE_TYPE_LINES, theLineMaterialId);
        ElementBuilder myAreaElementBuilder(PRIMITIVE_TYPE_TRIANGLES, theAreaMaterialId);

        theScene->getSceneBuilder()->appendShape(myShapeBuilder);

        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE,
                    thePositions.size());

        myLineElementBuilder.createIndex(POSITION_ROLE, POSITIONS, thePositions.size());
        myAreaElementBuilder.createIndex(POSITION_ROLE, POSITIONS, thePositions.size());

        for(unsigned i = 0; i < thePositions.size(); ++i) {
            myShapeBuilder.appendVertexData(POSITION_ROLE, thePositions[i]);
            myAreaElementBuilder.appendIndex(POSITIONS, i);
        }

        unsigned myTriangleCount = thePositions.size() / 3;
        for (unsigned i = 0; i < myTriangleCount; ++i) {
            unsigned myBaseIndex = i * 3;
            myLineElementBuilder.appendIndex(POSITIONS, myBaseIndex);
            myLineElementBuilder.appendIndex(POSITIONS, myBaseIndex + 1);
            myLineElementBuilder.appendIndex(POSITIONS, myBaseIndex + 1);
            myLineElementBuilder.appendIndex(POSITIONS, myBaseIndex + 2);
            myLineElementBuilder.appendIndex(POSITIONS, myBaseIndex + 2);
            myLineElementBuilder.appendIndex(POSITIONS, myBaseIndex);
        }

        myShapeBuilder.appendElements( myAreaElementBuilder );
        myShapeBuilder.appendElements( myLineElementBuilder );
        return myShapeBuilder.getNode();
    }


    dom::NodePtr createQuadStack(ScenePtr theScene, const Vector3i & theDimensions,
                         float theSize, const std::string & theMaterialId,
                         const std::string theName)
    {
        int myHSlices = int(1.3 * (sqrt(float(theDimensions[0] * theDimensions[0] +
                                        theDimensions[1] * theDimensions[1] +
                                        theDimensions[2] * theDimensions[2]))) / 4.0);

        int myNumSlices = 2 * myHSlices + 1;
        int myVertexCount = myNumSlices * 4;
        // XXX wild guess ...
        float myDeltaDist = sqrt(theSize * theSize + theSize * theSize + theSize * theSize) / myNumSlices;

        ShapeBuilder myShapeBuilder(theName);
        ElementBuilder myElementBuilder(PRIMITIVE_TYPE_QUADS, theMaterialId);

        theScene->getSceneBuilder()->appendShape(myShapeBuilder);
        myShapeBuilder.appendElements(myElementBuilder);

        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE,
                    myVertexCount);
        myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, myVertexCount);

        int myVertexNum(0);
        float myHSize = myHSlices * myDeltaDist;
        for (int i = - myHSlices; i <= myHSlices; ++i) {
            float myDist = i * myDeltaDist;
            myShapeBuilder.appendVertexData(POSITION_ROLE, asl::Vector3f(- myHSize, - myHSize, myDist));
            myElementBuilder.appendIndex(POSITIONS, myVertexNum++);

            myShapeBuilder.appendVertexData(POSITION_ROLE, asl::Vector3f(  myHSize, - myHSize, myDist));
            myElementBuilder.appendIndex(POSITIONS, myVertexNum++);

            myShapeBuilder.appendVertexData(POSITION_ROLE, asl::Vector3f(  myHSize,   myHSize, myDist));
            myElementBuilder.appendIndex(POSITIONS, myVertexNum++);

            myShapeBuilder.appendVertexData(POSITION_ROLE, asl::Vector3f(- myHSize,   myHSize, myDist));
            myElementBuilder.appendIndex(POSITIONS, myVertexNum++);
        }
        return myShapeBuilder.getNode();
    }

    void
    appendTexture(SceneBuilder & theSceneBuilder,
                  ScenePtr theScene,
                  MaterialBuilder & theMaterialBuilder,
                  const std::string & theTextureFilename,
                  bool theSpriteFlag,
                  unsigned theDepth)
    {
        // Texture code
        string myTexName           = theMaterialBuilder.getName() + "_tex";
        string myApplyMode         = TEXTURE_APPLY_MODULATE;
        string myUsage             = TEXTURE_USAGE_PAINT;
        string myWrapMode          = TEXTURE_WRAP_CLAMP;
		string myUVMappingMode     = TEXCOORD_UV_MAP;
        float myRanking            = 100.0f;
        bool myIsFallback          = true;
        float myFallbackRanking    = 100.0f;
        bool myCreateMipmapFlag    = true;
        asl::Vector4f myColorScale = asl::Vector4f(1.0f,1.0f,1.0f,1.0f);
        asl::Vector4f myColorBias  = asl::Vector4f(0.0f,0.0f,0.0f,0.0f);
        bool  mySpriteFlag         = theSpriteFlag;

        string myImageId;
        if (theMaterialBuilder.isMovie(theTextureFilename)) {
            unsigned myLoopCount = 0;
            myImageId = theMaterialBuilder.createMovie(theSceneBuilder, myTexName,
                                                       theTextureFilename, myLoopCount,
                                                       myColorScale, myColorBias,
                                                       "");
        } else {
            myImageId = theMaterialBuilder.createImage(theSceneBuilder, myTexName,
                                                       theTextureFilename, myUsage,
                                                       myCreateMipmapFlag, myColorScale,
                                                       myColorBias, SINGLE, "", IMAGE_RESIZE_PAD, theDepth);
        }

        theMaterialBuilder.createTextureNode(myImageId, myApplyMode, myUsage, myWrapMode, myUVMappingMode,
                                             Matrix4f::Identity(), myRanking, myIsFallback,
                                             myFallbackRanking, mySpriteFlag);
    }

    void
    appendUnlitProperties(MaterialBuilder & theMaterialBuilder, const Vector4f & theColor) {
        VectorOfRankedFeature myLightingFeature;
        createLightingFeature(myLightingFeature, UNLIT);
        theMaterialBuilder.setType(myLightingFeature);

        setPropertyValue<asl::Vector4f>(theMaterialBuilder.getNode(), "vector4f",
                SURFACE_COLOR_PROPERTY, theColor);
    }
    void
    appendPhongProperties(MaterialBuilder & theMaterialBuilder,
        const PhongProperties & thePhongProperties)
    {
        VectorOfRankedFeature myLightingFeature;
        createLightingFeature(myLightingFeature, PHONG);
        theMaterialBuilder.setType(myLightingFeature);

        setPropertyValue<asl::Vector4f>(theMaterialBuilder.getNode(), "vector4f",
                AMBIENT_PROPERTY, thePhongProperties.ambientColor);

        setPropertyValue<asl::Vector4f>(theMaterialBuilder.getNode(), "vector4f",
                DIFFUSE_PROPERTY, thePhongProperties.diffuseColor);

        setPropertyValue<asl::Vector4f>(theMaterialBuilder.getNode(), "vector4f",
                EMISSIVE_PROPERTY, thePhongProperties.emissiveColor);

        setPropertyValue<asl::Vector4f>(theMaterialBuilder.getNode(), "vector4f",
                SPECULAR_PROPERTY, thePhongProperties.specularColor);

        setPropertyValue<float>(theMaterialBuilder.getNode(), "float",
                SHININESS_PROPERTY, thePhongProperties.shininess);
    }

    void
    appendBlendFunction(MaterialBuilder & theMaterialBuilder) {
        string myBlendFunction = "[src_alpha, one_minus_src_alpha]";
        setPropertyValue<VectorOfString>(theMaterialBuilder.getNode(), "vectorofstring",
            BLENDFUNCTION_PROPERTY, asl::as<VectorOfString>(myBlendFunction));
    }

    dom::NodePtr
    createPhongTexturedMaterial(ScenePtr theScene, const std::string & theTextureFilename,
                                const std::string & theName, const PhongProperties & thePhongProperties,
                                bool theTransparencyFlag, bool theSpriteFlag,
                                unsigned theDepth)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        string myMaterialId = theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(theTransparencyFlag);

        appendPhongProperties(myMaterialBuilder, thePhongProperties);
        appendBlendFunction(myMaterialBuilder);
        appendTexture(*(theScene->getSceneBuilder()), theScene, myMaterialBuilder, theTextureFilename, theSpriteFlag, theDepth);

        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    dom::NodePtr
    createUnlitTexturedColoredMaterial(ScenePtr theScene, const std::string & theTextureFilename,
                                const std::string & theName, bool theSpriteFlag, unsigned theDepth)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        string myMaterialId = theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(true);
        appendUnlitProperties(myMaterialBuilder, Vector4f(1.0, 1.0, 1.0, 1.0));
        appendBlendFunction(myMaterialBuilder);

        if ( ! theTextureFilename.empty()) {
            appendTexture(*(theScene->getSceneBuilder()), theScene, myMaterialBuilder,
                            theTextureFilename, theSpriteFlag, theDepth);
        }
        myMaterialBuilder.addFeature("vertexparams", VectorOfRankedFeature(1, RankedFeature(100,"color")));
        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }


    dom::NodePtr
    createUnlitTexturedMaterial(ScenePtr theScene, dom::NodePtr theImageNode,
                                const std::string & theName, bool theTransparencyFlag, bool theSpriteFlag, const Vector4f & theColor)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        string myMaterialId = theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(theTransparencyFlag);
        appendUnlitProperties(myMaterialBuilder, theColor);
        appendBlendFunction(myMaterialBuilder);

        // Texture code
        string myApplyMode         = TEXTURE_APPLY_MODULATE;
        string myUsage             = TEXTURE_USAGE_PAINT;
        string myWrapMode          = TEXTURE_WRAP_CLAMP;
		string myUVMappingMode     = TEXCOORD_UV_MAP;
        float myRanking            = 100.0f;
        bool myIsFallback          = true;
        float myFallbackRanking    = 100.0f;
        bool  mySpriteFlag         = theSpriteFlag;

        string myImageId = theImageNode->getAttributeString(ID_ATTRIB);
        myMaterialBuilder.createTextureNode(myImageId, myApplyMode, myUsage, myWrapMode, myUVMappingMode,
                                             Matrix4f::Identity(), myRanking, myIsFallback,
                                             myFallbackRanking, mySpriteFlag);

        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    dom::NodePtr
    createUnlitTexturedMaterial(ScenePtr theScene, const std::string & theTextureFilename,
                                const std::string & theName, bool theTransparencyFlag, bool theSpriteFlag,
                                unsigned theDepth, const Vector4f & theColor)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        string myMaterialId = theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(theTransparencyFlag);
        appendUnlitProperties(myMaterialBuilder, theColor);
        appendBlendFunction(myMaterialBuilder);

         if ( ! theTextureFilename.empty()) {
            appendTexture(*(theScene->getSceneBuilder()), theScene, myMaterialBuilder,
                            theTextureFilename, theSpriteFlag, theDepth);
        }
        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    dom::NodePtr
    createLambertMaterial(y60::ScenePtr theScene,
                        const asl::Vector4f & theDiffuseColor,
                        const asl::Vector4f & theAmbientColor)
    {
        MaterialBuilder myMaterialBuilder("myMaterial", false);
        string myMaterialId = theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        VectorOfRankedFeature myLightingFeature;
        createLightingFeature(myLightingFeature, LAMBERT);
        myMaterialBuilder.setType(myLightingFeature);

        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                DIFFUSE_PROPERTY, theDiffuseColor);
        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                AMBIENT_PROPERTY, theAmbientColor);
        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    dom::NodePtr
    createColorMaterial(y60::ScenePtr theScene,
                        const asl::Vector4f & theColor)
    {
        MaterialBuilder myMaterialBuilder("myMaterial", false);
        theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        appendUnlitProperties(myMaterialBuilder, theColor);
        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    Vector3f
    calcNormal(const Vector3f & A, const Vector3f & B, const Vector3f & C) {
        return normalized(cross(B-A,C-A));
    }

    void
    smoothQuadNormals(const VectorOfUnsignedInt & theNormalIndices,
                      const VectorOfUnsignedInt & thePositionIndices,
                      VectorOfVector3f & theNormals,
                      const VectorOfVector3f & thePositions)
    {
        for (unsigned i = 0;i < theNormalIndices.size(); i+=4) {
            const Vector3f & A = thePositions[thePositionIndices[i]];
            const Vector3f & B = thePositions[thePositionIndices[i+1]];
            const Vector3f & C = thePositions[thePositionIndices[i+2]];
            const Vector3f & D = thePositions[thePositionIndices[i+3]];
            theNormals[theNormalIndices[i]] += calcNormal(D,A,B);
            theNormals[theNormalIndices[i+1]] += calcNormal(A,B,C);
            theNormals[theNormalIndices[i+2]] += calcNormal(B,C,D);
            theNormals[theNormalIndices[i+3]] += calcNormal(C,D,A);
        }
    }
    void
    smoothNormals(dom::NodePtr theShape) {
        dom::NodePtr myVertexDataList = theShape->childNode(VERTEX_DATA_NAME);
        dom::NodePtr myPositionsNode = myVertexDataList->childNodeByAttribute(SOM_VECTOR_VECTOR3F_NAME,
               NAME_ATTRIB, POSITION_ROLE);
        dom::NodePtr myNormalsNode = myVertexDataList->childNodeByAttribute(SOM_VECTOR_VECTOR3F_NAME,
               NAME_ATTRIB, NORMAL_ROLE);

        // get the positions and normals
        const VectorOfVector3f & myPositions = myPositionsNode->firstChild()->nodeValueAs<VectorOfVector3f>();
        dom::NodePtr myNormalsPtr(myNormalsNode->firstChild());

        dom::Node::WritableValue<VectorOfVector3f> myNormalsLock(myNormalsPtr);
        VectorOfVector3f & myNormals = myNormalsLock.get();
        // loop through all primitives/elements,
        // collect the index arrays
        dom::NodePtr myPrimitivesNode = theShape->childNode(PRIMITIVE_LIST_NAME);
        int myElementCount = myPrimitivesNode->childNodesLength();

        vector<const VectorOfUnsignedInt*> myPositionIndices(myElementCount);
        vector<const VectorOfUnsignedInt*> myNormalIndices(myElementCount);

        for (unsigned int i = 0; i < myElementCount; ++i) {
            dom::NodePtr myElementNode = myPrimitivesNode->childNode(i);
            if (myElementNode->nodeName() == ELEMENTS_NODE_NAME) {
                dom::NodePtr myElementPositionIndices =
                    myElementNode->childNodeByAttribute(VERTEX_INDICES_NAME, VERTEX_DATA_ROLE_ATTRIB, POSITION_ROLE);
                myPositionIndices[i] = &myElementPositionIndices->firstChild()->nodeValueRef<VectorOfUnsignedInt>();

                dom::NodePtr myElementNormalIndices =
                    myElementNode->childNodeByAttribute(VERTEX_INDICES_NAME, VERTEX_DATA_ROLE_ATTRIB, NORMAL_ROLE);
                myNormalIndices[i] = &myElementNormalIndices->firstChild()->nodeValueRef<VectorOfUnsignedInt>();
            }
        }
        // clear the normals
        for (unsigned int i=0; i < myNormals.size(); ++i) {
            myNormals[i] = Vector3f(0,0,0);
        }
        // sum up the normals for each element
        for (unsigned int i = 0; i < myElementCount; ++i) {
            dom::NodePtr myElementNode = myPrimitivesNode->childNode(ELEMENTS_NODE_NAME, i);
            PrimitiveType myType = Primitive::getTypeFromNode(myElementNode);
            switch (myType) {
                case QUADS:
                    smoothQuadNormals(*myNormalIndices[i], *myPositionIndices[i],
                            myNormals, myPositions);
                    break;
                default:
                    throw asl::Exception(std::string("TODO: smoothNormals - support more primitive types:") +
                        asl::as_string(*theShape), PLUS_FILE_LINE);
            }
        }
        // clean the normals, dirty the shape
        for (unsigned int i=0; i < myNormals.size(); ++i) {
            myNormals[i] = normalized(myNormals[i]);
        }
    }

}
