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
//    $RCSfile: ThreadLock.h,v $
//
//     $Author: thomas $
//
//   $Revision: 1.7 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef INCL_THREADLOCK
#define INCL_THREADLOCK

#include <pthread.h>


namespace asl {
/**
 * @ingroup aslbase
 * Threadlock is a wrapper around a mutex. It can be used
 * to protect resources that are shared over multiple threads.
 * 
 * 
 */
class ThreadLock {
    public:
        ThreadLock();
        virtual ~ThreadLock();

        /**
         * Locks (acquires a Mutex) for the Thread until the 
         * ThreadLock is unlocked.
         * @warn the internal pthread_mutex has deadlock
         *       prevention enabled, hence a thread can not
         *       lock itself. Once it owns a ThreadLock, future
         *       calls to lock are ignored.
         */
        void lock();

        /**
         * tries to lock (acquire) the ThreadLock.
         * @return 0 if locked, EBUSY if it could not be locked
         */
        int nonblock_lock();
        /**
         * Unlocks (releases) the ThreadLock.
         */
        void unlock();


    private:
        ThreadLock (ThreadLock & otherLock);
        ThreadLock& operator= (ThreadLock & otherLock);

        pthread_mutex_t _myMutex;
};
}

#endif
