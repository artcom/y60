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
//    $RCSfile: testPlugIn.cpp,v $
//
//     $Author: janbo $
//
//   $Revision: 1.2 $
//
// Description:
//
//=============================================================================

#include <asl/PlugInBase.h>
#include <asl/PlugInManager.h>
#include "ITest.h"

#include <iostream>

using namespace asl;
using namespace std;

class PluginManagerTest: public UnitTest {
public:
    PluginManagerTest() : UnitTest("PluginManagerTest") {  }
    void run() {
		ENSURE(PlugInManager::get().isLoaded("asltestPlugIn"));
    }
};

class TestPlugIn :
	public PlugInBase,
	public ITest
{
public:
	TestPlugIn (DLHandle theDLHandle);
	int initialize();
	void cleanUp();
	void addTests(UnitTestSuite & theSuite);
};

extern "C"
EXPORT PlugInBase* asltestPlugIn_instantiatePlugIn(DLHandle myDLHandle) {
	return new TestPlugIn(myDLHandle);
}

TestPlugIn :: TestPlugIn (DLHandle theDLHandle) :
	PlugInBase(theDLHandle)
{
}

int TestPlugIn :: initialize() {
	cout << "MyPlugIn :: initialize!" << endl;
	cout << "SingletonManager in MyPlugIn :: initialize" << &SingletonManager::get() << endl;
	cout << "PlugInManager search path:" << PlugInManager::get().getSearchPath() << endl;
	return 0;
}

void TestPlugIn :: addTests(UnitTestSuite & theSuite) {
	theSuite.addTest(new PluginManagerTest());
}

void TestPlugIn :: cleanUp() {
}
