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
//    $RCSfile: testHotCoding.tst.js,v $
//
//   $Revision: 1.5 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function HotCodingUnitTest() {
    this.Constructor(this, "HotCodingUnitTest");
};

HotCodingUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myModuleName = "TemporaryTestModule.js";
        obj.myModule = "function calculateSum(a,b) { return a+b; };";
        ENSURE("writeWholeStringToFile(obj.myModuleName, obj.myModule) == true");;
        ENSURE("fileExists(obj.myModuleName)");
        ENSURE("readWholeFileAsString(obj.myModuleName) == obj.myModule");
        use(obj.myModuleName);
        ENSURE("calculateSum(1,2) == 3");

        obj.myModule = "function calculateSum(a,b) { return a+b + 1; };";
        writeWholeStringToFile(obj.myModuleName, obj.myModule);
        ENSURE("readWholeFileAsString(obj.myModuleName) == obj.myModule");
        reuse();
        ENSURE("calculateSum(1,2) == 4");
        ENSURE("deleteFile(obj.myModuleName) == true");
        ENSURE("!fileExists(obj.myModuleName)");
    }
};

var myTestName = "testHotCoding.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new HotCodingUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
