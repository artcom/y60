/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testY60JSSL.tst.js,v $
//
//   $Revision: 1.5 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function Y60JSSLUnitTest() {
    this.Constructor(this, "Y60JSSLUnitTest");
};


Y60JSSLUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function testRemoveElement(theArray, theResult) {
        obj.myArray = theArray;        
        obj.myResult = theResult;
        for (var i = 0; i < obj.myArray.length; ++i) {
            if (obj.myArray[i] == 0) {
                obj.myArray = obj.myArray.removeElement(i);
                i--;
            }
        }

        ENSURE("obj.myArray.length == obj.myResult.length");
        for (obj.i = 0; obj.i < obj.myArray.length; ++obj.i) {
            ENSURE('obj.myArray[obj.i] == obj.myResult[obj.i]');
        }
    }

    obj.run = function() {
        testRemoveElement([], []);
        testRemoveElement([0], []);
        testRemoveElement([0,0], []);
        testRemoveElement([0,1,2], [1,2]);
        testRemoveElement([1,2,0], [1,2]);
        testRemoveElement([1,0,2], [1,2]);
        testRemoveElement([1,2,3], [1,2,3]);
        testRemoveElement([0,0,1,0,0,2,0,0,3,0,0], [1,2,3]);
    }
};

var myTestName = "testY60JSSL.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new Y60JSSLUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
