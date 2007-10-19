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
//    $RCSfile: ThreadLock.cpp,v $
//
//     $Author: david $
//
//   $Revision: 1.6 $
//
// Description:
//
//=============================================================================

#include <errno.h>
#include <pthread.h>

#include "Logger.h"

#include "ThreadHelper.h"

#include "ThreadLock.h"

namespace asl {

ThreadLock::ThreadLock()
{
#ifdef LINUX
    pthread_mutex_t myMutexInitializer = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    _myMutex = myMutexInitializer;
#else
    pthread_mutexattr_t myMutexAttr;
    int myRetVal;

    myRetVal = pthread_mutexattr_init(&myMutexAttr);
    checkRetVal(myRetVal, "ThreadLock::ThreadLock - pthread_mutexattr_init");
    myRetVal = pthread_mutexattr_settype(&myMutexAttr, PTHREAD_MUTEX_RECURSIVE);
    checkRetVal(myRetVal, "ThreadLock::ThreadLock - pthread_mutexattr_settype");

    myRetVal = pthread_mutex_init(&_myMutex, &myMutexAttr);
    checkRetVal(myRetVal, "ThreadLock::ThreadLock");
#endif
}

ThreadLock::~ThreadLock() {
    pthread_mutex_destroy(&_myMutex);
}

void
ThreadLock::lock() {
    int myReturnValue;

    myReturnValue = pthread_mutex_lock (&_myMutex);

    pthreadCheckReturnValue (myReturnValue, "pthread_mutex_lock");
}

bool
ThreadLock::trylock() {
    int myReturnValue;
    
    myReturnValue = pthread_mutex_trylock (&_myMutex);
    
    if(myReturnValue == EBUSY) {
        return false;
    }
    
    pthreadCheckReturnValue(myReturnValue, "pthread_mutex_trylock");
    
    return true;
}

bool
ThreadLock::timedlock(long theMicrosecondTimeout) {
    int myReturnValue;
    char *myFunctionName;
    
    const struct timespec myTimeout =
        { (theMicrosecondTimeout / 1000000), (theMicrosecondTimeout % 1000000) * 1000 };
    
    myReturnValue = pthread_mutex_timedlock(&_myMutex, &myTimeout);
    
    if(myReturnValue == ETIMEDOUT) {
        return false;
    }
    
    pthreadCheckReturnValue(myReturnValue, "pthread_mutex_timedwait");
    
    return true;
}

void
ThreadLock::unlock() {
    int myReturnValue;
    
    myReturnValue = pthread_mutex_unlock (&_myMutex);
    
    pthreadCheckReturnValue (myReturnValue, "pthread_mutex_unlock");
}

}
