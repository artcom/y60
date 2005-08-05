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
//    $RCSfile: ThreadSemFactory.h,v $
//
//     $Author: david $
//
//   $Revision: 1.3 $
//
// Description: 
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef ASL_THREAD_SEM_FACTORY_INCLUDED
#define ASL_THREAD_SEM_FACTORY_INCLUDED

#include "ThreadSem.h"

#ifdef OSX
#   include <sys/types.h>
#endif
#include <sys/sem.h>
#include <vector>
#include <set>


namespace asl {
   

/*! \addtogroup aslbase */
/* @{ */
    
// TODO: automatically remove system semaphores
// Attention: remember to call "delete ThreadSemFactory::instance();"
// at program exit! If you just kill a process, the semaphore factory won't
// clean up the systems semaphores: 
// use the commands "ipcs" and "/usr/share/Performer/bin/rmsem"

class ThreadSemFactory {
    public:
        ThreadSemFactory(bool doInstallSignalHandler=true);
        virtual ~ThreadSemFactory();

        static ThreadSemFactory * instance();
        static bool               hasInstance();

        ThreadSem * newLock( int value = 1 );
        void   freeLock( ThreadSem * myLock );

        void setDebug( bool value ){ _debug = value; };
        void printStatistic();

        int get_totalLocks()      { return _usedLocks.size(); };
        int get_recyclableLocks() { return _recyclableLocks.size(); };
        int get_usedLocks()       { return ( _usedLocks.size() + _recyclableLocks.size() ); };

        
        void    debug_setExitOnSignal(bool doExitOnSignal=true) {
            _exitOnSignal = doExitOnSignal;
        }
            
    protected:
    private:

        //This is the sort functor for LockBasket
        struct ltLock {
            bool operator() ( const ThreadSem * Lock1, const ThreadSem * Lock2 ) const {
                if ( Lock1->getSemaphoreSetId() < Lock2->getSemaphoreSetId() ) {
                    return true;
                } else if ( Lock1->getSemaphoreSetId() == Lock2->getSemaphoreSetId()) {
                    if ( Lock1->getSemaphoreSetIndex() < Lock2->getSemaphoreSetIndex() ) {
                        return true;
                    }
                }
                return false;
            }
        };


        typedef std::set<ThreadSem*, ltLock>         LockBasket;
        typedef std::set<ThreadSem*>::iterator       LockBasketIterator;
        
        typedef std::vector<ThreadSem*>              LockList;
        typedef std::vector<ThreadSem*>::iterator    LockListIterator;

        typedef std::vector<int>                     SemaphoreSetList;
        typedef std::vector<int>::iterator           SemaphoreSetListIterator;
        
        void lock();
        void unlock();

        void readKernelSpecs();        

        static ThreadSemFactory * _instance_;

        LockList   _recyclableLocks;
        LockBasket _usedLocks;

        SemaphoreSetList _usedSemaphoreSets;


        int _curSemaphoreSetId;
        int _curSemaphoreSetIndex;

        int _maxSemaphoreSetSize;

        int _myMemoryProtectionId;
        
        bool _debug;
        
        bool            _exitOnSignal;
        
        void            installSignalHandler();
        static void     handleSignal(int signal);
};

/* @} */

}; // end of namespace asl

#endif // ASL_THREAD_SEM_FACTORY_INCLUDED
