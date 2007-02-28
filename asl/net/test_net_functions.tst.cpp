//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "net_functions.h"
#include "net.h"

#include <asl/UnitTest.h>
#include <asl/settings.h>
#include <asl/Time.h>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <iostream>

using namespace std;  
using namespace asl;

class net_functions_UnitTest : public UnitTest {
    public:
        net_functions_UnitTest() : UnitTest("net_functions_UnitTest") {  }
        void run() {

            DPRINT(localhostname());            
            ENSURE(localhostname().size());            
      
            ENSURE(from_dotted_address("blafasel") == 0);
            ENSURE(from_dotted_address("255.254.253.252") == 0xfffefdfc);

            ENSURE(as_dotted_address(0xfffefdfc) == "255.254.253.252");
           
            ENSURE(hostaddress("255.254.253.252") == 0xfffefdfc);
            ENSURE(hostaddress("gibt-es-nicht.xyz.bla") == 0);
            ENSURE(hostaddress("BROADCAST") == 0xffffffff);

            ENSURE(hostaddress(localhostname()));
            DPRINT(hex << hostaddress(localhostname()) << dec);
            DPRINT(as_dotted_address(hostaddress(localhostname())));

            DPRINT(getHardwareAddress());
            ENSURE(getHardwareAddress().size() == 6); // MACs are 48 bits
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new net_functions_UnitTest);
    }
};

int main(int argc, char *argv[]) {
    inet::initSockets();

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;
         
    inet::terminateSockets();

    return mySuite.returnStatus();

}
