//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $RCSfile: testHalt.tst.cpp,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/25 15:31:57 $
//
//=============================================================================

#include <asl/UnitTest.h>
#include <asl/UDPSocket.h>
#include <asl/net.h>
#include <asl/Logger.h>

using namespace std;
using namespace inet;

class TestUDPHalt: public UnitTest {
public:
    TestUDPHalt()
        : UnitTest ("TestUDPHalt")
    {}

    virtual void run() {
        initSockets();

        int serverPort = 2342;
        UDPSocket myUDPClient = UDPSocket(INADDR_ANY, 2343);
        unsigned long inHostAddress = getHostAddress("localhost");

        try {
            myUDPClient.sendTo(inHostAddress, serverPort, "halt", 5);
            SUCCESS("sent UDP packet");
        } catch (SocketException & se) {
            cerr << se.what() << endl;
            FAILURE("could not send UDP packet");
        }
    }
};


int main( int argc, char *argv[] ) {
    UnitTestSuite mySuite("UDP halt tests", argc, argv);

    mySuite.addTest(new TestUDPHalt);
    mySuite.run();

    AC_DEBUG << ">> Finished test '" << argv[0] << "'"
          << ", return status = " << mySuite.returnStatus();

    return mySuite.returnStatus();
}


