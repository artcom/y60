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

/*jslint plusplus: false, nomen:false*/
/*globals print, use, UnitTest, ENSURE, Node, DTITLE, exit, UnitTestSuite,
          removeAttributeByName, removeElement, stripIdentifier,
          parseRankedFeature, js, __FILE__,
          ENSURE_EQUAL, arrayToString, Vector3f, Matrix4f*/

use("Y60JSSL.js");
use("UnitTest.js");

function Y60JSSLUnitTest() {
    this.Constructor(this, "Y60JSSLUnitTest");
}

Y60JSSLUnitTest.prototype.Constructor = function (obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function testRemoveElement(theArray, theResult) {
        DTITLE("removing elements");

        obj.myArray = theArray;
        obj.myResult = theResult;
        for (var i = 0; i < obj.myArray.length; ++i) {
            if (obj.myArray[i] === 0) {
                obj.myArray = removeElement(obj.myArray, i);
                i--;
            }
        }

        ENSURE("obj.myArray.length == obj.myResult.length");
        for (obj.i = 0; obj.i < obj.myArray.length; ++obj.i) {
            ENSURE('obj.myArray[obj.i] == obj.myResult[obj.i]');
        }
    }

    function testRemoveAttribute() {
        DTITLE("removing attributes");

        var myNode = Node.createElement("testNode");
        myNode.test = "testval";
        myNode.test2 = "testval2";
        var myAttributeCount = myNode.attributes.length;
        myNode = removeAttributeByName(myNode, "test");
        ENSURE(myAttributeCount === 2 && myNode.attributes.length === 1,
            "added two attributes and removed the first");

        myNode = removeAttributeByName(myNode, "nonexistingAttrib");
        ENSURE(myNode.attributes.length === 1,
               "trying to remove a non-existing attribute");

        myNode = removeAttributeByName(myNode, "test2");
        ENSURE(myNode.attributes.length === 0,
            "removed the last remaining attribute");
    }

    function testAngleFunctions() {
        DTITLE("angle functions");

        ENSURE('degBetween(0,0) == 0');
        ENSURE('degBetween(0, 90) == -90 ');
        ENSURE('degBetween(90, 0) == 90 ');

        ENSURE('degBetween(370, 350) == 20 ');
        ENSURE('degBetween(10, 350) == 20 ');

        ENSURE('degBetween(350, 370) == -20 ');
        ENSURE('degBetween(350, 10) == -20 ');

        ENSURE('degBetween(190, 170) == 20 ');
        ENSURE('degBetween(170, 190) == -20 ');
    }
    
    function testArrayToString() {
        DTITLE("arrayToString");
        
        ENSURE_EQUAL("[]",
                     arrayToString([]),
                     "empty array");
        ENSURE_EQUAL("[[]]",
                     arrayToString([[]]),
                     "empty array in an array");
        ENSURE_EQUAL("[[1,1,1],identity[[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]]",
                     arrayToString([new Vector3f(1, 1, 1), new Matrix4f()]),
                     "array with non-primitive members");
    }
    
    function testParseRankedFeature() {
        DTITLE("parseRankedFeature");
        
        var myRankedFeature = parseRankedFeature("123[]");
        ENSURE_EQUAL(true, myRankedFeature.features.constructor === Array, "features is an Array");
        ENSURE_EQUAL(1, myRankedFeature.features.length, "no features given - length is 1 (??)"); // This does not really make sense...
        ENSURE_EQUAL("", myRankedFeature.features[0], "and this feature is '' (??)");
        ENSURE_EQUAL("number", typeof myRankedFeature.rank, "rank is a Number");
        ENSURE_EQUAL(123, myRankedFeature.rank, "proper rank");
        
        myRankedFeature = parseRankedFeature("123[water,ice,fire]");
        ENSURE_EQUAL(3, myRankedFeature.features.length, "proper length of features");
        ENSURE_EQUAL("water", myRankedFeature.features[0], "first feature proper");
        ENSURE_EQUAL("ice", myRankedFeature.features[1], "second feature proper");
        ENSURE_EQUAL("fire", myRankedFeature.features[2], "third feature proper");
    }

    function teststripIdentifier() {
        DTITLE("stripIdentifier");
        
        ENSURE_EQUAL("theTestFile.js", stripIdentifier("theTest-File.js"));
        ENSURE_EQUAL("PathtonowheretheTestFile.js", stripIdentifier("Path/to\\nowhere/theTest-File.js"));
    }

    function testArrayIndexOf() {
        DTITLE("js.array.indexOf");
        
        ENSURE_EQUAL(-1, js.array.indexOf([], 1), "elements not in array return -1");
        ENSURE_EQUAL(0, js.array.indexOf([1, 1], 1), "first Element's index found is returned");
        ENSURE_EQUAL(-1, js.array.indexOf(["0","1","2"], 2), "elements are found by identity comparison");
        ENSURE_EQUAL(2, js.array.indexOf(["2","1","2"], "2", 1), "from argument starts search at that given index");
        ENSURE_EQUAL(0, js.array.indexOf(["2","1","2"], "2", 0), "from argument starts search at that given index");
    }

    obj.run = function () {
        testAngleFunctions();
        testRemoveElement([], []);
        testRemoveElement([0], []);
        testRemoveElement([0, 0], []);
        testRemoveElement([0, 1, 2], [1, 2]);
        testRemoveElement([1, 2, 0], [1, 2]);
        testRemoveElement([1, 0, 2], [1, 2]);
        testRemoveElement([1, 2, 3], [1, 2, 3]);
        testRemoveElement([0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0], [1, 2, 3]);
        testRemoveAttribute();
        testArrayToString();
        testParseRankedFeature();
        teststripIdentifier();
        testArrayIndexOf();
    };
};

try {
    var myScriptPathSegments = __FILE__().split("/");
    var mySuiteName = myScriptPathSegments[myScriptPathSegments.length - 1];
    var mySuite = new UnitTestSuite(mySuiteName);

    // may be more than one test here
    mySuite.addTest(new Y60JSSLUnitTest()); 
    mySuite.run();

    print(">> Finished test suite '" + mySuiteName + "', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}