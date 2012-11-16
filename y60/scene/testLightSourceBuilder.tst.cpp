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
#include "LightBuilder.h"
#include "LightSourceBuilder.h"
#include "WorldBuilder.h"
#include <y60/base/DataTypes.h>

#include <asl/base/string_functions.h>
#include <asl/base/UnitTest.h>
#include <asl/math/linearAlgebra.h>
#include <asl/dom/Nodes.h>

#include <y60/base/typedefs.h>
#include <iostream>
#include <cmath>

using namespace std;
using namespace asl;
using namespace y60;

class LightSourceBuilderUnitTest : public UnitTest {
    public:
        LightSourceBuilderUnitTest() : UnitTest("LightSourceBuilderUnitTest") {  }
        void run() {
            //==============================================================================
           // build a simple scene ....
            //==============================================================================

            dom::DocumentPtr myDocument(new dom::Document);
            SceneBuilder mySceneBuilder(myDocument);

            LightSourceBuilder myLightSourceBuilder("testlightsource");
            myLightSourceBuilder.setAmbient (Vector4f(1.0, 1.0, 0.0, 1.0));
            myLightSourceBuilder.setDiffuse (Vector4f(1.0, 0.0, 0.0, 1.0));
            myLightSourceBuilder.setSpecular(Vector4f(0.0, 1.0, 1.0, 1.0));
            myLightSourceBuilder.setType(POSITIONAL);
            mySceneBuilder.appendLightSource(myLightSourceBuilder);

            LightSourceBuilder mySpotLightSourceBuilder("testspotlightsource");
            mySpotLightSourceBuilder.setAttenuation(0.5);
            mySpotLightSourceBuilder.setSpotLight(45.0, 0.75);
            mySceneBuilder.appendLightSource(mySpotLightSourceBuilder);

            //==============================================================================
            // LightSource tests
            //==============================================================================
            dom::NodePtr mySceneNode = mySceneBuilder.getNode();

            // test first light (positional)
            dom::NodePtr myLightSourceNode = mySceneNode->childNode("lightsources")->childNode("lightsource");
            ENSURE(myLightSourceNode->nodeName() == std::string("lightsource"));
            ENSURE(myLightSourceNode->getAttribute("name")->nodeValue() == std::string("testlightsource"));
            ENSURE(myLightSourceNode->getAttribute("type")->nodeValue() == std::string("positional"));
            dom::NodePtr myPropertyListNode = myLightSourceNode->childNode("properties");
            ENSURE(myPropertyListNode);

            dom::NodePtr myAmbientNode = myPropertyListNode->childNode(0);
            ENSURE(myAmbientNode->getAttribute("name")->nodeValue() == std::string("ambient"));
            ENSURE(myAmbientNode->childNode(0)->nodeValue() == std::string("[1,1,0,1]"));
            ENSURE(myAmbientNode->nodeName() == std::string("vector4f"));

            // test property-change
            myLightSourceBuilder.setAmbient(Vector4f(1.0, 0.0, 1.0, 0.0));
            ENSURE(myAmbientNode->childNode(0)->nodeValue() == std::string("[1,0,1,0]"));
            ENSURE(myPropertyListNode->childNodesLength()==3);

            dom::NodePtr myDiffuseNode = myPropertyListNode->childNode(1);
            ENSURE(myDiffuseNode->getAttribute("name")->nodeValue() == std::string("diffuse"));
            ENSURE(myDiffuseNode->childNode(0)->nodeValue() == std::string("[1,0,0,1]"));

            dom::NodePtr mySpecularNode = myPropertyListNode->childNode(2);
            ENSURE(mySpecularNode->getAttribute("name")->nodeValue() == std::string("specular"));
            ENSURE(mySpecularNode->childNode(0)->nodeValue() == std::string("[0,1,1,1]"));


            // test second light (spotlight)
            dom::NodePtr mySpotLightSourceNode = mySceneNode->childNode("lightsources")->childNode("lightsource", 1);
            ENSURE(mySpotLightSourceNode->nodeName() == std::string("lightsource"));
            ENSURE(mySpotLightSourceNode->getAttribute("name")->nodeValue() == std::string("testspotlightsource"));
            ENSURE(mySpotLightSourceNode->getAttribute("type")->nodeValue() == std::string("spot"));
            myPropertyListNode = mySpotLightSourceNode->childNode("properties");
            ENSURE(myPropertyListNode);

            dom::NodePtr myAttenuationNode = myPropertyListNode->childNode(0);
            ENSURE(myAttenuationNode->nodeName() == std::string("float"));
            ENSURE(myAttenuationNode->getAttribute("name")->nodeValue() == std::string("attenuation"));
            ENSURE(myAttenuationNode->childNode(0)->nodeValue() == std::string("0.5"));

            dom::NodePtr myCutoffNode = myPropertyListNode->childNode(1);
            ENSURE(myCutoffNode->nodeName() == std::string("float"));
            ENSURE(myCutoffNode->getAttribute("name")->nodeValue() == std::string("cutoff"));
            ENSURE(myCutoffNode->childNode(0)->nodeValue() == std::string("45"));

            dom::NodePtr myExponentNode = myPropertyListNode->childNode(2);
            ENSURE(myExponentNode->nodeName() == std::string("float"));
            ENSURE(myExponentNode->getAttribute("name")->nodeValue() == std::string("exponent"));
            ENSURE(myExponentNode->childNode(0)->nodeValue() == std::string("0.75"));
        }
};

class LightBuilderUnitTest : public UnitTest {
    public:
        LightBuilderUnitTest() : UnitTest("LightBuilderUnitTest") {  }
        void run() {
            //==============================================================================
            // build a small world....
            //==============================i================================================

            dom::DocumentPtr myDocument(new dom::Document);
            SceneBuilder mySceneBuilder(myDocument);

            WorldBuilder myWorldBuilder;
            mySceneBuilder.appendWorld(myWorldBuilder);
            LightBuilder myLightBuilder("ls1", "myLight");
            myWorldBuilder.appendObject(myLightBuilder);

            myLightBuilder.setPosition(asl::Vector3f(1,2,3));

            dom::NodePtr mySceneNode = mySceneBuilder.getNode();
            dom::NodePtr myLightNode = mySceneNode->childNode("worlds")->childNode("world")
                ->childNode("light");
            ENSURE(myLightNode);
            ENSURE(myLightNode->getAttribute("name")->nodeValue() == "myLight");
            ENSURE(myLightNode->getAttribute("lightsource")->nodeValue() == "ls1");
            ENSURE(myLightNode->getAttribute("position")->nodeValue() == "[1,2,3]");
            ENSURE(myLightNode->getAttribute("scale")->nodeValue() == "[1,1,1]");
            ENSURE(myLightNode->getAttribute("orientation")->nodeValue() == "[0,0,0,1]");
            ENSURE(myLightNode->getAttribute("pivot")->nodeValue() == "[0,0,0]");
        }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new LightSourceBuilderUnitTest);
        addTest(new LightBuilderUnitTest);
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
