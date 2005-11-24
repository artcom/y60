/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_numeric_functions.tst.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: Test for Cg conforming types.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "ElementBuilder.h"
#include "MaterialBuilder.h"
#include "WorldBuilder.h"
#include "TransformBuilder.h"
#include "BodyBuilder.h"
#include <y60/DataTypes.h>

#include <asl/string_functions.h>
#include <asl/UnitTest.h>
#include <asl/linearAlgebra.h>
#include <asl/Matrix4.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace asl;
using namespace y60;

class ShapeBuilderUnitTest : public UnitTest {
    public:
        ShapeBuilderUnitTest() : UnitTest("ShapeBuilderUnitTest") {  }

        void run() {
            //==============================================================================
            // build a simple scene ....
            //==============================================================================

            dom::DocumentPtr myDocument(new dom::Document);
            SceneBuilder mySceneBuilder(myDocument);

            ShapeBuilder myShapeBuilder("testshape");
            mySceneBuilder.appendShape(myShapeBuilder);

            myShapeBuilder.createVertexDataBin<Vector3f>(POSITION_ROLE, 3);
            myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(1.0, 1.0, 0.0));
            myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(1.0, -1.0, 0.0));
            myShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(-1.0, -1.0, 0.0));

            myShapeBuilder.createVertexDataBin<Vector3f>(NORMAL_ROLE, 3);
            myShapeBuilder.appendVertexData(NORMAL_ROLE, Vector3f(0.0, 0.0, 1.0));
            myShapeBuilder.appendVertexData(NORMAL_ROLE, Vector3f(0.0, 0.0, 2.0));
            myShapeBuilder.appendVertexData(NORMAL_ROLE, Vector3f(0.0, 0.0, 3.0));

            myShapeBuilder.createVertexDataBin<Vector4f>(COLOR_ROLE, 3);
            myShapeBuilder.appendVertexData(COLOR_ROLE, Vector4f(1.0, 0.0, 0.0, 1.0));
            myShapeBuilder.appendVertexData(COLOR_ROLE, Vector4f(0.0, 1.0, 0.0, 0.5));
            myShapeBuilder.appendVertexData(COLOR_ROLE, Vector4f(0.0, 0.0, 1.0, 0.25));

            myShapeBuilder.createVertexDataBin<Vector2f>("testuvset2D", 4);
            myShapeBuilder.appendVertexData("testuvset2D", Vector2f(0.0, 0.0));
            myShapeBuilder.appendVertexData("testuvset2D", Vector2f(0.0, 1.0));
            myShapeBuilder.appendVertexData("testuvset2D", Vector2f(1.0, 1.0));
            myShapeBuilder.appendVertexData("testuvset2D", Vector2f(1.0, 0.0));

            myShapeBuilder.createVertexDataBin<Vector4f>("testuvset4D", 4);
            myShapeBuilder.appendVertexData("testuvset4D", Vector4f(0.0, 0.0, 0.0, 0.0));
            myShapeBuilder.appendVertexData("testuvset4D", Vector4f(0.0, 1.0, 0.0, 0.0));
            myShapeBuilder.appendVertexData("testuvset4D", Vector4f(1.0, 1.0, 0.0, 0.0));
            myShapeBuilder.appendVertexData("testuvset4D", Vector4f(1.0, 0.0, 0.0, 0.0));

            ElementBuilder myElementBuilder(PRIMITIVE_TYPE_TRIANGLES, "m0");
            myShapeBuilder.appendElements(myElementBuilder);

            myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, 3);
            myElementBuilder.createIndex(NORMAL_ROLE, NORMALS, 3);
            myElementBuilder.createIndex(COLOR_ROLE, COLORS, 3);
            myElementBuilder.createIndex("testuvset2D", getTextureRole(0), 3);
            myElementBuilder.createIndex("testuvset4D", getTextureRole(1), 3);

            myElementBuilder.appendIndex(POSITIONS, 0);
            myElementBuilder.appendIndex(NORMALS,   2);
            myElementBuilder.appendIndex(COLORS,    1);
            myElementBuilder.appendIndex(getTextureRole(0), 1);
            myElementBuilder.appendIndex(getTextureRole(1), 2);

            myElementBuilder.appendIndex(POSITIONS, 1);
            myElementBuilder.appendIndex(NORMALS,   1);
            myElementBuilder.appendIndex(COLORS,    1);
            myElementBuilder.appendIndex(getTextureRole(0), 1);
            myElementBuilder.appendIndex(getTextureRole(1), 1);

            myElementBuilder.appendIndex(POSITIONS, 2);
            myElementBuilder.appendIndex(NORMALS,   0);
            myElementBuilder.appendIndex(COLORS,    1);
            myElementBuilder.appendIndex(getTextureRole(0), 1);
            myElementBuilder.appendIndex(getTextureRole(1), 2);

            MaterialBuilder myMaterialBuilder("shinymaterial", false);
            mySceneBuilder.appendMaterial(myMaterialBuilder);

            VectorOfRankedFeature myRankings;
            RankedFeature _myLightingFeature;
            _myLightingFeature._myFeature.push_back("phong");
            _myLightingFeature._myRanking = 99;
            myRankings.push_back(_myLightingFeature);
            myMaterialBuilder.setType(myRankings);

            {
                string myImageId = myMaterialBuilder.createImage(mySceneBuilder, "testTexture1", "tex\\testtexture.jpg", TEXTURE_USAGE_PAINT, false,
                                                                 asl::Vector4f(1.0f,1.0f,1.0f,0.5f),asl::Vector4f(0.0f,0.0f,0.0f,0.0f), SINGLE,"");
                myMaterialBuilder.createTextureNode(myImageId, "SOME_APPLY_MODE", TEXTURE_USAGE_PAINT, TEXTURE_WRAP_CLAMP, TEXCOORD_UV_MAP, Matrix4f::Identity(), 100, false, 60);

                myImageId = myMaterialBuilder.createImage(mySceneBuilder, "testTexture2", "glossypattern.jpg", TEXTURE_USAGE_PAINT, false,
                                                          asl::Vector4f(1.0f,1.0f,1.0f,0.5f),asl::Vector4f(0.0f,0.0f,0.0f,0.0f), SINGLE,"");
                myMaterialBuilder.createTextureNode(myImageId, "OTHER_APPLY_MODE", TEXTURE_USAGE_PAINT, TEXTURE_WRAP_CLAMP, TEXCOORD_UV_MAP, Matrix4f::Identity(), 60, false, 50);

                string myMovieId = myMaterialBuilder.createMovie(mySceneBuilder, "testTexture3", "mymovie.mpg", 0,"");
                myMaterialBuilder.createTextureNode(myMovieId, "OTHER_APPLY_MODE", TEXTURE_USAGE_PAINT, TEXTURE_WRAP_CLAMP, TEXCOORD_UV_MAP, Matrix4f::Identity(), 60, false, 50);
            }

            //==============================================================================
            // ShapeBuilder tests
            //==============================================================================
            ShapeBuilder myOptShapeBuilder("optshape", true);

            myOptShapeBuilder.createVertexDataBin<Vector3f>(POSITION_ROLE, 2);
            unsigned myVertexIndex0 = myOptShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(1,0,0));
            unsigned myVertexIndex1 = myOptShapeBuilder.appendVertexData(POSITION_ROLE, Vector3f(1,0,0));
            ENSURE(myVertexIndex0 == myVertexIndex1);

            myOptShapeBuilder.createVertexDataBin<Vector3f>(NORMAL_ROLE, 2);
            unsigned myNormalIndex0 = myOptShapeBuilder.appendVertexData(NORMAL_ROLE, Vector3f(1,0,0));
            unsigned myNormalIndex1 = myOptShapeBuilder.appendVertexData(NORMAL_ROLE, Vector3f(1,0,0));
            ENSURE(myNormalIndex0 == myNormalIndex1);

            //==============================================================================
            // Shape tests
            //==============================================================================
            dom::NodePtr mySceneNode = mySceneBuilder.getNode();
            dom::NodePtr myShapeNode = mySceneNode->childNode("shapes")->childNode("shape");
            ENSURE(myShapeNode->nodeName() == std::string("shape"));
            ENSURE(myShapeNode->getAttribute("name")->nodeValue() == std::string("testshape"));
            ENSURE(myShapeNode->getAttribute("id")->nodeValue().size());

            //==============================================================================
            // Position tests
            //==============================================================================
            dom::NodePtr myVertexDataNode = myShapeNode->childNode("vertexdata");
cerr << *myShapeNode << endl;
            dom::NodePtr myPositionListNode = getVertexDataNode(myVertexDataNode, "position");
            ENSURE(myPositionListNode->childNodesLength() == 1);
            VectorOfVector3f myPositions = myPositionListNode->childNode(0)->nodeValueAs<VectorOfVector3f>();
            ENSURE(myPositions.size() == 3);
            ENSURE(almostEqual(myPositions[0], Vector3f(1.0, 1.0, 0.0)));
            ENSURE(almostEqual(myPositions[1], Vector3f(1.0, -1.0, 0.0)));
            ENSURE(almostEqual(myPositions[2], Vector3f(-1.0, -1.0, 0.0)));

            //==============================================================================
            // Normal tests
            //==============================================================================
            Vector3f myNormal;
            dom::NodePtr myNormalListNode = getVertexDataNode(myVertexDataNode, "normal");
            ENSURE(myNormalListNode);
            ENSURE(myNormalListNode->childNodesLength() == 1);

            VectorOfVector3f myNormals = myNormalListNode->childNode(0)->nodeValueAs<VectorOfVector3f>();
            ENSURE(myNormals.size() == 3);
            ENSURE(almostEqual(myNormals[0], Vector3f(0.0, 0.0, 1.0)));
            ENSURE(almostEqual(myNormals[1], Vector3f(0.0, 0.0, 2.0)));
            ENSURE(almostEqual(myNormals[2], Vector3f(0.0, 0.0, 3.0)));

            //==============================================================================
            // Color tests
            //==============================================================================
            Vector4f myColor;
            dom::NodePtr myColorListNode = getVertexDataNode(myVertexDataNode, "color");
            ENSURE(myColorListNode);
            ENSURE(myColorListNode->childNodesLength() == 1);
            VectorOfVector4f myColors = myColorListNode->childNode(0)->nodeValueAs<VectorOfVector4f>();
            ENSURE(myColors.size() == 3);
            ENSURE(almostEqual(myColors[0], Vector4f(1.0, 0.0, 0.0, 1.0)));
            ENSURE(almostEqual(myColors[1], Vector4f(0.0, 1.0, 0.0, 0.5)));
            ENSURE(almostEqual(myColors[2], Vector4f(0.0, 0.0, 1.0, 0.25)));

            //==============================================================================
            // UVSet tests
            //==============================================================================
            {
                dom::NodePtr myUVSetNode = getVertexDataNode(myVertexDataNode, "testuvset2D");
                ENSURE(myUVSetNode->childNodesLength() == 1);

                VectorOfVector2f myUVCoords = myUVSetNode->childNode(0)->nodeValueAs<VectorOfVector2f>();
                ENSURE(myUVCoords.size() == 4);

                ENSURE(almostEqual(myUVCoords[0], Vector2f(0.0, 0.0)));
                ENSURE(almostEqual(myUVCoords[1], Vector2f(0.0, 1.0)));
                ENSURE(almostEqual(myUVCoords[2], Vector2f(1.0, 1.0)));
                ENSURE(almostEqual(myUVCoords[3], Vector2f(1.0, 0.0)));
            }
            {
                dom::NodePtr myUVSetNode = getVertexDataNode(myVertexDataNode, "testuvset4D");
                ENSURE(myUVSetNode->childNodesLength() == 1);

                VectorOfVector4f myUVCoords = myUVSetNode->childNode(0)->nodeValueAs<VectorOfVector4f>();
                ENSURE(myUVCoords.size() == 4);

                ENSURE(almostEqual(myUVCoords[0], Vector4f(0.0, 0.0, 0.0, 0.0)));
                ENSURE(almostEqual(myUVCoords[1], Vector4f(0.0, 1.0, 0.0, 0.0)));
                ENSURE(almostEqual(myUVCoords[2], Vector4f(1.0, 1.0, 0.0, 0.0)));
                ENSURE(almostEqual(myUVCoords[3], Vector4f(1.0, 0.0, 0.0, 0.0)));
            }
            //==============================================================================
            // Primitive tests
            //==============================================================================
            dom::NodePtr myPrimitiveListNode = myShapeNode->childNode("primitives");
            ENSURE(myPrimitiveListNode);
            ENSURE(myPrimitiveListNode->childNodesLength() == 1);
            const dom::NodePtr myElementsNode = myPrimitiveListNode->childNode("elements");

            ENSURE(myElementsNode->childNodesLength() == 5);
            ENSURE((*myElementsNode)["material"].nodeValue() == "m0");
            ENSURE((*myElementsNode)["type"].nodeValue() == PRIMITIVE_TYPE_TRIANGLES);

            dom::NodePtr myPositionNode = getIndexDataNode(myElementsNode, "position");
            ENSURE(myPositionNode->childNode("#text")->nodeValue() == "[0,1,2]");
            ENSURE(myPositionNode->getAttributeString("role") == "position");
            dom::NodePtr myColorNode = getIndexDataNode(myElementsNode, "color");
            ENSURE(myColorNode->childNode("#text")->nodeValue() == "[1,1,1]");
            ENSURE(myColorNode->getAttributeString("role") == "color");
            dom::NodePtr myNormalNode = getIndexDataNode(myElementsNode, "normal");
            ENSURE(myNormalNode->childNode("#text")->nodeValue() == "[2,1,0]");
            ENSURE(myNormalNode->getAttributeString("role") == "normal");
            dom::NodePtr myUVset1Node = getIndexDataNode(myElementsNode, "testuvset2D");
            ENSURE(myUVset1Node->childNode("#text")->nodeValue() == "[1,1,1]");
            ENSURE(myUVset1Node->getAttributeString("role") == "texcoord0");
            dom::NodePtr myUVset2Node = getIndexDataNode(myElementsNode, "testuvset4D");
            ENSURE(myUVset2Node->childNode("#text")->nodeValue() == "[2,1,2]");
            ENSURE(myUVset2Node->getAttributeString("role") == "texcoord1");

            //==============================================================================
            // Material tests
            //==============================================================================
            dom::NodePtr myMaterialNode = mySceneNode->childNode("materials")->childNode(0);
            ENSURE(myMaterialNode->nodeName() == "material");
            ENSURE(myMaterialNode->getAttribute("name")->nodeValue() == "shinymaterial");
            ENSURE(myMaterialNode->getAttribute("id")->nodeValue().size());
            dom::NodePtr myTextureListNode = myMaterialNode->childNode("textures");
            ENSURE(myTextureListNode->childNodesLength() == 3);

            dom::NodePtr myTextureNode = myTextureListNode->childNode(0);
            // note the slash instead of backslash:
            cerr << *myTextureNode << endl;
            cerr << *mySceneNode << endl;
            string myImageId = myTextureNode->getAttribute("image")->nodeValue();
            dom::NodePtr myImageNode = mySceneNode->childNode("images")->getElementById(myImageId);
            ENSURE(myImageNode->getAttribute("src")->nodeValue() == "tex/testtexture.jpg");
            ENSURE(myTextureNode->getAttribute("applymode")->nodeValue() == "SOME_APPLY_MODE");

            myTextureNode = myTextureListNode->childNode(1);
            myImageId = myTextureNode->getAttribute("image")->nodeValue();
            myImageNode = mySceneNode->childNode("images")->getElementById(myImageId);

            ENSURE(myImageNode->getAttribute("src")->nodeValue() == "glossypattern.jpg");
            ENSURE(myTextureNode->getAttribute("applymode")->nodeValue() == "OTHER_APPLY_MODE");

            myTextureNode = myTextureListNode->childNode(2);
            myImageId = myTextureNode->getAttribute("image")->nodeValue();

            // it's actually myMovieNode...
            myImageNode = mySceneNode->childNode("images")->getElementById(myImageId);
            ENSURE(myImageNode->getAttribute("src")->nodeValue() == "mymovie.mpg");
            ENSURE(myImageNode->getAttribute("currentframe")->nodeValue() == "0");
            ENSURE(myImageNode->getAttribute("framecount")->nodeValue() == "0");

            //==============================================================================
            // Transform/Body tests
            //==============================================================================

            // Create the builder classes
            WorldBuilder myWorldBuilder;
            // Append them into one scene
            mySceneBuilder.appendWorld(myWorldBuilder);

            TransformBuilder myTransformBuilder1("rot");
            myWorldBuilder.appendObject(myTransformBuilder1);

            TransformBuilder myTransformBuilder2("gelb");
            myWorldBuilder.appendObject(myTransformBuilder2);

            TransformBuilder myTransformBuilder3("gruen_in_gelb");
            myTransformBuilder2.appendObject(myTransformBuilder3);

            BodyBuilder myBodyBuilder1("s1", "body1");
            myWorldBuilder.appendObject(myBodyBuilder1);

            BodyBuilder myBodyBuilder2("s2", "body2");
            myBodyBuilder1.appendObject(myBodyBuilder2);

            BodyBuilder myBodyBuilder3("s3", "body3");
            myTransformBuilder2.appendObject(myBodyBuilder3);
            myBodyBuilder3.setVisiblity(false);



            // Fill them with some data
            myTransformBuilder1.setPosition(asl::Vector3f(1,2,3));
            myTransformBuilder1.setOrientation(asl::Vector3f(0,0,0));
            myTransformBuilder1.setScale(asl::Vector3f(7,8,9));
            myTransformBuilder1.setShear(asl::Vector3f(10,11,12));
            myTransformBuilder1.setPivot(asl::Vector3f(13,14,15));
            myTransformBuilder2.setPosition(asl::Vector3f(9,9,9));
            myBodyBuilder1.setPosition(asl::Vector3f(9,9,9));

            dom::NodePtr myTransformNode1 = mySceneNode->childNode("worlds")->childNode("world")->childNode(0);
            ENSURE(myTransformNode1->nodeName() == "transform");
            ENSURE(myTransformNode1->getAttribute("id")->nodeValue().size());
            ENSURE(myTransformNode1->getAttribute("name")->nodeValue() == "rot");
            ENSURE(myTransformNode1->getAttribute("position")->nodeValue() == "[1,2,3]");
            ENSURE(myTransformNode1->getAttribute("orientation")->nodeValue() == "[0,0,0,1]");
            ENSURE(myTransformNode1->getAttribute("scale")->nodeValue() == "[7,8,9]");
            ENSURE(myTransformNode1->getAttribute("shear")->nodeValue() == "[10,11,12]");
            ENSURE(myTransformNode1->getAttribute("pivot")->nodeValue() == "[13,14,15]");

            dom::NodePtr myTransformNode2 = mySceneNode->childNode("worlds")->childNode("world")->childNode(1);
            ENSURE(myTransformNode2->nodeName() == "transform");
            ENSURE(myTransformNode2->getAttribute("id")->nodeValue().size());
            ENSURE(myTransformNode2->getAttribute("name")->nodeValue() == "gelb");

            dom::NodePtr myBodyNode1 = mySceneNode->childNode("worlds")->childNode("world")->childNode(2);
            ENSURE(myBodyNode1->nodeName() == "body");
            ENSURE(myBodyNode1->getAttribute("name")->nodeValue() == "body1");
            ENSURE(myBodyNode1->getAttribute("id")->nodeValue().size());
            ENSURE(myBodyNode1->getAttribute("position")->nodeValue() == "[9,9,9]");
            ENSURE(myBodyNode1->getAttribute("shape")->nodeValue() == "s1");
            ENSURE(myBodyNode1->getAttribute("scale")->nodeValue() == "[1,1,1]");
            ENSURE(myBodyNode1->getAttribute("orientation")->nodeValue() == "[0,0,0,1]");

            dom::NodePtr myBodyNode2 = myBodyNode1->childNode(0);
            ENSURE(myBodyNode2->nodeName() == "body");
             ENSURE(myBodyNode2->getAttribute("id")->nodeValue().size());
            ENSURE(myBodyNode2->getAttribute("name")->nodeValue() == "body2");
            ENSURE(myBodyNode2->getAttribute("shape")->nodeValue() == "s2");
            ENSURE(myBodyNode2->getAttribute("visible")->nodeValue() == "1");

            dom::NodePtr myTransformNode3 = myTransformNode2->childNode(0);
            ENSURE(myTransformNode3->nodeName() == "transform");
            ENSURE(myTransformNode3->getAttribute("id")->nodeValue().size());
            ENSURE(myTransformNode3->getAttribute("name")->nodeValue() == "gruen_in_gelb");
            ENSURE(myTransformNode3->getAttribute("visible")->nodeValue() == "1");
        }

    private:
        dom::NodePtr getVertexDataNode(dom::NodePtr theParentNode, std::string theName) {
            for (unsigned i = 0; i < theParentNode->childNodesLength(); ++i) {
                dom::NodePtr myChild = theParentNode->childNode(i);
                if (myChild->getAttributeString("name") == theName) {
                    return myChild;
                }
            }
            return dom::NodePtr(0);
        }
        dom::NodePtr getIndexDataNode(dom::NodePtr theParentNode, std::string theName) {
            for (unsigned i = 0; i < theParentNode->childNodesLength(); ++i) {
                dom::NodePtr myChild = theParentNode->childNode(i);
                if (myChild->getAttributeString("vertexdata") == theName) {
                    return myChild;
                }
            }
            return dom::NodePtr(0);
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new ShapeBuilderUnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    try {
        mySuite.run();
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
