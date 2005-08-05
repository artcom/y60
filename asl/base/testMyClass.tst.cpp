/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
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


using namespace std;  // automatically added!



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
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new MyClassUnitTest);
        addTest(new MyClassTemplateUnitTest<bool>("<bool>"));
        addTest(new MyClassTemplateUnitTest<int>("<int>"));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
