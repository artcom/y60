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

#include <y60/base/Y60xsd.h>

#include <y60/scene/Scene.h>
#include <y60/scene/TextureManager.h>

#include <asl/base/UnitTest.h>
#include <asl/math/Vector234.h>
#include <asl/dom/Nodes.h>
#include <asl/dom/Schema.h>

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
            for (VectorOfVector3f::size_type i = 0; i < theReference.size(); ++i) {
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
            myCameraMatrix.makeXRotating( static_cast<float>(asl::PI/2) );
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
            /*
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
            */
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

            const unsigned myExceptedIndices[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
                                                    25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
                                                    49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,
                                                    73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,
                                                    97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,
                                                    121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,
                                                    145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,
                                                    169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,
                                                    193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,
                                                    217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,
                                                    241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,
                                                    265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,
                                                    289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,
                                                    313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,
                                                    337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,
                                                    361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,
                                                    385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,
                                                    409,410,411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,
                                                    433,434,435,436,437,438,439,440,441,442,443,444,445,446,447,448,449,450,451,452,453,454,455,456,
                                                    457,458,459,460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,
                                                    481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,501,502,503,504,
                                                    505,506,507,508,509,510,511,512,513,514,515,516,517,518,519,520,521,522,523,524,525,526,527,528,
                                                    529,530,531,532,533,534,535,536,537,538,539,540,541,542,543,544,545,546,547,548,549,550,551,552,
                                                    553,554,555,556,557,558,559,560,561,562,563,564,565,566,567,568,569,570,571,572,573,574,575,576,
                                                    577,578,579,580,581,582,583,584,585,586,587,588,589,590,591,592,593,594,595,596,597,598,599,600,
                                                    601,602,603,604,605,606,607,608,609,610,611,612,613,614,615,616,617,618,619,620,621,622,623,624,
                                                    625,626,627,628,629,630,631,632,633,634,635,636,637,638,639,640,641,642,643,644,645,646,647,648,
                                                    649,650,651,652,653,654,655,656,657,658,659,660,661,662,663,664,665,666,667,668,669,670,671,672,
                                                    673,674,675,676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,
                                                    697,698};
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
