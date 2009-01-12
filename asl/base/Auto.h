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
// Description:  Automatic Resource Management Functions 
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor
//    test coverage          : poor
//    names                  : fair
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : ok
//    readability            : ok
//    understandabilty       : fair
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendation: 
//       - improve documentation
//       - improve test coverage
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_Auto_
#define _included_asl_Auto_

#include "asl_base_settings.h"

namespace asl {
/*! \addtogroup aslbase */
/* @{ */


    struct Lockable {
        virtual void lock() = 0;
        virtual void unlock() = 0;
    };

#ifdef _NEEDS_TO_BE_TESTED_BEFORE_USE_
    /** 
     * abstract base class that helps to implements a multi-level lock for a resource so that the
     * the number of lock() and unlock() calls can be balanced; you need to implement the
     * lockOnce() and unlockOnce() function, and this class makes sure that they will be called only once
     */
    struct MultiLockable : public Lockable {
        MultiLockable() : _myLockCount(0) {}
         void lock() {
            if (_myLockCount == 0) {
                lockOnce();
            }
            ++_myLockCount;
        }
        void unlock() {
            --_myLockCount;
            if (_myLockCount == 0) {
                unlockOnce();
            }
        }
        virtual int getLockCount() const {
            return _myLockCount;
        }
    protected:
        // implement these Fuctions
        virtual void lockOnce() = 0;
        virtual void unlockOnce() = 0;

        virtual void setLockCount(int theCount) {
            _myLockCount = theCount;
        }
    private:
        int _myLockCount;
    };
#endif


    struct ReadLockable {
        virtual void readlock()=0;
        virtual void readunlock()=0;
    };
    struct WriteLockable {
        virtual void writelock()=0;
        virtual void writeunlock()=0;
    };

    struct ReadWriteLockable : public ReadLockable, public WriteLockable {
    };


    template <class T>
        class AutoLocker {
            public:
                AutoLocker(T & theLock) : _myLock(theLock) {
                    _myLock.lock();
                }

                ~AutoLocker() {
                    _myLock.unlock();
                }

            private:
                T &  _myLock;
                AutoLocker(const AutoLocker&);
                AutoLocker& operator=(const AutoLocker&);
        };

    template <class T>
        class AutoReadLocker {
            public:
                AutoReadLocker(T & theLock) : _myLock(theLock) {
                    _myLock.readlock();
                }

                ~AutoReadLocker() {
                    _myLock.readunlock();
                }

            private:
                T &  _myLock;
                AutoReadLocker(const AutoReadLocker&);
                AutoReadLocker& operator=(const AutoReadLocker&);
        };

    template <class T>
        class AutoWriteLocker {
            public:
                AutoWriteLocker(T & theLock) : _myLock(theLock) {
                    _myLock.writelock();
                }

                ~AutoWriteLocker() {
                    _myLock.writeunlock();
                }

            private:
                T &  _myLock;
                AutoWriteLocker(const AutoWriteLocker&);
                AutoWriteLocker& operator=(const AutoWriteLocker&);
        };
/* @} */

}



#endif

