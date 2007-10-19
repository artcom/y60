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
//    $RCSfile: testThreadLock.tst.cpp,v $
//
//   $Revision: 1.2 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "ThreadLock.h"
#include "Time.h"

#include <string>
#include <iostream>

#include <sys/types.h>
#include <errno.h>

using namespace std;  // automatically added!
using namespace asl;


class LockUnitTest : public UnitTest {
public:
    LockUnitTest() : UnitTest("LockUnitTest") {  }
    void run();
    void secondThread();

private:
    int _mySharedVar;
    asl::ThreadLock _myLock;
    pthread_t _myThread;
};

void *threadFunc (void * This) {
    ((LockUnitTest*)This)->secondThread();
    pthread_exit (0);
    return 0;
}

void
LockUnitTest::run() {
    
    _myLock.lock();
    _myLock.unlock();
    ENSURE_MSG(true, "Simple locks work.");

    _myLock.lock();

    int myRetVal = pthread_create (&_myThread, 0, threadFunc, this);
    ENSURE_MSG (myRetVal == 0, "pthread_create succeeded"); 

    // Do something with the shared variable.
    for (_mySharedVar=0; _mySharedVar<1000;_mySharedVar++) ;
    ENSURE_MSG (_mySharedVar == 1000, "Basic Lock works");
    asl::msleep (500);
    _myLock.unlock();
    
    void * myThreadRetVal;
    myRetVal = pthread_join (_myThread, &myThreadRetVal); 
    ENSURE_MSG (myRetVal == 0, "pthread_join succeeded");
    
    _myLock.lock();
    _myLock.lock();
    _myLock.unlock();
    _myLock.unlock();
    ENSURE_MSG(true, "Recursive locks are allowed.");
    
    // See if resources are freed.
    for (int i=0; i< 1000000; i++) {
        asl::ThreadLock myLock;
        myLock.lock();
        myLock.unlock();
    }
    ENSURE_MSG(true, "Using a million locks sequentially is possible.");
}

void 
LockUnitTest::secondThread() {
    bool myRetVal;

    myRetVal = _myLock.trylock();
    ENSURE_MSG (!myRetVal, "nonblock_lock returns false");
    
    _myLock.lock();
    // Make sure the first thread had time to run.
    ENSURE (_mySharedVar == 1000);
    _mySharedVar = 2000;
    _myLock.unlock();

    myRetVal = _myLock.trylock();
    ENSURE_MSG (myRetVal, "nonblock_lock returns true");

    _myLock.unlock();
}


int main(int argc, char *argv[]) {

    LockUnitTest myTest;

    myTest.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << myTest.returnStatus() << endl;

    return myTest.returnStatus();

}
