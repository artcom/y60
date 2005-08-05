//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: Thread.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1.1.1 $
//
// Description: 
//
//=============================================================================

#include "Thread.h"

#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <string.h>

#ifdef LINUX
#include <sched.h>
#endif

using namespace std;  // automatically added!

char * OK_value = "Ok";

#define DB(x) // x

namespace asl {


#ifdef LINUX
void * 
threadFunc (void * This) {
#else
DWORD 
threadFunc( LPDWORD *This ) {
#endif
    int bla;
    int *blabla = &bla;
    DB(cerr << "stack address is = " << blabla << endl);
    DB(cerr << "threadFunc This= " << This << endl);

    Thread * myThread = static_cast<Thread*>((void*)This);
    DB(cerr << "threadFunc This (2)= " << This << endl);
    if (!myThread) {
        cerr << "threadFunc: This is not a Thread *" << endl;
        assert(0);
    }

    while (!myThread->getThreadID()) {
        myThread->yield();
    }

    myThread->_myIsActive = true;
    DB(cerr << "#INFO : Thread::threadfunc() : has set active variable for thread id " 
         <<myThread->getThreadID()<< endl);
    
    myThread->run();
    myThread->_myIsActive = false;

    DB(cerr << "#INFO : Thread::threadfunc() : has cleared active variable for thread id "
         <<myThread->getThreadID() << endl);
#ifdef LINUX
    pthread_exit ((void *)OK_value);
    return 0;
#else    
    return 1;
#endif    
}

bool
testRetVal (int theRetVal, const char * theMsg) {
    if (theRetVal != 0) {
        if (errno != 0) {
            perror (theMsg);
            return false;
        }
    }
    return true;
}

Thread::Thread(PRIORITY_CLASS myPriorityClass, double myPriority)
    : _myPriorityClass (myPriorityClass), 
      _myPriority (myPriority), 
      _myIsActive (false),
      _myWorkFunc(0),
      _myWorkArgs(0),
      _myThread(0)
{
}

Thread::Thread (WorkFunc * workFunc, void * workArgs, 
                PRIORITY_CLASS myPriorityClass, double myPriority)
    : _myPriorityClass (myPriorityClass), 
      _myPriority (myPriority), 
      _myIsActive (false),
      _myWorkFunc(workFunc),
      _myWorkArgs(workArgs),
      _myThread(0)
{
}

Thread::Thread ()
    : _myPriorityClass (NORMAL), 
      _myPriority (0), 
      _myIsActive (false),
      _myWorkFunc(0),
      _myWorkArgs(0),
      _myThread(0)
{
}

Thread::~Thread() {
    if (isActive()) {
        cerr << "#ERROR : ~Thread() : destruction of active thread" << endl;
    }
}


bool 
Thread::setPriority (PRIORITY_CLASS myPriorityClass, double myPriority) {
    _myPriorityClass = myPriorityClass;
    _myPriority = myPriority;
    return updatePriority();
}

bool
Thread::setRealtimePriority(double myPriority) {
    setPriority(REALTIME, myPriority);
    return updatePriority();
}

bool
Thread::setDefaultPriority() {
    setPriority(NORMAL, 0.5);
    return updatePriority();
}


bool 
Thread::updatePriority() {
    if (!isActive()) {
        cerr << "#ERROR : UpdatePriority() : thread not active" << endl;
        return false;
    }
    
#ifdef LINUX
    sched_param myParam;
    int mySchedPolicy;
    
    if (_myPriorityClass == REALTIME) {
        mySchedPolicy = SCHED_FIFO;
    } else {
        mySchedPolicy = SCHED_OTHER;
    }
    int max = getMaxPriority(mySchedPolicy);
    int min = getMinPriority(mySchedPolicy);
    myParam.sched_priority = min+(max-min)*_myPriority;  
    int myRetVal = pthread_setschedparam (_myThread, mySchedPolicy, &myParam);
    switch (myRetVal) {
        case 0:
            return true;
        case EPERM:
            return false;
        case EINVAL:
            cerr << "###ERROR: Thread::updatePriority() : pthread_setschedparam: EINVAL" 
                 << endl;
            return false;
        case ESRCH:
            cerr << "###ERROR: Thread::updatePriority() : pthread_setschedparam: ESRCH" 
                 << endl;
            return false;
        case EFAULT:
            cerr << "###ERROR: Thread::updatePriority() : pthread_setschedparam: EFAULT" 
                 << endl;
            return false;
        default:
            cerr << "###ERROR: Thread::updatePriority() : pthread_setschedparam: Unknown"
                    " error code" 
                 << endl;
            return false;
    }
#else
    int myWinPriority;
    if (_myPriorityClass == REALTIME) {
        myWinPriority = THREAD_PRIORITY_TIME_CRITICAL;
    } else if (_myPriority < 0.1) {
        myWinPriority = THREAD_PRIORITY_IDLE;
    } else if (_myPriority < 0.3) {
        myWinPriority = THREAD_PRIORITY_LOWEST;
    } else if (_myPriority < 0.5) {
        myWinPriority = THREAD_PRIORITY_BELOW_NORMAL;
    } else if (_myPriority < 0.7) {
        myWinPriority = THREAD_PRIORITY_NORMAL;
    } else if (_myPriority < 0.9) {
        myWinPriority = THREAD_PRIORITY_ABOVE_NORMAL;
    } else {
        myWinPriority = THREAD_PRIORITY_HIGHEST;
    }       
    
    bool myErr = SetThreadPriority(_myThread, myWinPriority);
    if (myErr) {
        DWORD err = GetLastError();
        
    }
    
#endif
    
}


void Thread::fork () {
    if (isActive()) {
        cerr << "#ERROR : Thread::fork() called on already active thread" << endl;
        assert (0);
    }
    int myRetVal;
   
    DB(Thread * This = dynamic_cast<Thread*>(this);
    cerr << "Thread::fork this = " << this << endl;
    cerr << "Thread::fork This = " << This << endl);
    
    DB(pthread_attr_t   threadAttr;
       size_t           stackSize = 2;
       size_t           guardSize = 2;
       void *           stackAddr = 0;
       pthread_attr_init(&threadAttr);
       pthread_attr_getstacksize(&threadAttr, &stackSize);
       pthread_attr_getguardsize(&threadAttr, &guardSize);
       int status = 0;
       status = pthread_attr_setstacksize(&threadAttr, 2*1024*1024);
       if (status) { 
           cerr << "#WARNING: Thread::fork() cannot set stack size. error= " 
                << strerror(status) << endl;
       }
       status = pthread_attr_setguardsize(&threadAttr, 16*4096);
       if (status) { 
           cerr << "#WARNING: Thread::fork() cannot set guard size. error= " 
                << strerror(status) << endl;
       }
       pthread_attr_getstacksize(&threadAttr, &stackSize);
       pthread_attr_getguardsize(&threadAttr, &guardSize);
       
       status = pthread_attr_getstackaddr(&threadAttr, &stackAddr);
       if (status) { 
           cerr << "#WARNING: Thread::fork() can not get stack addr. error= " 
                << strerror(status) << endl;
       }
       );
    DB(cerr << "#INFO : Thread::fork() : thread has stack size " << stackSize << " bytes" << endl;)
    DB(cerr << "#INFO : Thread::fork() : thread has stack addr " << stackAddr << "." << endl;)
    DB(cerr << "#INFO : Thread::fork() : thread has guard size " << guardSize << " bytes" << endl;)
#if defined(MAKE_DEBUG_VARIANT)  
    //myRetVal = pthread_create (&_myThread, &threadAttr, threadFunc, this);
    myRetVal = pthread_create (&_myThread, 0, threadFunc, this);
#else
    myRetVal = pthread_create (&_myThread, 0, threadFunc, this);
#endif
    
    DB(cerr << "#INFO : Thread::fork() : created thread id " <<getThreadID() << endl);
    testRetVal (myRetVal, "Thread:pthread_create");
    _myIsActive = true;
    DB(cerr << "#INFO : Thread::fork() : has set active variable for thread id " <<getThreadID() << endl);
}

void Thread::join () {
    //assert (isActive());
    if (!isActive()) {
        cerr << "#WARNING : Thread::join() : thread not active" << endl;
    }
    DB(cerr << "#INFO : Thread::join() called for thread id " <<getThreadID() << endl);
    void * myThreadRetVal=0;
    int myRetVal;
    myRetVal = pthread_join (_myThread, &myThreadRetVal);
    DB(cerr << "#INFO : Thread::joined() thread id " <<getThreadID() << endl);
    if (testRetVal (myRetVal, "Thread:pthread_join")) {
        if (myThreadRetVal && strcmp ((char *)(myThreadRetVal), "Ok")) {
            cerr << "Warning: premature thread exit in Thread::join()" << endl;
        }
    }
    _myIsActive = false;
    DB(cerr << "#INFO : Thread::join() : has cleared active variable" << endl);
}

bool    
Thread::kill(int signalNumber) {
    if (isActive()) {
        
        int myRetVal;
        myRetVal = pthread_kill (_myThread, signalNumber);
        switch (myRetVal) {
            case 0:
                _myIsActive = false;
                return true;
            case EPERM:
                return false;
            case EINVAL:
                cerr << "#WARNING : Thread::kill() : pthread_kill: EINVAL" << endl;
                return false;
            case ESRCH:
                cerr << "#WARNING : Thread::kill() : pthread_kill: ESRCH" << endl;
                _myIsActive = false;
                return true;
            default:
                cerr << "pthread_kill: Unknown error code" << endl;
                return false;
        }

    }
    return true;
}
    
bool Thread::isActive() {
    return _myIsActive;
}

void Thread::yield() {
    sched_yield();
}


int Thread::getMaxPriority (int theSchedPolicy) {
    return sched_get_priority_max (theSchedPolicy);
}

int Thread::getMinPriority (int theSchedPolicy) {
    return sched_get_priority_min (theSchedPolicy);
}

void
Thread::run() {
    //updatePriority();  // TODO: Does not work, WHY???
    if (!_myWorkFunc) {
        cerr << "Thread::run() called without valid work function" << endl;
        assert(0);
    }
    DB(cerr << "************ Thread::run() called" << endl);
    _myWorkFunc(*this);
}

}
