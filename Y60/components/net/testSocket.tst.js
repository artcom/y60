/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testSocket.tst.js,v $
//
//   $Revision: 1.4 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");
plug("jsnet");

function SocketUnitTest() {
    this.Constructor(this, "SocketUnitTest");
};

SocketUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.mySocket1 = new Socket(Socket.UDP, 1101, "127.0.0.1");
        obj.mySocket2 = new Socket(Socket.UDP, 1102, "127.0.0.1");

        obj.mySocket1.connect("127.0.0.1", 1102);
        obj.mySocket2.connect("127.0.0.1", 1101);
        
        print(obj.mySocket1);
        print(obj.mySocket2);

        ENSURE('obj.mySocket1.isValid == true');
        ENSURE('obj.mySocket2.isValid == true');

        ENSURE('obj.mySocket1.write("hallo") == 5');
        ENSURE('obj.mySocket2.read() == "hallo"');

        ENSURE('obj.mySocket2.write("hello") == 5');
        ENSURE('obj.mySocket1.read() == "hello"');

        ENSURE('obj.mySocket1.write([65,66,0,67,68]) == 5');
        ENSURE('obj.mySocket2.read() == "AB\0CD"');

        ENSURE('obj.mySocket2.write("bello") == 5');
        ENSURE('obj.mySocket1.peek(100) == 5');
        ENSURE('obj.mySocket1.peek(3) == 3');
        ENSURE('obj.mySocket1.read() == "bello"');
        
        obj.mySocket1.setBlockingMode(false);
        ENSURE('obj.mySocket1.read() == ""');
        obj.mySocket1.setBlockingMode(true);
        
        obj.mySocket3 = new Socket(Socket.UDP, 1103);
        obj.mySocket3.connect("BROADCAST", 1101);
        print(obj.mySocket3);

        ENSURE('obj.mySocket3.write("test") == 4');

        obj.mySocket1.close();
        obj.mySocket2.close();
        obj.mySocket3.close();
    }
};

var myTestName = "testSocket.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new SocketUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
