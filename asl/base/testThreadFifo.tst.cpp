/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1998, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testThreadFifo.tst.cpp,v $
//
//   $Revision: 1.4 $
//
//      Author: Axel Schmidt
//
// Description: ThreadFifo test prog
//              ATTENTION: DONT USE IOSTREAMS HERE,
//              BECAUSE THEY'RE NOT THREADSAFE!
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/


#include "ThreadFifo.h"
#include "PosixThread.h"

#include "Time.h"
#if 0
#ifdef WIN32
#include <winsock2.h> // for timeval
#else
#   include <unistd.h>
#   include <sys/time.h>
#endif
#endif
#include <signal.h>
#include <stdio.h>
#include <cassert>

using namespace std;

#define DB(x) // x

using namespace asl;

ThreadFifo<int>* sharedFifo;

int NUM_VALUES = 100000;

inline double
getTime()
{
    return static_cast<double>(NanoTime().micros());
}

void
pusher()
{
    int val = 0;

    while (val < NUM_VALUES) {
        sharedFifo->push(++val);

        if (!(val % 700)) {
            fprintf(stderr, "%d pushed, size = %d\n", val, sharedFifo->size()); fflush(stderr);
            sched_yield();
        }
    }
}

void
blocking_poper()
{
    int val = 0;
    do {
        if (sharedFifo->pop_blocking(val)) {
            if (!(val % 1100)) {
                fprintf(stderr, "%d popped, size = %d\n", val, sharedFifo->size()); fflush(stderr);
            }
        } else {
            if (sharedFifo->getSignalToStop()) {
                fprintf(stderr, "blocking_poper() : stop signal from FIFO. stopping now...");
                return;
            }
            fprintf(stderr, "blocking_poper() nap\n"); fflush(stderr);
            sched_yield();
        }
    } while (val < NUM_VALUES);
}

void
waiting_poper()
{
    int val = 0;
    do {
        if (sharedFifo->pop_waiting(val)) {
            if (!(val % 1100)) {
                fprintf(stderr, "%d popped, size = %d\n", val, sharedFifo->size()); fflush(stderr);
            }
        } else {
            if (sharedFifo->getSignalToStop()) {
                fprintf(stderr, "waiting_poper() : stop signal from FIFO. stopping now...");
                return;
            }
            assert(0);
        }
        sched_yield();
    } while (val < NUM_VALUES);
}


void
nonblocking_poper()
{
    int val = 0;
    do {
        if (sharedFifo->pop_nonblocking(val)) {
            if (!(val % 1100)) {
                fprintf(stderr, "%d poped, size = %d\n", val, sharedFifo->size()); fflush(stderr);
            }
        } else {
            if (sharedFifo->getSignalToStop()) {
                fprintf(stderr, "nonblocking_poper() : stop signal from FIFO. stopping now...");
                return;
            }
            fprintf(stderr, "nonblocking_poper() nap\n"); fflush(stderr);
            sched_yield();
            msleep(1);
        }
    } while (val < NUM_VALUES);
}

void 
pusher_thread_func(PosixThread & thread) {
    //int * args = (int * ) thread.getWorkArgs();
    pusher();
}

void 
blocking_test_popper_thread_func(PosixThread & thread) {
    blocking_poper();
}


void 
nonblocking_test_popper_thread_func(PosixThread & thread) {
    nonblocking_poper();
}

void 
waiting_test_popper_thread_func(PosixThread & thread) {
    waiting_poper();
}




bool
blocking_test(int ac, char** av)
{
    sharedFifo = new ThreadFifo<int>;

    if (ac == 2) {
        int nn = atoi(av[1]);
        if (nn >= 0) {
            NUM_VALUES = nn;
        }
    }

    fprintf(stderr, "running blocking ThreadFifo-test with %d elements.\n", NUM_VALUES);

    double start_time = getTime();

    PosixThread pushThread(pusher_thread_func);
    PosixThread popThread(blocking_test_popper_thread_func);
    
    pushThread.fork();
    popThread.fork();

    pushThread.join();
    popThread.join();

    double elapsed_time = getTime() - start_time;

    fprintf(stderr, "%d elements queued in %lf seconds.\n", NUM_VALUES, elapsed_time);
    return true;
}


bool
nonblocking_test(int ac, char** av)
{
    sharedFifo = new ThreadFifo<int>;

    if (ac == 2) {
        int nn = atoi(av[1]);
        if (nn >= 0) {
            NUM_VALUES = nn;
        }
    }

    fprintf(stderr, "running nonblocking ThreadFifo-test with %d elements.\n", NUM_VALUES);

    double start_time = getTime();

    PosixThread pushThread(pusher_thread_func);
    PosixThread popThread(nonblocking_test_popper_thread_func);
    
    popThread.fork();
    pushThread.fork();

    pushThread.join();
    popThread.join();

    double elapsed_time = getTime() - start_time;

    fprintf(stderr, "%d elements queued in %lf seconds.\n", NUM_VALUES, elapsed_time);
    return true;
}


bool
waiting_test(int ac, char** av)
{
    sharedFifo = new ThreadFifo<int>;

    if (ac == 2) {
        int nn = atoi(av[1]);
        if (nn >= 0) {
            NUM_VALUES = nn;
        }
    }

    fprintf(stderr, "running waiting ThreadFifo-test with %d elements.\n", NUM_VALUES);

    double start_time = getTime();

    PosixThread pushThread(pusher_thread_func);
    PosixThread popThread1(waiting_test_popper_thread_func);
    PosixThread popThread2(waiting_test_popper_thread_func);
    
    pushThread.fork();
    popThread1.fork();
    popThread2.fork();

    pushThread.join();

    msleep(10*1000);

    sharedFifo->setSignalToStop();
    sharedFifo->push(0); // value will be ignored, just sets semaphores
    
    popThread1.join();
    popThread2.join();
    
    double elapsed_time = getTime() - start_time;

    fprintf(stderr, "%d elements queued in %lf seconds.\n", NUM_VALUES, elapsed_time);
    return true;
}


int
main(int ac, char** av)
{
    ThreadSemFactory::instance();
   
    if (!nonblocking_test(ac,av)) {
        delete ThreadSemFactory::instance();
        return -1;
    }
    
    if (!blocking_test(ac,av)) {
        delete ThreadSemFactory::instance();
        return -1;
    }

    if (!waiting_test(ac,av)) {
        delete ThreadSemFactory::instance();
        return -1;
    }

    delete ThreadSemFactory::instance();

    return 0;
}
