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
//    $RCSfile: testPerlinNoise.tst.js,v $
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

function PerlinNoiseUnitTest() {
    this.Constructor(this, "PerlinNoiseUnitTest");
};

PerlinNoiseUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myPerlinNoise = new PerlinNoise(1, 3);
        ENSURE('obj.myPerlinNoise.persistence == 1');
        ENSURE('obj.myPerlinNoise.octavecount == 3');
        print("Noise [1,1] = " + obj.myPerlinNoise.noise2D([1,1]));
        obj.myPerlinNoise.persistence = 1.5;
        ENSURE('obj.myPerlinNoise.persistence == 1.5');
        var myTime = millisec();
        print("Noise [1,1] = " + obj.myPerlinNoise.noise2D([1,1]));
        print("timing: " + (millisec() - myTime));
        print("Noise [1,1] = " + obj.myPerlinNoise.noise2D(new Vector2f(1,1)));

/*
        var myWindow = new RenderWindow();
        for (var i = 0; i < 10; ++i) {
            print(i);
            for (var j = 0; j < 10; ++j) {
                var myNoise = obj.myPerlinNoise.noise2D([i,j])
                new Overlay(myWindow.scene, new Vector4f(myNoise, myNoise, myNoise, 1), [i * 10 + 1, j * 10 + 1], [10,10]);
            }
        }
        myWindow.go();
*/
    }
};

var myTestName = "testPerlinNoise.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new PerlinNoiseUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
