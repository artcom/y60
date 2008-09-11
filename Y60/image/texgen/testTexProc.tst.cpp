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
//    $RCSfile: testTexProc.tst.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1 $
//
//
// Description: 
//
//
//=============================================================================

#include <asl/base/settings.h>

#ifdef _SETTING_DISABLE_LONG_DEBUG_SYMBOL_WARNING_
#pragma warning(disable:4786)  // Debug symbols too long for std::map :-(
#endif

#include "TestFilterTile.h"
#include "TestTextureDefinition.h"
#include "TestTerrainTexGen.h"
#include "TestXMLTexGen.h"
#include "TestTextureGenerator.h"
#include "TestTiming.h"

using namespace std;  

int main( int argc, char *argv[] ) {
    UnitTestSuite mySuite ("TexGen tests");

    mySuite.addTest (new TestFilterTile(false));
    mySuite.addTest (new TestTextureDefinition);
    mySuite.addTest (new TestTerrainTexGen);
    mySuite.addTest (new TestXMLTexGen);
//    mySuite.addTest (new TestTextureGenerator);
    mySuite.addTest (new TestTiming);
    mySuite.run();

/*
    TestXMLTexGen theFailingTest;
    theFailingTest.run();
*/
    cerr << ">> Finished test '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}

