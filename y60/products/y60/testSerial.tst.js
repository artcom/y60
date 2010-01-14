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

use("UnitTest.js");

function SerialUnitTest() {
    this.Constructor(this, "TestUnitTest");
};

SerialUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function printFrame(theFrame) {
        //print("read Frame: '" + theFrame + "'");
        var myDecodedFrame = "";
        var isCode = false;
        for (var i = 0; i < theFrame.size; ++i) {
            if (theFrame[i] > 122 || theFrame[i] < 32) {
                myDecodedFrame += "|" + theFrame[i];
                isCode = true;
            } else {
                if (isCode) {
                     myDecodedFrame += "|";
                     isCode = false;
                }
                myDecodedFrame += String.fromCharCode(theFrame[i]);
            }
        }
        myDecodedFrame += "|";
        print("  decoded: " + myDecodedFrame);
    }

    obj.run = function() {
        // Test open/close
        obj.mySerialDevice = new SerialDevice(999);
        print("Device name: " + obj.mySerialDevice);
        ENSURE('obj.mySerialDevice.isOpen == false');
        obj.mySerialDevice.open();
        ENSURE('obj.mySerialDevice.isOpen == true');
        obj.mySerialDevice.close();
        ENSURE('obj.mySerialDevice.isOpen == false');
        obj.mySerialDevice.open(110);
        obj.mySerialDevice.close();
        obj.mySerialDevice.open(300, 7);
        obj.mySerialDevice.close();
        obj.mySerialDevice.open(600, 7, SerialDevice.ODD_PARITY);
        obj.mySerialDevice.close();
        obj.mySerialDevice.open(1200, 7, SerialDevice.EVEN_PARITY, 2);
        obj.mySerialDevice.close();
        obj.mySerialDevice.open(2400, 7, SerialDevice.NO_PARITY, 1, true);
        obj.mySerialDevice.close();

        obj.mySerialDevice.open();

        // Test status line
        ENSURE('(obj.mySerialDevice.status & SerialDevice.CTS) != 0');
        var myStatus = obj.mySerialDevice.status;
        obj.mySerialDevice.status = myStatus | SerialDevice.DTR;
        ENSURE('(obj.mySerialDevice.status & SerialDevice.DTR) != 0');

        // Test read/write
        obj.mySerialDevice.write("schreib mal wieder");
        ENSURE('obj.mySerialDevice.read() === "schreib mal wieder"');
        ENSURE('obj.mySerialDevice.read() === ""');

        // Test send/receive
        obj.mySerialDevice.printPacketFormat();
        obj.mySerialDevice.sendPacket("message in a bottle");
        ENSURE('obj.mySerialDevice.receivePacket() == "message in a bottle"');
        ENSURE('obj.mySerialDevice.receivePacket() == ""');

        // Test multiple packet queue
        obj.mySerialDevice.sendPacket("message in a bottle1");
        obj.mySerialDevice.sendPacket("message in a bottle2");
        obj.mySerialDevice.sendPacket("message in a bottle3");
        ENSURE('obj.mySerialDevice.receivePacket() == "message in a bottle1"');
        ENSURE('obj.mySerialDevice.receivePacket() == "message in a bottle2"');
        ENSURE('obj.mySerialDevice.receivePacket() == "message in a bottle3"');
        ENSURE('obj.mySerialDevice.receivePacket() == ""');

        // Test array send
        obj.mySerialDevice.write([65,66,67]);
        ENSURE('obj.mySerialDevice.read() == "ABC"');

        obj.mySerialDevice.sendPacket([65,66,67]);
        ENSURE('obj.mySerialDevice.receivePacket() == "ABC"');

        // Test different packet formats
        obj.mySerialDevice.setPacketFormat(200, 255, 15, SerialDevice.NO_CHECKING);
        obj.mySerialDevice.printPacketFormat();
        obj.mySerialDevice.sendPacket("air in a bottle");
        ENSURE('obj.mySerialDevice.receivePacket() == "air in a bottle"');
        obj.mySerialDevice.sendPacket("air in a bottle");
        printFrame(obj.mySerialDevice.readBlock());

        obj.mySerialDevice.setPacketFormat(201, 202, 0, SerialDevice.NO_CHECKING);
        obj.mySerialDevice.printPacketFormat();
        obj.mySerialDevice.sendPacket("house in a bottle");
        ENSURE('obj.mySerialDevice.receivePacket() == "house in a bottle"');
        obj.mySerialDevice.sendPacket("house in a bottle");
        printFrame(obj.mySerialDevice.readBlock());

        obj.mySerialDevice.setPacketFormat(203, 204, 31, SerialDevice.CHECKSUM_CHECKING);
        obj.mySerialDevice.printPacketFormat();
        obj.mySerialDevice.sendPacket("nothing interesting in a bottle");
        ENSURE('obj.mySerialDevice.receivePacket() == "nothing interesting in a bottle"');
        obj.mySerialDevice.sendPacket("nothing interesting in a bottle");
        printFrame(obj.mySerialDevice.readBlock());

        obj.mySerialDevice.setPacketFormat(255, 254, 0, SerialDevice.CRC8_CHECKING);
        obj.mySerialDevice.printPacketFormat();
        obj.mySerialDevice.sendPacket("ch in a bottle");
        ENSURE('obj.mySerialDevice.receivePacket() == "ch in a bottle"');
        obj.mySerialDevice.sendPacket("ch in a bottle");
        printFrame(obj.mySerialDevice.readBlock());

        obj.mySerialDevice.setPacketFormat(65, 66, 12, SerialDevice.CHECKSUM_CHECKING);
        obj.mySerialDevice.printPacketFormat();
        obj.mySerialDevice.write("ABad ChecksumxxB");
        ENSURE('obj.mySerialDevice.receivePacket() == ""');
        print(obj.mySerialDevice.receivePacket());

        obj.mySerialDevice.write("Bad Framexx");
        ENSURE('obj.mySerialDevice.receivePacket() == ""');

        obj.mySerialDevice.setPacketFormat(255, 254, 4, SerialDevice.NO_CHECKING);
        obj.mySerialDevice.write([255,65]);
        ENSURE('obj.mySerialDevice.receivePacket() == ""');
        obj.mySerialDevice.write([66,67]);
        ENSURE('obj.mySerialDevice.receivePacket() == ""');
        obj.mySerialDevice.write([68,254]);
        ENSURE('obj.mySerialDevice.receivePacket() == "ABCD"');

        obj.mySerialDevice.printPacketStatistic();
        obj.mySerialDevice.close();

        if (operatingSystem() == "OSX") {
            print("##### WARNING: Serial device not yet supported on OSX");
            return;
        }

        obj.mySerialDevice = new SerialDevice(0);
        obj.mySerialDevice.open();
        if (!obj.mySerialDevice.isOpen) {
            print("### WARNING: serial device open failed, probably no serial device attached!");
            return;
        }

        obj.mySerialDevice.setPacketFormat(255, 254, 2, SerialDevice.CHECKSUM_CHECKING);
        obj.mySerialDevice.printPacketFormat();
        obj.mySerialDevice.write("test");
        //obj.mySerialDevice.sendPacket(String.fromCharCode(146,64));
        obj.mySerialDevice.sendPacket([146,64]);

        obj.mySerialDevice.close();
    }
};

function main() {
    var myTestName = "testSerial.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new SerialUnitTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(1);
}

