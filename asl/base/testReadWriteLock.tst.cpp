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
//    $RCSfile: testReadWriteLock.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// Description: unit test template file - change Lock to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "ReadWriteLock.h"

#include <string>
#include <iostream>

#include <sys/types.h>
#ifdef LINUX
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#endif
#include <errno.h>


using namespace std;  // automatically added!



class LockUnitTest : public UnitTest {
public:
    LockUnitTest() : UnitTest("LockUnitTest") {  }
    void run();
    void secondThread();

private:
    volatile int _mySharedVar;
    volatile int _otherSharedVar;
    //asl::ReadWriteLock _myLock;
    asl::PriorityCeilingRWLock _myLock;
    pthread_t _myThread;
};

void *threadFunc (void * This) {
    ((LockUnitTest*)This)->secondThread();
    pthread_exit (0);
    return 0;
}

void
LockUnitTest::run() {

    try {

        _myLock.readlock();
        SUCCESS("readlock");
        _myLock.readunlock();
        SUCCESS("readunlock");
        _myLock.writelock();
        SUCCESS("writelock");
        _myLock.writeunlock();
        SUCCESS("writeunlock");

        _mySharedVar = 0;

        int myRetVal = pthread_create (&_myThread, 0, threadFunc, this);
        ENSURE_MSG(myRetVal == 0, "pthread_create succeeded"); 

        _myLock.readlock();
        do {
            _myLock.readunlock();
#ifdef LINUX
            usleep(1000);
#endif
#ifdef WINDOWS
            ::Sleep(1);
#endif
            _myLock.readlock();
        } while (_mySharedVar != 1000);
        _myLock.readunlock();

        // perform many more tests here

        void * myThreadRetVal;
        myRetVal = pthread_join (_myThread, &myThreadRetVal); 
        ENSURE_MSG (myRetVal == 0, "pthread_join succeeded");
    } 
    catch (asl::Exception & ex) {
        FAILURE("Exception");
        cerr << ex << endl;
    }
}

void 
LockUnitTest::secondThread() {

    try {
        _myLock.readlock();
        do {
            _myLock.readunlock();
            _myLock.writelock();
            ++_mySharedVar;
            _myLock.writeunlock();
#ifdef LINUX
            usleep(1000);
#endif
#ifdef WINDOWS
            ::Sleep(1);
#endif
            _myLock.readlock();
        } while (_mySharedVar != 1000);
        _myLock.readunlock();
    }
    catch (asl::Exception & ex) {
        FAILURE("Exception");
        cerr << ex << endl;
    }
    SUCCESS("secondthread came through");
}

int main(int argc, char *argv[]) {

    LockUnitTest myTest;

    myTest.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << myTest.returnStatus() << endl;

    return myTest.returnStatus();

}
