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
//   $RCSfile: ThreadSemaphore.h,v $
//
//   $Author: thomas $
//
//   $Revision: 1.8 $
//
//=============================================================================

#ifndef _ac_base_ThreadSemaphore_h_
#define _ac_base_ThreadSemaphore_h_

#include "asl_base_settings.h"

#include <pthread.h>
#include "Exception.h"

namespace asl {
    /**
     * @ingroup aslbase
     *
     * Implements POSIX semaphores. A semaphore is an atomic counter
     * that can be incremented by post and decremented by wait. The
     * semaphore cannot be decremented below 0. In case that it already
     * is 0, a call to wait blocks until the semaphore is incremented
     * from somewhere else.
     */
    class ASL_BASE_DECL ThreadSemaphore {
    public:
        enum {
            WAIT_INFINITE = ~0  ///< Wait forever or until the condition is met
        };
        /**
         * Constructor.
         * @param value initial value of the semaphore. Usually this is 0.
         */
        ThreadSemaphore(int value = 0);
        ~ThreadSemaphore();
        /**
         * Increments the semaphore by theTimes.
         * @exception ThreadSemaphore::Exception the Semaphore had a locking error
         */
        void post(int theTimes = 1);
        /**
         * Tries to decrement the semaphore. If milliseconds ms have passed
         * and the semaphore could not be decremented it abandons the wait
         * and returns false. If milliseconds is set to
         * ThreadSemaphore::WAIT_INFINTE it blocks infinitely (or until the
         * semaphore is posted.)
         *
         * @param milliseconds milliseconds to wait or ThreadSemaphore::WAIT_INFINTE
         *        for an infinite wait.
         * @exception ThreadSemaphore::ClosedException the Semaphore was closed
         *            while waiting.s
         * @exception ThreadSemaphore::Exception the Semaphore had a locking error
         * @return true, if the waiting succeeded. false if a timeout occured.
         */
        bool wait(long milliseconds = WAIT_INFINITE);
        /**
         * @exception ThreadSemaphore::Exception the Semaphore had a locking error
         * @return value of the semaphore
         */
        int getValue();
        /**
         * Closes the semaphore. All threads hanging in a wait get awoken and
         * a ThreadSemaphore::ClosedException is thrown from them.
         * @exception ThreadSemaphore::Exception the Semaphore had a locking error
         */
        void close();
        /**
         * Resets a previously closed semaphore to value.
         * @param value Value to initially set for the semaphore. Defaults to 0.
         * @exception ThreadSemaphore::Exception the Semaphore had a locking error
         */
        void reset(int value = 0);
        DEFINE_EXCEPTION(Exception, asl::Exception);
        DEFINE_EXCEPTION(ClosedException, asl::Exception);
    private:
        pthread_mutex_t	_myMutex;
        pthread_cond_t	_myCondition;
        int			    _myValue;
        bool            _myCreated;
    };
}

#endif // _ac_base_ThreadSemaphore_h_
