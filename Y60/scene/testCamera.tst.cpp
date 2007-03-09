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
//    $RCSnumeric: test_numeric_functions.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: Test for Cg conforming types.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "WorldBuilder.h"
#include "CameraBuilder.h"
#include "MaterialBuilder.h"

#include <y60/DataTypes.h>
#include <y60/Y60xsd.h>
#include <y60/Facades.h>
#include <asl/string_functions.h>
#include <asl/UnitTest.h>
#include <asl/linearAlgebra.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace asl;
using namespace y60;

class CameraUnitTest : public UnitTest {
    public:
        CameraUnitTest() : UnitTest("CameraUnitTest") {  }
        void run() {
            //==============================================================================
            // build a simple scene ....
            //==============================================================================

            dom::DocumentPtr myDocument(new dom::Document);

            dom::Document mySchema(ourY60xsd);
            myDocument->addSchema(mySchema, "");
            myDocument->setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
            dom::registerStandardTypes(*myDocument->getValueFactory());
            registerSomTypes(*myDocument->getValueFactory());

            dom::FacadeFactoryPtr myFacadeFactory = dom::FacadeFactoryPtr(new dom::FacadeFactory());
            registerSceneFacades(myFacadeFactory);
            myDocument->setFacadeFactory(myFacadeFactory);

            SceneBuilder mySceneBuilder(myDocument);

            WorldBuilder  myWorldBuilder;
            mySceneBuilder.appendWorld(myWorldBuilder);

            CameraBuilder myCameraBuilder("testCamera");
            myWorldBuilder.appendObject(myCameraBuilder);
            myCameraBuilder.setHFov(45.0);
            myCameraBuilder.setPosition(asl::Vector3f(1,2,3));

            //==============================================================================
            // test the results
            //==============================================================================
            dom::NodePtr mySceneNode = mySceneBuilder.getNode();
            dom::NodePtr myCameraNode = mySceneNode->childNode("worlds")->childNode("world")->childNode("camera");
            ENSURE(myCameraNode);
            ENSURE(myCameraNode->getAttribute("name")->nodeValue()=="testCamera");
            // ENSURE(myCameraNode->getAttribute("id")->nodeValue()=="c0");
            ENSURE(myCameraNode->getAttribute("position")->nodeValue() == "[1,2,3]");
            ENSURE(myCameraNode->getAttribute("scale")->nodeValue() == "[1,1,1]");
            DPRINT(myCameraNode->getAttribute("orientation")->nodeValue());
            ENSURE(myCameraNode->getAttribute("orientation")->nodeValue() == "[0,0,0,1]");
            // XXX ENSURE(myCameraNode->getAttribute("hfov")->nodeValue() == "45");
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new CameraUnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

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
