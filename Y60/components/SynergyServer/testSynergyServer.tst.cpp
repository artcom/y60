//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.  
//=============================================================================


#include "TestSynergyServer.h"

#include <asl/base/Logger.h>
#include <asl/base/settings.h>

int main( int argc, char ** argv ) {

#ifndef WIN32
    UnitTestSuite mySuite( "SynergyServer tests", argc, argv );
    mySuite.addTest( new TestSynergyServer );
    mySuite.run();

    AC_INFO << ">> Finished test '" << argv[0] << "'"
            << ", return status = " << mySuite.returnStatus();

    return mySuite.returnStatus();
#else 
    return 0;
#endif

}

