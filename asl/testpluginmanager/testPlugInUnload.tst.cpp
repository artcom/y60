
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
//    $RCSfile: testPlugInUnload.tst.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.1 $
//
// Description:
//
//=============================================================================

#include <iostream>
#include <string>

#include <asl/base/PlugInManager.h>
#include <asl/base/Exception.h>
#include <asl/base/file_functions.h>
#include <asl/base/UnitTest.h>

#include <asl/testPlugIn/ITest.h>

using namespace asl;
using namespace std;

string myPlugInName = "asltestPlugIn";

class PluginManagerTest: public UnitTest {
public:
    PluginManagerTest() : UnitTest("PluginManagerTest") {  }
    void run() {
        {
            cout << "Loading PlugIn " << myPlugInName << endl;
	        asl::Ptr<PlugInBase> myPlugIn = PlugInManager::get().getPlugIn(myPlugInName);
        }
        ENSURE(!PlugInManager::get().isLoaded(myPlugInName));    

        {
            cout << "Loading PlugIn twice" << myPlugInName << endl;
	        asl::Ptr<PlugInBase> myPlugIn = PlugInManager::get().getPlugIn(myPlugInName);
            {
                asl::Ptr<PlugInBase> myPlugIn2 = PlugInManager::get().getPlugIn(myPlugInName);
            }
            ENSURE(PlugInManager::get().isLoaded(myPlugInName));    
        }
        ENSURE(!PlugInManager::get().isLoaded(myPlugInName));    

        ENSURE_EXCEPTION(PlugInManager::get().getPlugIn("\?*"), PlugInException);    
    }
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message   
        PlugInManager::get().setSearchPath("${PRO}/lib");
        addTest(new PluginManagerTest());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
