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

function OpenEXRUnitTest() {
    this.Constructor(this, "OpenEXRUnitTest");
};


OpenEXRUnitTest.prototype.Constructor = function(obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    obj.myHDRWindow = null;

    obj.run = function() {
        plug("OpenEXR");
        var window = new RenderWindow();

        //var myScene = new Scene();
        var myScene = window.scene;
        var myMaterial = Modelling.createUnlitTexturedMaterial(myScene, "testimages/Desk.exr");
        var myShape    = Modelling.createQuad(myScene, myMaterial.id, [-0.4,-0.4,0], [0.4,0.4,0]);
        var myBody     = Modelling.createBody(myScene.world, myShape.id);

        var myImage = myScene.images.firstChild;
        myImage.resize = "pad";
        myMaterial.requires.textures = "[100[hdr_paint]]";

        window.scene = myScene;
        window.camera.position = [0,0,2];

        window.onFrame = function(theTime) {
            if (obj.myHDRWindow == null) {
                obj.myHDRWindow = myMaterial.properties.hdr;
                ENSURE("almostEqual(obj.myHDRWindow, [0,1])");
            }
            if (theTime > 1.0) {
                window.stop();
            }

            window.renderText([10,10], "HDR scale: " + obj.myHDRWindow.x.toFixed(2) + ", bias: " + obj.myHDRWindow.y.toFixed(2));
        }

        window.onMouseMotion = function(theX, theY) {
            if (obj.myHDRWindow) {
                obj.myHDRWindow.x = theX / window.width * 2; // scale
                obj.myHDRWindow.y = (window.height - theY) * -2/ window.height; // bias
            }
        }
        window.go();
    }
};

var myTestName = "testOpenEXR.tst.js";
var mySuite = new UnitTestSuite(myTestName);
mySuite.addTest(new OpenEXRUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
