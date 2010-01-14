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
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new AutoTemplateUnitTest<FakeLock>("<FakeLock>"),1000000);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
