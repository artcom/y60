//==============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin 
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#include "Dashboard.h"
#include "UnitTest.h"

using namespace std;
using namespace asl;

class DashboardUnitTest : public UnitTest {
public:
    explicit DashboardUnitTest() : UnitTest("DashboardUnitTest") {}
    void run() {
        ENSURE(true);
        int myCountCycles = 10000;
        for (int i = 0; i < myCountCycles; i++) {
            COUNT(CyclesTest);
        }
        
        asl::Dashboard & myDashboard = getDashboard();
        myDashboard.cycle();
        unsigned long myCounterResult = myDashboard.getCounterValue("CyclesTest");
        AC_PRINT << "Counter result: " << myCounterResult;
    }
};


int main(int argc, char *argv[]) {

    UnitTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.addTest(new DashboardUnitTest);
        mySuite.run();
    }
    catch (...) {
        cerr << "## An unknown exception occured during execution." << endl;
        mySuite.incrementFailedCount();
    }

    int returnStatus = -1;
    if (mySuite.getPassedCount() != 0) {
        returnStatus = 0;
    } else {
        cerr << "## No tests." << endl;
        
    }
    cerr << ">> Finsihed test suite '" << argv[0] << "', return status = " << returnStatus << endl;
    return returnStatus;

}
