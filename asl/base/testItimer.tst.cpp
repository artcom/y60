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
//    $RCSfile: testItimer.tst.cpp,v $
//
//   $Revision: 1.1.1.1 $
//
// Description: unit test template file - change Lock to whatever
//              you want to test and add the apprpriate tests.
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "UnitTest.h"
//#define USE_MUTEX_BASED_IMPLEMENTATION
#include "ReadWriteLock.h"
#include "Time.h"

#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>


using namespace std;  // automatically added!



class LockUnitTest : public UnitTest {
public:
    LockUnitTest() : UnitTest("LockUnitTest"), _ready(false) {  }
    void run();
    void mediumPrioThread();
    void lowPrioThread(); 

private:
    //asl::ReadWriteLock _myLock;
    asl::PriorityCeilingRWLock _myLock;
    pthread_t _myMediumThread;
    pthread_t _myLowThread;
    bool _ready;
};

void *threadFuncLow(void * This) {
    ((LockUnitTest*)This)->lowPrioThread();
    pthread_exit (0);
    return 0;
}

void *threadFuncMedium (void * This) {
    ((LockUnitTest*)This)->mediumPrioThread();
    pthread_exit (0);
    return 0;
}

void
LockUnitTest::run() {

    asl::Time start;
    try {
        _myLock.readlock();
        SUCCESS("readlock");
        _myLock.readunlock();
        SUCCESS("readunlock");
        _myLock.writelock();
        SUCCESS("writelock");
        _myLock.writeunlock();
        SUCCESS("writeunlock");

        // Block SIGALRM globally before starting threads

        sigset_t set;

        sigemptyset(&set);
        sigaddset(&set, SIGALRM);

        const sigset_t alarmSet = set;

        sigprocmask(SIG_BLOCK, &alarmSet, 0);

        int myRetVal = pthread_create (&_myMediumThread, 0, threadFuncMedium, this);
        ENSURE_MSG(myRetVal == 0, "pthread_create (medium) succeeded"); 

        myRetVal = pthread_create (&_myLowThread, 0, threadFuncLow, this);
        ENSURE_MSG(myRetVal == 0, "pthread_create (low) succeeded"); 

        // we are the high priority thread and may be kept by the low prio thread from running
        struct sched_param schedulingParm;
        schedulingParm.sched_priority = sched_get_priority_max(SCHED_FIFO);
        cerr << "LowPrioThrad: Setting SCHED_FIFO max prio=" << schedulingParm.sched_priority << endl;
        cerr << "LowPrioThrad: SCHED_FIFO min prio=" << sched_get_priority_min(SCHED_FIFO) << endl;
        int status = pthread_setschedparam(pthread_self(),
                SCHED_FIFO,
                &schedulingParm);
        if (status != 0) {
            cerr << "### main thread: Setting max priority failed" << endl;
        }

        cycles_t cycles;
        _myLock.readlock();
        for (int i = 0; i < 10; ++i) {
            cerr << "HIGH_LOCKED_LOOP START " << get_cycles() - cycles << endl;
            cycles=get_cycles();
            for (int i = 0; i < 10000; ++i) {
              ;
            }
            cerr << "HIGH_LOCKED_LOOP END " << get_cycles() - cycles << endl;
            cycles=get_cycles();
            _myLock.readunlock();
            cerr << "HIGH_LOOP START " << get_cycles() - cycles << endl;
            cycles=get_cycles();
//              for (int i = 0; i < 100000; ++i) {
//                ;
//              }

            asl::Time sleep_start;
            usleep(1000);
            asl::Time sleep_stop;
            cerr << "HIGH_LOOP Sleep " << (sleep_stop - sleep_start) * 1e6 << " usec.";

            struct itimerval sleepForMe;

            sleepForMe.it_value.tv_sec = 0;
            sleepForMe.it_value.tv_usec = 3;
            sleepForMe.it_interval.tv_sec = 0;
            sleepForMe.it_interval.tv_usec = 0;

            int theSignal;

            asl::Time sigwait_start;
            if (setitimer(ITIMER_REAL, &sleepForMe, 0) != 0) {
              cerr << "### main thread: Setting timer failed" << endl;
            } else {
              sigwait(&alarmSet, &theSignal);
            }
            asl::Time sigwait_stop;
            cerr << "HIGH_LOOP sigwait " << (sigwait_stop - sigwait_start) * 1e6 << " usec.";

            cerr << "HIGH_LOOP END " << get_cycles() - cycles << endl;
            cycles=get_cycles();
            _myLock.readlock();
        };
        _myLock.readunlock();

        // perform many more tests here

        _ready = true;
        void * myThreadRetVal;
        myRetVal = pthread_join (_myMediumThread, &myThreadRetVal); 
        ENSURE_MSG (myRetVal == 0, "pthread_join succeeded");
        myRetVal = pthread_join (_myLowThread, &myThreadRetVal); 
        ENSURE_MSG (myRetVal == 0, "pthread_join succeeded");
    } 
    catch (asl::Exception & ex) {
        FAILURE("Exception");
        cerr << ex << endl;
    }
    asl::Time stop;
    cerr << "Total time: " << stop-start << " secs" << endl;

}

void 
LockUnitTest::lowPrioThread() {

    struct sched_param schedulingParm;
//#define RR
#ifdef RR
    schedulingParm.sched_priority = sched_get_priority_min(SCHED_RR);
    cerr << "LowPrioThrad: Setting SCHED_RR min prio=" << schedulingParm.sched_priority << endl;
    cerr << "LowPrioThrad: SCHED_RR max prio is=" << sched_get_priority_max(SCHED_RR) << endl;
    int status = pthread_setschedparam(pthread_self(),
            SCHED_RR,
            &schedulingParm);
#else
    schedulingParm.sched_priority = sched_get_priority_min(SCHED_OTHER);
    cerr << "LowPrioThrad: Setting SCHED_OTHER prio=" << schedulingParm.sched_priority << endl;
    cerr << "LowPrioThrad: SCHED_OTHER max prio is=" << sched_get_priority_max(SCHED_OTHER) << endl;
    int status = pthread_setschedparam(pthread_self(),
            SCHED_OTHER,
            &schedulingParm);
#endif
    if (status != 0) {
        cerr << "### low thread: Setting min priority failed" << endl;
    }

    cycles_t cycles;

    try {
        _myLock.readlock();
        do {
            cerr << "LOW_LOCKED_LOOP START " << get_cycles()-cycles<< endl;cycles=get_cycles();
            for (int i = 0; i < 10000;++i);
            cerr << "LOW_LOCKED_LOOP END " << get_cycles()-cycles<< endl;cycles=get_cycles();
            _myLock.readunlock();
            cerr << "LOW_LOOP START " << get_cycles()-cycles<< endl;cycles=get_cycles();
            for (int i = 0; i < 10000;++i);
            cerr << "LOW_LOOP END " << get_cycles()-cycles<< endl;cycles=get_cycles();
             //_myLock.writelock();
            //_myLock.writeunlock();
            #if 0
            cerr << "LOW_SLEEP_START " << get_cycles()-cycles<< endl;cycles=get_cycles();
            usleep(1000);
            cerr << "LOW_SLEEP_END " << get_cycles()-cycles<< endl;cycles=get_cycles();
            #endif
            _myLock.readlock();
        } while (!_ready);;
        _myLock.readunlock();
    }
    catch (asl::Exception & ex) {
        FAILURE("Exception");
        cerr << ex << endl;
    }
    SUCCESS("secondthread came through");
}

void 
LockUnitTest::mediumPrioThread() {
    cycles_t cycles;
    while (!_ready) {
        for (int i=0; i < 2; ++i) {
            _myLock.readlock();
            cerr << "MED_LOCKED_LOOP START " << get_cycles()-cycles<< endl;cycles=get_cycles();
            for (int i = 0; i < 10000;++i);
            cerr << "MED_LOCKED_LOOP END " << get_cycles()-cycles<< endl;cycles=get_cycles();
             _myLock.readunlock();
            cerr << "MED_LOOP START " << get_cycles()-cycles<< endl;cycles=get_cycles();
            for (int i = 0; i < 10000;++i);
            cerr << "MED_LOOP END " << get_cycles()-cycles<< endl;cycles=get_cycles();
         }
        //usleep(1000);
    }
}

 
int main(int argc, char *argv[]) {

    LockUnitTest myTest;

    myTest.run();

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << myTest.returnStatus() << endl;

    return myTest.returnStatus();

}
