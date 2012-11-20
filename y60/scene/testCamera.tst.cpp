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

#include "SceneBuilder.h"
#include "ShapeBuilder.h"
#include "WorldBuilder.h"
#include "CameraBuilder.h"
#include "MaterialBuilder.h"

#include <y60/base/DataTypes.h>
#include <y60/base/Y60xsd.h>
#include <y60/scene/Facades.h>
#include <asl/base/string_functions.h>
#include <asl/base/UnitTest.h>
#include <asl/math/linearAlgebra.h>
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

            dom::Document mySchema(y60::ourY60xsd);
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
