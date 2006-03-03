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
