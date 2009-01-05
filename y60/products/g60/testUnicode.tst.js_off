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

