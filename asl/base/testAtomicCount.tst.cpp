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
//    $RCSfile: testAtomicCount.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: unit test template file - change AtomicCount to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "AtomicCount.h"
#include "UnitTest.h"

#include <string>
#include <iostream>

using namespace std;  // automatically added!

template <class T>
class AtomicCountTemplateUnitTest : public TemplateUnitTest {
public:
    AtomicCountTemplateUnitTest(const char * theTemplateArgument) 
        : TemplateUnitTest("AtomicCountTemplateUnitTest",theTemplateArgument) {}
    void run() {
        asl::AtomicCount<T> myCount(0);
        ENSURE(myCount == 0);
        myCount.increment();
        ENSURE(myCount == 1);
        myCount.increment();
        ENSURE(myCount == 2);
        ENSURE(!myCount.decrement_and_test());
        ENSURE(myCount == 1);
        ENSURE(myCount.post_increment() == 1);
        ENSURE(myCount == 2);
        ENSURE(!myCount.decrement_and_test());
        ENSURE(myCount == 1);
        ENSURE(myCount.decrement_and_test());
        ENSURE(myCount == 0);
        ENSURE(!myCount.decrement_and_test());
        ENSURE(myCount == -1);
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new AtomicCountTemplateUnitTest<asl::SingleThreaded>("<SingleThreaded>"));
        addTest(new AtomicCountTemplateUnitTest<asl::SingleProcessor>("<SingleProcessor>"));
        addTest(new AtomicCountTemplateUnitTest<asl::MultiProcessor>("<MultiProcessor>"));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
