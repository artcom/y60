//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
  
#include "TestSound.h"

#ifdef USE_DASHBOARD
#include <asl/Dashboard.h>
#endif

using namespace std;
using namespace asl;

int main(int argc, char *argv[])
{
#ifdef USE_DASHBOARD
    Dashboard::get();  // Make sure the dashboard gets deleted after the pumps.
#endif
    SoundTestSuite mySuite(argv[0], true, argc, argv);

    try {
        mySuite.run();
#ifdef USE_DASHBOARD
        Dashboard::get().print(cerr);
#endif
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
