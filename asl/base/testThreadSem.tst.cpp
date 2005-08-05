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
//    $RCSfile: testThreadSem.tst.cpp,v $
//
//   $Revision: 1.3 $
//
// Description: unit test template file - change Lock to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "ThreadSem.h"

#include <string>
#include <iostream>
#include <errno.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


using namespace std;  // automatically added!



using namespace asl;

#ifdef OSX

static semun _semarg;

#else

static union semun {
        int val;
        struct ::semid_ds *buf;
        unsigned short int *array;
        struct ::seminfo *__buf;
} _semarg;
#endif

int  svnewsema (int value){
    int _semaphore = 0;
    _semaphore = semget( IPC_PRIVATE, sizeof( int ), 0660 | IPC_CREAT );
    if (_semaphore == -1) {
        return -1;
    } else {
        _semarg.val = value;
        if (semctl( _semaphore, 0, SETVAL, _semarg) == -1) {
            perror ("LockUnitTest::svnewsema() ");
            return -1;
        } else {
            return _semaphore;
        }
    }
}


class LockUnitTest : public UnitTest {
public:
    LockUnitTest() : UnitTest("LockUnitTest") {  }
    void run() {
        int mySemSetId = svnewsema(1);
        DPRINT(mySemSetId);
        ENSURE(mySemSetId >= 0);
        ThreadSem myLock(mySemSetId,0,1);
        ENSURE(myLock.getValue() == 1);
        ENSURE(myLock.getNCount()==0);
        DPRINT(myLock.getNCount());
        ENSURE(myLock.getZCount()==0);
        DPRINT(myLock.getZCount());
        DPRINT(myLock.getValue());
        ENSURE(myLock.unlock() == 0);
        ENSURE(myLock.getValue() == 2);
        DPRINT(myLock.getValue());
        ENSURE(myLock.unlock() == 0);
        ENSURE(myLock.getValue() == 3);
        DPRINT(myLock.getValue());
        ENSURE(myLock.lock() == 0);
        ENSURE(myLock.getValue() == 2);
        DPRINT(myLock.getValue());
        ENSURE(myLock.lock() == 0);
        ENSURE(myLock.getValue() == 1);
        DPRINT(myLock.getValue());
        ENSURE(myLock.lock() == 0);
        DPRINT(myLock.getValue());
        DPRINT(myLock.getNCount());
        DPRINT(myLock.getZCount());
        ENSURE(myLock.getValue() == 0);
        ENSURE(myLock.nonblock_lock() == 0);
        ENSURE(myLock.getValue() == 0);
        ENSURE(myLock.nonblock_lock() == 0);
        // perform more test here
    }
};

template <class T>
class LockTemplateUnitTest : public TemplateUnitTest {
public:
    LockTemplateUnitTest(const char * theTemplateArgument) 
        : TemplateUnitTest("LockTemplateUnitTest",theTemplateArgument) {}
    void run() {
        T someVariable = 1;
        ENSURE(someVariable);
        DPRINT2(getMyName(),someVariable);
    }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new LockUnitTest);
        addTest(new LockTemplateUnitTest<bool>("<bool>"));
        addTest(new LockTemplateUnitTest<int>("<int>"));
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    mySuite.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
