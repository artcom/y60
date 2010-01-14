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

//own header
#include "ITest.h"

#include <asl/base/PlugInBase.h>
#include <asl/base/PlugInManager.h>

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

TestPlugIn :: TestPlugIn (DLHandle theDLHandle)
    : PlugInBase(theDLHandle)
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
