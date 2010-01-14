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

#include <string>

#include <asl/base/UnitTest.h>

#include <asl/base/PlugInManager.h>

#include "testPlugIn.h"

using namespace std;
using namespace asl;

const string ourPlugInName = "asltestplugin";

class PlugInManagerTest : public UnitTest {
public:
    PlugInManagerTest()
        : UnitTest("PluginManagerTest") {}

    void
    run() {
        PlugInManager& myManager = PlugInManager::get();

        ENSURE(!myManager.isLoaded(ourPlugInName));

        {
            DTITLE("Loading PlugIn " << ourPlugInName << " for the first time");

            _myTouchIndicator = "untouched";

            Ptr<PlugInBase> myPlugIn = myManager.getPlugIn(ourPlugInName);
            // ensure we actually got something
            ENSURE(myPlugIn.get());
            // ensure the manager considers the plugin loaded
            ENSURE(myManager.isLoaded(ourPlugInName));

            // cast the plugin to the expected class
            Ptr<ITestPlugIn> myTestPlugIn = dynamic_cast_Ptr<ITestPlugIn>(myPlugIn);
            // ensure that it was of the expected class
            ENSURE(myPlugIn.get());
            // make the plugin change the indicator
            myTestPlugIn->touchThatString(_myTouchIndicator);
            // check the indicator
            ENSURE(_myTouchIndicator == "touched");
        }

        ENSURE(!myManager.isLoaded(ourPlugInName));

        {
            DTITLE("Loading PlugIn " << ourPlugInName << " for the second time");

            _myTouchIndicator = "untouched";

	        asl::Ptr<PlugInBase> myPlugIn = myManager.getPlugIn(ourPlugInName);
            // ensure we actually got something
            ENSURE(myPlugIn.get());
            // ensure the manager considers the plugin loaded
            ENSURE(myManager.isLoaded(ourPlugInName));

            // cast the plugin to the expected class
            Ptr<ITestPlugIn> myTestPlugIn = dynamic_cast_Ptr<ITestPlugIn>(myPlugIn);
            // ensure that it was of the expected class
            ENSURE(myPlugIn.get());
            // make the plugin change the indicator
            myTestPlugIn->touchThatString(_myTouchIndicator);
            // check the indicator
            ENSURE(_myTouchIndicator == "touched");
        }

        ENSURE(!myManager.isLoaded(ourPlugInName));

        // Ensure loading a non-existent plugin throws
        // NOTE: the strange string is an invalid filename.
        ENSURE_EXCEPTION(myManager.getPlugIn("\?*"), PlugInException);
    }

private:
    string _myTouchIndicator;

};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[])
        : UnitTestSuite(myName, argc, argv) {}

    void
    setup() {
        UnitTestSuite::setup();

        // XXX: this does not consider RelWithDebInfo, Profiling and Coverage
#if defined(_DEBUG)
        PlugInManager::get().setSearchPath(".;../../lib;../../lib/Debug");
#else //defined(DEBUG_VARIANT)
        PlugInManager::get().setSearchPath(".;../../lib;../../lib/Release");
#endif //defined(DEBUG_VARIANT)

        addTest(new PlugInManagerTest());
    }

};

int
main(int argc, char **argv) {
    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
