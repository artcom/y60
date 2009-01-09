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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//    $RCSfile: PosixThread.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.7 $
//
// Description:
//
//=============================================================================

// own header
#include "PosixThread.h"

#include "Logger.h"

#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <sched.h>
#include <string.h>


using namespace std;

#define DB(x) // x

namespace asl {

void *
threadFunc (void * This) {
    int bla;
    int *blabla = &bla; (void)blabla;
    DB(AC_TRACE << "stack address is = " << blabla << endl);

    DB(AC_TRACE << "threadFunc This= " << This << endl);

    PosixThread * myThread = static_cast<PosixThread*>(This);
    DB(AC_TRACE << "threadFunc This (2)= " << This << endl);
    if (!myThread) {
        AC_ERROR << "threadFunc: This is not a PosixThread *" << endl;
        return 0;
    }
//DB(
    while (!myThread->getThreadID()) {
        myThread->yield();
    }
//);
    myThread->_myIsActive = true;
    DB(AC_TRACE << "#INFO : PosixThread::threadfunc() : has set active variable for thread id "
         <<myThread->getThreadID()<< endl);
    try {
        myThread->run();
    } catch (const asl::Exception & e) {
        myThread->_myIsActive = false;
        AC_ERROR << "PosixThread: Unhandled asl::Exception in thread with id " <<
                myThread->getThreadID() << endl;
        AC_ERROR << e;
        AC_ERROR << "Rethrowing." << endl;
        throw;
    } catch (const std::exception & e) {
        myThread->_myIsActive = false;
        AC_ERROR << "PosixThread: Unhandled std::exception in thread with id " <<
                myThread->getThreadID() << endl;
        AC_ERROR << " Message was: " << e.what() << endl;
        AC_ERROR << "Rethrowing." << endl;
        throw;
    } catch (...) {
        myThread->_myIsActive = false;
        AC_ERROR << "PosixThread: Unhandled, unknown exception in thread with id " <<
                myThread->getThreadID() << endl;
        AC_ERROR << "Rethrowing." << endl;
        throw;
    }
    myThread->_myIsActive = false;

    DB(AC_TRACE << "#INFO : PosixThread::threadfunc() : has cleared active variable for thread id "
         <<myThread->getThreadID() << endl);

    return 0;
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

PosixThread::PosixThread(int mySchedPolicy, int myPriority)
    : _mySchedPolicy (mySchedPolicy),
      _myPriority (myPriority),
      _myIsActive (false),
      _myWorkFunc(0),
      _myWorkArgs(0),
      _myThread(0),
      _myShouldTerminate(false)
{
}

PosixThread::PosixThread (WorkFunc * workFunc, void * workArgs,
                          int mySchedPolicy, int myPriority)
    : _mySchedPolicy (mySchedPolicy),
      _myPriority (myPriority),
      _myIsActive (false),
      _myWorkFunc(workFunc),
      _myWorkArgs(workArgs),
      _myThread(0),
      _myShouldTerminate(false)
{
}

PosixThread::PosixThread ()
    : _mySchedPolicy (SCHED_OTHER),
      _myPriority (0),
      _myIsActive (false),
      _myWorkFunc(0),
      _myWorkArgs(0),
      _myThread(0),
      _myShouldTerminate(false)
{
}

PosixThread::~PosixThread() {
    if (isActive()) {
        AC_ERROR << "~PosixThread() : destruction of active thread" << endl;
    }
}


bool
PosixThread::setPriority (int mySchedPolicy, int myPriority) {
    _mySchedPolicy = mySchedPolicy;
    _myPriority = myPriority;
    return updatePriority();
}

bool
PosixThread::setRealtimePriority(int modifier) {
    setPriority(SCHED_FIFO, sched_get_priority_max(SCHED_FIFO) + modifier);
    return updatePriority();
}

bool
PosixThread::setDefaultPriority() {
    setPriority(SCHED_OTHER, 0);
    return updatePriority();
}


bool
PosixThread::updatePriority() {
    if (!isActive()) {
        AC_ERROR << "PosixThread::updatePriority() : thread not active" << endl;
        return false;
    }

    sched_param myParam;
    myParam.sched_priority = _myPriority;
    int myRetVal = pthread_setschedparam (_myThread, _mySchedPolicy, &myParam);
    switch (myRetVal) {
        case 0:
            return true;
        case EPERM: 
            return false;
		case ENOTSUP:{
            // This happens if the scheduling policy isn't SCHED_OTHER under windows.
            DB(AC_TRACE << "###ERROR: PosixThread::updatePriority() : pthread_setschedparam: ENOTSUP"
                 << endl;)
				 return false;
					 }
		case EINVAL:{
            AC_ERROR << "PosixThread::updatePriority() : pthread_setschedparam: EINVAL"
                 << endl;
			return false;}
		case ESRCH:{
            AC_ERROR << "PosixThread::updatePriority() : pthread_setschedparam: ESRCH"
                 << endl;
			return false;}
		case EFAULT:{
            AC_ERROR << "PosixThread::updatePriority() : pthread_setschedparam: EFAULT"
                 << endl;
			return false;}
		default:{
            AC_ERROR << "PosixThread::updatePriority() : pthread_setschedparam: Unknown"
                    " error code (" << myRetVal << ") "
                 << endl;
			return false;}
    }
}


bool
PosixThread::shouldTerminate() {
  return _myShouldTerminate;
}


void PosixThread::fork () {
    if (isActive()) {
        AC_ERROR << "PosixThread::fork() called on already active thread" << endl;
        return;
    }
    int myRetVal;

    DB(PosixThread * This = dynamic_cast<PosixThread*>(this);
    AC_TRACE << "PosixThread::fork this = " << this << endl;
    AC_TRACE << "PosixThread::fork This = " << This << endl);

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
           AC_WARNING << "PosixThread::fork() cannot set stack size. error= "
                << strerror(status) << endl;
       }
       status = pthread_attr_setguardsize(&threadAttr, 16*4096);
       if (status) {
           AC_WARNING << "PosixThread::fork() cannot set guard size. error= "
                << strerror(status) << endl;
       }
       pthread_attr_getstacksize(&threadAttr, &stackSize);
       pthread_attr_getguardsize(&threadAttr, &guardSize);

       status = pthread_attr_getstackaddr(&threadAttr, &stackAddr);
       if (status) {
           AC_WARNING << "PosixThread::fork() can not get stack addr. error= "
                << strerror(status) << endl;
       }
    );
    DB(AC_TRACE << "#INFO : PosixThread::fork() : thread has stack size " << stackSize << " bytes" << endl;)
    DB(AC_TRACE << "#INFO : PosixThread::fork() : thread has stack addr " << stackAddr << "." << endl;)
    DB(AC_TRACE << "#INFO : PosixThread::fork() : thread has guard size " << guardSize << " bytes" << endl;)
#if defined(MAKE_DEBUG_VARIANT)
    //myRetVal = pthread_create (&_myThread, &threadAttr, threadFunc, this);
    myRetVal = pthread_create (&_myThread, 0, threadFunc, this);
#else
    myRetVal = pthread_create (&_myThread, 0, threadFunc, this);
#endif

    DB(AC_TRACE << "#INFO : PosixThread::fork() : created thread id " <<getThreadID() << endl);
    testRetVal (myRetVal, "PosixThread:pthread_create");
    _myIsActive = true;
    DB(AC_TRACE << "#INFO : PosixThread::fork() : has set active variable for thread id " <<getThreadID() << endl);
}

void PosixThread::join () {
    if (!isActive()) {
        AC_DEBUG << "PosixThread::join() : thread not active" << endl;
    }
    DB(AC_TRACE << "#INFO : PosixThread::join() called for thread id " <<getThreadID() << endl);
    void * myThreadRetVal=0;
    int myRetVal;
    _myShouldTerminate = true;
    myRetVal = pthread_join (_myThread, &myThreadRetVal);
    DB(AC_TRACE << "#INFO : PosixThread::joined() thread id " <<getThreadID() << endl);
    testRetVal (myRetVal, "PosixThread:pthread_join");
    _myIsActive = false;
    _myShouldTerminate = false;
    DB(AC_TRACE << "#INFO : PosixThread::join() : has cleared active variable" << endl);
}

#ifndef WIN32
bool
PosixThread::kill(int signalNumber) {
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
                AC_WARNING << "PosixThread::kill() : pthread_kill: EINVAL" << endl;
                return false;
            case ESRCH:
                AC_WARNING << "PosixThread::kill() : pthread_kill: ESRCH" << endl;
                _myIsActive = false;
                return true;
            default:
                AC_WARNING << "PosixThread::kill() : pthread_kill: Unknown error code" << endl;
                return false;
        }

    }
    return true;
}
#endif


bool PosixThread::isActive() {
    return _myIsActive;
}

void PosixThread::yield() {
    sched_yield();
}


int PosixThread::getMaxPriority (int theSchedPolicy) {
    return sched_get_priority_max (theSchedPolicy);
}

int PosixThread::getMinPriority (int theSchedPolicy) {
    return sched_get_priority_min (theSchedPolicy);
}

void
PosixThread::run() {
    //updatePriority();  // TODO: Does not work, WHY???
    if (!_myWorkFunc) {
        AC_ERROR << "PosixThread::run() called without valid work function" << endl;
        return;
    }
    DB(AC_TRACE << "************ PosixThread::run() called" << endl);
    _myWorkFunc(*this);
}

}

