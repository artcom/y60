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

#include "AnimationBuilder.h"
#include "SceneBuilder.h"
#include <y60/base/DataTypes.h>
#include <y60/base/Y60xsd.h>
#include <y60/scene/Facades.h>

#include <asl/base/UnitTest.h>
#include <asl/math/FixedVector.h>

using namespace std;
using namespace asl;
using namespace y60;

class AnimationBuilderUnitTest : public UnitTest {
    public:
        AnimationBuilderUnitTest() : UnitTest("AnimationBuilderUnitTest") {  }
        void run() {
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

            // Setup animation node
            const std::string myName      = "anim1";
            const std::string myNodeRef   = "c1";
            const std::string myAttribute = "hfov";
            const float myDuration        = 10.0f;

            AnimationBuilder myAnimationBuilder;
            mySceneBuilder.appendAnimation(myAnimationBuilder);
            myAnimationBuilder.setName(myName);
            myAnimationBuilder.setNodeRef(myNodeRef);
            myAnimationBuilder.setAttribute(myAttribute);
            myAnimationBuilder.setDuration(myDuration);

            // Test constructor and default attributes
            dom::NodePtr myAnimationNode = myAnimationBuilder.getNode();
            ENSURE(myAnimationNode->nodeName() == "animation");
            ENSURE(myAnimationNode->getAttribute("name")->nodeValue() == myName);
            // ENSURE(myAnimationNode->getAttribute("id")->nodeValue() == "a0");
            ENSURE(myAnimationNode->getAttribute("attribute")->nodeValue() == myAttribute);
            ENSURE(myAnimationNode->getAttribute("node")->nodeValue() == myNodeRef);
            ENSURE(myAnimationNode->getAttribute("duration")->nodeValue() == asl::as_string(myDuration));

            ENSURE(myAnimationNode->getAttribute("enabled")->nodeValue() == "1");
            ENSURE(myAnimationNode->getAttribute("begin")->nodeValue() == "0");
            ENSURE(myAnimationNode->getAttribute("pause")->nodeValue() == "0");
            ENSURE(myAnimationNode->getAttribute("count")->nodeValue() == "1");
            ENSURE(myAnimationNode->getAttribute("direction")->nodeValue() == "forward");

            // Set custom attributes
            const bool myEnable      = true;
            const float myBegin      = 2.5f;
            const float myPause      = 3.3f;
            const unsigned myCount   = 4;
            const AnimationDirection myDirection = PONG;

            myAnimationBuilder.setEnable(myEnable);
            myAnimationBuilder.setBegin(myBegin);
            myAnimationBuilder.setPause(myPause);
            myAnimationBuilder.setCount(myCount);
            myAnimationBuilder.setDirection(myDirection);

            // Test custom attributes
            ENSURE(myAnimationNode->getAttribute("enabled")->nodeValue() == asl::as_string(myEnable));
            ENSURE(myAnimationNode->getAttribute("begin")->nodeValue() == asl::as_string(myBegin));
            ENSURE(myAnimationNode->getAttribute("pause")->nodeValue() == asl::as_string(myPause));
            ENSURE(myAnimationNode->getAttribute("count")->nodeValue() == asl::as_string(myCount));
            ENSURE(myAnimationNode->getAttribute("direction")->nodeValue() == "pong");

            // Add values
            VectorOfString myStringValues;
            myStringValues.push_back("christian");
            myStringValues.push_back("nina");
            myStringValues.push_back("johanna");
            myAnimationBuilder.appendValues(myStringValues);

            VectorOfFloat myFloatValues;
            myFloatValues.push_back(0.2f);
            myFloatValues.push_back(0.4f);
            myFloatValues.push_back(7.5f);
            myAnimationBuilder.appendValues(myFloatValues);

            VectorOfVector2f myVector2fValues;
            myVector2fValues.push_back(as<Vector2f>("[0.2,0.4]"));
            myVector2fValues.push_back(as<Vector2f>("[7,5]"));
            myAnimationBuilder.appendValues(myVector2fValues);

            VectorOfVector3f myVector3fValues;
            myVector3fValues.push_back(as<Vector3f>("[0.2,0.4,7.0]"));
            myVector3fValues.push_back(as<Vector3f>("[7,5,8.6]"));
            myAnimationBuilder.appendValues(myVector3fValues);

            VectorOfVector4f myVector4fValues;
            myVector4fValues.push_back(as<Vector4f>("[0.2,0.4,8,1.1]"));
            myVector4fValues.push_back(as<Vector4f>("[7,5,0.2,6]"));
            myAnimationBuilder.appendValues(myVector4fValues);

            // Test values
            VectorOfString myStrings = myAnimationNode->childNode(0)->
                        childNode(0)->nodeValueAs<VectorOfString>();
            ENSURE(myStrings[0] == myStringValues[0]);
            ENSURE(myStrings[1] == myStringValues[1]);
            ENSURE(myStrings[2] == myStringValues[2]);
            VectorOfFloat myFloats = myAnimationNode->childNode(1)->
                        childNode(0)->nodeValueAs<VectorOfFloat>();
            for (unsigned i = 0; i < myFloats.size(); ++i) {
                ENSURE(asl::almostEqual(myFloats[i], myFloatValues[i]));
            }
            VectorOfVector2f myVector2fs = myAnimationNode->childNode(2)->
                childNode(0)->nodeValueAs<VectorOfVector2f>();
            for (unsigned i = 0; i < myVector2fs.size(); ++i) {
                ENSURE(asl::almostEqual(myVector2fs[i], myVector2fValues[i]));
            }
            VectorOfVector3f myVector3fs = myAnimationNode->childNode(3)->
                childNode(0)->nodeValueAs<VectorOfVector3f>();
            for (unsigned i = 0; i < myVector3fs.size(); ++i) {
                ENSURE(asl::almostEqual(myVector3fs[i], myVector3fValues[i]));
            }
            VectorOfVector4f myVector4fs = myAnimationNode->childNode(4)->
                childNode(0)->nodeValueAs<VectorOfVector4f>();
            for (unsigned i = 0; i < myVector4fs.size(); ++i) {
                ENSURE(asl::almostEqual(myVector4fs[i], myVector4fValues[i]));
            }

            // Test id counter
            AnimationBuilder myAnimationBuilder2;
            mySceneBuilder.appendAnimation(myAnimationBuilder2);
            myAnimationBuilder2.setName(myName);
            myAnimationBuilder2.setNodeRef(myNodeRef);
            myAnimationBuilder2.setAttribute(myAttribute);
            myAnimationBuilder2.setDuration(myDuration);

            dom::NodePtr myAnimationNode2 = myAnimationBuilder2.getNode();
            // ENSURE(myAnimationNode2->getAttribute("id")->nodeValue() == "a1");
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup();
        addTest(new AnimationBuilderUnitTest);
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
