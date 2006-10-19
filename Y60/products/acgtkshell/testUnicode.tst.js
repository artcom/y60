//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testGTK.tst.js,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2004/11/25 11:45:08 $
//
//
//=============================================================================

print("Running test.js");

use("UnitTest.js");
use("Y60JSSL.js");

function UnicodeTest() {
    this.Constructor(this, "UTF-8 Conversion");
};

UnicodeTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);
   
    obj.run = function() {
        obj.myBlock = new Block([65,66,67]);
        ENSURE('obj.myBlock.size == 3');
        ENSURE('obj.myBlock.toString() == "ABC"');
        
        obj.myBlock = new Block([0xc2,0xa9]); // copyright sign
        ENSURE('obj.myBlock.size == 2');  // two bytes
        ENSURE('obj.myBlock.toString().length == 1');  // but 1 char 
        
        obj.myBlock = new Block([0x61, 0xa9]); // illegal utf-8 byte stream
        ENSURE('obj.myBlock.size == 2');  // two bytes
        ENSURE_EXCEPTION('obj.myBlock.toString().length');  // but 1 char 
    };
};

function main() {
    var myTestName = "testUnicode.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new UnicodeTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(1);
};
exit(0);

