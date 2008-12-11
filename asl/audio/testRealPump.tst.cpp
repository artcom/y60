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

#ifdef _MSC_VER
// For M_PI
#define _USE_MATH_DEFINES 
#endif
#include "TestPump.h"

#include "Pump.h"

#include <asl/base/UnitTest.h>
#include <asl/base/Time.h>
#include <asl/base/string_functions.h>

#ifdef USE_DASHBOARD
#include <asl/base/Dashboard.h>
#endif

#include <math.h>
#include <stdlib.h>

using namespace std;
using namespace asl;

class TestRealPump: public TestPump {
public:
    explicit TestRealPump(): TestPump ("TestRealPump") {}
    
    virtual void run() {
        runWithPump(false);
        // 8 hours of noise :-).
//        stressTest(60*60*8);
    }

private:
};


int main( int argc, char *argv[] ) {
#ifdef USE_DASHBOARD
    Dashboard::get();  // Make sure the dashboard gets deleted after the pumps.
#endif
    
    // For use in MSDev, where we can't set the environment prior to starting the
    // program.
    //Logger::get().setVerbosity(SEV_DEBUG);

    UnitTestSuite mySuite ("Sound tests", argc, argv);
    mySuite.addTest(new TestRealPump);

    mySuite.run();

    cerr << ">> Finished test '" << argv[0] << "'"
          << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}

