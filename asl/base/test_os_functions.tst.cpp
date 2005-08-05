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
//    $RCSfile: test_os_functions.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: unit test for file functions
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "os_functions.h"

#include "UnitTest.h"
#include "settings.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#ifdef LINUX
#include "Time.h"
#endif
#include <iostream>

using namespace std;  
using namespace asl;

class os_functions_UnitTest : public UnitTest {
    public:
        os_functions_UnitTest() : UnitTest("os_functions_UnitTest") {  }
        void run() {

            DPRINT(asl::hostname());
            ENSURE(asl::hostname().size());

            DPRINT(asl::appname());
            ENSURE(asl::appname().size());
            
            DPRINT(asl::hostappid());
            ENSURE(asl::hostappid().size());
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new os_functions_UnitTest);
    }
};

// XXX: Major porting effort - postponed.
#ifdef LINUX
class Cycles {
public:
    inline Cycles() {
        set();
    }
    volatile LONGLONG value;
    inline void set() {
        value = get_cycles();
    }
    operator LONGLONG () {
        return value;
    }
    static LONGLONG per_second;
};

LONGLONG Cycles::per_second = 1000000000LL;
#endif

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;
         
    return mySuite.returnStatus();

}
