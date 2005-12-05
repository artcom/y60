/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testUnitTest.tst.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"

#include <string>


using namespace std;  // automatically added!



class TestUnitTest : public UnitTest {
public:
    explicit TestUnitTest() : UnitTest("TestUnitTest") {}
    void run() {
        setAbortOnFailure(false);
        DTITLE("------------ This is a Title --------------"); 
        ENSURE(std::string(getMyName())=="TestUnitTest");
        ENSURE(getPassedCount() == 1);
        ENSURE(getFailedCount() == 0);
        ENSURE(0 == 0);
        ENSURE(getPassedCount() == 4);
        ENSURE(getFailedCount() == 0);
        ENSURE(1 == 0);
        ENSURE(getPassedCount() == 6);
        ENSURE(getFailedCount() == 1);
        setFailedCount(getFailedCount()+1);
        ENSURE(getFailedCount() == 2);
        setPassedCount(getPassedCount()+1);
        ENSURE(getPassedCount() == 10);
        int myVariable = 23;
        DPRINT(myVariable);
        char * multiLineVariable = "1)bla\n2)blub\n3)yeah";
        DDUMP(multiLineVariable);
        DPRINT2("TestUnitTest",myVariable);
        SUCCESS("just getting here is counted as success");
        FAILURE("just getting here is counted as failure");
        setSilentSuccess();
        ENSURE(true);
        setSilentSuccess(false);
        ENSURE_EXCEPTION(throw 1,int);
    } 
};

template <class T>
class myTemplateUnitTest : public TemplateUnitTest {
public:
    myTemplateUnitTest(const char * theTemplateArgument) 
        : TemplateUnitTest("myTemplateUnitTest",theTemplateArgument) {}
    void run() {
        T someVariable = 1;
        ENSURE(someVariable != 0);
        SUCCESS("yeah");
        DPRINT2("blafasel",someVariable);
    }
};


int main(int argc, char *argv[]) {

    UnitTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.addTest(new TestUnitTest);
        mySuite.addTest(new myTemplateUnitTest<bool>("<bool>"));
        mySuite.addTest(new myTemplateUnitTest<int>("<int>"));
        mySuite.addTest(new Repeat<3,myTemplateUnitTest<int> >("<int>"));
        mySuite.run();
    }
    catch (...) {
        cerr << "## An unknown exception occured during execution." << endl;
        mySuite.incrementFailedCount();
    }

    int returnStatus = 1;
    if (mySuite.getPassedCount() != 0 && mySuite.getFailedCount() == 3) {
        returnStatus = 0;
    } else {
        cerr << "## Test were not completed successfully" << endl;
        
    }
    cerr << ">> Finsihed test suite '" << argv[0] << "', return status = " << returnStatus << endl;
    return returnStatus;

}
