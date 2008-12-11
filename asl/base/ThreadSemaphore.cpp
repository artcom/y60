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

#include "ThreadSemaphore.h"

#include "Logger.h"

#include <errno.h>

namespace asl {
    ThreadSemaphore::ThreadSemaphore(int value) : _myCreated(false) {
        int myReturnCode;

        myReturnCode = pthread_mutex_init(&_myMutex, NULL);
        if (0 != myReturnCode) {
            throw ThreadSemaphore::Exception("Mutex initialization failed.", PLUS_FILE_LINE);
        }
        myReturnCode = pthread_cond_init(&_myCondition, NULL);
        if (0 != myReturnCode) {
            pthread_mutex_destroy(&_myMutex);
            throw ThreadSemaphore::Exception("Condition initialization failed.", PLUS_FILE_LINE);
        }
        _myValue = value;
        _myCreated = true;
    }

    ThreadSemaphore::~ThreadSemaphore() {
        pthread_cond_destroy(&_myCondition);
        pthread_mutex_destroy(&_myMutex);
    }

    void ThreadSemaphore::post(int theTimes) {
        int myReturnCode;
        AC_TRACE << "post::lock";
        myReturnCode = pthread_mutex_lock(&_myMutex);
        if (0 != myReturnCode)
        {
            throw ThreadSemaphore::Exception("Mutex locking failed in post", PLUS_FILE_LINE);
        }
        if (!_myCreated) {
            throw ThreadSemaphore::ClosedException("", PLUS_FILE_LINE);
        }
        _myValue += theTimes;
        myReturnCode = pthread_mutex_unlock(&_myMutex);
        AC_TRACE << "post::unlocked";
        if (0 != myReturnCode) {
            throw ThreadSemaphore::Exception("Unlock Mutex while Signalling failed in post", PLUS_FILE_LINE);
        }
        for (int i = 0; i < theTimes; ++i) {
            myReturnCode = pthread_cond_signal(&_myCondition);
            AC_TRACE << "post::singnalled";
            if (0 != myReturnCode) {
                throw ThreadSemaphore::Exception("Signalling failed in post", PLUS_FILE_LINE);
            }
        }
    }

    bool ThreadSemaphore::wait(long milliseconds) {
        int myReturnCode;

        AC_TRACE << "wait::lock";
        myReturnCode = pthread_mutex_lock(&_myMutex);
        if (0 != myReturnCode)
        {
            throw ThreadSemaphore::Exception("Mutex locking failed in wait", PLUS_FILE_LINE);
        }
        if (!_myCreated) {
            pthread_mutex_unlock(&_myMutex);
            throw ThreadSemaphore::ClosedException("", PLUS_FILE_LINE);
        }
        if (ThreadSemaphore::WAIT_INFINITE == milliseconds) {
            while (_myValue <= 0)
            {
                AC_TRACE << "wait::cond_wait";
                myReturnCode = pthread_cond_wait(&_myCondition, &_myMutex);
                if (myReturnCode && errno != EINTR) {
                    break;
                } else {
                    if (!_myCreated) {
                        // XXX Do we have to unlock the Mutex here?
                        pthread_mutex_unlock(&_myMutex);
                        throw ThreadSemaphore::ClosedException("", PLUS_FILE_LINE);
                    }
                }
            }
        } else {
            myReturnCode = 0;
            // Convert to timespec
            Time myTime;
            timespec myTimeSpec = timespec(myTime);
            
            myTimeSpec.tv_sec += milliseconds/1000;
            myTimeSpec.tv_nsec += (milliseconds%1000) * 1000;
            
            while (_myValue <= 0)
            {
                AC_TRACE << "wait::cond_timedwait";
                myReturnCode = pthread_cond_timedwait(&_myCondition, &_myMutex, &myTimeSpec);
                if (myReturnCode && (errno != EINTR)) {
                    break;
                } else {
                    if (!_myCreated) {
                        // XXX Do we have to unlock the Mutex here?
                        pthread_mutex_unlock(&_myMutex);
                        throw ThreadSemaphore::ClosedException("", PLUS_FILE_LINE);
                    }
                }
            }
            if (0 != myReturnCode) 
            {
                if (0 != pthread_mutex_unlock(&_myMutex)) {
                    throw ThreadSemaphore::Exception("Unlock Mutex failed in wait", PLUS_FILE_LINE);
                }                
                if (ETIMEDOUT == myReturnCode) {
                    AC_TRACE << "wait::unlocked on timeout";
                    // We have a timeout
                    return false;
                } else {
                    AC_TRACE << "wait::unlocked on error";
                    // We have an error
                    throw ThreadSemaphore::Exception("Timedwait failed in wait", PLUS_FILE_LINE);
                }
            } 
        }
        if (!_myCreated) {
            pthread_mutex_unlock(&_myMutex);
            throw ThreadSemaphore::ClosedException("", PLUS_FILE_LINE);
        }
        --_myValue;
        AC_TRACE << "wait::unlock";
        myReturnCode = pthread_mutex_unlock(&_myMutex);
        AC_TRACE << "wait unlocked";
        if (0 != myReturnCode) {
            throw ThreadSemaphore::Exception("Unlock Mutex failed in wait", PLUS_FILE_LINE);
        }
        return true;
    }

    int ThreadSemaphore::getValue() {
        AC_TRACE << "getValue::lock";
        if (0 != pthread_mutex_lock(&_myMutex))
        {
            throw ThreadSemaphore::Exception("Mutex locking failed in getValue", PLUS_FILE_LINE);
        }
        int myTempValue = _myValue;
        if (0 != pthread_mutex_unlock(&_myMutex)) {
            throw ThreadSemaphore::Exception("Unlock Mutex failed in getValue", PLUS_FILE_LINE);
        }
        AC_TRACE << "getValue::unlock";
        return myTempValue;
    }

    void ThreadSemaphore::close() {
        AC_TRACE << "close::lock";
        if (0 != pthread_mutex_lock(&_myMutex)) {
            throw ThreadSemaphore::Exception("Mutex locking failed in close", PLUS_FILE_LINE);
        }
        _myCreated = false;
        _myValue = 9999;
        pthread_cond_signal(&_myCondition);
        if (0 != pthread_mutex_unlock(&_myMutex)) {
            throw ThreadSemaphore::Exception("Unlock Mutex failed in close", PLUS_FILE_LINE);
        }
        AC_TRACE << "close::unlocked";
    }

    void ThreadSemaphore::reset(int value) {
        AC_TRACE << "reset::lock";
        if (0 != pthread_mutex_lock(&_myMutex))
        {
            throw ThreadSemaphore::Exception("Mutex locking failed in reset", PLUS_FILE_LINE);
        }
        _myCreated = true;
        _myValue = value;
        AC_TRACE << "reset::unlock";
        if (0 != pthread_mutex_unlock(&_myMutex)) {
            throw ThreadSemaphore::Exception("Unlock Mutex failed in reset", PLUS_FILE_LINE);
        }
        AC_TRACE << "reset::unlocked";
    }
}
