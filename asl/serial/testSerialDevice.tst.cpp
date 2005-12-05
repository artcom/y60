//============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: testSerialDevice.tst.cpp,v $
//
//     $Author: martin $
//
//   $Revision: 1.8 $
//
//=============================================================================

#include <asl/UnitTest.h>
#include <asl/Time.h>

#include "SerialDevice.h"
#include "SerialDeviceFactory.h"
#include "uCCP.h"

using namespace asl;
using namespace std;

class SerialDeviceUnitTest : public UnitTest {
public:
    SerialDeviceUnitTest() : UnitTest("SerialDeviceUnitTest") {  }
    void run() {
        SerialDevice * myPort = getSerialDevice(0);
#ifdef WIN32
        ENSURE_MSG(myPort->getDeviceName() == "COM1:", "Testing name factory")
#else
        ENSURE_MSG(myPort->getDeviceName() == "/dev/ttyS0", "Testing name factory")
#endif
        try {
            myPort->open(9600, 8, SerialDevice::NO_PARITY, 1);
        } catch (SerialPortException ex) {
            cerr << "###WARNING: Cannot open serial device '" << myPort->getDeviceName() <<
                    "': " << ex << " skipping serial device tests" << endl; 
            return;
        }
        
        ENSURE(myPort->isOpen());
        myPort->close();
        ENSURE( ! myPort->isOpen());
        
        // WARNING this test may fail if you have more than 500 serial ports ;-)
        SerialDevice * otherPort = getSerialDevice(500);
        ENSURE_EXCEPTION(otherPort->open(9600, 8, SerialDevice::NO_PARITY, 1), SerialPortException);
        ENSURE( ! otherPort->isOpen());
        ENSURE_EXCEPTION(otherPort->write("ttt", 3), SerialPortException);
        char myBuffer[3];
        size_t mySize = 3;
        ENSURE_EXCEPTION(otherPort->read(myBuffer, mySize), SerialPortException);

        ENSURE_EXCEPTION(otherPort->close(), SerialPortException);
        
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
