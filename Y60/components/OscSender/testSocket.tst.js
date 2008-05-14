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
//    $RCSfile: testOscSender.tst.js,v $
//
//   $Revision: 1.4 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");
plug("OscSender");

function OscSenderUnitTest() {
    this.Constructor(this, "OscSenderUnitTest");
};

OscSenderUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.myOscSender1 = new OscSender();

        obj.myOscSender1.connect("127.0.0.1", 7001, 7000);
        //obj.myOscSender1.connect("10.1.2.55", 12000, 7000);
        obj.myNode = new Node("<OscEvent type='move'/>");
        obj.myChildNode = new Node("<float>15</float>");        
        obj.myNode.firstChild.appendChild(obj.myChildNode.firstChild);
        ENSURE('obj.myOscSender1.write(obj.myNode.firstChild) == 36');        

        obj.myInvalidNode = new Node("<OscEvent typo='move'/>");
        ENSURE('obj.myOscSender1.write(obj.myInvalidNode.firstChild) == 0');                
    }
};

var myTestName = "testOscSender.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new OscSenderUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
