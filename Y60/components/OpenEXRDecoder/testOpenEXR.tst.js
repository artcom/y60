/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testOpenEXR.tst.js,v $
//
//   $Revision: 1.5 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
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
        plug("y60OpenEXR");
        var window = new RenderWindow();

        GLResourceManager.loadShaderLibrary("../../../../shader/shaderlibrary.xml");

        //var myScene = new Scene();
        var myScene = window.scene;
        var myMaterial = myScene.createTexturedMaterial("../../testimages/Desk.exr");
        var myShape    = myScene.createQuadShape(myMaterial, [-0.4,-0.4,0], [0.4,0.4,0]);
        var myBody     = myScene.createBody(myShape);

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
            obj.myHDRWindow.x = theX / window.width * 2; // scale
            obj.myHDRWindow.y = (window.height - theY) * -2/ window.height; // bias
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
