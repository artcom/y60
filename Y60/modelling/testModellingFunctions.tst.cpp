/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testModellingFunctions.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test template file - change Scene to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include <y60/Y60xsd.h>

#include <y60/Scene.h>
#include <y60/TextureManager.h>

#include <asl/UnitTest.h>
#include <asl/Vector234.h>
#include <dom/Nodes.h>
#include <dom/Schema.h>

#include <string>
#include <iostream>

#include "modelling_functions.h"


using namespace std;
using namespace y60;
using namespace asl;

class ProxyGeometryUnitTest : public UnitTest {
    public:
        ProxyGeometryUnitTest() : UnitTest("ProxyGeometryUnitTest") {}

        bool positionsEqual(const VectorOfVector3f & theCandidate,
                            const VectorOfVector3f & theReference,
                            const Matrix4f & theCameraMatrix) {
            bool myResult = true;
            if (theCandidate.size() != theReference.size()) {
                return false;
            }
            Matrix4f myTransformation = theCameraMatrix;
            //myTransformation.invert();
            for (int i = 0; i < theReference.size(); ++i) {
                Point3f myPoint = Point3f(theReference[i]);
                Vector3f myTransformedReference = product(myPoint, myTransformation);
                if (!almostEqual(theCandidate[i], myTransformedReference)) {
                    AC_WARNING << "Candidate " << i << " is " << theCandidate[i] <<
                        " but it is expected to be " << myTransformedReference;
                    //return false;
                    myResult = false;
                }
            }
            return myResult;
        }

        bool extractPositions(dom::NodePtr theShape, VectorOfVector3f & thePositions) {
            dom::NodePtr myVertexData = theShape->childNode("vertexdata");
            int myCandidates = myVertexData->childNodesLength("vectorofvector3f");
            for (int i = 0; i < myCandidates; ++i) {
                dom::NodePtr myCandidate = myVertexData->childNode("vectorofvector3f", i);
                if (myCandidate->getAttributeString("name") == "position") {
                    VectorOfVector3f & myRef = myCandidate->childNode(0)->nodeValueRefOpen<VectorOfVector3f>();
                    thePositions.insert(thePositions.begin(), myRef.begin(), myRef.end());
                    return true;
                }
            }
            return false;
        }


        void run() {
            PackageManagerPtr myPackageManager( new PackageManager );
            ScenePtr myScene = Scene::createStubs(myPackageManager);

            dom::NodePtr myMaterial = createColorMaterial(myScene, Vector4f(0.8f,0.8f,0.6f,1.0f));

            Box3f myVoxelBox;
            myVoxelBox.makeEmpty();
            myVoxelBox.extendBy( Point3f(0.0f, 0.0f, 0.0f));
            //myVoxelBox.extendBy( Point3f(10.0, 20.0, 30.0f)); // 10x20x30 voxels
            myVoxelBox.extendBy( Point3f(1.0f, 1.0f, 1.0f)); // 1x2x4 voxels
            Vector3i myVolumeSize(10, 10, 10);
            Matrix4f myModelMatrix;
            Matrix4f myCameraMatrix;
            float mySampleRate = 1.0f;

            VectorOfVector3f myReference;
            VectorOfVector3f myCandidate;

            myModelMatrix.makeIdentity();
            myCameraMatrix.makeIdentity();
            dom::NodePtr myShape = createVoxelProxyGeometry(myScene, myVoxelBox,
                    myModelMatrix, myCameraMatrix, myVolumeSize, mySampleRate,
                    myMaterial->getAttributeString(ID_ATTRIB), "VoxelProxy");
            ENSURE(extractPositions(myShape, myReference));
            myCameraMatrix.makeXRotating(asl::PI/2);
            myShape = createVoxelProxyGeometry(myScene, myVoxelBox, myModelMatrix, myCameraMatrix,
                    myVolumeSize, mySampleRate, myMaterial->getAttributeString(ID_ATTRIB),
                    "VoxelProxy");
            ENSURE(extractPositions(myShape, myCandidate));

            //ENSURE(positionsEqual(myCandidate, myReference, myCameraMatrixf));
            //myModelViewMatrix.rotateY(asl::PI * 0.125);

            //DPRINT( * myShape );

            //dom::NodePtr myBody = createBody(myScene->getWorldRoot(), myShape->getAttributeString(ID_ATTRIBf));
            //myScene->save("proxy.x60", false);
        }
};

class ModellingUnitTest : public UnitTest {
public:
    ModellingUnitTest() : UnitTest("ModellingUnitTest") {}
    void run() {
        {
            SUCCESS("TODO: NOTHING TESTED HERE YET");
            return;
            TextureManager * myTextureManager = 0;
            PackageManagerPtr myPackageManager( new PackageManager );
            ScenePtr myScene = Scene::createStubs(myPackageManager);

            dom::NodePtr myMaterial = y60::createUnlitTexturedMaterial(myScene, "myTexture.jpg", "myMaterial");
            dom::NodePtr myColorMaterial = y60::createColorMaterial(myScene, asl::Vector4f(1,0,1,0));

            dom::NodePtr myShape = createPlane(
                  myScene, 4, 3,
                  "myTest",
                  (*myMaterial)["id"].nodeValue(),
                  QuadBuilder(),
                  y60::PlanePosition(),
                  y60::ConstNormal(),
                  y60::PlaneUVCoord(),
                  y60::ConstColor());

            cerr << *myScene->getSceneDom();
        }
    }
};

class TesselationUnitTest : public UnitTest {
public:
    TesselationUnitTest() : UnitTest("TesselationUnitTest") {}
    void run() {
        {
            PackageManagerPtr myPackageManager( new PackageManager );
            ScenePtr myScene = Scene::createStubs(myPackageManager);

            dom::NodePtr myMaterial = y60::createColorMaterial(myScene, Vector4f(1.0f,0,0,1) , "myMaterial");
            dom::NodePtr myColorMaterial = y60::createColorMaterial(myScene, asl::Vector4f(1,0,1,0.0f));

            VectorOfVector2f myCountour;
            myCountour.push_back(Vector2f(1149.25f,615.917f));
            myCountour.push_back(Vector2f(1148.58f,614.25f));
            myCountour.push_back(Vector2f(1148.08f,612.5f));
            myCountour.push_back(Vector2f(1147.08f,610.833f));
            myCountour.push_back(Vector2f(1146.92f,609.667f));
            myCountour.push_back(Vector2f(1147.92f,608.583f));
            myCountour.push_back(Vector2f(1147.42f,607.75f));
            myCountour.push_back(Vector2f(1146.08f,607.25f));
            myCountour.push_back(Vector2f(1145.08f,605.5f));
            myCountour.push_back(Vector2f(1146.67f,604.667f));
            myCountour.push_back(Vector2f(1147.25f,603.167f));
            myCountour.push_back(Vector2f(1146.58f,601.75f));
            myCountour.push_back(Vector2f(1147.f,599.667f));
            myCountour.push_back(Vector2f(1148.92f,599.417f));
            myCountour.push_back(Vector2f(1149.08f,598.417f));
            myCountour.push_back(Vector2f(1150.42f,598.75f));
            myCountour.push_back(Vector2f(1153.08f,598.083f));
            myCountour.push_back(Vector2f(1153.92f,597.f));
            myCountour.push_back(Vector2f(1154.25f,595.333f));
            myCountour.push_back(Vector2f(1155.33f,594.75f));
            myCountour.push_back(Vector2f(1156.92f,594.667f));
            myCountour.push_back(Vector2f(1157.08f,596.667f));
            myCountour.push_back(Vector2f(1157.92f,598.333f));
            myCountour.push_back(Vector2f(1158.75f,599.333f));
            myCountour.push_back(Vector2f(1159.75f,598.833f));
            myCountour.push_back(Vector2f(1159.5f,597.083f));
            myCountour.push_back(Vector2f(1160.f,596.583f));
            myCountour.push_back(Vector2f(1160.42f,595.417f));
            myCountour.push_back(Vector2f(1160.67f,594.417f));
            myCountour.push_back(Vector2f(1161.67f,595.083f));
            myCountour.push_back(Vector2f(1164.42f,596.167f));
            myCountour.push_back(Vector2f(1166.42f,597.417f));
            myCountour.push_back(Vector2f(1166.5f,596.25f));
            myCountour.push_back(Vector2f(1166.08f,595.417f));
            myCountour.push_back(Vector2f(1165.83f,594.083f));
            myCountour.push_back(Vector2f(1166.5f,592.667f));
            myCountour.push_back(Vector2f(1166.75f,591.75f));
            myCountour.push_back(Vector2f(1165.92f,590.333f));
            myCountour.push_back(Vector2f(1164.75f,589.667f));
            myCountour.push_back(Vector2f(1166.17f,588.667f));
            myCountour.push_back(Vector2f(1167.17f,589.667f));
            myCountour.push_back(Vector2f(1168.33f,590.417f));
            myCountour.push_back(Vector2f(1168.17f,591.583f));
            myCountour.push_back(Vector2f(1169.42f,592.083f));
            myCountour.push_back(Vector2f(1171.33f,591.083f));
            myCountour.push_back(Vector2f(1172.08f,591.583f));
            myCountour.push_back(Vector2f(1172.92f,591.583f));
            myCountour.push_back(Vector2f(1175.f,591.417f));
            myCountour.push_back(Vector2f(1176.25f,590.917f));
            myCountour.push_back(Vector2f(1178.58f,590.833f));
            myCountour.push_back(Vector2f(1179.67f,590.417f));
            myCountour.push_back(Vector2f(1182.33f,590.75f));
            myCountour.push_back(Vector2f(1183.67f,591.833f));
            myCountour.push_back(Vector2f(1186.f,593.5f));
            myCountour.push_back(Vector2f(1187.f,594.667f));
            myCountour.push_back(Vector2f(1188.58f,596.333f));
            myCountour.push_back(Vector2f(1190.33f,598.f));
            myCountour.push_back(Vector2f(1190.83f,599.333f));
            myCountour.push_back(Vector2f(1191.75f,601.333f));
            myCountour.push_back(Vector2f(1191.42f,602.333f));
            myCountour.push_back(Vector2f(1190.42f,603.333f));
            myCountour.push_back(Vector2f(1190.42f,604.5f));
            myCountour.push_back(Vector2f(1190.75f,605.583f));
            myCountour.push_back(Vector2f(1190.25f,607.083f));
            myCountour.push_back(Vector2f(1189.5f,608.5f));
            myCountour.push_back(Vector2f(1188.25f,609.167f));
            myCountour.push_back(Vector2f(1186.83f,608.417f));
            myCountour.push_back(Vector2f(1186.75f,606.917f));
            myCountour.push_back(Vector2f(1185.75f,605.083f));
            myCountour.push_back(Vector2f(1185.67f,603.333f));
            myCountour.push_back(Vector2f(1187.33f,602.917f));
            myCountour.push_back(Vector2f(1187.67f,601.583f));
            myCountour.push_back(Vector2f(1187.25f,600.667f));
            myCountour.push_back(Vector2f(1186.5f,600.333f));
            myCountour.push_back(Vector2f(1185.08f,600.75f));
            myCountour.push_back(Vector2f(1183.67f,601.f));
            myCountour.push_back(Vector2f(1183.08f,601.583f));
            myCountour.push_back(Vector2f(1182.17f,601.083f));
            myCountour.push_back(Vector2f(1181.75f,601.833f));
            myCountour.push_back(Vector2f(1181.5f,602.333f));
            myCountour.push_back(Vector2f(1180.08f,602.75f));
            myCountour.push_back(Vector2f(1179.33f,602.417f));
            myCountour.push_back(Vector2f(1178.33f,602.833f));
            myCountour.push_back(Vector2f(1178.33f,603.25f));
            myCountour.push_back(Vector2f(1178.75f,604.667f));
            myCountour.push_back(Vector2f(1179.17f,605.833f));
            myCountour.push_back(Vector2f(1179.67f,607.417f));
            myCountour.push_back(Vector2f(1179.17f,608.417f));
            myCountour.push_back(Vector2f(1178.42f,608.833f));
            myCountour.push_back(Vector2f(1178.67f,610.f));
            myCountour.push_back(Vector2f(1178.17f,611.333f));
            myCountour.push_back(Vector2f(1178.17f,612.f));
            myCountour.push_back(Vector2f(1179.f,613.083f));
            myCountour.push_back(Vector2f(1179.92f,614.25f));
            myCountour.push_back(Vector2f(1180.f,614.917f));
            myCountour.push_back(Vector2f(1181.5f,616.f));
            myCountour.push_back(Vector2f(1181.f,617.417f));
            myCountour.push_back(Vector2f(1181.58f,618.083f));
            myCountour.push_back(Vector2f(1182.67f,617.833f));
            myCountour.push_back(Vector2f(1183.5f,617.25f));
            myCountour.push_back(Vector2f(1184.58f,617.667f));
            myCountour.push_back(Vector2f(1185.92f,618.833f));
            myCountour.push_back(Vector2f(1187.f,619.667f));
            myCountour.push_back(Vector2f(1187.f,622.25f));
            myCountour.push_back(Vector2f(1188.08f,624.167f));
            myCountour.push_back(Vector2f(1188.58f,626.083f));
            myCountour.push_back(Vector2f(1189.75f,627.083f));
            myCountour.push_back(Vector2f(1190.75f,628.583f));
            myCountour.push_back(Vector2f(1191.5f,629.083f));
            myCountour.push_back(Vector2f(1192.83f,630.583f));
            myCountour.push_back(Vector2f(1194.42f,632.f));
            myCountour.push_back(Vector2f(1196.25f,632.333f));
            myCountour.push_back(Vector2f(1197.67f,632.417f));
            myCountour.push_back(Vector2f(1199.58f,632.5f));
            myCountour.push_back(Vector2f(1201.f,632.833f));
            myCountour.push_back(Vector2f(1202.f,633.f));
            myCountour.push_back(Vector2f(1202.25f,633.5f));
            myCountour.push_back(Vector2f(1202.58f,634.5f));
            myCountour.push_back(Vector2f(1200.92f,635.f));
            myCountour.push_back(Vector2f(1200.5f,635.833f));
            myCountour.push_back(Vector2f(1201.08f,636.667f));
            myCountour.push_back(Vector2f(1202.5f,637.333f));
            myCountour.push_back(Vector2f(1204.58f,637.833f));
            myCountour.push_back(Vector2f(1206.25f,638.75f));
            myCountour.push_back(Vector2f(1207.92f,639.f));
            myCountour.push_back(Vector2f(1208.83f,640.583f));
            myCountour.push_back(Vector2f(1209.5f,641.083f));
            myCountour.push_back(Vector2f(1210.83f,642.f));
            myCountour.push_back(Vector2f(1211.f,642.583f));
            myCountour.push_back(Vector2f(1212.42f,642.833f));
            myCountour.push_back(Vector2f(1213.5f,644.167f));
            myCountour.push_back(Vector2f(1213.83f,645.167f));
            myCountour.push_back(Vector2f(1214.75f,645.75f));
            myCountour.push_back(Vector2f(1215.58f,646.417f));
            myCountour.push_back(Vector2f(1214.92f,647.083f));
            myCountour.push_back(Vector2f(1213.33f,647.667f));
            myCountour.push_back(Vector2f(1212.33f,646.417f));
            myCountour.push_back(Vector2f(1211.25f,645.667f));
            myCountour.push_back(Vector2f(1210.08f,645.417f));
            myCountour.push_back(Vector2f(1209.33f,645.583f));
            myCountour.push_back(Vector2f(1208.08f,645.25f));
            myCountour.push_back(Vector2f(1207.83f,644.5f));
            myCountour.push_back(Vector2f(1206.33f,644.5f));
            myCountour.push_back(Vector2f(1205.08f,645.167f));
            myCountour.push_back(Vector2f(1204.58f,645.917f));
            myCountour.push_back(Vector2f(1203.67f,647.833f));
            myCountour.push_back(Vector2f(1203.83f,649.25f));
            myCountour.push_back(Vector2f(1204.83f,650.417f));
            myCountour.push_back(Vector2f(1206.25f,651.75f));
            myCountour.push_back(Vector2f(1207.33f,652.583f));
            myCountour.push_back(Vector2f(1207.f,654.f));
            myCountour.push_back(Vector2f(1206.83f,655.25f));
            myCountour.push_back(Vector2f(1206.58f,655.833f));
            myCountour.push_back(Vector2f(1205.58f,655.75f));
            myCountour.push_back(Vector2f(1203.5f,656.667f));
            myCountour.push_back(Vector2f(1204.f,657.583f));
            myCountour.push_back(Vector2f(1203.17f,659.5f));
            myCountour.push_back(Vector2f(1201.58f,660.5f));
            myCountour.push_back(Vector2f(1201.f,661.417f));
            myCountour.push_back(Vector2f(1200.42f,662.583f));
            myCountour.push_back(Vector2f(1199.5f,663.25f));
            myCountour.push_back(Vector2f(1197.67f,662.667f));
            myCountour.push_back(Vector2f(1197.33f,661.917f));
            myCountour.push_back(Vector2f(1197.83f,660.667f));
            myCountour.push_back(Vector2f(1198.75f,659.583f));
            myCountour.push_back(Vector2f(1199.f,658.083f));
            myCountour.push_back(Vector2f(1199.83f,657.667f));
            myCountour.push_back(Vector2f(1199.92f,655.917f));
            myCountour.push_back(Vector2f(1199.83f,653.333f));
            myCountour.push_back(Vector2f(1198.92f,651.167f));
            myCountour.push_back(Vector2f(1198.5f,649.75f));
            myCountour.push_back(Vector2f(1198.67f,648.417f));
            myCountour.push_back(Vector2f(1198.17f,647.417f));
            myCountour.push_back(Vector2f(1196.33f,647.25f));
            myCountour.push_back(Vector2f(1194.83f,646.917f));
            myCountour.push_back(Vector2f(1193.58f,646.f));
            myCountour.push_back(Vector2f(1193.33f,644.583f));
            myCountour.push_back(Vector2f(1193.58f,643.5f));
            myCountour.push_back(Vector2f(1193.5f,642.833f));
            myCountour.push_back(Vector2f(1191.75f,642.333f));
            myCountour.push_back(Vector2f(1190.33f,642.333f));
            myCountour.push_back(Vector2f(1191.08f,640.917f));
            myCountour.push_back(Vector2f(1190.83f,639.f));
            myCountour.push_back(Vector2f(1189.75f,638.583f));
            myCountour.push_back(Vector2f(1188.83f,639.417f));
            myCountour.push_back(Vector2f(1188.5f,641.083f));
            myCountour.push_back(Vector2f(1187.5f,641.583f));
            myCountour.push_back(Vector2f(1186.92f,641.583f));
            myCountour.push_back(Vector2f(1186.83f,640.417f));
            myCountour.push_back(Vector2f(1187.33f,639.25f));
            myCountour.push_back(Vector2f(1187.25f,638.333f));
            myCountour.push_back(Vector2f(1186.25f,638.083f));
            myCountour.push_back(Vector2f(1184.25f,637.833f));
            myCountour.push_back(Vector2f(1183.08f,637.833f));
            myCountour.push_back(Vector2f(1181.92f,637.f));
            myCountour.push_back(Vector2f(1180.75f,636.5f));
            myCountour.push_back(Vector2f(1180.67f,635.f));
            myCountour.push_back(Vector2f(1180.25f,633.667f));
            myCountour.push_back(Vector2f(1179.f,632.5f));
            myCountour.push_back(Vector2f(1177.92f,632.25f));
            myCountour.push_back(Vector2f(1176.83f,631.75f));
            myCountour.push_back(Vector2f(1176.08f,631.f));
            myCountour.push_back(Vector2f(1174.92f,629.833f));
            myCountour.push_back(Vector2f(1174.17f,628.833f));
            myCountour.push_back(Vector2f(1172.17f,628.5f));
            myCountour.push_back(Vector2f(1171.33f,627.25f));
            myCountour.push_back(Vector2f(1170.83f,626.25f));
            myCountour.push_back(Vector2f(1170.33f,624.833f));
            myCountour.push_back(Vector2f(1169.08f,624.333f));
            myCountour.push_back(Vector2f(1167.5f,623.417f));
            myCountour.push_back(Vector2f(1167.42f,622.25f));
            myCountour.push_back(Vector2f(1167.17f,621.417f));
            myCountour.push_back(Vector2f(1167.08f,619.833f));
            myCountour.push_back(Vector2f(1166.5f,619.25f));
            myCountour.push_back(Vector2f(1166.17f,618.833f));
            myCountour.push_back(Vector2f(1166.f,617.667f));
            myCountour.push_back(Vector2f(1166.08f,616.333f));
            myCountour.push_back(Vector2f(1166.5f,614.5f));
            myCountour.push_back(Vector2f(1165.25f,614.083f));
            myCountour.push_back(Vector2f(1164.17f,613.75f));
            myCountour.push_back(Vector2f(1163.17f,613.f));
            myCountour.push_back(Vector2f(1162.08f,613.f));
            myCountour.push_back(Vector2f(1161.5f,612.833f));
            myCountour.push_back(Vector2f(1160.58f,612.167f));
            myCountour.push_back(Vector2f(1159.83f,612.75f));
            myCountour.push_back(Vector2f(1158.58f,612.333f));
            myCountour.push_back(Vector2f(1158.17f,611.583f));
            myCountour.push_back(Vector2f(1157.5f,611.167f));
            myCountour.push_back(Vector2f(1156.25f,611.25f));
            myCountour.push_back(Vector2f(1154.5f,613.f));
            myCountour.push_back(Vector2f(1154.58f,614.167f));
            myCountour.push_back(Vector2f(1154.5f,615.167f));
            myCountour.push_back(Vector2f(1153.5f,615.75f));
            myCountour.push_back(Vector2f(1152.08f,616.333f));
            myCountour.push_back(Vector2f(1151.33f,616.75f));

            const unsigned myExceptedIndices[] = { 23,57,13,15,23,13,14,15,13,15,22,23,57,23,24,57,24,56,58,72,73,73,12,13,73,
                                                    13,57,58,73,57,12,73,74,12,74,75,12,75,77,12,77,11,58,71,72,59,71,58,59,
                                                    70,71,60,69,70,60,70,59,69,60,61,69,61,68,62,68,61,62,67,68,63,67,62,63,
                                                    66,67,64,66,63,64,65,66,75,76,77,78,11,77,11,78,79,11,79,81,11,81,82,11,
                                                    82,10,79,80,81,83,10,82,83,9,10,84,9,83,84,8,9,85,8,84,85,7,8,86,
                                                    7,85,86,6,7,87,6,86,87,5,6,88,5,87,88,4,5,89,4,88,89,3,4,90,
                                                    227,3,90,3,89,3,227,228,3,228,2,90,226,227,91,226,90,226,91,223,226,223,225,220,
                                                    221,222,92,220,222,92,222,223,92,223,91,223,224,225,92,219,220,93,218,219,93,219,92,93,
                                                    217,218,94,217,93,217,94,95,217,95,216,96,216,95,96,215,216,97,215,96,97,214,215,101,
                                                    214,97,101,97,98,101,98,100,101,213,214,102,213,101,102,212,213,103,210,211,103,211,212,103,
                                                    212,102,103,209,210,104,209,103,104,208,209,105,207,208,105,208,104,105,206,207,106,206,105,106,
                                                    205,206,107,204,205,107,205,106,107,203,204,108,203,107,108,202,203,109,202,108,109,201,202,110,
                                                    200,201,110,201,109,110,199,200,111,199,110,199,111,112,199,112,198,113,198,112,198,113,114,198,
                                                    114,115,198,115,116,198,116,197,117,197,116,117,196,197,118,196,117,196,118,119,196,119,195,120,
                                                    195,119,120,194,195,121,194,120,121,193,194,192,193,121,122,192,121,122,191,192,123,183,190,123,
                                                    190,191,123,191,122,183,189,190,123,182,183,124,182,123,182,124,125,182,125,181,126,181,125,181,
                                                    126,127,181,127,180,179,180,127,128,179,127,128,178,179,129,178,128,129,177,178,130,177,129,177,
                                                    130,142,177,142,176,141,142,130,131,141,130,143,176,142,176,143,144,176,144,175,145,172,173,145,
                                                    173,174,145,174,175,145,175,144,145,171,172,146,171,145,146,170,171,147,170,146,147,169,170,148,
                                                    169,147,169,148,149,169,149,168,150,168,149,168,150,151,168,151,153,168,153,167,151,152,153,154,
                                                    167,153,167,154,155,167,155,166,156,165,166,156,166,155,156,164,165,157,164,156,157,163,164,158,
                                                    163,157,158,162,163,159,162,158,159,161,162,159,160,161,131,140,141,132,137,138,132,138,140,132,
                                                    140,131,138,139,140,132,136,137,133,136,132,136,133,134,136,134,135,184,189,183,184,188,189,185,
                                                    188,184,185,187,188,185,186,187,98,99,100,229,2,228,2,229,230,2,230,1,231,1,230,1,
                                                    231,232,1,232,0,233,0,232,233,234,0,31,56,24,25,31,24,31,55,56,31,25,26,31,
                                                    26,30,27,30,26,27,29,30,27,28,29,32,55,31,55,32,33,55,33,54,34,54,33,34,
                                                    53,54,35,53,34,53,35,43,53,43,52,36,43,35,45,46,52,45,52,43,44,45,43,52,
                                                    46,47,52,47,48,52,48,49,52,49,51,49,50,51,36,42,43,37,41,42,37,42,36,37,
                                                    40,41,38,40,37,38,39,40,16,22,15,22,16,17,22,17,21,18,21,17,21,18,19,21,19,20};
            
            dom::NodePtr myShape = createSurface2DFromContour(myScene, myMaterial->getAttributeString("id"),
                                                              myCountour, "TestSurface2DShape");
            dom::NodePtr myIndicesNode = myShape->childNode("primitives")->childNode(0)->childNode(0)->childNode("#text");


            const VectorOfUnsignedInt & myVertexIndices = myIndicesNode->nodeValueRef<VectorOfUnsignedInt >();
            bool mySuccess = true;
            for (int i = 0; i < 699; i++) {
                if (myExceptedIndices[i] != myVertexIndices[i] ) {
                    mySuccess = false;
                    break;
                }
            }
            ENSURE_MSG(mySuccess, "Tesselation of Italien outline");
            return;

        }
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new TesselationUnitTest);
        addTest(new ProxyGeometryUnitTest); // XXX
        addTest(new ModellingUnitTest);
    }
};

int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
