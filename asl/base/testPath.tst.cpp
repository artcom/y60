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

#include "Path.h"
#include "UnitTest.h"

using namespace std;
using namespace asl;

class PathUnitTest : public UnitTest {
public:
    explicit PathUnitTest() : UnitTest("PathUnitTest") {}
    void run() {
        ENSURE(true);
#ifdef WINDOWS
        Path myLocalePath("\u00B5", Locale); // mu
		ENSURE(myLocalePath.toUTF8().size() == 2);
#else
        Path myLocalePath("m", Locale); // m
#endif
		ENSURE(myLocalePath.toLocale().size() == 1);
		ENSURE(myLocalePath == myLocalePath);

		Path myUTFPath(myLocalePath.toUTF8(), UTF8);
		ENSURE(myUTFPath.toLocale().size() == 1);

		ENSURE(myUTFPath.toLocale() == myLocalePath.toLocale());
		ENSURE(myUTFPath.toUTF8() == myLocalePath.toUTF8());
		ENSURE(myUTFPath == myLocalePath);

        {
            Path myCopy(myLocalePath);
		    ENSURE(myCopy.toLocale() == myLocalePath.toLocale());
		    ENSURE(myCopy.toUTF8() == myLocalePath.toUTF8());
		    ENSURE(myCopy == myLocalePath);
        }
        {
            Path myCopy("foo", Locale);
            myCopy = myLocalePath;
		    ENSURE(myCopy.toLocale() == myLocalePath.toLocale());
		    ENSURE(myCopy.toUTF8() == myLocalePath.toUTF8());
		    ENSURE(myCopy == myLocalePath);
        }
    }
};


int main(int argc, char *argv[]) {

    UnitTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.addTest(new PathUnitTest);
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
