/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
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
//
//    $RCSfile: testSerialDevice.tst.cpp,v $
//
//     $Author: martin $
//
//   $Revision: 1.8 $
//
//=============================================================================

#include <asl/base/UnitTest.h>
#include <asl/base/Time.h>

#include "DebugPort.h"
#include "SerialDevice.h"
#include "SerialDeviceFactory.h"
#include "uCCP.h"

using namespace asl;
using namespace std;

class SerialDeviceUnitTest : public UnitTest {
public:
    SerialDeviceUnitTest() : UnitTest("SerialDeviceUnitTest") {  }
    void run() {
        int openablePort = -1;
        int unOpenablePort = -1;
        std::vector<std::string> myDeviceNames;
        if (getSerialDeviceNames(myDeviceNames)) {
            for (unsigned i = 0; i < myDeviceNames.size();++i) {
				DPRINT(myDeviceNames[i]);
                SerialDevice * myPort = getSerialDevice(i);
                ENSURE_MSG(myPort->getDeviceName() == myDeviceNames[i], "Testing name factory");
                try {
                    myPort->open(9600, 8, SerialDevice::NO_PARITY, 1);
                } catch (SerialPortException ex) {
                    cerr << "###WARNING: Cannot open serial device '" << myPort->getDeviceName() <<
                        "': " << ex << " skipping serial device tests" << endl;
                    unOpenablePort = i;
                    continue;
                }
                openablePort = i;
                ENSURE(myPort->isOpen());
                myPort->close();
                ENSURE( ! myPort->isOpen());
                delete myPort;
            }
        }

        // WARNING this test may fail if you have more than 5000 serial ports ;-)
        ENSURE(getSerialDevice(5000) == 0);

        // test error handling
        if (unOpenablePort >= 0) {
                SerialDevice * otherPort = getSerialDevice(unOpenablePort);
                ENSURE_EXCEPTION(otherPort->open(9600, 8, SerialDevice::NO_PARITY, 1), SerialPortException);
                ENSURE( ! otherPort->isOpen());
                ENSURE_EXCEPTION(otherPort->write("ttt", 3), SerialPortException);
                char myBuffer[3];
                size_t mySize = 3;
                ENSURE_EXCEPTION(otherPort->read(myBuffer, mySize), SerialPortException);

                ENSURE_EXCEPTION(otherPort->close(), SerialPortException);
                delete otherPort;
        }

        // at least instatiate this thing once ... [DS]
        SerialDevice * myDebugPort = new DebugPort("Debug Port");
        delete myDebugPort;

    }
};

#if 0

class uCCPUnitTest : public UnitTest {
public:
    uCCPUnitTest() : UnitTest("uCCPUnitTest") {  }
    void run() {
        SerialDevice * myTTY = getSerialDevice(0);
        myTTY->open(9600, 8, SerialDevice::NO_PARITY, 1);

        uCCP myUCCP(myTTY);

        for (unsigned i = 0; i < 100; ++i) {
            myUCCP.recive();
            msleep(100);
            while (myUCCP.pendingPackets()) {
                cerr << "'" << myUCCP.popPacket() << "'" << endl;
            }
        }
        myUCCP.send("k0000");
        msleep(2000);
        myUCCP.send("k083C");
        msleep(2000);
        myUCCP.send("k0000");
        msleep(2000);
        myUCCP.send("k083C");
        while (myUCCP.pendingPackets()) {
            cerr << "'" << myUCCP.popPacket() << "'" << endl;
        }

        myUCCP.printStatistic(cerr);

        myTTY->close();
        ENSURE( ! myTTY->isOpen());

        delete myTTY;
    }
};

#endif


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new SerialDeviceUnitTest);
        //addTest(new uCCPUnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    std::cerr << ">> Finished test suite '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << std::endl;

    return mySuite.returnStatus();

}
