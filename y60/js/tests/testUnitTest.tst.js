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
        ENSURE('1 == 0', "this is expected to fail");
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
        ENSURE_EXCEPTION("throw Exception('MyMessage', {file:'MyFile', line:0}, 'MyName')",
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
