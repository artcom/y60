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
//    $RCSfile: testUnitTest.tst.js,v $
//
//   $Revision: 1.7 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function TestUnitTest() {
    this.Constructor(this, "TestUnitTest");
};

TestUnitTest.prototype.Constructor = function(obj, theName) {
    
    UnitTest.prototype.Constructor(obj, theName);
    
    obj.run = function() {
        //DTITLE("------------ This is a Title --------------"); 
        ENSURE('obj.getMyName()=="TestUnitTest"');

        ENSURE('obj.getPassedCount() == 1');
        ENSURE('obj.getFailedCount() == 0');
        ENSURE('0 == 0');
        ENSURE('obj.getPassedCount() == 4');
        ENSURE('obj.getFailedCount() == 0');
        ENSURE('1 == 0');
        ENSURE('obj.getPassedCount() == 6');
        ENSURE('obj.getFailedCount() == 1');
        obj.setFailedCount(obj.getFailedCount()+1);
        ENSURE('obj.getFailedCount() == 2');
        obj.setPassedCount(obj.getPassedCount()+1);
        ENSURE(obj.getPassedCount() == 10);

        obj.myVariable = 23;
        DPRINT('obj.myVariable');
        DPRINT2("TestUnitTest",'obj.myVariable');
        SUCCESS("just getting here is counted as success");
        FAILURE("just getting here is counted as failure");
        obj.setSilentSuccess();
        ENSURE('true');
        obj.setSilentSuccess(false);
        ENSURE_EXCEPTION("throw 'ex'","ex");
        ENSURE_EXCEPTION("throw Exception('MyWhat','MyWhere')",
                        Exception('MyMessage', {file:'MyFile', line:0}, 'MyName'));
    } 
};

var myTestName = tail(__FILE__());
var mySuite = new UnitTestSuite("UnitTest");

try {
    mySuite.addTest(new TestUnitTest);
    mySuite.run();
}
catch (e) {
    print("## An unknown exception occured during execution." + e + "");
    mySuite.incrementFailedCount();
}

var returnStatus = -1;
if (mySuite.getPassedCount() != 0 && mySuite.getFailedCount() == 3) {
    returnStatus = 0;
} else {
    print("## Test were not completed successfully");

}

print(">> Finished test suite '"+myTestName+"', return status = " + returnStatus + "");
exit(returnStatus);
