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

function Y60JSSLUnitTest() {
    this.Constructor(this, "Y60JSSLUnitTest");
};


Y60JSSLUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function testRemoveElement(theArray, theResult) {
        DTITLE("removing elemnts");

        obj.myArray = theArray;
        obj.myResult = theResult;
        for (var i = 0; i < obj.myArray.length; ++i) {
            if (obj.myArray[i] == 0) {
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
        ENSURE(myAttributeCount == 2 && myNode.attributes.length == 1,
            "added two attributes and removed the first");

        myNode = removeAttributeByName(myNode, "nonexistingAttrib");
        ENSURE(myNode.attributes.length == 1,
               "trying to remove a non-existing attribute");

        myNode = removeAttributeByName(myNode, "test2");
        ENSURE(myNode.attributes.length == 0,
            "removed the last remaining attribute");
    }

    function testAngleFunctions() {
        DTITLE("angle functions");

        ENSURE( 'degBetween(0,0) == 0');
        ENSURE( 'degBetween(0, 90) == -90 ' );
        ENSURE( 'degBetween(90, 0) == 90 ' );

        ENSURE( 'degBetween(370, 350) == 20 ' );
        ENSURE( 'degBetween(10, 350) == 20 ' );

        ENSURE( 'degBetween(350, 370) == -20 ' );
        ENSURE( 'degBetween(350, 10) == -20 ' );

        ENSURE( 'degBetween(190, 170) == 20 ' );
        ENSURE( 'degBetween(170, 190) == -20 ' );
    }

    obj.run = function() {
        testAngleFunctions();
        testRemoveElement([], []);
        testRemoveElement([0], []);
        testRemoveElement([0,0], []);
        testRemoveElement([0,1,2], [1,2]);
        testRemoveElement([1,2,0], [1,2]);
        testRemoveElement([1,0,2], [1,2]);
        testRemoveElement([1,2,3], [1,2,3]);
        testRemoveElement([0,0,1,0,0,2,0,0,3,0,0], [1,2,3]);
        testRemoveAttribute();
    }
};

var myTestName = "testY60JSSL.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new Y60JSSLUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
