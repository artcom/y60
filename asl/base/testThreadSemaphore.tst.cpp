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
//    $RCSfile: testThreadSemaphore.tst.cpp,v $
//
//   $Revision: 1.4 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
#include "ThreadSemaphore.h"
#include "Time.h"
#include "Logger.h"
#include "string_functions.h"
// #include "numeric_functions.h"

#include <string>
#include <iostream>

#include <sys/types.h>
#include <errno.h>

using namespace std;  // automatically added!
using namespace asl;



class SemaphoreUnitTest : public UnitTest {
public:
    SemaphoreUnitTest() : UnitTest("SemaphoreUnitTest") {  }
    void run();
    void secondThread();

private:
    ThreadSemaphore _mySemaphore;
    ThreadSemaphore _mySecondSemaphore;
    pthread_t _myThread;
};

void *threadFunc (void * This) {
    ((SemaphoreUnitTest*)This)->secondThread();
    pthread_exit (0);
    return 0;
}

void
SemaphoreUnitTest::run() {
    _mySemaphore.post();
    _mySemaphore.wait();
    ENSURE_MSG(true, "Simple semaphores work.");
    ThreadSemaphore myInitSema(5);
    ENSURE_MSG(myInitSema.getValue() == 5, "Non-Default Initialization works.");
    for (int i = 0; i < 4; ++i) {
        myInitSema.wait();
    }
    ENSURE_MSG(myInitSema.getValue() == 1, "Non-Default initialized Semaphores work.");
    ENSURE_MSG(myInitSema.wait(0), "Non-Default initialized Semaphores timeout works(1).");
    ENSURE_MSG(!myInitSema.wait(0), "Non-Default initialized Semaphore timeout works(2).");
    ENSURE_MSG(myInitSema.getValue() == 0, "Non-Default initialized Semaphores value correct.");
    for (int i = 0; i < 50000; ++i) {
        _mySemaphore.post();
    }
    ENSURE_MSG(50000 == _mySemaphore.getValue(), "Posted Semaphore 50000 times");
    int myRetVal = pthread_create (&_myThread, 0, threadFunc, this);
    ENSURE_MSG (myRetVal == 0, "pthread_create succeeded");

    // Increment the semaphore
    for (int i = 0; i < 10000; ++i) {
        _mySemaphore.post();
    }
    _mySecondSemaphore.wait();
    ENSURE_MSG(_mySemaphore.getValue() == 0, string("Posted Semaphore 1000 times. Value is: " + as_string(_mySemaphore.getValue())).c_str());
    ENSURE_MSG(_mySecondSemaphore.getValue() == 0, string("Second Semaphore is: " + as_string(_mySecondSemaphore.getValue())).c_str());
    for (int i = 0; i < 100000; ++i) {
        _mySemaphore.post();
        _mySecondSemaphore.wait();
    }
    ENSURE_MSG(true, "Successfully ran interleaved post/wait 100000 items.");
    msleep(1000);
    _mySemaphore.close();
    ENSURE_MSG(true, "Semaphore closed.");
    void * myThreadRetVal;
    myRetVal = pthread_join (_myThread, &myThreadRetVal);
    ENSURE_MSG (myRetVal == 0, "pthread_join succeeded");

    int diff1, diff2;
    Time start, end;

    start = Time();
    _mySecondSemaphore.wait(1000);
    end = Time();
    diff1 = int((end.micros() - start.micros()) / 1000);

    start = Time();
    _mySecondSemaphore.wait(2000);
    end = Time();
    diff2 = int((end.micros() - start.micros()) / 1000);

    string msg = string("waiting 2 secs takes roughly twice as long as waiting 1 sec. waited ")
               + as_string(2*diff1) + " vs. " + as_string(diff2) + " milliseconds.";
    ENSURE_MSG(abs(2*diff1 - diff2) < 100, msg.c_str());

    ENSURE_MSG(true, "Waiting for 3 seconds");
    ENSURE_MSG(!_mySecondSemaphore.wait(3000), "Timeout works.");
    _mySemaphore.reset(1);
    ENSURE_MSG(1 == _mySemaphore.getValue(), "Reset works");
    ENSURE_MSG(_mySemaphore.wait(10000), "Non-Timeout in wait returns true");
}

void
SemaphoreUnitTest::secondThread() {
    for (int i = 0; i < 50000; ++i) {
        _mySemaphore.wait();
    }
    ENSURE_MSG(true, "Successfully waited 50000 times in second thread.");
    for (int i = 0; i < 10000; ++i) {
        _mySemaphore.wait();
    }
    ENSURE_MSG(true, "Successfully waited additional 10000 times in second thread.");
    _mySecondSemaphore.post();
    for (int i = 0; i < 100000; ++i) {
        _mySemaphore.wait();
        _mySecondSemaphore.post();
    }
    msleep(500);
    ENSURE_MSG(true, "Blocking second thread.");
    try {
        _mySemaphore.wait();
    } catch (ThreadSemaphore::ClosedException &) {
        msleep(500);
        ENSURE_MSG(true, "Semaphore closed exception caught.");
    }
    ENSURE_MSG(true, "Second thread finished");
}


int main(int argc, char *argv[]) {
    Logger::get().setVerbosity(SEV_DEBUG);

    SemaphoreUnitTest myTest;

    myTest.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << myTest.returnStatus() << endl;

    return myTest.returnStatus();

}
