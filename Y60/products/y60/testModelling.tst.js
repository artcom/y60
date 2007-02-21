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
//    $RCSfile: testMyClass.tst.js,v $
//
//   $Revision: 1.5 $
p//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");
//use("ImageManager.js");

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
        var myTestFile = "../../testfiles/DiffuseRamp.png";


        var myTransform = Modelling.createTransform(myScene.world);
        ENSURE(myTransform.parentNode.id == myScene.world.id);


        var myColorMaterial = Modelling.createColorMaterial(myScene , myBlueColor);
        ENSURE(almostEqual(myColorMaterial.properties.surfacecolor, myBlueColor));
   
     
        var myQuad = Modelling.createQuad(myScene, 
                                          myColorMaterial.id, 
                                          new Vector3f([-1,-1,0]),
                                          new Vector3f([1,1,0]));
        myQuad.name = "theQuad";
        ENSURE(getDescendantByName(myScene.dom, "theQuad", true) != undefined);
   
     
        var myFileImage = Modelling.createImage(myScene,
                                                myTestFile);
        myFileImage.name = "theImage";
        ENSURE(getDescendantByName(myScene.dom, "theImage", true) != undefined);

        var myTestImage = Modelling.createImage(myScene, 800, 520, "RGB");
        myTestImage.name = "theTestImage";
        ENSURE(getDescendantByName(myScene.dom, "theTestImage", true) != undefined);


        var myImageMaterial = Modelling.createUnlitTexturedMaterial(myScene,
                                                                    myFileImage,
                                                                    "theImageMaterial",
                                                                    1,
                                                                    1,
                                                                    myRedColor);

        ENSURE(getDescendantByName(myScene.dom, "theImageMaterial", true) != undefined);
        ENSURE(almostEqual(myImageMaterial.properties.surfacecolor,myRedColor));


        var myTexturedMaterial = Modelling.createUnlitTexturedMaterial(myScene,
                                                                       myTestFile,
                                                                       "theTexturedMaterial",
                                                                       1,
                                                                       1,
                                                                       myGreenColor,
                                                                       1);
        ENSURE(almostEqual(myTexturedMaterial.properties.surfacecolor,myGreenColor));
        ENSURE(getDescendantByName(myScene.dom, "theTexturedMaterial", true) != undefined);


        var myLambertMaterial = Modelling.createLambertMaterial(myScene,
                                                                myBlueColor,
                                                                myRedColor);
        myLambertMaterial.name = "theLambertMaterial";
        ENSURE(almostEqual(myLambertMaterial.properties.diffuse,myBlueColor));
        ENSURE(almostEqual(myLambertMaterial.properties.ambient,myRedColor));
        ENSURE(getDescendantByName(myScene.dom, "theLambertMaterial", true) != undefined);
      
  
        var myBody = Modelling.createBody(myScene.world, myQuad);
        myBody.name = "theBody";
        ENSURE(getDescendantByName(myScene.dom, "theBody", true) != undefined);


        var myCanvas = Modelling.createCanvas(myScene, "theCanvas");
        ENSURE(getDescendantByName(myScene.dom, "theCanvas", true) != undefined);


        var myCrosshair = Modelling.createCrosshair(myScene, 
                                                    myLambertMaterial.id, 
                                                    5,
                                                    10,
                                                    "theCrosshair");
        ENSURE(getDescendantByName(myScene.dom, "theCrosshair", true) != undefined);

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
        ENSURE(getDescendantByName(myScene.dom, "theLineStrip", true) != undefined);
        

        var myQuadStrip = Modelling.createQuadStrip(myScene,
                                                    myColorMaterial.id,
                                                    myPositions);
        myQuadStrip.name = "theQuadStrip";
        ENSURE(getDescendantByName(myScene.dom, "theQuadStrip", true) != undefined);


        var myTriangleStrip = Modelling.createTriangleStrip(myScene,
                                                            myColorMaterial.id,
                                                            myPositions);
        myTriangleStrip.name = "theTriangleStrip";
        ENSURE(getDescendantByName(myScene.dom, "theTriangleStrip", true) != undefined);
        
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
