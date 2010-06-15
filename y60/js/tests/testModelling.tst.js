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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __

*/

use("UnitTest.js");

function ModellingUnitTest() {
    this.Constructor(this, "ModellingUnitTest");
}


ModellingUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        var myScene = new Scene();
        myScene.setup();
        var myRedColor = new Vector4f([1,0,0,1]);
        var myGreenColor = new Vector4f([0,1,0,1]);
        var myBlueColor = new Vector4f([0,0,1,1]);
        var myTestFile = "fixtures/DiffuseRamp.png";


        var myTransform = Modelling.createTransform(myScene.world);
        ENSURE(myTransform.parentNode.id == myScene.world.id);


        var myColorMaterial = Modelling.createColorMaterial(myScene , myBlueColor);
        ENSURE(almostEqual(myColorMaterial.properties.surfacecolor, myBlueColor));


        var myQuad = Modelling.createQuad(myScene,
                                          myColorMaterial.id,
                                          new Vector3f([-1,-1,0]),
                                          new Vector3f([1,1,0]));
        myQuad.name = "theQuad";
        ENSURE(myScene.dom.find(".//*[@name='theQuad']") != undefined);


        var myFileImage = Modelling.createImage(myScene,
                                                myTestFile);
        myFileImage.name = "theImage";
        ENSURE(myScene.dom.find(".//*[@name='theImage']") != undefined);

        var myTestImage = Modelling.createImage(myScene, 800, 520, "RGB");
        myTestImage.name = "theTestImage";
        ENSURE(myScene.dom.find(".//*[@name='theTestImage']") != undefined);


        var myImageMaterial = Modelling.createUnlitTexturedMaterial(myScene,
                                                                    myFileImage,
                                                                    "theImageMaterial",
                                                                    1,
                                                                    1,
                                                                    myRedColor);

        ENSURE(myScene.dom.find(".//*[@name='theImageMaterial']") != undefined);
        ENSURE(almostEqual(myImageMaterial.properties.surfacecolor,myRedColor));


        var myTexturedMaterial = Modelling.createUnlitTexturedMaterial(myScene,
                                                                       myTestFile,
                                                                       "theTexturedMaterial",
                                                                       1,
                                                                       1,
                                                                       myGreenColor,
                                                                       1);
        ENSURE(almostEqual(myTexturedMaterial.properties.surfacecolor,myGreenColor));
        ENSURE(myScene.dom.find(".//*[@name='theTexturedMaterial']") != undefined);


        var myLambertMaterial = Modelling.createLambertMaterial(myScene,
                                                                myBlueColor,
                                                                myRedColor);
        myLambertMaterial.name = "theLambertMaterial";
        ENSURE(almostEqual(myLambertMaterial.properties.diffuse,myBlueColor));
        ENSURE(almostEqual(myLambertMaterial.properties.ambient,myRedColor));
        ENSURE(myScene.dom.find(".//*[@name='theLambertMaterial']") != undefined);


        var myBody = Modelling.createBody(myScene.world, myQuad);
        myBody.name = "theBody";
        ENSURE(myScene.dom.find(".//*[@name='theBody']") != undefined);


        var myCanvas = Modelling.createCanvas(myScene, "theCanvas");
        ENSURE(myScene.dom.find(".//*[@name='theCanvas']") != undefined);

        var myCrosshair = Modelling.createCrosshair(myScene,
                                                    myLambertMaterial.id,
                                                    5,
                                                    10,
                                                    "theCrosshair");
        ENSURE(myScene.dom.find(".//*[@name='theCrosshair']") != undefined);

        //generate same strip positions for all three shapes
        var myNumStripElements = 6;
        var myPositions = [];
        for(var i = 0; i < myNumStripElements; ++i) {
            var myXPos = i-(myNumStripElements-1)/2;
            var myUpperPosition = new Vector3f(myXPos, 0.5, 0);
            var myLowerPosition = new Vector3f(myXPos, -0.5, 0);
            myPositions.push(myUpperPosition);
            myPositions.push(myLowerPosition);
        }
        var myLineStrip = Modelling.createLineStrip(myScene,
                                                    myColorMaterial.id,
                                                    myPositions);
        myLineStrip.name = "theLineStrip";
        ENSURE(myScene.dom.find(".//*[@name='theLineStrip']") != undefined);


        var myQuadStrip = Modelling.createQuadStrip(myScene,
                                                    myColorMaterial.id,
                                                    myPositions);
        myQuadStrip.name = "theQuadStrip";
        ENSURE(myScene.dom.find(".//*[@name='theQuadStrip']") != undefined);


        var myTriangleStrip = Modelling.createTriangleStrip(myScene,
                                                            myColorMaterial.id,
                                                            myPositions);
        myTriangleStrip.name = "theTriangleStrip";
        ENSURE(myScene.dom.find(".//*[@name='theTriangleStrip']") != undefined);

//         {"createTriangleStack",             CreateQuadStack,         5},
//         {"createVoxelProxyGeometry",    CreateVoxelProxyGeometry,    7},
//         {"setAlpha",                    SetAlpha,                    2},
//         {"createSurface2DFromContour",  CreateSurface2DFromContour,  4},
//         {"createDistanceMarkup",        CreateDistanceMarkup,        5},
//         {"createAngleMarkup",           CreateAngleMarkup,           6},
//         {"createTriangleMeshMarkup",    CreateTriangleMeshMarkup,    5},

    }
}

var myTestName = "testModelling.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new ModellingUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
