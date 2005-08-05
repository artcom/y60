//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
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

var ourTriggerCount = 0;

function TestObserver() {
    this.Constructor(this);
}

TestObserver.prototype.Constructor = function(obj) {

    obj.trigger = function() {
        ourTriggerCount++;
    }
}


function GCTest() {
    this.Constructor(this, "Garbage Collection Test");
};

GCTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);
   
    var ourSubject = null;

    function testObserver() {
        var myObserver = new TestObserver();

        ourSubject.test_signal.connect(myObserver, "trigger");
        ENSURE('ourTriggerCount == 0');
        ourSubject.trigger();
        ENSURE('ourTriggerCount == 1');

        // now dispose the observer
        myObserver = null;
    }
        
    obj.run = function() {
        ourSubject = new TestSubject();
        // perform some tests with an attached observer
        testObserver();
        // collect garbage to delete the observer
        gc();
        // now test again without the observer
        // if the GCObserver does not work correctly, this
        // will segfault
        ourSubject.trigger();
        ENSURE('ourTriggerCount == 1');
        ourSubject = null;
        gc();

        // now try the other way around
        var myObserver = new TestObserver();
        var mySubject = new TestSubject();
        mySubject.test_signal.connect(myObserver, "trigger");
        myObserver = null;
        gc();
        mySubject = null;
        gc();
            

        



    };
};

function main() {
    var myTestName = "testGtk.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new GCTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(1);
};


