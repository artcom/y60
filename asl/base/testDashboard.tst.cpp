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
//    $RCSfile: testDashboard.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test template file - change Time to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#include "Dashboard.h"

#include "UnitTest.h"
//#include <asl/UnitTest.h>
#include "Time.h"
#include <string>
#include <iostream>

using namespace std;
using namespace asl;

class DashboardUnitTest : public UnitTest {
public:
    DashboardUnitTest() : UnitTest("DashboardUnitTest") {}
    void run() {
        for (int f = 0; f < 4; ++f) {
            TimerPtr runTimer = getDashboard().getTimer("run");
            CounterPtr runCounter = getDashboard().getCounter("run");
            runTimer->start();
            ENSURE(runCounter->getCount() == f * 5);
            NanoTime myTime = runTimer->getElapsed();
            for (int i = 0; i < 5; ++i) {
                msleep(1);
                NanoTime myNewTime = runTimer->getElapsed().ticks();
                DPRINT(myNewTime.ticks());
                ENSURE(myNewTime.ticks() > myTime.ticks());
                myTime = myNewTime;
                runCounter->count();
            }
            ENSURE(runCounter->getCount() == 5 * (f+1));
            runTimer->stop();
            myTime = runTimer->getElapsed();
            for (int i = 0; i < 5; ++i) {
                msleep(1);
                NanoTime myNewTime = runTimer->getElapsed().ticks();
                ENSURE(myNewTime.ticks() == myTime.ticks());
            }
            TimerPtr runTimerSibling = getDashboard().getTimer("runsibling");
            runTimerSibling->start();
            ENSURE(runTimerSibling->getParent() == 0);
            runTimer->start();
            TimerPtr runTimerChild = getDashboard().getTimer("runTimerChild");
            runTimerChild->start();
            ENSURE(runTimerChild->getParent() == runTimer);
            {
                MAKE_SCOPE_TIMER(level3);
                MAKE_SCOPE_TIMER(level4);
                {
                    MAKE_SCOPE_TIMER(level5a);
                }
                for (int i = 0; i<1000;++i) {
                    {
                        MAKE_SCOPE_TIMER(level5b);
                    }       
                    {
                        MAKE_SCOPE_TIMER(level5c);
                    }
                }
                SCOPE_TIMER(level3)->stop();
                MAKE_SCOPE_TIMER(level4a);
            }
            getDashboard().print(std::cerr);
            getDashboard().cycle();
        }

        getDashboard().print(std::cerr);
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new DashboardUnitTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
