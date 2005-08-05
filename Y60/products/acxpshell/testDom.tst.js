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

function main() {
    var myTestName = "testMyClass.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new MyClassUnitTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    ERROR;
};


