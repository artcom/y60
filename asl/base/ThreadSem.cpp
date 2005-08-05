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
//    $RCSfile: ThreadSem.cpp,v $
//
//     $Author: uzadow $
//
//   $Revision: 1.1.1.1 $
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "ThreadSem.h"

#include <iostream>
#include <sys/sem.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


using namespace std;  // automatically added!



using namespace asl;

ThreadSem::ThreadSem():
    _mySemaphoreSetId( 0 ),
    _mySemaphoreSetIndex( 0 )
{

}

ThreadSem::ThreadSem( int SemaphoreSetId, int SemaphoreSetIndex, int value ): 
    _mySemaphoreSetId( SemaphoreSetId ),
    _mySemaphoreSetIndex( SemaphoreSetIndex )
{
    Semun semarg;
    semarg.val = value;
    if ( semctl( _mySemaphoreSetId, _mySemaphoreSetIndex, SETVAL, semarg ) == -1) {
        cerr << "ThreadSem::ThreadSem(): Failed to set value on semaphore: "
             << strerror( errno ) << endl;
        exit( 1 );
    }

}

ThreadSem::ThreadSem(ThreadSem & otherLock ) {
    if ( this != & otherLock ) {
        _mySemaphoreSetId = otherLock.getSemaphoreSetId();
        _mySemaphoreSetIndex = otherLock.getSemaphoreSetIndex();
    }
}

ThreadSem & ThreadSem::operator=(ThreadSem & otherLock) {
    if ( this != & otherLock ) {
        _mySemaphoreSetId = otherLock.getSemaphoreSetId();
        _mySemaphoreSetIndex = otherLock.getSemaphoreSetIndex();
    }
    return *this;
}


ThreadSem::~ThreadSem() {

}


int
ThreadSem::lock() {
    struct sembuf semop_P[1] = { _mySemaphoreSetIndex, -1, SEM_UNDO };
    if ( semop( _mySemaphoreSetId, semop_P, 1) == -1 ) {
        cerr << "ERROR: lock (P) on semaphore failed: " << strerror( errno ) << endl;
        cerr << "getValue() = " << getValue() << endl;
        cerr << "getNCount() = " << getNCount() << endl;
        cerr << "getZCount() = " << getZCount() << endl;
        return -1;
    }
    return 0;
}


int
ThreadSem::unlock() {
    struct sembuf semop_V[1] = { _mySemaphoreSetIndex, 1, SEM_UNDO };
    if ( semop( _mySemaphoreSetId, semop_V, 1) == -1 ) {
        cerr << "ERROR: unlock (V) on semaphore failed: " << strerror( errno ) << endl;
        cerr << "getValue() = " << getValue() << endl;
        cerr << "getNCount() = " << getNCount() << endl;
        cerr << "getZCount() = " << getZCount() << endl;
        return -1;
    }
    return 0;
}

// two wait for two semaphores
int
ThreadSem::lock(ThreadSem & other) {
    
    assert(_mySemaphoreSetId == other._mySemaphoreSetId);
    
    struct sembuf semop_P[2] = { {_mySemaphoreSetIndex, -1, SEM_UNDO },
                                 {other._mySemaphoreSetIndex, -1, SEM_UNDO }};

    if ( semop( _mySemaphoreSetId, semop_P, 2) == -1 ) {
        cerr << "ERROR: lock (P) on two semaphores failed: " << strerror( errno ) << endl;
        cerr << "getValue() = " << getValue() << endl;
        cerr << "getNCount() = " << getNCount() << endl;
        cerr << "getZCount() = " << getZCount() << endl;
        cerr << "other.getValue() = " << other.getValue() << endl;
        cerr << "other.getNCount() = " << other.getNCount() << endl;
        cerr << "other.getZCount() = " << other.getZCount() << endl;
        return -1;
    }
    return 0;
}


int
ThreadSem::unlock(ThreadSem & other) {
    
    assert(_mySemaphoreSetId == other._mySemaphoreSetId);
    
    struct sembuf semop_V[2] = { {_mySemaphoreSetIndex, -1, SEM_UNDO },
                                 {other._mySemaphoreSetIndex, -1, SEM_UNDO }};

    if ( semop( _mySemaphoreSetId, semop_V, 2) == -1 ) {
        cerr << "ERROR: unlock (V) on two semaphores failed: " << strerror( errno ) << endl;
        cerr << "getValue() = " << getValue() << endl;
        cerr << "getNCount() = " << getNCount() << endl;
        cerr << "getZCount() = " << getZCount() << endl;
        cerr << "other.getValue() = " << other.getValue() << endl;
        cerr << "other.getNCount() = " << other.getNCount() << endl;
        cerr << "other.getZCount() = " << other.getZCount() << endl;
        return -1;
    }
    return 0;
}

int
ThreadSem::nonblock_lock() {
    struct sembuf semop_CP[1] = { _mySemaphoreSetIndex, -1, SEM_UNDO|IPC_NOWAIT };
     // This is _not_ really threadsafe, because errno isn't

    errno = 0;
    if ( semop( _mySemaphoreSetId, semop_CP, 1 ) == -1) {
        if ( errno == EAGAIN ) {
            return 0;    // not acquired
        } else {
            cerr << "ERROR: Nonblocking lock on semaphore failed: " 
                 << strerror( errno ) << endl;
            return -1;   // error
        }
    } else {
        return 1;        // acquired
    }
}


void
ThreadSem::setValue( int value ) {

    Semun semarg;
    semarg.val = value;
    if ( semctl( _mySemaphoreSetId, _mySemaphoreSetIndex, SETVAL, semarg ) == -1) {
        cerr << "ThreadSem::setValue(): Failed to set value on semaphore: "
             << strerror( errno ) << endl;
        exit( 1 );
    }
}


int
ThreadSem::getValue() const {
    int result = semctl( _mySemaphoreSetId, _mySemaphoreSetIndex, GETVAL, 0 );
    if ( result == -1) {
        cerr << "ThreadSem::getValue(): Failed to get value from semaphore: "
             << strerror( errno ) << endl;
    }
    return result;
}

int
ThreadSem::getNCount() const {
    int result = semctl( _mySemaphoreSetId, _mySemaphoreSetIndex, GETNCNT, 0 );
    if ( result == -1) {
        cerr << "ThreadSem::getValue(): Failed to get semncnt from semaphore: "
             << strerror( errno ) << endl;
    }
    return result;
}

int
ThreadSem::getZCount() const {
    int result = semctl( _mySemaphoreSetId, _mySemaphoreSetIndex, GETZCNT, 0 );
    if ( result == -1) {
        cerr << "ThreadSem::getValue(): Failed to get semncnt from semaphore: "
             << strerror( errno ) << endl;
    }
    return result;
}

bool
ThreadSem::is_valid()const {
    if ( _mySemaphoreSetId > 0 ) {
        return true;
    } else {
        return false;
    }
}

//=============================================================================
//
// $Log: ThreadSem.cpp,v $
// Revision 1.1.1.1  2003/05/12 14:20:23  uzadow
// no message
//
// Revision 1.2  2002/09/26 16:34:12  wolfger
// removed some nasty warnings.
//
// Revision 1.1.1.1  2002/09/17 15:37:04  wolfger
// initial checkin
//
// Revision 1.2  2002/09/06 18:13:28  valentin
// merged linuxport_2nd_try branch into trunk
// -branch tag before merge : branch_before_merge_final
// -trunk tag before merge  : trunk_before_merge
//
//
// -Merge conflicts:
//  image/glutShowImage/Makefile
//  image/glutShowImage/glShowImage.C
//  loader/libPfIv/pfiv.c++
//  lso/script/vrfm/Intercom.h
//  tools/Pfconvert/pfconvert.c
//  vrap/libMover/HMD.c++
//
//  - compiled && testrun
//  - commit
//  - merged trunk tag : trunk_after_merge
//
// Revision 1.1.2.4  2002/08/30 16:25:28  martin
// more std-ization
//
// Revision 1.1.2.3  2002/02/14 12:56:27  stefan
// - added a kill method to PosixThread
// - fixed some problems with ThreadFifo and its test
// - added setStopSignal() to ThreadFifo, which allows controlled
//   stopping og threads that wait on the FIFO
//
// Revision 1.1.2.2  2002/02/13 16:36:14  stefan
// - added another constructor to PosixThread(), now allows to pass a working
//   function, so you don't have to subclass PosixThread to get a thread
// - implemented some missing methods in PosixThread
//
// - added class ThreadFifo: FIFOs for threads with "real" waiting: pop_waiting()
//   now actually waits on a semaphore until the FIFO in non-empty. added the
//   tests
//
// Revision 1.1.2.1  2002/02/07 09:28:02  stefan
// - semaphores for threads. unlike the MpLock/MpLockFactory classes these
//   sempahore classes do not rely upon Performer, and are not suitable
//   for multi-processing
//
//
//=============================================================================
