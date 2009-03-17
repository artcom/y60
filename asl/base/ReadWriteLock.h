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
//    $RCSfile: ReadWriteLock.h,v $
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

#ifndef _asl_ReadWriteLock_h_included_
#define _asl_ReadWriteLock_h_included_

#include "asl_base_settings.h"
#include "Exception.h"
#include "Logger.h"
#include <pthread.h>

#define DB3(x) // x

namespace asl {


    /*! \addtogroup aslbase */
    /* @{ */
    
    class ReadWriteLock;

    class ASL_BASE_DECL ScopeLocker {
        public:
            ScopeLocker(ReadWriteLock & theLock, bool theLockWriteFlag=true);
            ~ScopeLocker();
        private:
            ReadWriteLock & _myLock;
            bool _myWriteLockFlag;
    };

    class ASL_BASE_DECL ReadWriteLock {
        public:
            DEFINE_NAMED_EXCEPTION(Exception,ReadWriteLock::Exception,asl::Exception);

#ifndef _SETTING_USE_MUTEX_BASED_RWLOCK_IMPLEMENTATION_
            DEFINE_NAMED_EXCEPTION(LockInitFailed,ReadWriteLock::LockInitFailed,Exception);
            DEFINE_NAMED_EXCEPTION(ReadLockFailed,ReadWriteLock::ReadLockFailed,Exception);
            DEFINE_NAMED_EXCEPTION(ReadUnlockFailed,ReadWriteLock::ReadUnlockFailed,Exception);
            DEFINE_NAMED_EXCEPTION(WriteLockFailed,WriteWriteLock::WriteLockFailed,Exception);
            DEFINE_NAMED_EXCEPTION(WriteUnlockFailed,WriteWriteLock::WriteUnlockFailed,Exception);
#else
            enum magic { RW_LOCK_VALID = 0xfacade };
            DEFINE_NAMED_EXCEPTION(MutexInitFailed,ReadWriteLock::MutexInitFailed,Exception);
            DEFINE_NAMED_EXCEPTION(MutexLockFailed,ReadWriteLock::MutexLockFailed,Exception); 
            DEFINE_NAMED_EXCEPTION(MutexUnlockFailed,ReadWriteLock::MutexUnlockFailed,Exception);
            DEFINE_NAMED_EXCEPTION(CondInitFailed,ReadWriteLock::CondInitFailed,Exception); 
            DEFINE_NAMED_EXCEPTION(CondWaitFailed,ReadWriteLock::CondWaitFailed,Exception); 
            DEFINE_NAMED_EXCEPTION(CondSignalFailed,ReadWriteLock::CondSignalFailed,Exception); 
            DEFINE_NAMED_EXCEPTION(CondBroadcastFailed,ReadWriteLock::CondBroadcastFailed,Exception); 
            DEFINE_NAMED_EXCEPTION(Invalid,ReadWriteLock::Invalid,Exception);
#endif

            ReadWriteLock();
            ~ReadWriteLock();
            void readlock();
            void readunlock();
            void writelock();
            void writeunlock();

#ifndef _SETTING_USE_MUTEX_BASED_RWLOCK_IMPLEMENTATION_
        private:
            pthread_rwlock_t _myLock;
#else
        private:
            static void read_cleanup(void * theLock) ;
            static void write_cleanup(void * theLock);
        private:
            pthread_mutex_t _myMutex;
            pthread_cond_t _readProceed;
            pthread_cond_t _writeProceed;
            int _valid;
            int _activeReaderCount;
            int _hasActiveWriter;
            int _waitingReaderCount;
            int _waitingWriterCount;
#endif
    };

    class PriorityCeilingRWLock : public ReadWriteLock {
    public:
        PriorityCeilingRWLock() : _disabled(false) {
            pthread_key_create(&_normalPriorityKey,0);
            _ceilingPriority.schedulingParm.sched_priority = sched_get_priority_max(SCHED_FIFO);
            _ceilingPriority.schedulingPolicy = SCHED_FIFO;
        DB3(_cycles=get_cycles());        
    }

        void readlock() {
            raisePriority();
DB3(std::cerr<<std::endl<<pthread_self()<<" RR "<< get_cycles()-_cycles<<std::endl;_cycles=get_cycles());            
            ReadWriteLock::readlock();
DB3(std::cerr<<pthread_self()<<" RA "<< get_cycles()-_cycles<<std::endl;_cycles=get_cycles());            
        }
        void readunlock() {
            ReadWriteLock::readunlock(); 
DB3(std::cerr<<pthread_self()<<" RU "<< get_cycles()-_cycles<<std::endl;_cycles=get_cycles());            
            lowerPriority(); 
DB3(std::cerr<<pthread_self()<<" RV "<< get_cycles()-_cycles<<std::endl<<std::endl;_cycles=get_cycles());            
        };
        void writelock() {
            raisePriority();
DB3(std::cerr<<std::endl<<pthread_self()<<" W^ "<< get_cycles()-_cycles<<std::endl;_cycles=get_cycles());            
            ReadWriteLock::writelock();
DB3(std::cerr<<pthread_self()<<" WA "<< get_cycles()-_cycles<<std::endl;_cycles=get_cycles());            
        }
        void writeunlock() {
            ReadWriteLock::writeunlock();
DB3(std::cerr<<pthread_self()<<" WU "<< get_cycles()-_cycles<<std::endl;_cycles=get_cycles());            
            lowerPriority();
DB3(std::cerr<<pthread_self()<<" WV "<< get_cycles()-_cycles<<std::endl<<std::endl;_cycles=get_cycles());            
        }
DB3(cycles_t _cycles);
    private:
        struct Priority {
            struct sched_param schedulingParm;
            int schedulingPolicy;
        };
        void raisePriority() {
            Priority * myPriority = getPriorityStorage();
            getPriority(myPriority);
            setPriority(&_ceilingPriority); 
       }
        void lowerPriority() {
            Priority * myPriority = getPriorityStorage();
            setPriority(myPriority); 
        }

        void setPriority(const Priority * thePriority) {
            if (_disabled) {
                return;
            }
            int status = pthread_setschedparam(pthread_self(),
                                               thePriority->schedulingPolicy,
                                               &thePriority->schedulingParm);
            if (status != 0) {
                AC_WARNING << "PriorityCeilingRWLock: can't set priority, disabling priority ceiling" << std::endl;
                _disabled = true;
            }
         }
        void getPriority(Priority * thePriority) {
            if (_disabled) {
                return;
            }
            int status = pthread_getschedparam(pthread_self(),
                                               &thePriority->schedulingPolicy,
                                               &thePriority->schedulingParm);
            if (status != 0) {
                AC_ERROR << "PriorityCeilingRWLock: can't get priority, disabling priority ceiling" << std::endl;
                _disabled = true;
            }
          }


        Priority * getPriorityStorage() {
            Priority * myPriority = reinterpret_cast<Priority *>(pthread_getspecific(_normalPriorityKey));
            if (myPriority == 0) {
                myPriority = new Priority;
                int status = pthread_setspecific(_normalPriorityKey,reinterpret_cast<void*>(myPriority));
                (void)status;
            }
            return myPriority;
        }
        
        Priority _ceilingPriority;
        pthread_key_t _normalPriorityKey;
        bool    _disabled;
    };
} // asl

/* @} */


#endif 
