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
//    $RCSfile: testShell.tst.js,v $
//
//   $Revision: 1.34 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function JSAppUnitTest() {
    this.Constructor(this, "JSAppUnitTest");
};

JSAppUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        if (operatingSystem() == "Win32") {
            obj.myReturnCode = exec("..\\..\\testfiles\\exec.bat", "2");
    		ENSURE('obj.myReturnCode == 2');
            obj.myReturnCode = exec("..\\..\\testfiles\\exec.bat 3");
    		ENSURE('obj.myReturnCode == 3');
            obj.myReturnCode = exec("..\\..\\testfiles\\test.butt");
    		ENSURE('obj.myReturnCode == -1');
            obj.myReturnCode = exec("..\\..\\testfiles\\exec.bat", "2", false);
    		ENSURE('obj.myReturnCode == 0');
        } else if (operatingSystem() == "Linux") {
        } else if (operatingSystem() == "MacOsX") {
        }
        
    }
};

try {
    var myTestName = "testJSapp.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new JSAppUnitTest());

	mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    exit(mySuite.returnStatus());
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(1);
}
