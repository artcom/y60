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
//    $RCSfile: testAtomicCount.tst.cpp,v $
//
//   $Revision: 1.1 $
//
// Description: unit test template file - change AtomicCount to whatever
//              you want to test and add the apprpriate tests.
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "AtomicCount.h"
#include "UnitTest.h"
#include "PosixThread.h"
#include "settings.h"

#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace asl;

template <class T>
class AtomicCountTemplateUnitTest : public TemplateUnitTest {
public:
    AtomicCountTemplateUnitTest(const char * theTemplateArgument) 
        : TemplateUnitTest("AtomicCountTemplateUnitTest",theTemplateArgument) {}
    void run() {
        asl::AtomicCount<T> myCount(0);
        ENSURE_EQUAL(myCount , 0);
        myCount.increment();
        ENSURE_EQUAL(myCount , 1);
        myCount.increment();
        ENSURE_EQUAL(myCount , 2);
        ENSURE(!myCount.decrement_and_test());
        ENSURE_EQUAL(myCount , 1);
	long myValue = myCount.post_increment();
        ENSURE_EQUAL(myValue , 1);
        ENSURE_EQUAL(myCount , 2);
        ENSURE(!myCount.decrement_and_test());
        ENSURE_EQUAL(myCount , 1);
        ENSURE(myCount.decrement_and_test());
        ENSURE_EQUAL(myCount , 0);
        ENSURE(!myCount.decrement_and_test());
        ENSURE_EQUAL(myCount , -1);
        myCount.set(2);
        ENSURE(!myCount.conditional_decrement());
        ENSURE_EQUAL(myCount , 1);
        ENSURE(myCount.conditional_decrement());
        ENSURE_EQUAL(myCount , 0);
        ENSURE(!myCount.conditional_decrement());
        ENSURE_EQUAL(myCount , 0);
        ENSURE(!myCount.conditional_increment());
        ENSURE_EQUAL(myCount , 0);
        myCount.set(1);
        ENSURE(myCount.conditional_increment());
    }
};

class AtomicThreadSafetyTest: public UnitTest {
public:    
    AtomicThreadSafetyTest()
        : UnitTest("AtomicThreadSafetyTest"),
          _myThread(workFunction, this), I(0), D(_myArraySize)
    { 
    }
    
    std::vector<Unsigned32> _myIncArray;
  
    AtomicCount<MultiProcessor> I;
    AtomicCount<MultiProcessor> D;
   
    enum {_myArraySize = 2000000};
    
    void run() {
        _myIncArray.resize(_myArraySize);
        std::fill(_myIncArray.begin(), _myIncArray.end(), 0);
        _workThreadCounter = 0;
        _mainThreadCounter = 0;
        
        I.set(1);
        _myThread.fork();

        int i;
        while ((i = I.conditional_increment()) < _myArraySize) {
            ++_myIncArray[i];
            ++_mainThreadCounter;
            D.conditional_decrement();
        }
        SUCCESS("atomic increment thread 1 ready");
        _myThread.join();
        SUCCESS("atomic increment thread 2 ready");
        ENSURE(D.conditional_decrement());
        DPRINT(D);
        for (int i = 1; i < _myIncArray.size(); ++i) {
            if (_myIncArray[i] != 1) {
                FAILURE("Non-Atomic increment detected");
                DPRINT(i);
                DPRINT(_myIncArray[i]);
                return;
            }
        }
        SUCCESS("atomic increment ok");
        DPRINT(_workThreadCounter);
        DPRINT(_mainThreadCounter);
    }

    static void workFunction(PosixThread & myThread) {
        AtomicThreadSafetyTest * mySelf = (AtomicThreadSafetyTest *)(myThread.getWorkArgs());
        ((AtomicThreadSafetyTest *)mySelf)->work();
    }

    void work() {
        int i;
        while ((i = I.conditional_increment()) < _myArraySize) {
            ++_myIncArray[i];
            ++_workThreadCounter;
            D.conditional_decrement();
        }
    }
    
private:
    PosixThread _myThread;
    Unsigned32 _workThreadCounter;
    Unsigned32 _mainThreadCounter;
};

class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new AtomicCountTemplateUnitTest<asl::SingleThreaded>("<SingleThreaded>"),100);
		// TODO: run this test only on machines with a single processor, or maybe we drop it at all
        // addTest(new AtomicCountTemplateUnitTest<asl::SingleProcessor>("<SingleProcessor>"),100);
        addTest(new AtomicCountTemplateUnitTest<asl::MultiProcessor>("<MultiProcessor>"),100);
        addTest(new AtomicThreadSafetyTest());
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}

