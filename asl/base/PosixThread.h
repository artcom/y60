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

#include <pthread.h>
#include <signal.h>


namespace asl {
/*! \addtogroup aslbase */
/* @{ */

class PosixThread {
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
#ifndef WIN32
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
