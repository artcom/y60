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


use("Exception.js");
use("Y60JSSL.js");

function tail(thePath) {
    var myPath = new String(thePath);
    var myMatch = myPath.lastIndexOf("/");
    if (myMatch >= 0) {
        return myPath.substr(myMatch+1);
    }
    return thePath;
}

function UnitTest(theName) {
    this.Constructor(this, theName);
}


UnitTest.prototype.Constructor = function(obj, theName) {

    // private members:
    var _myName = theName;
    var _passedCount = 0;
    var _failedCount = 0;
    var _silentSuccess = false;
    var _abortOnFailure = false;

    // privileged member function
    obj.getFailedCount = function() {
        return _failedCount;
    }

    obj.getPassedCount = function() {
        return _passedCount;
    }

    obj.getMyName = function() {
        return _myName;
    }

    obj.setMyName = function(theName) {
        _myName = theName;
    }

    obj.incrementFailedCount = function() {
        ++_failedCount;
    }

    obj.incrementPassedCount = function() {
        ++_passedCount;
    }

    obj.incrementFailedCount = function() {
        ++_failedCount;
    }

    obj.incrementPassedCount = function() {
        ++_passedCount;
    }

    obj.setSilentSuccess = function(makeSilent) {
        if (makeSilent == undefined) {
            _silentSuccess = true;
        } else {
            _silentSuccess = makeSilent;
        }
    }

    obj.setAbortOnFailure = function(makeAbort) {
        if (makeAbort == undefined) {
            _abortOnFailure = true;
        } else {
            _abortOnFailure = makeAbort;
        }
    }
    obj.returnStatus = function() {
        if (_passedCount != 0 && _failedCount == 0) {
            return 0;
        } else {
            return -1;
        }
    }
    obj.ensure = function(/*bool*/ myExpressionResult,
            /*string*/ myExpression,
            /*string*/ mySourceLine)
    {
        var myPrefix = ">>>>>> OK   ";
        if (myExpressionResult == true) {
            obj.incrementPassedCount();
            if (_silentSuccess) {
                return;
            }
        } else {
            obj.incrementFailedCount();
            myPrefix = "###### FAIL ";
        }
        print(myPrefix + " ("+ myExpression + "), Line " + mySourceLine + "");
        if (!myExpressionResult && _abortOnFailure) {
            print("UnitTest::ensure: Execution aborted");
            abort();
        }
    }
    obj.run = function() {};

    obj.getTracePrefix = function() {
        return ">>>>>>        ";
    }
    obj.setup = function() {
        print(">>>> Launching Test Unit '" + _myName + "'");
    }
    obj.teardown = function() {
        print(">>>> Completed Test Unit '" + _myName + "'"
                + ", " + obj.getFailedCount() + " tests failed"
                + ", " + obj.getPassedCount() + " tests passed");
    }
    obj.setFailedCount = function(failedTests) {
        _failedCount = failedTests;
    }
    obj.setPassedCount = function(passedTests) {
        _passedCount = passedTests;
    }
    obj.ensureWith = function(theWithObj) {
        ourWithObj = theWithObj;
    }
}

function UnitTestSuite(theName) {
    this.Constructor(this, theName);
}

UnitTestSuite.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    var _myTests = new Array();

    obj.addTest = function(myTest) {
        if (myTest) {
            _myTests.push(myTest);
        } else {
            print("### Failed to add Test");
            obj.incrementFailedCount();
        }
    }

    obj.run = function() {
        try {
            try {
                obj.setup();
            } catch (ex) {
                print("### An exception occured during setup of test suite '" + obj.getMyName() + "'");
                reportException(ex);
                obj.incrementFailedCount();
            }
            try {
                for (var i = 0; i < _myTests.length; ++i) {
                    _myTests[i].setup();
                    ourCurrentTest = _myTests[i];
                    _myTests[i].run();
                    obj.setFailedCount(obj.getFailedCount() + _myTests[i].getFailedCount());
                    obj.setPassedCount(obj.getPassedCount() + _myTests[i].getPassedCount());
                    _myTests[i].teardown();
                }
            } catch (ex) {
                print("### An exception occured during execution of test suite '" + obj.getMyName() + "'");
                reportException(ex);
                obj.incrementFailedCount();
            }
            try {
                obj.teardown();
            } catch (ex) {
                print("### An exception occured during teardown of test suite '" + obj.getMyName() + "'");
                reportException(ex);
                obj.incrementFailedCount();
            }
        } catch (ex) {
            reportException(ex);
        }
    }
    obj.setup = function() {
        print(">> Launching Test Suite '" + obj.getMyName() + "'");
    }
    obj.teardown = function() {
        print(">> Completed Test Suite '" + obj.getMyName() + "'"
            + ", " + obj.getFailedCount() + " total tests failed"
            + ", " + obj.getPassedCount() + " total tests passed");
        obj.destroyMyTests();
    }
    obj.getNumTests = function() {
        return _myTests.length;
    }
    obj.getTest = function(theTestIndex) {
        return _myTests[theTestIndex];
    }
    obj.destroyMyTests = function() {
        while (_myTests.length) {
            _myTests.pop();
        }
    }
}

function DTITLE(TITLE) {
    print(ourCurrentTest.getTracePrefix() + TITLE + ":");
}

if (ourCurrentTest == undefined) var ourCurrentTest = null;
if (ourWithObj == undefined)     var ourWithObj = [];

function DPRINT(theVariable) {
    var myExpression = "with (ourWithObj) {var obj = ourCurrentTest; with (obj) { "+theVariable+ "} } ";
    //var myExpression = "var obj = ourCurrentTest;"+theVariable;
    var myResult = eval(myExpression);
    print(ourCurrentTest.getTracePrefix() + "      "+ theVariable + " = " + myResult);
}

function DPRINT2(theMessage, theVariable) {
    var myExpression = "var obj = ourCurrentTest;"+theVariable;
    var myResult = eval(myExpression);
    print(ourCurrentTest.getTracePrefix() +"      "+ theMessage + ": " + theVariable + " = " + myResult);
}

function ENSURE(theExpression, theMessage) {
    var myExpression = "with (ourWithObj) {var obj = ourCurrentTest; with (obj) { "+theExpression+ "} } ";
    try {
        var myResult = eval(myExpression);
    } catch(e) {
        var myResult = false;
        print('###### EXCEPTION:'+e);
    }
    //dumpstack();
    //ourCurrentTest.ensure(myResult,theExpression,__LINE__(2),__FILE__(2));
    if (theMessage) {
        theExpression = theMessage;
    }
    ourCurrentTest.ensure(myResult,theExpression,__LINE__(2),theMessage);
    return myResult;
}

function ENSURE_EQUAL(theExpected, theResult, theMessage) {
    try {
        var myResult = (theExpected === theResult);
    } catch(e) {
        var myResult = false;
        print('###### EXCEPTION:'+e);
    }

    if (!theMessage)
        theMessage = "no message";

    var myResultMessage;
    if (myResult){
        myResultMessage = theMessage;
    } else {
        myResultMessage = theMessage + ": Should be '" +
            theExpected + "' but was '" + theResult + "'";
    }

    ourCurrentTest.ensure(myResult, myResultMessage,
                              __LINE__(2));

    return myResult;
}

function ENSURE_NOT_EQUAL(theExpected, theResult, theMessage) {
    try {
        var myResult = (theExpected !== theResult);
    } catch(e) {
        var myResult = false;
        print('###### EXCEPTION:'+e);
    }

    if (!theMessage)
        theMessage = "no message";

    var myResultMessage;
    if (myResult){
        myResultMessage = theMessage;
    } else {
        myResultMessage = theMessage + ": Should not be '" +
            theExpected + "' but was '" + theResult + "'";
    }

    ourCurrentTest.ensure(myResult, myResultMessage,
                              __LINE__(2));

    return myResult;
}

function ENSURE_ALMOST_EQUAL(theExpected, theResult, theMessage) {
    try {
        var myResult = almostEqual(theExpected, theResult);
    } catch(e) {
        var myResult = false;
        print('###### EXCEPTION:'+e);
    }

    if (!theMessage)
        theMessage = "no message";

    var myResultMessage;
    if (myResult){
        myResultMessage = theMessage;
    } else {
        myResultMessage = theMessage + ": Should be around " +
            theExpected + " but was " + theResult;
    }

    ourCurrentTest.ensure(myResult, myResultMessage,
                              __LINE__(2));

    return myResult;
}

function ENSURE_WITHIN_TIMEOUT(theConditionFunction, theTimeout, theMessage){
    var startTime = millisec();
    while(! theConditionFunction()){
        msleep(33);
        if (millisec() - startTime > theTimeout){
            break;
        }
    }

    ENSURE(theConditionFunction(), theMessage);
}

function FAILURE(MSG) {
    ourCurrentTest.ensure(false, MSG, __LINE__(2));
}

function SUCCESS(MSG) {
    ourCurrentTest.ensure(true, MSG, __LINE__(2));
}

function FAILURE2(MSG) {
    ourCurrentTest.ensure(false, MSG, __LINE__(3));
}

function SUCCESS2(MSG) {
    ourCurrentTest.ensure(true, MSG, __LINE__(3));
}

function ENSURE_EXCEPTION(theTest, theException) {
    try {
        var myExpression = "with (ourWithObj) {var obj = ourCurrentTest; with (obj) { "+theTest+ "} } ";
        eval(myExpression);
        FAILURE2(theTest);
    } catch (e) {
        if (e == theException || theException == "*" || theException == undefined) {
            SUCCESS2("EXCEPTION("+theTest+")");
        } else {
            FAILURE2("EXCEPTION("+theTest+")");
        }
    }
}

