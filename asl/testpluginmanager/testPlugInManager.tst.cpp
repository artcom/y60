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

#include <asl/base/PlugInManager.h>
#include <asl/base/Exception.h>
#include <asl/base/file_functions.h>
#include <asl/base/UnitTest.h>

#include <asl/testPlugIn/ITest.h>

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
        dynamic_cast<ITest*>(_myPlugIn.get())->addTests(*this);
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
