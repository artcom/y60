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
//    $RCSfile: PosixThread.h,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
// Description:
//
//
//=============================================================================

#ifndef INCL_POSIXTHREAD
#define INCL_POSIXTHREAD

#include "asl_base_settings.h"

#include <pthread.h>
#include <signal.h>


namespace asl {
/*! \addtogroup aslbase */
/* @{ */

class ASL_BASE_DECL PosixThread {
    public:
        typedef void (WorkFunc)(PosixThread &);

        PosixThread(int mySchedPolicy, int myPriority);
        PosixThread(WorkFunc * workFunc, void * workArgs = 0,
                    int mySchedPolicy=SCHED_OTHER, int myPriority=0);
        PosixThread();

        virtual ~PosixThread();

        bool    setPriority(int mySchedPolicy, int myPriority);
        bool    setRealtimePriority(int modifier=0);
        bool    setDefaultPriority();

        void    fork();
        void    join();
        bool    isActive();
        void    yield();
#ifndef _WIN32
        bool    kill(int signalNumber=SIGKILL);
#endif

        static int getMaxPriority (int theSchedPolicy);
        static int getMinPriority (int theSchedPolicy);

        void * getWorkArgs() {
            return _myWorkArgs;
        }

        pthread_t   getThreadID() const {
            return _myThread;
        }

        bool updatePriority();
        bool shouldTerminate();

    private:
        PosixThread (const PosixThread & otherThread);
        PosixThread& operator= (const PosixThread & otherThread);

        virtual void run();

        pthread_t   _myThread;
        pthread_t   _myCreator;
        int         _mySchedPolicy;
        int         _myPriority;
        bool        _myIsActive;
        WorkFunc *  _myWorkFunc;
        void *      _myWorkArgs;
        bool        _myShouldTerminate; // if this is true, join() has been called.

        friend void * threadFunc (void * This);

};

/* @} */

}

#endif
