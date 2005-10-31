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
//    $RCSfile: testDom.tst.js,v $
//
//   $Revision: 1.9 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

function MyClassUnitTest() {
    this.Constructor(this, "TestUnitTest");
};

MyClassUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);


    obj.run = function() {
    	obj.myDocument = Node.createDocument();

        // Test parsing of quoted characters
    	obj.myDocument.parse('<hello myattr="&lt;p&gt;">world&lt;p&gt;</hello>');
        ENSURE('obj.myDocument.firstChild.nodeName == "hello"');
        ENSURE('obj.myDocument.firstChild.myattr == "<p>"');
        ENSURE('obj.myDocument.firstChild.firstChild.nodeValue == "world<p>"');

        // Test file saving
        var myDate = new Date();
        obj.myDateString = myDate.toLocaleString();
        obj.mySaveDoc = Node.createDocument();
        obj.mySaveDoc.parse('<date>' + obj.myDateString + '</date>');
        obj.mySaveDoc.saveFile("../../testfiles/output.xml");
        obj.myLoadDoc = Node.createDocument();
        obj.myLoadDoc.parseFile("../../testfiles/output.xml");
        ENSURE('obj.myLoadDoc.firstChild.firstChild.nodeValue == obj.myDateString');

        var myXMLDoc = new Node();
        myXMLDoc.parseFile("../../testfiles/unicode.xml");
        var myXMLNode = myXMLDoc.firstChild;

        var myUnicode = myXMLNode.firstChild.firstChild.firstChild.nodeValue;

//        print("Arabic in UNICODE=\n" + myUnicode);

        ENSURE('1 + 1 == 2');
    }
};

function MyDomEventTest() {
    this.Constructor(this, "DomEventTest");
};

MyDomEventTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);


    obj.run = function() {
    	obj.myDocument = Node.createDocument();
    	obj.myDocument.parse('<test name="root">\
    	                      <child1 name="child1">\
    	                      <child2 name="child2">\
    	                      <child3 name="child3"></child3></child2></child1>\
    	                      </test>');
    	var myChildNode1 = obj.myDocument.childNode(0).childNode(0);
    	var myChildNode2 = myChildNode1.childNode(0);
    	var myChildNode3 = myChildNode2.childNode(0);
    	var myVector = new Vector3f(0,1,2);
    	var myDomEvent = new JSEvent("testEvent", myVector, true, false, 0.0);
    	//ENSURE = print;
    	ENSURE(myDomEvent.stopPropagation());    	
    	ENSURE(myDomEvent.preventDefault());
    	ENSURE(myDomEvent.type == "testEvent");
    	ENSURE(myDomEvent.eventPhase == 1);
    	ENSURE(myDomEvent.bubbles == true);
    	ENSURE(myDomEvent.cancelable == false);
    	ENSURE(myDomEvent.isDefaultPrevented == true);
    	
    	myChildNode2.addEventListener("testEvent", obj, true);
    	myChildNode3.dispatchEvent(myDomEvent);
    	//print(myDomEvent.target);
    	//print(myDomEvent.currentTarget);

    }
    obj.handleEvent = function(theEvent) {
        print("eventListener: " + theEvent);
    }
};
var myTestName = "testMyClass.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new MyDomEventTest());
mySuite.addTest(new MyClassUnitTest());

mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
