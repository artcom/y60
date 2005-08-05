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

#include "ThreadLock.h"
#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>


using namespace std;  // automatically added!
namespace asl {
void checkRetVal (int theRetVal, char * theFunc) {
    if (theRetVal != 0) {
        static bool myIsInCheckRetVal = false;
        if (myIsInCheckRetVal) {
            // This happens if we're being called recursively and probably means that 
            // AC_ERROR itself isn't working, so we make a feeble attempt to output
            // stuff anyway.
            cerr << "function="<<theFunc<<", error="<<strerror(theRetVal) << ", retVal= " << theRetVal << endl;
        } else {
            myIsInCheckRetVal = true;
            AC_ERROR << "function="<<theFunc<<", error="<<strerror(theRetVal) << ", retVal= " << theRetVal << endl;
            // Rumms. Besser, wir machten hier was sinnvolles.
            exit (-1);
        }
    }
}

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
#ifdef WIN32
    pthread_mutex_destroy(&_myMutex);
#endif
}

void
ThreadLock::lock() {
    int myRetVal;
    myRetVal = pthread_mutex_lock (&_myMutex);
    checkRetVal (myRetVal, "pthread_mutex_lock");
}

int
ThreadLock::nonblock_lock() {
    int myRetVal;
    myRetVal = pthread_mutex_trylock (&_myMutex);
    if (myRetVal != EBUSY && myRetVal != 0) {
        checkRetVal (myRetVal, "pthread_mutex_trylock");
    }
    return myRetVal;
}

void
ThreadLock::unlock() {
    int myRetVal;
    // This causes an 'interrupted system call' error if the mutex isn't locked
    // before the call.
    myRetVal = pthread_mutex_unlock (&_myMutex);
    checkRetVal (myRetVal, "pthread_mutex_unlock");
}
}

