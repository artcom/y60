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
