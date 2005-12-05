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
use("Overlay.js");
use("Y60JSSL.js");

function OpenEXRUnitTest() {
    this.Constructor(this, "OpenEXRUnitTest");
};


OpenEXRUnitTest.prototype.Constructor = function(obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    var _myMaterial = null;
    var _myHDRWindow = null;

    obj.setWindow = function(theMin, theMax) {
        theMax = 1.0;
        if (theMin > theMax) {
            var t = theMin;
            theMin = theMax;
            theMax = t;
        }
        _myHDRWindow.x = 1.0 / (theMax - theMin);
        _myHDRWindow.y = -_myHDRWindow.x * theMin;
        //print("min=" + theMin, "max=" + theMax, "window=" + _myHDRWindow);
    }

    obj.run = function() {
        plug("y60OpenEXR");

        var window = new RenderWindow();

        _myMaterial = window.scene.createTexturedMaterial("../../testimages/Desk.exr");
        var myShape    = window.scene.createQuadShape(_myMaterial, [-0.4,-0.4,0], [0.4,0.4,0]);
        var myBody     = window.scene.createBody(myShape);

        var myImage = window.scene.images.firstChild;
        myImage.resize = "pad";
        _myMaterial.requires.lighting = "[120[hdr]]";
        window.scene.update(Scene.ALL);

        window.camera.position = [0,0,2];

        window.onStartMainLoop = function() {
            //print("onStartMainLoop");
            //print(_myMaterial.properties.hdr);
        }

        window.onFrame = function(theTime) {
            if (_myHDRWindow == null) {
                _myHDRWindow = _myMaterial.properties.hdr;
                obj.setWindow(0,1);
            }
            if (theTime > 5.0) {
                exit(0);
            }
        }
        
        window.onMouseMotion = function(theX, theY) {
            var min = theX / window.width;
            var max = (window.height - theY) / window.height;
            obj.setWindow(min, max);
        }
        window.go();

        obj.myVar = 1;
        ENSURE('obj.myVar == 1');
    }
};

var myTestName = "testOpenEXR.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new OpenEXRUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
