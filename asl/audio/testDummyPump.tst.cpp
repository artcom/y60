//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "TestPump.h"

#include "Pump.h"

#include <asl/UnitTest.h>
#include <asl/Time.h>
#include <asl/Dashboard.h>
#include <asl/string_functions.h>

#include <math.h>
#include <stdlib.h>

using namespace std;
using namespace asl;

class TestDummyPump: public TestPump {
public:
    explicit TestDummyPump(): TestPump ("TestDummy") {}
    
    virtual void run() {
        runWithPump(true);
    }

private:
};


int main( int argc, char *argv[] ) {
    Dashboard::get();  // Make sure the dashboard gets deleted after the pumps.
    // For use in MSDev, where we can't set the environment prior to starting the
    // program.
    //Logger::get().setVerbosity(SEV_DEBUG);

    UnitTestSuite mySuite ("Sound tests");
    mySuite.addTest(new TestDummyPump);

    mySuite.run();

    cerr << ">> Finished test '" << argv[0] << "'"
          << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}

