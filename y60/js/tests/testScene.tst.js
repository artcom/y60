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

function MySceneUnitTest() {
    this.Constructor(this, "MySceneUnitTest");
}


MySceneUnitTest.prototype.Constructor = function(obj, theName) {

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

        /*var myTimeStart = millisec();
        var myIterations = 10000;
        for (var i = 0; i < myIterations; i++) {
            var myImages = obj.myScene.images;
        }
        print("Duration for " + myIterations + " : " + ((millisec() - myTimeStart) / 1000));
        */
    }
}

var myTestName = "testScene.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new MySceneUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
