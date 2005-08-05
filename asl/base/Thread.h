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
//    $RCSfile: Thread.h,v $
//
//     $Author: david $
//
//   $Revision: 1.2 $
//
// Description: 
//
//
//=============================================================================

#ifndef INCL_THREAD
#define INCL_THREAD

#ifdef LINUX
#include <pthread.h>
#include <signal.h>
#else
#include <windows.h>
#endif

namespace asl {


/*! \addtogroup aslbase */
/* @{ */
    
// Note on thread priorities: Priorities are completely different in win32 and
// linux. The interface attempts to provide the nessesary priorities for the
// tasks we're likely to see. _myPriorityClass is either REALTIME or NORMAL.
// _myPriority is normalized so it ranges from 0(lowest) to 1(highest). These 
// values are translated as follows:
//
//    _myPriorityClass    _myPriority   Linux              Windows
//    REALTIME            0             SCHED_FIFO, 20     THREAD_PRIORITY_TIME_CRITICAL
//    REALTIME            1             SCHED_FIFO, -20    THREAD_PRIORITY_TIME_CRITICAL
//    NORMAL              0             SCHED_OTHER, 20    THREAD_PRIORITY_IDLE
//    NORMAL              0.2           SCHED_OTHER, 12    THREAD_PRIORITY_LOWEST
//    NORMAL              1             SCHED_OTHER, -20   THREAD_PRIORITY_HIGHEST
//
// In the event that more fine-grained control is nessesary, plattform-specific
// functions can be added to set priorities.
//
// Note that windows thread priorities are affected by the process priority as 
// well.

class Thread {
    public:
        typedef void (WorkFunc)(Thread &);
        enum PRIORITY_CLASS {NORMAL, REALTIME};
#ifdef LINUX
        typedef pthread_t THREAD_HANDLE;
#else
        typedef HANDLE THREAD_HANDLE;
#endif

        Thread(PRIORITY_CLASS myPriorityClass=NORMAL, double myPriority=0.5);
        Thread(WorkFunc * workFunc, void * workArgs = 0, 
               PRIORITY_CLASS myPriorityClass=NORMAL, double myPriority=0.5);
        Thread();

        virtual ~Thread();

        bool    setPriority (PRIORITY_CLASS myPriorityClass=NORMAL, double myPriority=0.5);
        bool    setRealtimePriority(double myPriority=0.5);
        bool    setDefaultPriority();

        void    fork ();
        void    join ();
        bool    isActive();
        void    yield();
//        bool    kill(int signalNumber=SIGKILL);

        void * getWorkArgs() {
            return _myWorkArgs;
        }

        THREAD_HANDLE getThreadID() const {
            return _myThread;
        }

        bool updatePriority();
    private:
        Thread (const Thread & otherThread);
        Thread& operator= (const Thread & otherThread);

        virtual void run();

        THREAD_HANDLE  _myThread;
        
        PRIORITY_CLASS _myPriorityClass;
        double         _myPriority;
        bool           _myIsActive;
        WorkFunc *     _myWorkFunc;
        void *         _myWorkArgs;
        
#ifdef LINUX
        friend void * threadFunc (void * This);
#else
        friend DWORD threadFunc( LPDWORD *This );
#endif        

};

/* @} */

}; // end of namespace asl

#endif 
