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
            //myVoxelBox.extendBy( Point3f(10.0, 20.0, 30.0)); // 10x20x30 voxels
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

            //ENSURE(positionsEqual(myCandidate, myReference, myCameraMatrix));
            //myModelViewMatrix.rotateY(asl::PI * 0.125);

            //DPRINT( * myShape );

            //dom::NodePtr myBody = createBody(myScene->getWorldRoot(), myShape->getAttributeString(ID_ATTRIB));
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
            y60::ScenePtr myScene;//(new y60::Scene);

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

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
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
