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
//    $RCSfile: testPosixThread.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "PosixThread.h"
#include "Time.h"

#include <string>
#include <iostream>

#include <sys/types.h>
#include <errno.h>

using namespace std;  // automatically added!

class DerivedThread: public PosixThread {
public:    
    DerivedThread (): PosixThread() { _myVar = 0; };
    virtual ~DerivedThread () {};

    int getVar () { return _myVar; };

private:
    virtual void run () {
        _myVar = 1;
        while (!shouldTerminate()) {
            asl::msleep(1000);
        }            
    }
    int _myVar;
};

class PosixThreadUnitTest : public UnitTest {
public:
    PosixThreadUnitTest() : UnitTest("PosixThreadUnitTest") {  }
    void run();

private:
    DerivedThread _myThread1;
    DerivedThread _myThread2;
    
};


void
PosixThreadUnitTest::run() {
    _myThread1.fork();
    asl::msleep(10);
    ENSURE_MSG (_myThread1.isActive(), "Thread sais it's active when it is.");
    _myThread1.join();
    ENSURE_MSG (_myThread1.getVar() == 1, "Thread executed successfully");
    ENSURE_MSG (!_myThread1.isActive(), "Thread sais it isn't active when it isn't.");

    bool myOK;
    _myThread2.fork();
    myOK = _myThread2.setPriority (SCHED_OTHER, PosixThread::getMaxPriority(SCHED_OTHER));
    ENSURE_MSG (myOK, "setPriority to max(SCHED_OTHER) works.");
    _myThread2.join();
}

int main(int argc, char *argv[]) {

    PosixThreadUnitTest myTest;

    myTest.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << myTest.returnStatus() << endl;

    return myTest.returnStatus();

}
