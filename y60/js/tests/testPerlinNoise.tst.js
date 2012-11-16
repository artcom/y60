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

/*jslint plusplus:false*/
/*globals print, use, UnitTest, PerlinNoise, ENSURE, DPRINT, millisec, exit,
          UnitTestSuite*/

use("UnitTest.js");
use("Overlay.js");

function PerlinNoiseUnitTest() {
    this.Constructor(this, "PerlinNoiseUnitTest");
}

PerlinNoiseUnitTest.prototype.Constructor = function (obj, theName) {
    
    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function () {
        obj.myPerlinNoise = new PerlinNoise(3);
        ENSURE('obj.myPerlinNoise.octavecount == 3');
        ENSURE('obj.myPerlinNoise.amplitudefalloff == 0.5');
        obj.myPerlinNoise = new PerlinNoise(4, 0.9);
        ENSURE('obj.myPerlinNoise.octavecount == 4');
        ENSURE('almostEqual(obj.myPerlinNoise.amplitudefalloff, 0.9)');
        DPRINT('obj.myPerlinNoise.noise(0.1)');
        DPRINT('obj.myPerlinNoise.noise([0.1,0.1])');
        DPRINT('obj.myPerlinNoise.noise([1,2,3])');
        DPRINT('obj.myPerlinNoise.noise(new Vector2f(0.2,0.3))');

        var myNoise = new PerlinNoise(3);
        var myTime = millisec();
        for (var i = 0; i < 1000; ++i) {
            myNoise.noise([1, 2, 3]);
        }
        print("timing: " + (millisec() - myTime) / 1000);
/*
        var myWindow = new RenderWindow();
        for (var i = 0; i < 10; ++i) {
            for (var j = 0; j < 10; ++j) {
                var myNoise = obj.myPerlinNoise.noise([i,j])
                new Overlay(myWindow.scene, new Vector4f(myNoise, myNoise, myNoise, 1), [i * 10 + 1, j * 10 + 1], [10,10]);
            }
        }
        myWindow.go();
*/
    };
};

var myTestName = "testPerlinNoise.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new PerlinNoiseUnitTest());
mySuite.run();

print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
