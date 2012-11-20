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

/*jslint */
/*globals use, print, Vector3f, UnitTest, ENSURE, SUCCESS, FAILURE, Matrix4f,
          Node, DomEvent, crash, crash1, exit, UnitTestSuite, RenderWindow*/

use("UnitTest.js");

function MyClassUnitTest() {
    this.Constructor(this, "MyClassUnitTest");
}

MyClassUnitTest.prototype.Constructor = function (obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function () {
        obj.myVar = 1;
        ENSURE('obj.myVar == 1');
        ENSURE('1 + 1 == 2');

        var myVector;
        try {
            myVector = new Vector3f("hallo");
            FAILURE("No exception thrown");
        } catch (ex) {
            SUCCESS("Exception thrown and caught:" + ex);
        }

        try {
            myVector = new Matrix4f("hallo");
            FAILURE("No exception thrown");
        } catch (ex) {
            SUCCESS("Exception thrown and caught:" + ex);
        }

        var myMatrix = new Matrix4f();
        try {
            myMatrix.badMethod();
            FAILURE("No exception thrown");
        } catch (ex) {
            SUCCESS("Exception thrown and caught:" + ex);
        }
        try {
            myMatrix.postMultiply("bad_argument");
            FAILURE("No exception thrown");
        } catch (ex) {
            SUCCESS("Exception thrown and caught:" + ex);
        }
    };
};

function FailedDomEventTest() {
    this.Constructor(this, "DomEventTest");
}

FailedDomEventTest.prototype.Constructor = function (obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function () {
        obj.myDocument = Node.createDocument();
        obj.myDocument.parse('<test name="root">\n' +
                             '<child1 name="child1">\n' +
                             '<child2 name="child2">\n' +
                             '<child3 name="child3"></child3></child2></child1>\n' +
                             '</test>');
        var myChildNode1 = obj.myDocument.childNode(0).childNode(0);
        var myChildNode2 = myChildNode1.childNode(0);
        var myChildNode3 = myChildNode2.childNode(0);
        var myVector = new Vector3f(0, 1, 2);
        var myDomEvent = new DomEvent("testEvent", myVector, true, false, 0.0);
        //ENSURE = print;
        ENSURE(myDomEvent.stopPropagation());
        ENSURE(myDomEvent.preventDefault());
        ENSURE(myDomEvent.type === "testEvent");
        ENSURE(myDomEvent.eventPhase === 1);
        ENSURE(myDomEvent.bubbles === true);
        ENSURE(myDomEvent.cancelable === false);
        ENSURE(myDomEvent.isDefaultPrevented === true);

        myChildNode2.addEventListener("testEvent", obj, true, "handleEvent");
        try {
            myChildNode3.dispatchEvent(myDomEvent);
            FAILURE("No exception thrown");
        } catch (ex) {
            SUCCESS("Exception thrown and caught:" + ex);
        }
    };

    obj.handleEvent = function (theEvent) {
        print("eventListener: " + theEvent);
        crash();
    };
};

function OnFrameUnitTest() {
    this.Constructor(this, "OnFrameUnitTest");
}

OnFrameUnitTest.prototype.Constructor = function (obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.onFrame = function (theTime) {
        var myArray = [];
        myArray[1].foo = "bar";
        FAILURE("No exception thrown");
    };

    obj.run = function () {
        obj.myVar = 1;
        ENSURE('obj.myVar == 1');
        ENSURE('1 + 1 == 2');

        try {
            var myArray = [];
            print(myArray[1].foo = "bar");
            FAILURE("No exception thrown");
        } catch (ex) {
            SUCCESS("Exception thrown and caught:" + ex);
        }

        var window = new RenderWindow();
        window.onFrame = obj.onFrame;
        try {
            window.go();
            FAILURE("No exception thrown");
        } catch (ex) {
            SUCCESS("Exception thrown and caught:" + ex);
        }
    };
};

print("the next error is intentional");
try {
    crash1();
} catch (ex) {
    print("Exception:" + ex + ", name=" + ex.name + ",fileName=" + ex.fileName + ", lineNumber=" + ex.lineNumber);
}


var myTestName = "testMyClass.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new MyClassUnitTest());
mySuite.addTest(new FailedDomEventTest());
mySuite.addTest(new OnFrameUnitTest());
mySuite.run();

print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());