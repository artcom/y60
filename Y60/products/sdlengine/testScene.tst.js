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
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function MyClassUnitTest() {
    this.Constructor(this, "MyClassUnitTest");
}


MyClassUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myScene = new Scene(); // create scene with stubs
        obj.myScene.setup();
        
        obj.testScene();
    }

    obj.testScene = function() {
        // Test convinience scene access properties
        ENSURE('obj.myScene.dom.nodeName == "scene"');
        ENSURE('obj.myScene.world.nodeName == "world"');
        ENSURE('obj.myScene.canvases.nodeName == "canvases"');
        ENSURE('obj.myScene.canvas.nodeName == "canvas"');
        ENSURE('obj.myScene.materials.nodeName == "materials"');
        ENSURE('obj.myScene.lightsources.nodeName == "lightsources"');
        ENSURE('obj.myScene.animations.nodeName == "animations"');
        ENSURE('obj.myScene.characters.nodeName == "characters"');
        ENSURE('obj.myScene.shapes.nodeName == "shapes"');
        ENSURE('obj.myScene.images.nodeName == "images"');
        ENSURE('obj.myScene.cameras[0].nodeName == "camera"');

        // Test array assignment to vector nodes        
        obj.myScene.world.position = new Vector3f(1,1,1);
        ENSURE('almostEqual(obj.myScene.world.position, new Vector3f(1,1,1))');
        obj.myScene.world.position = [2,3,4];
        ENSURE('almostEqual(obj.myScene.world.position, new Vector3f(2,3,4))');
        obj.myScene.world.position = [1.1,2.2,3.3];
        ENSURE('almostEqual(obj.myScene.world.position, new Vector3f(1.1,2.2,3.3))');
        obj.myScene.world.position = ["1","2","3"];
        ENSURE('almostEqual(obj.myScene.world.position, new Vector3f(1,2,3))');
    }
}

var myTestName = "testMyClass.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new MyClassUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
