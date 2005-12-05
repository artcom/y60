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
//
//    $RCSfile: testINet.tst.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.6 $
//
//
// Description: 
//
//
//=============================================================================

#ifndef WIN32
#include "TestINetMonitor.h"
#endif
#include "TestSocket.h"
#include "net.h"

#include <asl/Logger.h>
#include <asl/settings.h>

using namespace std;
using namespace inet;

int main( int argc, char *argv[] ) {
    initSockets();
    
    UnitTestSuite mySuite ("INet tests", argc, argv);

#ifndef WIN32
    mySuite.addTest (new TestINetMonitor);
#endif
    mySuite.addTest (new TestSocket);

    mySuite.run();


    AC_INFO << ">> Finished test '" << argv[0] << "'"
           << ", return status = " << mySuite.returnStatus();

    terminateSockets();

    return mySuite.returnStatus();
}

