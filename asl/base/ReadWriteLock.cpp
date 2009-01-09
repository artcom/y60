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
//    $RCSfile: ReadWriteLock.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.5 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

// own header
#include "ReadWriteLock.h"

#include <stdio.h>
#include <errno.h>
#include <cstring>
#include <iostream>

using namespace std;  // automatically added!

namespace asl {

ScopeLocker::ScopeLocker(ReadWriteLock & theLock, bool theLockWriteFlag) :
    _myWriteLockFlag(theLockWriteFlag), _myLock(theLock) 
    {
        if (_myWriteLockFlag) {
            _myLock.writelock();
        } else {
            _myLock.readlock();
        }
    }
ScopeLocker:: ~ScopeLocker() {
    if (_myWriteLockFlag) {
        _myLock.writeunlock();
    } else {
        _myLock.readunlock();
    }
}


#ifndef _SETTING_USE_MUTEX_BASED_RWLOCK_IMPLEMENTATION_
ReadWriteLock::ReadWriteLock()
{
    pthread_rwlockattr_t myAttributes;
    pthread_rwlockattr_init(&myAttributes);
    pthread_rwlockattr_setkind_np(&myAttributes,PTHREAD_RWLOCK_PREFER_READER_NP);
    int myStatus = pthread_rwlock_init(&_myLock,&myAttributes);
    if (myStatus != 0) {
        throw LockInitFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
}
ReadWriteLock::~ReadWriteLock() {
    int myStatus = pthread_rwlock_destroy(&_myLock);
    if (myStatus != 0) {
        AC_ERROR << "~ReadWriteLock: can't destroy rwlock:" << strerror(myStatus) << endl;
    }
}
void
ReadWriteLock::readlock() {
    int myStatus = pthread_rwlock_rdlock(&_myLock);
    if (myStatus != 0) {
        throw ReadLockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
}
void
ReadWriteLock::readunlock() {
    int myStatus = pthread_rwlock_unlock(&_myLock);
    if (myStatus != 0) {
        throw ReadUnlockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
}
void
ReadWriteLock::writelock() {
    int myStatus = pthread_rwlock_wrlock(&_myLock);
    if (myStatus != 0) {
        throw WriteLockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
}
void
ReadWriteLock::writeunlock() {
    int myStatus = pthread_rwlock_unlock(&_myLock);
    if (myStatus != 0) {
        throw WriteUnlockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
}
#else

ReadWriteLock::ReadWriteLock()
:   _activeReaderCount(0),
    _waitingReaderCount(0),
    _waitingWriterCount(0),
    _hasActiveWriter(0)
{
    int myStatus = pthread_mutex_init(&_myMutex,0);
    if (myStatus != 0) {
        throw MutexInitFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    myStatus = pthread_cond_init(&_readProceed,0);
    if (myStatus != 0) {
        pthread_mutex_destroy(&_myMutex);
        throw CondInitFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    myStatus = pthread_cond_init(&_writeProceed,0);
    if (myStatus != 0) {
        pthread_cond_destroy(&_readProceed);
        pthread_mutex_destroy(&_myMutex);
        throw CondInitFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    _valid = RW_LOCK_VALID;
}

ReadWriteLock::~ReadWriteLock() {
    if (_valid != RW_LOCK_VALID) return;
    int myStatus = pthread_mutex_lock(&_myMutex);
    if (myStatus) {
        AC_ERROR << "~ReadWriteLock: can't lock:" << strerror(myStatus) << endl;
    }
    if (_activeReaderCount > 0 || _hasActiveWriter > 0) {
        pthread_mutex_unlock(&_myMutex);
        AC_ERROR << "~ReadWriteLock: busy - active readers or writers" << endl;
    }
    if (_waitingReaderCount > 0 || _waitingWriterCount > 0) {
        pthread_mutex_unlock(&_myMutex);
        AC_ERROR << "~ReadWriteLock: threads are still waiting" << endl;
    }
    _valid = 0;
    pthread_mutex_unlock(&_myMutex);
    myStatus = pthread_mutex_destroy(&_myMutex);
    if (myStatus) {
        AC_ERROR << "~ReadWriteLock: can't destroy mutex:"
            << strerror(myStatus) << endl;
    }
    myStatus = pthread_cond_destroy(&_readProceed);
    if (myStatus) {
        AC_ERROR << "~ReadWriteLock: can't destroy read cv:"
            << strerror(myStatus) << endl;
    }
    myStatus = pthread_cond_destroy(&_writeProceed);
    if (myStatus) {
        AC_ERROR << "~ReadWriteLock: can't destroy write cv:"
            << strerror(myStatus) << endl;
    }
}
// lock for read access
void
ReadWriteLock::readlock() {
    if (_valid != RW_LOCK_VALID) throw Invalid(JUST_FILE_LINE);
    int myStatus = pthread_mutex_lock(&_myMutex);
    if (myStatus != 0) {
        throw MutexLockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    if (_hasActiveWriter) {
        _waitingReaderCount++;
        pthread_cleanup_push(read_cleanup, this);
        while (_hasActiveWriter) {
            myStatus = pthread_cond_wait(&_readProceed,&_myMutex);
            if (myStatus!=0) {
                break;
            }
        }
        pthread_cleanup_pop(0);
        _waitingReaderCount--;
    }
    if (myStatus == 0) {
        _activeReaderCount++;
    } else {
        throw CondWaitFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    myStatus = pthread_mutex_unlock(&_myMutex);
    if (myStatus != 0) {
        throw MutexUnlockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
}
void
ReadWriteLock::readunlock() {
    if (_valid != RW_LOCK_VALID) throw Invalid(JUST_FILE_LINE);
    int myStatus = pthread_mutex_lock(&_myMutex);
    if (myStatus != 0) {
        throw MutexLockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    _activeReaderCount--;
    if (_activeReaderCount == 0 && _waitingWriterCount > 0) {
        myStatus = pthread_cond_signal(&_writeProceed);
    }
    int myStatus2 = pthread_mutex_unlock(&_myMutex);
    if (myStatus != 0) {
        throw CondSignalFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    if (myStatus2 != 0) {
        throw MutexUnlockFailed(strerror(myStatus2),PLUS_FILE_LINE);
    }
}
void
ReadWriteLock::writelock() {
    if (_valid != RW_LOCK_VALID) throw Invalid(JUST_FILE_LINE);
    int myStatus = pthread_mutex_lock(&_myMutex);
    if (myStatus != 0) {
        throw MutexLockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    if (_hasActiveWriter || _activeReaderCount > 0) {
        _waitingWriterCount++;
        pthread_cleanup_push(write_cleanup,(void*)this);
        while (_hasActiveWriter || _activeReaderCount > 0) {
            myStatus = pthread_cond_wait(&_writeProceed,&_myMutex);
            if (myStatus != 0) {
                break;
            }
        }
        pthread_cleanup_pop(0);
        _waitingWriterCount--;
    }
    if (myStatus == 0) {
        _hasActiveWriter = 1;
    } else {
        pthread_mutex_unlock(&_myMutex);
        throw CondWaitFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    pthread_mutex_unlock(&_myMutex);
}
void
ReadWriteLock::writeunlock() {
    if (_valid != RW_LOCK_VALID) throw Invalid(JUST_FILE_LINE);
    int myStatus = pthread_mutex_lock(&_myMutex);
    if (myStatus != 0) {
        throw MutexLockFailed(strerror(myStatus),PLUS_FILE_LINE);
    }
    _hasActiveWriter = 0;
    if (_waitingReaderCount > 0) {
        myStatus = pthread_cond_broadcast(&_readProceed);
        if (myStatus != 0) {
            pthread_mutex_unlock(&_myMutex);
            throw CondBroadcastFailed(strerror(myStatus),PLUS_FILE_LINE);
        }
    } else if (_waitingWriterCount > 0) {
        myStatus = pthread_cond_signal(&_writeProceed);
        if (myStatus != 0) {
            pthread_mutex_unlock(&_myMutex);
            throw CondSignalFailed(strerror(myStatus),PLUS_FILE_LINE);
        }
    }
    pthread_mutex_unlock(&_myMutex);
}

void
ReadWriteLock::read_cleanup(void * theLock) {
    ReadWriteLock * myLock = static_cast<ReadWriteLock*>(theLock);
    myLock->_waitingReaderCount--;
    pthread_mutex_unlock(&myLock->_myMutex);
}

void
ReadWriteLock::write_cleanup(void * theLock) {
    ReadWriteLock * myLock = static_cast<ReadWriteLock*>(theLock);
    myLock->_waitingWriterCount--;
    pthread_mutex_unlock(&myLock->_myMutex);
}
#endif
}
