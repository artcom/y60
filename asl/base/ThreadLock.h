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

#include "asl_base_settings.h"

#include <pthread.h>


namespace asl {
/**
 * @ingroup aslbase
 * Threadlock is a wrapper around a mutex. It can be used
 * to protect resources that are shared over multiple threads.
 *
 *
 */
class ASL_BASE_DECL ThreadLock {
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
