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
//    $RCSfile: ThreadSem.h,v $
//
//     $Author: david $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef ASL_THREAD_SEM_INCLUDED
#define ASL_THREAD_SEM_INCLUDED

namespace asl {

/*! \addtogroup aslbase */
/* @{ */
    
// TODO: automatically remove system semaphores
// Attention: remember to call "delete ThreadSemFactory::instance();"
// at program exit! If you just kill a process, the semaphore factory won't
// clean up the systems semaphores: 
// use the commands "ipcs" and "/usr/share/Performer/bin/rmsem"

class ThreadSem {
    public:
        ThreadSem();
        ThreadSem( int SemaphoreSetId, int SemaphoreSetIndex, int value );
        ThreadSem( ThreadSem & otherLock );
        ThreadSem& operator=(ThreadSem & otherLock);
        virtual ~ThreadSem();

        inline int  P() {
            return lock();
        }
        inline int  P2(ThreadSem & other) {
            return lock(other);
        }
        inline int  V() {
            return unlock();
        }
        inline int  V2(ThreadSem & other) {
            return unlock(other);
        }
        
        int lock();
        int nonblock_lock();
        int unlock();
        
        // two wait for two semaphores 
        // ATTENTION: Both semaphores have to belong to the same set!
        // This is not easy to assure...
        int lock(ThreadSem & other);
        int unlock(ThreadSem & other);

        void setValue( int value );

        int getValue() const;
        int getNCount() const;
        int getZCount() const;

        bool is_valid() const;

        int getSemaphoreSetId() const { return _mySemaphoreSetId; };
        int getSemaphoreSetIndex() const { return _mySemaphoreSetIndex; };
        
    protected:
       friend class ThreadSemFactory;
       typedef union Semun {
            int val;                    // value for SETVAL 
            struct semid_ds *buf;       // buffer for IPC_STAT, IPC_SET 
            unsigned short int *array;  // array for GETALL, SETALL 
            struct seminfo *__buf;      // buffer for IPC_INFO 
        };

    private:
       int _mySemaphoreSetId;
       int _mySemaphoreSetIndex;
};

/* @} */

}; // end of namespace asl

#endif // ASL_THREAD_SEM_INCLUDED

