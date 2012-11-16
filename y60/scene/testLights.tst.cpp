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

#include "LightSource.h"
#include "Light.h"
#include "Facades.h"

#include <y60/base/DataTypes.h>
#include <y60/base/Y60xsd.h>

#include <asl/base/Ptr.h>
#include <asl/base/UnitTest.h>
#include <asl/math/linearAlgebra.h>

#include <string>
#include <iostream>

using namespace std;  // automatically added!
using namespace asl;
using namespace y60;
extern const char * myY60xml;

class LightUnitTest : public UnitTest {
public:
    LightUnitTest() : UnitTest("LightUnitTest") {  }
    void run() {
            dom::Document myDocument;

            dom::Document mySchema(y60::ourY60xsd);
            myDocument.addSchema(mySchema, "");
            myDocument.setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
            dom::registerStandardTypes(*myDocument.getValueFactory());
            registerSomTypes(*myDocument.getValueFactory());

            dom::FacadeFactoryPtr myFacadeFactory = dom::FacadeFactoryPtr(new dom::FacadeFactory());
            registerSceneFacades(myFacadeFactory);
            myDocument.setFacadeFactory(myFacadeFactory);

            myDocument.parse(myY60xml);
            ENSURE(myDocument);

            // Test directional light
            {
                LightSourcePtr myLight = myDocument.getElementById("light0")->getFacade<Light>()->getLightSource();
                LightPropertiesFacadePtr myLightPropFacade = myLight->getChild<LightPropertiesTag>();

                ENSURE(myLight);
                ENSURE(myLight->getType() == DIRECTIONAL);
                ENSURE(myLight->get<IdTag>() == "l0" );
                ENSURE(myLightPropFacade->get<LightAmbientTag>() == Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
                ENSURE(myLightPropFacade->get<LightDiffuseTag>() == Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
                ENSURE(myLightPropFacade->get<LightSpecularTag>() == Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            }

            // Test empty light with defaults
            {
                LightSourcePtr myLight = myDocument.getChildElementById("light1", "id")->getFacade<Light>()->getLightSource();;
                LightPropertiesFacadePtr myLightPropFacade = myLight->getChild<LightPropertiesTag>();
                ENSURE(myLight);
                ENSURE(myLight->getType() == UNSUPPORTED);
                ENSURE(myLight->get<IdTag>() == "l1" );
                ENSURE(myLightPropFacade->get<LightAmbientTag>() == Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
                ENSURE(myLightPropFacade->get<LightDiffuseTag>() == Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
                ENSURE(myLightPropFacade->get<LightSpecularTag>() == Vector4f(0,0,0,1));
                ENSURE(myLightPropFacade->get<AttenuationTag>() == 0);
                ENSURE(myLightPropFacade->get<CutOffTag>() == 180);
                ENSURE(myLightPropFacade->get<ExponentTag>() == 0);
            }

            // Test positional light
            {
                LightSourcePtr myPositionalLight = myDocument.getElementById("light4")->getFacade<Light>()->getLightSource();
                LightPropertiesFacadePtr myLightPropFacade = myPositionalLight->getChild<LightPropertiesTag>();
                ENSURE(myPositionalLight);
                ENSURE(myPositionalLight->getType() == POSITIONAL);
                ENSURE(myPositionalLight->get<IdTag>() == "l4" );
                ENSURE(myLightPropFacade->get<AttenuationTag>() == 0.475f);
            }

            LightSourcePtr mySpotLight = myDocument.getElementById("light2")->getFacade<Light>()->getLightSource();
            LightPropertiesFacadePtr myLightPropFacade = mySpotLight->getChild<LightPropertiesTag>();
            ENSURE(mySpotLight);
            ENSURE(mySpotLight->getType() == POSITIONAL);
            ENSURE(mySpotLight->get<IdTag>() == "l2" );
            ENSURE(myLightPropFacade->get<LightAmbientTag>() == Vector4f(0.0f, 1.0f, 1.0f, 1.0f));
            ENSURE(myLightPropFacade->get<LightDiffuseTag>() == Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
            ENSURE(myLightPropFacade->get<LightSpecularTag>() == Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
            ENSURE(myLightPropFacade->get<AttenuationTag>() == 0.275f);
            ENSURE(myLightPropFacade->get<CutOffTag>() == 45.0);
            ENSURE(myLightPropFacade->get<ExponentTag>() == 0.25f);
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new LightUnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);
    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

const char * myY60xml =
"<scene xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' >\n"
"   <lightsources>\n"
"        <lightsource name='parallel light' type='directional' id='l0'>\n"
"            <properties>\n"
"                <vector4f name='ambient'>[1,1,1,1]</vector4f>\n"
"                <vector4f name='diffuse'>[1,1,1,1]</vector4f>\n"
"                <vector4f name='specular'>[1,1,1,1]</vector4f>\n"
"            </properties>\n"
"        </lightsource>\n"
"        <lightsource name='empty'id='l1'/>\n"
"        <lightsource name='Spotlight mit eckigem spot' type='positional' id='l2'>\n"
"            <properties>\n"
"                <vector4f name='ambient'>[0,1,1,1]</vector4f>\n"
"                <vector4f name='diffuse'>[1,0,1,1]</vector4f>\n"
"                <vector4f name='specular'>[1,1,0,1]</vector4f>\n"
"                <float name='attenuation'>0.275</float>\n"
"                <float name='cutoff'>45.0</float>\n"
"                <float name='exponent'>0.25</float>\n"
"            </properties>\n"
"        </lightsource>\n"
"        <lightsource name='Spotlight mit rundem spot' type='positional' id='l3'>\n"
"            <properties>\n"
"                <vector4f name='ambient'>[1,1,1,1]</vector4f>\n"
"                <vector4f name='diffuse'>[1,1,1,1]</vector4f>\n"
"                <vector4f name='specular'>[1,1,1,1]</vector4f>\n"
"                <float name='attenuation'>0.375</float>\n"
"                <float name='cutoff'>10.0</float>\n"
"                <float name='exponent'>0.75</float>\n"
"            </properties>\n"
"        </lightsource>\n"
"        <lightsource name='Positional Light' type='positional' id='l4'>\n"
"            <properties>\n"
"                <vector4f name='ambient'>[0.3555,0.1755,0.3105,1]</vector4f>\n"
"                <vector4f name='diffuse'>[0.3555,0.1755,0.3105,1]</vector4f>\n"
"                <vector4f name='specular'>[0.3555,0.1755,0.3105,1]</vector4f>\n"
"                <float name='attenuation'>0.475</float>\n"
"            </properties>\n"
"        </lightsource>\n"
"    </lightsources>\n"
"    <worlds>\n"
"        <world id='w0'>\n"
"            <light id='light0' name='parallel light' lightsource='l0' position='[-94.3214,-14.0177,87.1063]' scale='[1,1,1]' orientation='[0,0,0,1]'/>\n"
"            <light id='light1' name='distant light' lightsource='l1' position='[-31.2275,-151.391,-46.6983]' scale='[1,1,1]' orientation='[0,0,0,1]'/>\n"
"            <light id='light2' name='Spotlight mit eckigem spot' lightsource='l2' position='[290.206,-33.6423,-307.522]' scale='[3,0.5,0.5]' orientation='[0,0,0,1]'/>\n"
"            <light id='light3' name='Spotlight mit runder spot' lightsource='l3' position='[322.899,155.884,-260.047]' scale='[1,2,3]' orientation='[0,0,0.785398,1]'/>\n"
"            <light id='light4' name='Positional Light' lightsource='l4' position='[133.739,-132.633,-144.569]' scale='[1,1,1]' orientation='[0.139626,0.785398,0,1]'/>\n"
"        </world>\n"
"    </worlds>\n"
"</scene>\n";
