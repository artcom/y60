/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testAuto.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// Description: unit test template file - change Auto to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Auto.h"
#include "UnitTest.h"

using namespace std; 

class FakeLock {
    public:
        FakeLock() : _myLocked (false) {
        }
        void lock() {
            _myLocked = true;
        }
        void unlock() {
            _myLocked = false;
        }
        bool isLocked() {
            return _myLocked;
        }

    private:
        bool _myLocked;
};

using namespace asl;

template <class T>
class AutoTemplateUnitTest : public TemplateUnitTest {
public:
    AutoTemplateUnitTest(const char * theTemplateArgument) 
        : TemplateUnitTest("AutoTemplateUnitTest",theTemplateArgument) {}
    void run() {
        T myLock;
        ENSURE (!myLock.isLocked());
        {
            AutoLocker<T> myLocker(myLock);
            ENSURE (myLock.isLocked());
        }
        ENSURE (!myLock.isLocked());
        try {
            ENSURE (!myLock.isLocked());
            AutoLocker<T> myLocker(myLock);
            ENSURE (myLock.isLocked());
            throw 1;
            FAILURE("No Exception was thrown");
        }
        catch (...) {
            SUCCESS("Exception was caught");
        }
        ENSURE (!myLock.isLocked()); 
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new AutoTemplateUnitTest<FakeLock>("<FakeLock>"));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
