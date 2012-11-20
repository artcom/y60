/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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

