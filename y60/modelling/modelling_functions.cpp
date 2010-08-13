/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
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

// own header
#include "modelling_functions.h"

#include "SimpleTesselator.h"

#include <y60/scene/TransformBuilder.h>
#include <y60/scene/SceneBuilder.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/ElementBuilder.h>
#include <y60/scene/MaterialBuilder.h>
#include <y60/scene/CanvasBuilder.h>

#include <y60/base/property_functions.h>
#include <y60/base/PropertyNames.h>
#include <asl/base/Assure.h>
#include <asl/math/Matrix4.h>

#define DB(x) // x
#define DB2(x) // x

using namespace std;
using namespace asl;

namespace y60 {

    dom::NodePtr
    createTransform(dom::NodePtr theParentNode, const std::string & theTransformName) {
        if (!theParentNode) {
            throw asl::Exception("createTransform:: theParentNode is null!");
        }
        WorldBuilderBase myParent(theParentNode);
        TransformBuilder myTransform(theTransformName);
        myParent.appendObject(myTransform);
        return myTransform.getNode();
    }

    dom::NodePtr
    createBody(dom::NodePtr theParentNode, const std::string & theShapeId) {
        if (!theParentNode) {
            throw asl::Exception("createBody:: theParentNode is null!");
        }
        dom::NodePtr myNode = dom::NodePtr(new dom::Element(BODY_NODE_NAME));
        myNode->appendAttribute(BODY_SHAPE_ATTRIB, theShapeId);
        myNode = theParentNode->appendChild(myNode);
        return myNode;
    }

    bool
    setAlpha(dom::Node * theNode, float theAlpha) {
        if (theNode->nodeName() == "body") {
            AC_DEBUG << "Fetching shape from body: " << *theNode;
            theNode = theNode->getElementById(theNode->getAttributeString("shape")).get();
        }

        dom::Node * myShapeNode;
        dom::NodePtr myMaterial;

        if (theNode->nodeName() == "shape") {
            myShapeNode = theNode;
            myMaterial = theNode->getElementById(theNode->childNode("primitives",0)->childNode("elements",0)->getAttributeString("material"));
        } else if (theNode->nodeName() == "elements") {
            AC_DEBUG << "Parent of element: " << *(theNode->parentNode());
            AC_DEBUG << "Grand-parent of element: " << *(theNode->parentNode()->parentNode());
            myShapeNode = theNode->parentNode()->parentNode();
            myMaterial = theNode->getElementById(theNode->getAttributeString("material"));
        } else {
            AC_ERROR << "Unsupported node type '" << theNode->nodeName() << "'";
            return false;
        }
        ShapePtr myShape = myShapeNode->getFacade<Shape>();

        dom::NodePtr myVertexDataNode = myShape->getNode().childNode("vertexdata", 0);
        if (!myVertexDataNode) {
            AC_ERROR << "Shape '" << myShape->get<NameTag>() << "' has no 'vertexdata'";
            return false;
        }

        dom::NodePtr myVertexParamColorNode;
        if( myMaterial ) {
            myVertexParamColorNode = myMaterial->childNode("requires",0)->childNodeByAttribute(FEATURE_NODE_NAME, NAME_ATTRIB, "vertexparams");
        } else {
            AC_ERROR << "sth is wrong when you don't find the material";
            return false;
        }

        // XXX go through all elements of that shape

        if( myVertexParamColorNode ) {
            VectorOfRankedFeature myVertexParam = myVertexParamColorNode->firstChild()->nodeValueRef<VectorOfRankedFeature>();
            if( myVertexParam[0]._myFeature[0] == "color") {
                dom::NodePtr myColorsNode = myVertexDataNode->childNodeByAttribute(SOM_VECTOR_VECTOR4F_NAME, NAME_ATTRIB, COLOR_ROLE);
                if (!myColorsNode) {
                    AC_WARNING << "Shape '" << myShape->get<NameTag>() << "' has no vertex colors";
                    return false;
                }

                VectorOfVector4f & myColors = myColorsNode->firstChild()->nodeValueRefOpen<VectorOfVector4f>();
                if (theNode->nodeName() == "shape") {
                    // set all vertex colors
                    for (unsigned i = 0; i < myColors.size(); ++i) {
                        myColors[i][3] = theAlpha;
                    }
                } else {
                    // set only vertices that this element uses
                    dom::NodePtr myColorIndicesNode = theNode->childNodeByAttribute("indices", "role", "color");
                    if (myColorIndicesNode) {
                        VectorOfUnsignedInt myColorIndices = myColorIndicesNode->firstChild()->nodeValueRefOpen<VectorOfUnsignedInt>();
                        for (unsigned i = 0; i < myColorIndices.size(); ++i) {
                            myColors[myColorIndices[i]][3] = theAlpha;
                        }
                        myColorIndicesNode->firstChild()->nodeValueRefClose<VectorOfUnsignedInt>();
                    } else {
                        AC_WARNING << "Element has no vertex color indices";
                    }
                }
                myColorsNode->firstChild()->nodeValueRefClose<VectorOfVector4f>();

                return true;
            }

        } else {
            return setAlphaByChangingMaterialColor(myMaterial, theAlpha);
        }
        return false;
    }

    bool
    setAlphaByChangingMaterialColor(dom::NodePtr theMaterial, float theAlpha) {

        dom::NodePtr myVertexParamColorNode = theMaterial->childNode("requires",0)->childNodeByAttribute(FEATURE_NODE_NAME, NAME_ATTRIB, "lighting");
        VectorOfRankedFeature myVertexParam = myVertexParamColorNode->firstChild()->nodeValueRef<VectorOfRankedFeature>();
        if( myVertexParam[0]._myFeature[0] == "unlit") {
            dom::NodePtr mySurfaceColorsNode = theMaterial->childNode("properties",0)->childNodeByAttribute("vector4f", "name", "surfacecolor");
            Vector4f & mySurfaceColor = mySurfaceColorsNode->firstChild()->nodeValueRefOpen<Vector4f>();
            mySurfaceColor[3] = theAlpha;
            mySurfaceColorsNode->firstChild()->nodeValueRefClose<Vector4f>();
        } else {
            dom::NodePtr myDiffuseColorsNode = theMaterial->childNode("properties",0)->childNodeByAttribute("vector4f", "name", "diffuse");
            Vector4f & myDiffuseColor = myDiffuseColorsNode->firstChild()->nodeValueRefOpen<Vector4f>();
            myDiffuseColor[3] = theAlpha;
            myDiffuseColorsNode->firstChild()->nodeValueRefClose<Vector4f>();

            dom::NodePtr myAmbientColorsNode = theMaterial->childNode("properties",0)->childNodeByAttribute("vector4f", "name", "ambient");
            Vector4f & myAmbientColor = myAmbientColorsNode->firstChild()->nodeValueRefOpen<Vector4f>();
            myAmbientColor[3] = theAlpha;
            myAmbientColorsNode->firstChild()->nodeValueRefClose<Vector4f>();

            dom::NodePtr mySpecularColorsNode = theMaterial->childNode("properties",0)->childNodeByAttribute("vector4f", "name", "specular");
            Vector4f & mySpecularColor = mySpecularColorsNode->firstChild()->nodeValueRefOpen<Vector4f>();
            mySpecularColor[3] = theAlpha;
            mySpecularColorsNode->firstChild()->nodeValueRefClose<Vector4f>();

            dom::NodePtr myEmissiveColorsNode = theMaterial->childNode("properties",0)->childNodeByAttribute("vector4f", "name", "emissive");
            Vector4f & myEmissiveColor = myEmissiveColorsNode->firstChild()->nodeValueRefOpen<Vector4f>();
            myEmissiveColor[3] = theAlpha;
            myEmissiveColorsNode->firstChild()->nodeValueRefClose<Vector4f>();

        }
        return true;
    }

    dom::NodePtr
    createCanvas(ScenePtr theScene, const std::string & theCanvasName) {
        CanvasBuilder myCanvas(theCanvasName);
        theScene->getSceneBuilder()->appendCanvas(myCanvas);
        return myCanvas.getNode();
    }

    dom::NodePtr
    createQuad(ScenePtr theScene, const std::string & theMaterialId,
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

    dom::NodePtr
    createPlane(ScenePtr theScene, const std::string & theMaterialId,
        asl::Vector3f theTopLeftCorner, asl::Vector3f theBottomRightCorner,
        int theHSubdivision, int theVSubdivision)
    {
        float myZ = (theBottomRightCorner[2] - theTopLeftCorner[2]) / 2;
        asl::Vector3f myXVector(theBottomRightCorner[0] - theTopLeftCorner[0], 0, myZ);
        asl::Vector3f myYVector(0, theBottomRightCorner[1] - theTopLeftCorner[1], myZ);

        Vector3f myXStep = myXVector * ( 1.0 / theHSubdivision );
        Vector3f myYStep = myYVector * ( 1.0 / theVSubdivision );

        Vector2f myUStep( 1.0f / theHSubdivision, 0.0f);
        Vector2f myVStep( 0.0f, 1.0f / theVSubdivision);

        return createPlane(theScene, theHSubdivision +1, theVSubdivision + 1, "myPlane", theMaterialId,
            QuadBuilder(),
            PlanePosition(asPoint(theTopLeftCorner), myXStep, myYStep),
            ConstNormal(),
            PlaneUVCoord(Point2f(0.0, 0.0), myUStep, myVStep ),
            WhiteColor());
    }

    dom::NodePtr
    createSphericalPlane(ScenePtr theScene, const std::string & theMaterialId,
        asl::Sphere<float> theSphere,
        asl::Box2<float> thePolarBounds,
        asl::Vector2<unsigned> theSubdivision)
    {
AC_DEBUG << "createSphericalPlane:" << " theSphere = " << theSphere << ", thePolarBounds = " << thePolarBounds << ", theSubdivision = " << theSubdivision;
        asl::Vector2f mySubDivision = asl::Vector2f( static_cast<float>(theSubdivision[0])
                                                   , static_cast<float>(theSubdivision[1]) );
        asl::Vector2f myPolarUVector = asl::Vector2f(thePolarBounds.getSize()[0], 0)/mySubDivision;
        asl::Vector2f myPolarVVector = asl::Vector2f(0, thePolarBounds.getSize()[1])/mySubDivision;

        Vector2f myUStep( 1.0f / mySubDivision[0], 0.0f);
        Vector2f myVStep( 0.0f, 1.0f / mySubDivision[1]);

AC_DEBUG << "createSphericalPlane:" << " myPolarUVector = " << myPolarUVector << ", myPolarVVector = " << myPolarVVector << ", mySubDivision = " << mySubDivision;

        return createPlane(theScene, theSubdivision[0]+1, theSubdivision[1]+1, "mySpherical", theMaterialId,
            QuadBuilder(),
            SphericalPosition(theSphere,thePolarBounds.getMin(),myPolarUVector,myPolarVVector),
            SphericalNormal(theSphere,thePolarBounds.getMin(),myPolarUVector,myPolarVVector),
            PlaneUVCoord(Point2f(0.0, 0.0), myUStep, myVStep ),
            WhiteColor());
    }

    dom::NodePtr createCrosshair(ScenePtr theScene, const std::string & theMaterialId,
        float theInnerRadius, float theHairLength,
        const std::string & theName)
    {
        ShapeBuilder myShapeBuilder(theName);
        theScene->getSceneBuilder()->appendShape(myShapeBuilder);

        const unsigned myCircleSubdivision = 16;
        unsigned myCount = myCircleSubdivision + 4;
        float myStep = 2 * float(PI) / myCircleSubdivision;

        CircularPosition myPositionFunction(Vector3f(0.0, 0.0, 0.0), theInnerRadius, myStep);
        ElementBuilder myCircleElementBuilder(PrimitiveTypeStrings[LINE_LOOP], theMaterialId);

        myShapeBuilder.appendElements(myCircleElementBuilder);

        // the ShapeBuilder:: prefix is necessary for gcc to work
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

        ElementBuilder myHairElementBuilder(PrimitiveTypeStrings[LINES], theMaterialId);
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
    createAngleMarkup(ScenePtr theScene, const std::string & theMaterialId,
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


        ElementBuilder myElementBuilder(PrimitiveTypeStrings[LINE_STRIP], theMaterialId);
        myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, 3);

        myElementBuilder.appendIndex(POSITIONS, 1);
        myElementBuilder.appendIndex(POSITIONS, 0);
        myElementBuilder.appendIndex(POSITIONS, 2);

        myShapeBuilder.appendElements( myElementBuilder );

        ElementBuilder myCircleElementBuilder(PrimitiveTypeStrings[LINE_STRIP], theMaterialId);
        myCircleElementBuilder.createIndex(POSITION_ROLE, POSITIONS, mySteps);

        for (unsigned i = 0; i < mySteps; ++i) {
            myCircleElementBuilder.appendIndex(POSITIONS, i + 3);
        }
        myShapeBuilder.appendElements( myCircleElementBuilder );

        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createPartialDisk(y60::ScenePtr theScene, const std::string & theMaterialId,
                      float theRadius, int theSteps, float theStartDegrees, float theSweepDegrees, const std::string & theName)
    {
        // normalize sweepAngle
        if (theSweepDegrees < -360.0) theSweepDegrees = 360.0;
        if (theSweepDegrees > 360.0)  theSweepDegrees = 360.0;

        // make angles absolute
        if (theSweepDegrees < 0) {
            theStartDegrees += theSweepDegrees;
            theSweepDegrees = -theSweepDegrees;
        }

        // direction factor (outer vs inner angles)
        const float myDirectionFactor = (theSweepDegrees >= 180.0 && theSweepDegrees < 360.0) ? -1.0f : +1.0f;

        // convert to radians
        const float myStartRadians = static_cast<float>(theStartDegrees * ( PI/180 ));
        const float mySweepRadians = static_cast<float>(theSweepDegrees * ( PI/180 ));
        const float myEndRadians   = myStartRadians + mySweepRadians;

        // positioning
        asl::Vector3f myPointA( sin(myStartRadians) * theRadius, cos(myStartRadians) * theRadius, 0);
        asl::Vector3f myPointB( sin(myEndRadians)   * theRadius, cos(myEndRadians)   * theRadius, 0);
        asl::Vector3f myPointC( 0, 0, 0 );
        asl::Vector3f myV1 = normalized( myPointA );
        asl::Vector3f myV2 = normalized( myPointB );
        asl::Vector3f myU = normalized( myV1 );
        asl::Vector3f myV = normalized( myV2 - (dot(myV1, myV2) * myV1));

        // tesselation
        const unsigned myFullCircleSubdivisions = theSteps;
        const unsigned myDiskSubdivisions = maximum<unsigned>(1, round<float>(static_cast<float>(myFullCircleSubdivisions / (2*PI / mySweepRadians))));
        const unsigned myDiskVertices     = myDiskSubdivisions + 1;
        const float    myVertexRadians    = (myDiskSubdivisions > 1) ? mySweepRadians / myDiskSubdivisions : mySweepRadians;

        // compute the vertices
        ShapeBuilder myShapeBuilder(theName);
        theScene->getSceneBuilder()->appendShape(myShapeBuilder);
        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE, 1 + myDiskVertices);
        myShapeBuilder.appendVertexData(POSITION_ROLE, myPointC);

        for(int i = (myDiskVertices - 1); i >= 0; i--) {
            float myPhi = i * myVertexRadians;
            asl::Vector3f myVertex(myPointC + theRadius * (cos(myPhi) * myU + myDirectionFactor * sin(myPhi) * myV));
            myShapeBuilder.appendVertexData(POSITION_ROLE, myVertex);
        }

        // build the triangle fan
        ElementBuilder myCircleElementBuilder(PrimitiveTypeStrings[TRIANGLE_FAN], theMaterialId);
        myCircleElementBuilder.createIndex(POSITION_ROLE, POSITIONS, 1 + myDiskSubdivisions);
        myCircleElementBuilder.appendIndex(POSITIONS, 0);

        for (unsigned i = 0; i < myDiskVertices; i++) {
            myCircleElementBuilder.appendIndex(POSITIONS, 1 + i);
        }

        myShapeBuilder.appendElements( myCircleElementBuilder );

        // return the result
        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createDistanceMarkup(ScenePtr theScene, const std::string & theMaterialId,
            const std::vector<asl::Vector3f> & thePositions,
            const std::string & theName)
    {
        ShapeBuilder myShapeBuilder(theName);
        theScene->getSceneBuilder()->appendShape(myShapeBuilder);
        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE, 2);

        ElementBuilder myElementBuilder(PrimitiveTypeStrings[LINE_STRIP], theMaterialId);
        myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, thePositions.size());

        for(unsigned i = 0; i < thePositions.size(); ++i) {
            myShapeBuilder.appendVertexData(POSITION_ROLE, thePositions[i]);
            myElementBuilder.appendIndex(POSITIONS, i);
        }

        myShapeBuilder.appendElements( myElementBuilder );
        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createLineStrip(ScenePtr theScene, const std::string & theMaterialId,
            const std::vector<asl::Vector3f> & thePositions,
            const std::vector<asl::Vector2f> & theTexCoords,
            const std::vector<asl::Vector4f> & theColors)
    {
        return createStrip(PrimitiveTypeStrings[LINE_STRIP], theScene,
                theMaterialId, thePositions, theTexCoords, theColors);
    }

    dom::NodePtr
    createQuadStrip(ScenePtr theScene, const std::string & theMaterialId,
            const std::vector<asl::Vector3f> & thePositions,
            const std::vector<asl::Vector2f> & theTexCoords,
            const std::vector<asl::Vector4f> & theColors)
    {
        return createStrip(PrimitiveTypeStrings[QUAD_STRIP], theScene,
                theMaterialId, thePositions, theTexCoords, theColors);
    }

    dom::NodePtr
    createStrip(const std::string & theType, ScenePtr theScene,
            const std::string & theMaterialId,
            const std::vector<asl::Vector3f> & thePositions,
            const std::vector<asl::Vector2f> & theTexCoords,
            const std::vector<asl::Vector4f> & theColors)
    {
        bool needColors = theColors.size() !=0;
        bool needsNormals = false;
        if (theType == PrimitiveTypeStrings[QUAD_STRIP] || theType == PrimitiveTypeStrings[TRIANGLE_STRIP]) {
            needsNormals = true;
        }

        string myDefaultName = string("my") + theType;
        ShapeBuilder myShapeBuilder(myDefaultName);
        ElementBuilder myElementBuilder(theType, theMaterialId);

        theScene->getSceneBuilder()->appendShape(myShapeBuilder);

        myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(POSITION_ROLE,
                thePositions.size());
        myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, thePositions.size());

        if ( ! theTexCoords.empty()) {
            if (theTexCoords.size() != thePositions.size()) {
                throw asl::Exception(string("createStrip:: theTexCoords count does not match thePosition count: ") + asl::as_string(theTexCoords.size()) + " vs " + asl::as_string(thePositions.size()) );

            }
            myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector2f>("uvset", theTexCoords.size());
            myElementBuilder.createIndex("uvset", getTextureRole(0), theTexCoords.size());
        }

        if (needColors) {
            if (theColors.size() != thePositions.size()) {
                throw asl::Exception(string("createStrip:: theColors count does not match thePosition count: ") + asl::as_string(theColors.size()) + " vs " + asl::as_string(thePositions.size()) );
            }
            myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector4f>(COLOR_ROLE, theColors.size());
            myElementBuilder.createIndex(COLOR_ROLE, COLORS, theColors.size());
        }
        if (needsNormals) {
            myShapeBuilder.ShapeBuilder::createVertexDataBin<asl::Vector3f>(NORMAL_ROLE, thePositions.size());
            myElementBuilder.createIndex(NORMAL_ROLE, NORMALS, thePositions.size());
        }

        for(unsigned i = 0; i < thePositions.size(); ++i) {
            myShapeBuilder.appendVertexData(POSITION_ROLE, thePositions[i]);
            myElementBuilder.appendIndex(POSITIONS, i);

            if (needsNormals) {
                asl::Vector3f myNormal;
                int ii = i;
                if (i >= thePositions.size()-2) {
                    ii = (i%2 == 0) ? i-1 : i-2;
                }
                const asl::Vector3f & myV1 = (ii%2 == 0) ?
                    thePositions[ii+1] : thePositions[ii+2];
                const asl::Vector3f & myV2 = (ii%2 == 0) ?
                    thePositions[ii+2] : thePositions[ii+1];
                myNormal = cross(difference(myV1, thePositions[ii]),
                        difference(myV2, thePositions[ii]) );
                myNormal = normalized(myNormal);

                myShapeBuilder.appendVertexData(NORMAL_ROLE, myNormal);
                myElementBuilder.appendIndex(NORMALS, i);
            }
            if ( ! theTexCoords.empty()) {
                myShapeBuilder.appendVertexData("uvset", theTexCoords[i]);
                myElementBuilder.appendIndex(getTextureRole(0), i);
            }
            if (needColors) {
                myShapeBuilder.appendVertexData(COLOR_ROLE, theColors[i]);
                myElementBuilder.appendIndex(COLORS, i);
            }
        }

        myShapeBuilder.appendElements( myElementBuilder );
        return myShapeBuilder.getNode();
    }

    dom::NodePtr
    createTriangleMeshMarkup(ScenePtr theScene, const std::string & theLineMaterialId,
            const std::string & theAreaMaterialId,
            const std::vector<asl::Vector3f> & thePositions,
            const std::string & theName)
    {
        ShapeBuilder myShapeBuilder(theName);
        ElementBuilder myLineElementBuilder(PrimitiveTypeStrings[LINES], theLineMaterialId);
        ElementBuilder myAreaElementBuilder(PrimitiveTypeStrings[TRIANGLES], theAreaMaterialId);

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


    dom::NodePtr
    createQuadStack(ScenePtr theScene, const Vector3i & theDimensions,
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
        ElementBuilder myElementBuilder(PrimitiveTypeStrings[QUADS], theMaterialId);

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
    appendTexture(ScenePtr theScene,
            MaterialBuilder & theMaterialBuilder,
            const std::string & theTextureFilename,
            bool theSpriteFlag,
            unsigned theDepth)
    {
        // Texture code
        string myTexName             = theMaterialBuilder.getName() + "_tex";
        TextureApplyMode myApplyMode = MODULATE;
        string myUVMappingMode       = TEXCOORD_UV_MAP;
        TextureUsage myUsage         = PAINT;
        TextureWrapMode myWrapMode   = CLAMP_TO_EDGE;
        bool myCreateMipmapFlag      = true;
        float myRanking              = 100.0f;
        bool myIsFallback            = true;
        float myFallbackRanking      = 100.0f;
        asl::Vector4f myColorScale   = asl::Vector4f(1.0f,1.0f,1.0f,1.0f);
        asl::Vector4f myColorBias    = asl::Vector4f(0.0f,0.0f,0.0f,0.0f);
        bool  mySpriteFlag           = theSpriteFlag;

        dom::NodePtr myNode;
        if (theMaterialBuilder.isMovie(theTextureFilename)) {
            myNode = theMaterialBuilder.createMovieNode(*(theScene->getSceneBuilder()), myTexName,
                    theTextureFilename, 0);
        } else {
            myNode = theMaterialBuilder.createImageNode(*(theScene->getSceneBuilder()), myTexName,
                    theTextureFilename, myUsage,
                    SINGLE, IMAGE_RESIZE_PAD, theDepth, false);
        }
        string myImageId = myNode->getAttributeString(ID_ATTRIB);

        myNode = theMaterialBuilder.createTextureNode(*(theScene->getSceneBuilder()),
                myTexName, myImageId,
                myWrapMode, myCreateMipmapFlag,
                Matrix4f::Identity(), "", myColorScale, myColorBias);
        string myTextureId = myNode->getAttributeString(ID_ATTRIB);

        myNode = theMaterialBuilder.createTextureUnitNode(myTextureId,
                myApplyMode, myUsage, myUVMappingMode, Matrix4f::Identity(), mySpriteFlag,
                myRanking, myIsFallback, myFallbackRanking);

        /*theMaterialBuilder.createTextureNode(myImageId, myApplyMode, myUsage, myUVMappingMode,
          Matrix4f::Identity(), myRanking, myIsFallback,
          myFallbackRanking, mySpriteFlag);*/
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
        VectorOfBlendFunction myBlendFunctions;
        myBlendFunctions.push_back(SRC_ALPHA);
        myBlendFunctions.push_back(ONE_MINUS_SRC_ALPHA);
        setPropertyValue<VectorOfBlendFunction>(theMaterialBuilder.getNode(), "vectorofblendfunction",
                BLENDFUNCTION_PROPERTY, myBlendFunctions);
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
 //       if ( ! theTextureFilename.empty() ) {
            appendTexture(theScene, myMaterialBuilder, theTextureFilename,
                          theSpriteFlag, theDepth);
//        }

        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    dom::NodePtr
    createPhongMaterial(ScenePtr theScene,
                                const std::string & theName, const PhongProperties & thePhongProperties,
                                bool theTransparencyFlag, bool theSpriteFlag)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        string myMaterialId = theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(theTransparencyFlag);
        appendPhongProperties(myMaterialBuilder, thePhongProperties);
        appendBlendFunction(myMaterialBuilder);
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
            appendTexture(theScene, myMaterialBuilder,
                    theTextureFilename, theSpriteFlag, theDepth);
        }
        myMaterialBuilder.addFeature("vertexparams", VectorOfRankedFeature(1, RankedFeature(100,"color")));
        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }



    dom::NodePtr
    createUnlitTexturedMaterial(ScenePtr theScene,
                                dom::NodePtr theImageOrTextureNode,
                                const std::string & theName,
                                bool theTransparencyFlag,
                                bool theSpriteFlag,
                                const Vector4f & theColor)
    {
        // enforce image load
        MaterialBuilder myMaterialBuilder(theName, false);
        string myMaterialId = theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(theTransparencyFlag);
        appendUnlitProperties(myMaterialBuilder, theColor);
        appendBlendFunction(myMaterialBuilder);

        // Texture code
        TextureApplyMode myApplyMode = MODULATE;
        string myUVMappingMode       = TEXCOORD_UV_MAP;
        TextureUsage myUsage         = PAINT;
        TextureWrapMode myWrapMode   = CLAMP_TO_EDGE;
        bool myCreateMipmapFlag      = true;
        float myRanking              = 100.0f;
        bool myIsFallback            = true;
        float myFallbackRanking      = 100.0f;
        bool  mySpriteFlag           = theSpriteFlag;

        string myTextureId = theImageOrTextureNode->getAttributeString(ID_ATTRIB);
        if (theImageOrTextureNode->nodeName() == IMAGE_NODE_NAME ||
            theImageOrTextureNode->nodeName() == MOVIE_NODE_NAME)
        {
            theImageOrTextureNode->getFacade<Image>()->get<ImageWidthTag>();
            string myImageId = theImageOrTextureNode->getAttributeString(ID_ATTRIB);
            dom::NodePtr myTextureNode = myMaterialBuilder.createTextureNode(*(theScene->getSceneBuilder()),
                    theName, myImageId,
                    myWrapMode, myCreateMipmapFlag,
                    Matrix4f::Identity(), "");
            myTextureId = myTextureNode->getAttributeString(ID_ATTRIB);
            theScene->getTexturesRoot()->appendChild(myTextureNode);
        }

        dom::NodePtr myTextureUnitNode = myMaterialBuilder.createTextureUnitNode(myTextureId,
                myApplyMode, myUsage, myUVMappingMode, Matrix4f::Identity(), mySpriteFlag,
                myRanking, myIsFallback, myFallbackRanking);

        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    dom::NodePtr
    createUnlitTexturedMaterial(ScenePtr theScene, const std::string & theTextureFilename,
            const std::string & theName, bool theTransparencyFlag, bool theSpriteFlag,
            unsigned theDepth, const Vector4f & theColor)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(theTransparencyFlag);
        appendUnlitProperties(myMaterialBuilder, theColor);
        appendBlendFunction(myMaterialBuilder);

        if ( ! theTextureFilename.empty()) {
            appendTexture( theScene, myMaterialBuilder,
                    theTextureFilename, theSpriteFlag, theDepth);
        }
        myMaterialBuilder.computeRequirements();
        return myMaterialBuilder.getNode();
    }

    dom::NodePtr
    createLambertMaterial(ScenePtr theScene,
            const asl::Vector4f & theDiffuseColor,
            const asl::Vector4f & theAmbientColor,
            const std::string & theName)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
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
    createColorMaterial(ScenePtr theScene,
            const asl::Vector4f & theColor,
            const std::string & theName,
            bool theTransparencyFlag)
    {
        MaterialBuilder myMaterialBuilder(theName, false);
        theScene->getSceneBuilder()->appendMaterial(myMaterialBuilder);
        myMaterialBuilder.setTransparencyFlag(theTransparencyFlag);
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
        dom::NodeList::size_type myElementCount = myPrimitivesNode->childNodesLength();

        vector<const VectorOfUnsignedInt*> myPositionIndices(myElementCount);
        vector<const VectorOfUnsignedInt*> myNormalIndices(myElementCount);

        for (dom::NodeList::size_type i = 0; i < myElementCount; ++i) {
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
        for (VectorOfVector3f::size_type i=0; i < myNormals.size(); ++i) {
            myNormals[i] = Vector3f(0,0,0);
        }
        // sum up the normals for each element
        for (dom::NodeList::size_type  i = 0; i < myElementCount; ++i) {
            dom::NodePtr myElementNode = myPrimitivesNode->childNode(ELEMENTS_NODE_NAME, i);
            //PrimitiveType myType = Primitive::getTypeFromNode(myElementNode);
            PrimitiveType myType = myElementNode->getFacade<Primitive>()->get<PrimitiveTypeTag>();
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

    enum CornerNames {
        LTF, RBF, RTF, LBF,
        LTBK, RBBK, RTBK, LBBK,
        MAX_CORNER
    };

    void
    intersectBoxWithPlane(const asl::Point3f * theCorners, const asl::Planef & thePlane,
            std::vector<Point3f> & theIntersections, float theEpsilon)
    {
        asl::Point3f myIntersection;

        if (intersection( LineSegment<float>(theCorners[LTF], theCorners[RTF]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[RTF], theCorners[RBF]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[RBF], theCorners[LBF]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[LBF], theCorners[LTF]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[LTBK], theCorners[RTBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[RTBK], theCorners[RBBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[RBBK], theCorners[LBBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[LBBK], theCorners[LTBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }


        if (intersection( LineSegment<float>(theCorners[LTF], theCorners[LTBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[RTF], theCorners[RTBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[LBF], theCorners[LBBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

        if (intersection( LineSegment<float>(theCorners[RBF], theCorners[RBBK]), thePlane, myIntersection, theEpsilon)) {
            theIntersections.push_back(myIntersection);
        }

    }

    Point3f
    averageVertices( const std::vector<Point3f> & thePoints ) {
        Point3f mySum(0.0, 0.0, 0.0);
        for (unsigned i = 0; i < thePoints.size(); ++i) {
            mySum += asVector(thePoints[i]);
        }
        return mySum / float(thePoints.size());
    }

    float
    pseudoAngle(const Point3f & p1, const Point3f & p2) {
        float myDX = p2[0] - p1[0];
        float myDY = p2[1] - p1[1];
        if (myDX == 0.0 && myDY == 0.0) {
            return -1.0;
        } else {
            float myT = myDY / (fabs( myDX) + fabs(myDY));
            if (myDX < 0.0) {
                myT = 2.0f - myT;
            } else if (myDY < 0.0) {
                myT += 4.0f;
            }
            return myT;
        }
    }

    class PseudoAngle {
        public:
            PseudoAngle(const Point3f & theCenter) :
                _myCenter(theCenter) {};

            bool operator()(const Point3f & p1, const Point3f & p2) {
                return pseudoAngle(_myCenter, p1) < pseudoAngle(_myCenter, p2);
            }
        private:
            Point3f _myCenter;
    };

    Vector3f
    calcUV(const Point3f & myPosition,
            const Box3f & theVoxelBox,
            const asl::Matrix4f & theInverseModelView)
    {
        Point3f myModelPosition = myPosition * theInverseModelView;
        return quotient((asVector(myModelPosition) - asVector(theVoxelBox[Box3f::MIN])), theVoxelBox.getSize());
    }

    dom::NodePtr
    createVoxelProxyGeometry(ScenePtr theScene, const asl::Box3f & theVoxelBox,
            const asl::Matrix4f & theModelMatrix, const asl::Matrix4f & theCameraMatrix,
            const Vector3i & theVolumeSize, float theSampleRate,
            const std::string & theMaterialId, const std::string & theName)
    {
        Point3f myCorners[MAX_CORNER];

        theVoxelBox.getCorners(myCorners[LTF], myCorners[RBF], myCorners[RTF], myCorners[LBF],
                myCorners[LTBK], myCorners[RBBK], myCorners[RTBK], myCorners[LBBK]);

        Matrix4f myViewMatrix = theCameraMatrix;
        myViewMatrix.invert();
        Matrix4f myModelViewMatrix = theModelMatrix;
        myModelViewMatrix.postMultiply(myViewMatrix);

        for (unsigned i = 0; i < MAX_CORNER; ++i) {
            myCorners[i]  = myCorners[i] * myModelViewMatrix;
        }

        // find min and max in view direction
        float myMaxZ = - numeric_limits<float>::max();
        float myMinZ = numeric_limits<float>::max();
        for (unsigned i = 0; i < MAX_CORNER; ++i) {
            if (myCorners[i][2] < myMinZ) {
                myMinZ = myCorners[i][2];
            }
            if (myCorners[i][2] > myMaxZ) {
                myMaxZ = myCorners[i][2];
            }
        }
        ShapeBuilder myShape(theName);
        ElementBuilder myElementBuilder(PrimitiveTypeStrings[TRIANGLES], theMaterialId);

        theScene->getSceneBuilder()->appendShape( myShape );
        myShape.appendElements( myElementBuilder );

        dom::NodePtr myVertexNode = myShape.createVertexDataBin<Vector3f>(POSITION_ROLE);
        vector<Vector3f> * myVertices = myVertexNode->nodeValuePtrOpen<vector<Vector3f> >();

        dom::NodePtr myUVNode = myShape.createVertexDataBin<asl::Vector3f>("uvset");
        vector<Vector3f> * myUVSet = myUVNode->nodeValuePtrOpen<vector<Vector3f> >();

        dom::NodePtr myIndexNode = myElementBuilder.createIndex(POSITION_ROLE, POSITIONS);
        vector<unsigned> * myIndices = myIndexNode->nodeValuePtrOpen<vector<unsigned int> >();

        //float mySampleDistance = (myMaxZ - myMinZ) * theSampleRate; // XXX TODO: non-cubic voxels
        unsigned myVertexCount = 0;
        float myEpsilon = 1e-6f;

        Matrix4f myInverseModelView = myModelViewMatrix;
        myInverseModelView.invert();

        float myZ = myMinZ;
        while (myZ < myMaxZ || almostEqual(myZ, myMaxZ)) {
            asl::Planef myPlane(Vector3f(0.0, 0.0, 1.0), -myZ);
            std::vector<Point3f> myIntersections;
            intersectBoxWithPlane(myCorners, myPlane, myIntersections, myEpsilon);
            if ( ! myIntersections.empty()) {

                Point3f myCenter = averageVertices( myIntersections );
                std::vector<Point3f>::size_type myNumIntersections = myIntersections.size();

                // sort vertices in CCW order
                std::sort(myIntersections.begin(), myIntersections.end(), PseudoAngle(myCenter) );

                myVertices->push_back(product(myCenter, myInverseModelView));

                myUVSet->push_back(calcUV(myCenter, theVoxelBox, myInverseModelView));
                myVertexCount++;

                for (std::vector<Point3f>::size_type j = 0; j < myNumIntersections; ++j) {
                    myVertices->push_back(product(myIntersections[j], myInverseModelView));

                    myUVSet->push_back(calcUV(myIntersections[j], theVoxelBox, myInverseModelView));
                    myIndices->push_back(myVertexCount-1);
                    myIndices->push_back(myVertexCount+j);
                    myIndices->push_back(myVertexCount + ( (j+1) % myIntersections.size() ));
                }
                myVertexCount += myIntersections.size();
            }

            // XXX non cubic voxels ....
            myZ += (myMaxZ - myMinZ) / (theVolumeSize[0] * theSampleRate);
        }
        myElementBuilder.copyIndexBin(POSITIONS, getTextureRole(0), "uvset");

        myVertexNode->nodeValuePtrClose<vector<Vector3f> >();
        myVertexNode = dom::NodePtr();
        myVertices = 0;

        myUVNode->nodeValuePtrClose<vector<Vector3f> >();
        myUVNode = dom::NodePtr();
        myUVSet = 0;

        myIndexNode->dom::Node::nodeValuePtrClose<vector<unsigned int> >();
        myIndexNode = dom::NodePtr();
        myIndices = 0;


        return myShape.getNode();
    }

    dom::NodePtr
    createSurface2DFromContour(y60::ScenePtr theScene, const std::string & theMaterialId,
            const VectorOfVector2f & theContours,
            const std::string & theName,
            float theEqualPointsThreshold) {
        SimpleTesselator myTesselator;
        return myTesselator.createSurface2DFromContour(theScene, theMaterialId, theContours, theName, theEqualPointsThreshold);
    }
}
