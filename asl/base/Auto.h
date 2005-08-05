/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Auto.h,v $
//
//   $Revision: 1.3 $
//
// Description:  Automatic Resource Management Functions 
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_Auto_
#define _included_asl_Auto_



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
        };
/* @} */

}



#endif

