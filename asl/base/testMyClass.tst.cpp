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
//    $RCSfile: testMyClass.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test template file - change MyClass to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"

#include <string>
#include <iostream>

using namespace std;

class MyClassUnitTest : public UnitTest {
public:
    MyClassUnitTest() : UnitTest("MyClassUnitTest") {  }
    void run() {
        ENSURE(1 + 1 == 2);
        // perform more test here
    }
};

template <class T>
class MyClassTemplateUnitTest : public TemplateUnitTest {
public:
    MyClassTemplateUnitTest(const char * theTemplateArgument)
        : TemplateUnitTest("MyClassTemplateUnitTest",theTemplateArgument) {}
    void run() {
        T someVariable = 1;
        ENSURE(someVariable != 0);
        DPRINT2(getMyName(),someVariable);
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new MyClassUnitTest, 10000);
        addTest(new MyClassTemplateUnitTest<bool>("<bool>"), 10000);
        addTest(new MyClassTemplateUnitTest<int>("<int>"), 10000);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
