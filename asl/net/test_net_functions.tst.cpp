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
//    $RCSfile: test_net_functions.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: unit test for file functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

            DPRINT(asl::localhostname());            
            ENSURE(asl::localhostname().size());            
      
            ENSURE(from_dotted_address("blafasel") == 0);
            ENSURE(from_dotted_address("255.254.253.252") == 0xfffefdfc);

            ENSURE(as_dotted_address(0xfffefdfc) == "255.254.253.252");
           
            ENSURE(hostaddress("255.254.253.252") == 0xfffefdfc);
            ENSURE(hostaddress("gibt-es-nicht.xyz.bla") == 0);
        
            ENSURE(hostaddress(localhostname()));
            DPRINT(hex<<hostaddress(localhostname()));
            DPRINT(as_dotted_address(hostaddress(localhostname())));

        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new net_functions_UnitTest);
    }
};

// XXX: Major porting effort - postponed.
#ifdef LINUX
class Cycles {
public:
    inline Cycles() {
        set();
    }
    volatile cycles_t value;
    inline void set() {
        value = get_cycles();
    }
    operator cycles_t () {
        return value;
    }
};

#endif

int main(int argc, char *argv[]) {
#if 0
    const int sz = 1000;
    vector<cycles_t> vec(sz);

    for (int i = 0; i < sz;++i) {
        vec[i]=get_cycles();
    }
    for (int i = 0; i < sz-1;++i) {
        cerr << vec[i] << " (" << vec[i+1] - vec[i] << ")" << endl;
    }

    vector<Cycles> svec(1000);
    for (int i = 0; i < sz-1;++i) {
        cerr << "-- " << svec[i] << " (" << svec[i+1] - svec[i] << ")" << endl;
    }

    Cycles start;
    Cycles stop;
    cerr << "delta = " << stop - start << endl;

    int a = 0;
    start.set();
    ++a;
    stop.set();
    cerr << "delta = " << stop - start << endl;

    cycles_t lstart = get_cycles();
    cycles_t lstop = get_cycles();
    cerr << "delta1 = " << lstop - lstart << endl;
    lstart = get_cycles();
    lstop = get_cycles();
    cerr << "delta2 = " << lstop - lstart << endl;
    
    return 0;
#endif
    inet::initSockets();

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;
         
    inet::terminateSockets();

    return mySuite.returnStatus();

}
