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

using namespace std;
using namespace asl;

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
    ENSURE_MSG (_myThread1.isUnjoined(), "Thread is joined after forking");
    _myThread1.join();
    ENSURE_MSG (_myThread1.getVar() == 1, "Thread executed successfully");
    ENSURE_MSG (!_myThread1.isUnjoined(), "Thread is unjoined after joining");

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
