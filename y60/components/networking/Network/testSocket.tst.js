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

plug("Network");

function SocketUnitTest() {
    this.Constructor(this, "SocketUnitTest");
};

SocketUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        obj.mySocket1 = new Socket(Socket.UDP, 51101, "127.0.0.1");
        obj.mySocket2 = new Socket(Socket.UDP, 51102, "127.0.0.1");

        obj.mySocket1.connect("127.0.0.1", 51102);
        obj.mySocket2.connect("127.0.0.1", 51101);

        print(obj.mySocket1);
        print(obj.mySocket2);

        ENSURE('obj.mySocket1.isValid == true');
        ENSURE('obj.mySocket2.isValid == true');

        ENSURE('obj.mySocket1.write("hallo") == 5');
        ENSURE('obj.mySocket2.read() == "hallo"');

        ENSURE('obj.mySocket2.write("hello") == 5');
        ENSURE('obj.mySocket1.read() == "hello"');

        // binary data test
        ENSURE('obj.mySocket1.write([65,66,0,67,68]) == 5');
        ENSURE('obj.mySocket2.readBlock().size == 5');

        ENSURE('obj.mySocket2.write("bello") == 5');
        msleep(10); // give the socket time to send
        obj.waitingCount = obj.mySocket1.peek(100);
        ENSURE('obj.mySocket1.peek(100) == 5');
        DPRINT('obj.waitingCount');
        ENSURE('obj.mySocket1.peek(3) == 3');
        ENSURE('obj.mySocket1.read() == "bello"');

        obj.mySocket1.setBlockingMode(false);
        ENSURE('obj.mySocket1.read() == ""');
        obj.mySocket1.setBlockingMode(true);

        obj.mySocket3 = new Socket(Socket.UDP, 51103, "0.0.0.0");
        obj.mySocket3.connect("BROADCAST", 51101);
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
