//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: testPlugInManager.tst.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.2 $
//
// Description:
//
//=============================================================================

#include <iostream>
#include <string>

#include <asl/PlugInManager.h>
#include <asl/Exception.h>
#include <asl/file_functions.h>
#include <asl/UnitTest.h>

#include <asl/ITest.h>

using namespace asl;
using namespace std;

string myPlugInName = "asltestPlugIn";


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message 
        PlugInManager::get().setSearchPath("${PRO}/lib");
        cout << "Loading PlugIn " << myPlugInName << endl;
	    _myPlugIn = PlugInManager::get().getPlugIn(myPlugInName);
        dynamic_cast<ITest*>(&*_myPlugIn)->addTests(*this);
    }
protected:
    asl::Ptr<PlugInBase> _myPlugIn;
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
