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
//    $RCSfile: testFileFunctions.tst.js,v $
//
//   $Revision: 1.2 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function TestFileFunctions() {
    this.Constructor(this, "TestFileFunctions");
};

TestFileFunctions.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        print("Directory Listing for .: " + getDirectoryEntries("."));
        print("Directory Listing for ${PRO}: " + getDirectoryEntries("${PRO}"));
        ENSURE('fileExists("bla") == false');
        ENSURE('fileExists(__FILE__()) == true');
        ENSURE('getDirectoryEntries("bla") == null');
        print("Director Listing for bla: " + getDirectoryEntries("bla"));

       //ENSURE_EXCEPTION("getDirectoryEntries('bla')");
    }
};

var mySuite = new UnitTestSuite("UnitTest");

try {
    mySuite.addTest(new TestFileFunctions);
    mySuite.run();
} catch (e) {
    print("## An unknown exception occured during execution." + e + "");
    mySuite.incrementFailedCount();
}
