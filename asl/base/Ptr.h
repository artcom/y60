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
//    $RCSfile: Ptr.h,v $
//
//   $Revision: 1.20 $
//
// Description: reference counting smart pointer
//              based on shared_ptr from www.boost.org
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_Ptr_
#define _included_asl_Ptr_

#include "asl_base_settings.h"

// XXX ### boost pointers currently cause crashs in JS
#define AC_DONT_USE_BOOST_PTR

#if !defined(AC_USE_BOOST_PTR)
#   define AC_USE_BOOST_PTR
#endif //defined(AC_USE_BOOST_PTR)

#if defined(AC_DONT_USE_BOOST_PTR)
#   if defined(AC_USE_BOOST_PTR)
#       undef AC_USE_BOOST_PTR
#   endif //defined(AC_USE_BOOST_PTR)
#endif //defined(AC_DONT_USE_BOOST_PTR)

#if defined(AC_USE_BOOST_PTR)
#   define BOOST_SP_USE_QUICK_ALLOCATOR
#   include <boost/shared_ptr.hpp>
#   include <boost/weak_ptr.hpp>
#endif //defined(AC_USE_BOOST_PTR)

#include "Exception.h"

#include "AtomicCount.h"
#ifdef WIN32
#include <stddef.h>
#endif

#include <assert.h>
#include <pthread.h>

#ifdef OSX
#   include <malloc/malloc.h>
#else
#   include <malloc.h>
#endif

#define DBP2(x) // x

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */

    DEFINE_EXCEPTION(Forbidden, Exception);
    DEFINE_EXCEPTION(BadWeakPtrException, Exception);

    template <class ThreadingModel>
    struct ReferenceCounter {
        ReferenceCounter(ptrdiff_t theSmartCount = 1, ptrdiff_t theWeakCount = 1) :
            smartCount(theSmartCount),
            weakCount(theWeakCount)
        {}

        ReferenceCounter<ThreadingModel>* getNextPtr() const {
            return reinterpret_cast<ReferenceCounter<ThreadingModel>*>((ptrdiff_t)smartCount);
        }

        void setNextPtr(ReferenceCounter<ThreadingModel> * theNext) {
            smartCount.set(reinterpret_cast<ptrdiff_t>(theNext));
        }

        void init() {
            smartCount.set(1);
            weakCount.set(1);
        }

        //  ReferenceCounter<ThreadingModel> * next;
        AtomicCount<ThreadingModel> smartCount;

	// Note:
        // weakCount is one higher than the number of weak pointers as long as the smart
        // count is at least one. This allows an atomic test for destruction of the reference
        // counter: if the weak count is 0, no more references exist.
        AtomicCount<ThreadingModel> weakCount;
    };


    // This is a default, fast free-list using version of the allocator, which is
    // thread-safe and probably the fastest you can get in your app
    class SpinLockedPtrFreeListAllocator {
        public:
            typedef MultiProcessor ThreadingModel;

            static ReferenceCounter<ThreadingModel> * allocate() {

                while (!_theThreadLock_.decrement_and_test()) {
                    _theThreadLock_.increment();
                }

                if (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * result = _theFreeListHead_;
                    _theFreeListHead_ = _theFreeListHead_->getNextPtr();
                    result->init();
                    _theThreadLock_.increment();
                    return result;
                } else {
                    _theThreadLock_.increment();
                    return new ReferenceCounter<ThreadingModel>;
                }
            }
            static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                while (!_theThreadLock_.decrement_and_test()) {
                    _theThreadLock_.increment();
                }
                anOldPtr->setNextPtr(_theFreeListHead_);
                _theFreeListHead_ = anOldPtr;
                _theThreadLock_.increment();
            }
            static void free_all() {
                while (!_theThreadLock_.decrement_and_test()) {
                    _theThreadLock_.increment();
                }
                while (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * next = _theFreeListHead_->getNextPtr();
                    delete _theFreeListHead_;
                    _theFreeListHead_ = next;
                }
                _theThreadLock_.increment();
            }
        private:
            ASL_BASE_DECL static ReferenceCounter<ThreadingModel> * volatile _theFreeListHead_;
            ASL_BASE_DECL static AtomicCount<MultiProcessor> _theThreadLock_;
    };

    // This is a thread safe free-list using version of the allocator
    // it achieves thread safety by using thread specific free list heads
    // but on our x86 MP Linux 2.4.x platform it is slower than the simple
    // and also threadsafe heap allocator when there are very few collisions
    // There is also a possible issue when one thread periodically deallocates
    // pointers that were allocated by other threads

    template <class TheThreadingModel>
    class PtrThreadSpecificFreeListAllocator {
        public:
            typedef TheThreadingModel ThreadingModel;
            static ReferenceCounter<ThreadingModel> * allocate() {
                // make sure pthread_key_create is called only once
                static int _theKeyStatus_ = pthread_key_create(&_theKey_,0);
                (void)(_theKeyStatus_); // avoid unused warning
                ReferenceCounter<ThreadingModel> * myHead = reinterpret_cast<ReferenceCounter<ThreadingModel> *>
                                    (pthread_getspecific(_theKey_));
                if (myHead) {
                    int status = pthread_setspecific(_theKey_,myHead->getNextPtr());
                    myHead->init();
                    return myHead;
                } else {
                    return new ReferenceCounter<ThreadingModel>;
                }
            }
            static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                anOldPtr->setNextPtr(reinterpret_cast<ReferenceCounter<ThreadingModel>*>(pthread_getspecific(_theKey_)));
                int status = pthread_setspecific(_theKey_,anOldPtr);
            }
        private:
            static pthread_key_t _theKey_;
    };

    // This is a thread safe free-list using version of the allocator
    // it achieves thread safety by using thread specific free list heads
    // but on our x86 MP Linux 2.4.x platform it is slower than the simple
    // and also threadsafe heap allocator when there are very few collisions
    // There is also a possible issue when one thread periodically deallocates
    // pointers that were allocated by other threads
    // This Allocator differs from the previous by preallocating whole chunks
    class PtrThreadSpecificFreeListChunkAllocator {
        public:
            typedef MultiProcessor ThreadingModel;
            typedef ReferenceCounter<ThreadingModel> RefCount;
            static RefCount * allocate() {
                // make sure pthread_key_create is called only once
                static int _theKeyStatus_ = pthread_key_create(&_theKey_,0);
                (void)(_theKeyStatus_); // avoid unused warning
                RefCount * myHead = reinterpret_cast<RefCount *>(pthread_getspecific(_theKey_));
                if (myHead) {
                    int status = pthread_setspecific(_theKey_,myHead->getNextPtr());
                    myHead->init();
                    (void)status;
                    return myHead;
                } else {
                    const int CHUNK_SIZE = 64;
                    RefCount * myNewChunk = reinterpret_cast<RefCount *>(malloc(sizeof(RefCount)*CHUNK_SIZE));
                    for (int i = 1; i < CHUNK_SIZE-1; ++i) {
                       myNewChunk[i].setNextPtr(myNewChunk+i+1);
                    }
                    myNewChunk[CHUNK_SIZE-1].setNextPtr(0);
                    int status = pthread_setspecific(_theKey_,myNewChunk+1);
                    myNewChunk->init();
                    (void)status;
                    return myNewChunk;
                }
            }
            static void free(RefCount * anOldPtr) {
                anOldPtr->setNextPtr(reinterpret_cast<RefCount*>(pthread_getspecific(_theKey_)));
                int status = pthread_setspecific(_theKey_,anOldPtr);
                (void)status;
            }
        private:
            ASL_BASE_DECL static pthread_key_t _theKey_;
    };

    // This is thread-safe free-list version of the allocator using pthread mutexes
    class MutexPtrFreeListAllocator {
        public:
            typedef MultiProcessor ThreadingModel;
            static ReferenceCounter<ThreadingModel> * allocate() {
                static int myStatus = pthread_mutex_init(&_theMutex_,0);
                (void)(myStatus); // avoid unused Warning
                pthread_mutex_lock(&_theMutex_);
                if (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * result = _theFreeListHead_;
                    DBP2(std::cout << "recycling old: " << (void*)result << std::endl);
                    _theFreeListHead_ = result->getNextPtr();
                    DBP2(std::cout << "new head: " << (void*)_theFreeListHead_ << std::endl);
                    pthread_mutex_unlock(&_theMutex_);
                    result->init();
                    return result;
                } else {
                    DBP2(std::cout << "allocating new" << std::endl);
                    pthread_mutex_unlock(&_theMutex_);
                    return new ReferenceCounter<ThreadingModel>;
                }
            }
            static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                pthread_mutex_lock(&_theMutex_);
                DBP2(std::cout << "free: " << (void*)anOldPtr << std::endl);
                anOldPtr->setNextPtr(_theFreeListHead_);
                _theFreeListHead_ = anOldPtr;
                pthread_mutex_unlock(&_theMutex_);
            }
        private:
            ASL_BASE_DECL static ReferenceCounter<ThreadingModel> * _theFreeListHead_;
            ASL_BASE_DECL static pthread_mutex_t _theMutex_;
};

    // PtrMultiFreeListAllocator is not really thread-safe by using multiple free list heads
    // so you should avoid it; it is here just for experimental purposes
    class PtrMultiFreeListAllocator {
        public:
           typedef MultiProcessor ThreadingModel;
           static ReferenceCounter<ThreadingModel> * allocate() {
            int myIndex = _theIndex_.post_increment()&MASK;
                if (_theFreeListHead_[myIndex]) {
                    ReferenceCounter<ThreadingModel> * result = _theFreeListHead_[myIndex];
                    _theFreeListHead_[myIndex] = _theFreeListHead_[myIndex]->getNextPtr();
                    result->init();
                    return result;
                } else {
                    ReferenceCounter<ThreadingModel> * result = new ReferenceCounter<ThreadingModel>;
                    return result;
                }
            }
            static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                int myIndex = _theIndex_.post_increment()&MASK;
                anOldPtr->setNextPtr(_theFreeListHead_[myIndex]);
                _theFreeListHead_[myIndex] = anOldPtr;
            }
        private:
            enum Values {BITS = 4, NUM_LISTS = 1<<BITS, MASK = 0xf };
            static AtomicCount<MultiProcessor> _theIndex_;
            static ReferenceCounter<ThreadingModel> * _theFreeListHead_[NUM_LISTS];
    };

    // This is *non thread-safe* fast free-list using version of the allocator
    // It was the fastest you could get before we had the PtrFreeListChunkAllocator
    class PtrFreeListAllocator {
        public:
            typedef SingleThreaded ThreadingModel;
            static ReferenceCounter<ThreadingModel> * allocate() {
                if (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * result = _theFreeListHead_;
                    _theFreeListHead_ = result->getNextPtr();
                    result->init();
                    return result;
                } else {
                    return new ReferenceCounter<ThreadingModel>;
                }
            }
            static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                anOldPtr->setNextPtr( _theFreeListHead_ );
                _theFreeListHead_ = anOldPtr;
            }
            static void free_all() {
                while (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * next = _theFreeListHead_->getNextPtr();
                    delete _theFreeListHead_;
                    _theFreeListHead_ = next;
                }
            }
        private:
            ASL_BASE_DECL static ReferenceCounter<ThreadingModel> * _theFreeListHead_;
    };
#if 0
    DBP2(
    template <class TheThreadingModel>
    void printList(ReferenceCounter<TheThreadingModel> * theHead) {
        while (theHead) {
            std::cerr << theHead << "->";
            theHead = theHead->getNextPtr();
        }
        std::cerr << 0 << std::endl;
    }
    )
#endif

    // This is *non thread-safe* fast free-list using version of the allocator
    // It is probably the fastest you can get
    template <class TheThreadingModel>
    class PtrFreeListChunkAllocator {
        public:
            typedef TheThreadingModel ThreadingModel;
            static ReferenceCounter<ThreadingModel> * allocate() {
                DBP2(std::cerr << "allocate(1): _theFreeListHead_ = "<<_theFreeListHead_<<std::endl);
                DBP2(printList(_theFreeListHead_));
                if (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * result = _theFreeListHead_;
                    _theFreeListHead_ = result->getNextPtr();
                    DBP2(std::cerr << "allocate(2): _theFreeListHead_ = "<<_theFreeListHead_<<std::endl);
                    DBP2(std::cerr << "allocate(3): result = "<<result<<std::endl);
                    result->init();
                    return result;
                } else {
                    const int CHUNK_SIZE = 64;
                    ReferenceCounter<ThreadingModel> * myNewChunk = reinterpret_cast<ReferenceCounter<ThreadingModel> *>
                                                    (malloc(sizeof(ReferenceCounter<ThreadingModel>)*CHUNK_SIZE));
                    for (int i = 1; i < CHUNK_SIZE-1; ++i) {
                        myNewChunk[i].setNextPtr(&myNewChunk[i+1]);
                        DBP2(std::cerr << "allocate(4): setNextPtr of "<<&myNewChunk[i]<<" = "<<&myNewChunk[i+1]<<std::endl);
                    }
                    myNewChunk[CHUNK_SIZE-1].setNextPtr(0);
                    DBP2(std::cerr << "allocate(4): setNextPtr of "<<&myNewChunk[CHUNK_SIZE-1]<<" = "<<0<<std::endl);
                    _theFreeListHead_ = &myNewChunk[1];
                    DBP2(std::cerr << "allocate(6): _theFreeListHead_ = "<<_theFreeListHead_<<std::endl);
                    myNewChunk[0].init();
                    DBP2(std::cerr << "allocate(7): result = "<<&myNewChunk[0]<<std::endl);
                    return &myNewChunk[0];
                }
            }
            static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                DBP2(std::cerr << "free(1): anOldPtr = "<<anOldPtr<<", _theFreeListHead_ = "<<_theFreeListHead_<<std::endl);
                DBP2(printList(_theFreeListHead_));
                anOldPtr->setNextPtr(_theFreeListHead_);
                DBP2(std::cerr << "free(2): setNextPtr = "<<_theFreeListHead_<<std::endl);
                _theFreeListHead_ = anOldPtr;
                DBP2(std::cerr << "free(3): set _theFreeListHead_ = "<<_theFreeListHead_<<std::endl);
            }
            static void free_all() {
                /*
                while (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * next = reinterpret_cast<ReferenceCounter<ThreadingModel> *>(*_theFreeListHead_);
                    delete _theFreeListHead_;
                    _theFreeListHead_ = next;
                }
                */
            }
        private:
            ASL_BASE_DECL static ReferenceCounter<ThreadingModel> * _theFreeListHead_;
    };

    template <class ThreadingModel>
    class PtrHeapAllocator {
        public:
            static ReferenceCounter<ThreadingModel> * allocate() {
                return new ReferenceCounter<ThreadingModel>;
            }
            static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                delete anOldPtr;
            }
    };

    template <class Threading> class PtrAllocator;

// the PtrFreeListChunkAllocator does not work correct in a shared environment, i.e. plugins, because of its
// static member '_theFreeListHead_', which will than be different pointers -> multiple freelists -> MEMORY LEAK
// Solution: By default the heap allocator is always used, no matter if you use
// MultiProcessor, SingleProcessor or SingleThreaed policy, performance measurements did not show any difference.
// To get the true variance define 'USE_NON_SHARED_ENVIRONMENT_ONLY_ALLOCATOR' (vs)

#ifdef USE_NON_SHARED_ENVIRONMENT_ONLY_ALLOCATOR
    template <>
    class PtrAllocator<SingleProcessor> : public PtrThreadSpecificFreeListAllocator<SingleProcessor> {};
    template <>
    class PtrAllocator<SingleThreaded> : public PtrFreeListChunkAllocator<SingleThreaded> {};
#else
    template <>
    class PtrAllocator<SingleProcessor> : public PtrHeapAllocator<SingleProcessor> {};
    template <>
    class PtrAllocator<SingleThreaded> : public PtrHeapAllocator<SingleThreaded> {};
#endif

    // template <>
    // class PtrAllocator<MultiProcessor> : public PtrThreadSpecificFreeListChunkAllocator {};

    //template <>
    //class PtrAllocator<SingleThreaded> : public PtrHeapAllocator<SingleThreaded> {};
    //template <>
    //class PtrAllocator<SingleProcessor> : public PtrHeapAllocator<SingleProcessor> {};
    template <>
    class PtrAllocator<MultiProcessor> : public PtrHeapAllocator<MultiProcessor> {};

#   if defined(AC_USE_BOOST_PTR)

        template< typename Allocator >
        struct PtrDeleter {
            template< typename T >
            void operator()(T* p)                           {Allocator().free(p);}
        };

        template <class T,
                  class ThreadingModel=MultiProcessor,
                  class Allocator=PtrAllocator<ThreadingModel> >
        class WeakPtr;

        template<class T,
                 class ThreadingModel=MultiProcessor,
                 class Allocator=PtrAllocator<ThreadingModel> >
        class Ptr : public boost::shared_ptr<T> {
        public:
            typedef boost::shared_ptr<T>                          ptr_type;
            Ptr()                                               : boost::shared_ptr<T>() {}
            template<class Y> Ptr(boost::shared_ptr<Y> const& r): boost::shared_ptr<T>(r) {}
            template<class Y> explicit Ptr(Y* p)                : boost::shared_ptr<T>(p/*,PtrDeleter<Allocator>(),std::allocator<T>()*/) {}
            template<class Y> Ptr(const Ptr<Y>& r)              : boost::shared_ptr<T>(r) {}
            template<class Y> explicit Ptr(WeakPtr<Y> const & r): boost::shared_ptr<T>(r) {}
        };

        template <class T,
                  class ThreadingModel,
                  class Allocator>
        class WeakPtr : public boost::weak_ptr<T> {
        public:
            typedef boost::weak_ptr<T>                            ptr_type;
            WeakPtr()                                           : boost::weak_ptr<T>() {}
            WeakPtr(const boost::weak_ptr<T>& r)                : boost::weak_ptr<T>(r) {}
            WeakPtr(const WeakPtr& r)                           : boost::weak_ptr<T>(r) {}
            WeakPtr(const Ptr<T, ThreadingModel, Allocator>& r) : boost::weak_ptr<T>(r) {}
        };

#   else //defined(AC_USE_BOOST_PTR)

        template <class T,
                  class ThreadingModel=MultiProcessor,
                  class Allocator=PtrAllocator<ThreadingModel> >
        class WeakPtr;

        template<class T,
                 class ThreadingModel=MultiProcessor,
                 class Allocator=PtrAllocator<ThreadingModel> >
        class Ptr;

        // Use this class almost exactly as you would use a pointer
        template<class T,
                 class ThreadingModel,
                 class Allocator >
        class Ptr {
            friend class WeakPtr<T,ThreadingModel,Allocator>;
        public:
            typedef Ptr<T,ThreadingModel,Allocator> ptr_type;
            typedef ThreadingModel ThreadingModelType;
            typedef Allocator AllocatorType;

            explicit Ptr(T * nativePtr = 0) : _myNativePtr(nativePtr), _myRefCountPtr(0) {
                if (_myNativePtr) {
                    _myRefCountPtr = reinterpret_cast<ReferenceCounter<ThreadingModel>*>(Allocator::allocate());
                }
            }

            explicit Ptr(T * nativePtr, ReferenceCounter<ThreadingModel> * refCountPtr)
                : _myNativePtr(nativePtr), _myRefCountPtr(refCountPtr)
            {
                if (!reference()) {
                    throw BadWeakPtrException(JUST_FILE_LINE);
                }
            }

            template <class D>
            Ptr(const Ptr<D,ThreadingModel,Allocator> & otherType)
                : _myNativePtr(otherType.getNativePtr()),
                  _myRefCountPtr(0)
            {
                if (_myNativePtr) {
                    _myRefCountPtr = otherType.getRefCountPtr();
                    reference();
                }
            }

            Ptr(const Ptr & otherPtr)
                : _myNativePtr(otherPtr._myNativePtr),
                  _myRefCountPtr(otherPtr._myRefCountPtr)
            {
                reference();
            }

            template<class D>
            const Ptr<T,ThreadingModel,Allocator> & operator=(const Ptr<D,ThreadingModel,Allocator> & otherType)
            {
                if (static_cast<const void *>(&otherType) != static_cast<const void*>(this)) {
                    unreference();
                    _myNativePtr = otherType.getNativePtr();
                    if (_myNativePtr) {
                        _myRefCountPtr = otherType.getRefCountPtr();
                        if (!reference()) {
                            _myNativePtr = 0;
                            _myRefCountPtr = 0;
                        }
                    } else {
                        _myRefCountPtr = 0;
                    }
                }
                return *this;
            }

            const Ptr& operator=(const Ptr & otherPtr) {
                if (&otherPtr != this) {
                    unreference();
                    _myNativePtr = otherPtr._myNativePtr;
                    _myRefCountPtr = otherPtr._myRefCountPtr;
                    if (!reference()) {
                        _myNativePtr = 0;
                        _myRefCountPtr = 0;
                    }
                }
                return *this;
            }

            ~Ptr() {
                unreference();
            }

            T * get() {
                return _myNativePtr;
            }

            const T * get() const {
                return _myNativePtr;
            }

            operator const void*() const {
                return _myNativePtr ? this : NULL;
            }

            T & operator*() {
                return *get();
            }

            const T& operator*() const {
                return *get();
            }

            T * operator->() {
                return get();
            }

            const T * operator->() const {
                return get();
            }

            bool operator==(const Ptr& rhs) const {return getNativePtr() == rhs.getNativePtr();}
            bool operator!=(const Ptr& rhs) const {return !(*this==rhs);}
            bool operator< (const Ptr& rhs) const {return getNativePtr() < rhs.getNativePtr();}
            bool operator> (const Ptr& rhs) const {return rhs < *this;}
            bool operator<=(const Ptr& rhs) const {return !(*this>rhs);}
            bool operator>=(const Ptr& rhs) const {return !(*this<rhs);}

            bool operator==(const WeakPtr<T,ThreadingModel,Allocator>& rhs) const;
            bool operator!=(const WeakPtr<T,ThreadingModel,Allocator>& rhs) const;
            bool operator< (const WeakPtr<T,ThreadingModel,Allocator>& rhs) const;
            bool operator> (const WeakPtr<T,ThreadingModel,Allocator>& rhs) const;
            bool operator<=(const WeakPtr<T,ThreadingModel,Allocator>& rhs) const;
            bool operator>=(const WeakPtr<T,ThreadingModel,Allocator>& rhs) const;

            ptrdiff_t use_count() const {
                return getRefCount();
            }

            T * getNativePtr() const {
                return _myNativePtr;
            }

            ReferenceCounter<ThreadingModel> * getRefCountPtr() const {
                return _myRefCountPtr;
            }

        private:
            T *    _myNativePtr;
            ReferenceCounter<ThreadingModel> * _myRefCountPtr;

            ptrdiff_t getRefCount() const {
                if (_myRefCountPtr) {
                    return _myRefCountPtr->smartCount;
                }
                return 0;
            }

            bool reference() {
                DBP2(std::cerr<<"Ptr::reference _myRefCountPtr = "<<_myRefCountPtr<<std::endl);
                if (_myNativePtr) {
                    if (!_myRefCountPtr || !_myRefCountPtr->smartCount.conditional_increment()) {
                        return false;
                    }
                    return true;
                }
                return false;
            }

            // Ptr::
            void unreference() {
                DBP2(std::cerr<<"Ptr::unreference _myRefCountPtr = "<<_myRefCountPtr<<std::endl);
                if (_myNativePtr) {
                    DBP2(std::cerr<<"Ptr::unreference weakCount = "<<_myRefCountPtr->weakCount<<std::endl);
                    DBP2(std::cerr<<"Ptr::unreference smartCount = "<<_myRefCountPtr->smartCount<<std::endl);
                    if (_myRefCountPtr->smartCount.decrement_and_test()) {
                        if (_myRefCountPtr->weakCount.decrement_and_test()) {
                            DBP2(std::cerr<<"Ptr: calling free _myRefCountPtr"<<std::endl);
                            // play nice & thread-safe in case another thread will call
                            // reference() while we are unreference
                            ReferenceCounter<ThreadingModel> * _refCountSavePtr = _myRefCountPtr;
                            _myRefCountPtr = 0;
                            Allocator::free(_refCountSavePtr);
                        }
                        delete _myNativePtr;
                        _myNativePtr = 0;
                    }
                }
            }

        private:
            // do not allow comparison with other types and forbid comparison of bool results
            // template<class D> bool operator==(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const;
            // template<class D> bool operator!=(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const;
        };

        template <class T,
                  class ThreadingModel,
                  class Allocator>
        class WeakPtr {
                friend class Ptr<T,ThreadingModel,Allocator>;
            public:
                typedef WeakPtr<T,ThreadingModel,Allocator> ptr_type;

                WeakPtr() :
                    _myNativePtr(0),
                    _myRefCountPtr(0)
                {}
                WeakPtr(const Ptr<T, ThreadingModel, Allocator> & theSmartPtr) :
                    _myNativePtr(theSmartPtr.getNativePtr()),
                    _myRefCountPtr(theSmartPtr.getRefCountPtr())
                {
                    reference();
                }

                WeakPtr(const WeakPtr & otherWeakPtr) :
                    _myNativePtr(otherWeakPtr._myNativePtr),
                    _myRefCountPtr(otherWeakPtr._myRefCountPtr)
                {
                    reference();
                }

                ~WeakPtr() {
                    unreference();
                }

                const WeakPtr & operator=(const WeakPtr & otherPtr) {
                    if (this != & otherPtr) {
                        unreference();
                        _myNativePtr = otherPtr._myNativePtr;
                        _myRefCountPtr = otherPtr._myRefCountPtr;
                        reference();
                    }
                    return *this;
                }

                bool expired() const {
                    return !isValid();
                }

                //operator const void*() const {
                //    return isValid() ? this : NULL;
                //}

                //ReferenceCounter<ThreadingModel> * getRefCountPtr() const {
                //    return _myRefCountPtr;
                //}

                Ptr<T, ThreadingModel, Allocator> lock() {
                    if (isValid()) {
                        try {
                            return Ptr<T, ThreadingModel, Allocator>(_myNativePtr, _myRefCountPtr);
                        } catch (BadWeakPtrException&) {
                            std::cerr << "BadWeakPtrException: refCount=" << getRefCount() <<
                                    ", weakCount=" << getWeakCount() << std::endl;
                        }
                    }
                    return Ptr<T, ThreadingModel, Allocator>(0);
                }

                const Ptr<T, ThreadingModel, Allocator> lock() const {
                    if (isValid()) {
                        try {
                            return Ptr<T, ThreadingModel, Allocator>(_myNativePtr, _myRefCountPtr);
                        } catch (BadWeakPtrException&) {
                            std::cerr << "BadWeakPtrException: refCount=" << getRefCount() <<
                                    ", weakCount=" << getWeakCount() << std::endl;
                        }
                    }
                    return Ptr<T, ThreadingModel, Allocator>(0);
                }

                ptrdiff_t use_count() const {
                    return getRefCount();
                }

                bool operator==(const WeakPtr& rhs) const {return _myNativePtr == rhs._myNativePtr;}
                bool operator!=(const WeakPtr& rhs) const {return !(*this==rhs);}
                bool operator< (const WeakPtr& rhs) const {return _myNativePtr < rhs._myNativePtr;}
                bool operator> (const WeakPtr& rhs) const {return rhs < *this;}
                bool operator<=(const WeakPtr& rhs) const {return !(*this>rhs);}
                bool operator>=(const WeakPtr& rhs) const {return !(*this<rhs);}

                bool operator==(const Ptr<T,ThreadingModel,Allocator>& rhs) const {return _myNativePtr == rhs.getNativePtr();}
                bool operator!=(const Ptr<T,ThreadingModel,Allocator>& rhs) const {return !(*this==rhs);}
                bool operator< (const Ptr<T,ThreadingModel,Allocator>& rhs) const {return _myNativePtr < rhs.getNativePtr();}
                bool operator> (const Ptr<T,ThreadingModel,Allocator>& rhs) const {return rhs < *this;}
                bool operator<=(const Ptr<T,ThreadingModel,Allocator>& rhs) const {return !(*this>rhs);}
                bool operator>=(const Ptr<T,ThreadingModel,Allocator>& rhs) const {return !(*this<rhs);}

            private:
                T *                                _myNativePtr;
                ReferenceCounter<ThreadingModel> * _myRefCountPtr;

                // only use for debugging and tests
                ptrdiff_t getRefCount() const {
                    if (_myRefCountPtr) {
                        return _myRefCountPtr->smartCount;
                    }
                    return 0;
                }

                ptrdiff_t getWeakCount() const {
                    if (_myRefCountPtr) {
                        return _myRefCountPtr->weakCount;
                    }
                    return 0;
                }

                bool isValid() const {
                    if (_myRefCountPtr && _myRefCountPtr->smartCount != 0) {
                        return true;
                    }
                    return false;
                }


                void reference() {
                    DBP2(std::cerr<<"WeakPtr::reference _myRefCountPtr = "<<_myRefCountPtr<<std::endl);
                    if (_myNativePtr) {
                        _myRefCountPtr->weakCount.increment();
                    }
                }

                // WeakPtr::
                void unreference() {
                    DBP2(std::cerr<<"WeakPtr::unreference _myRefCountPtr = "<<_myRefCountPtr<<std::endl);
                    if (_myNativePtr) {
                        DBP2(std::cerr<<"WeakPtr::unreference weakCount = "<<_myRefCountPtr->weakCount<<std::endl);
                        DBP2(std::cerr<<"WeakPtr::unreference smartCount = "<<_myRefCountPtr->smartCount<<std::endl);
                        if (_myRefCountPtr->weakCount.decrement_and_test())
                        {
                            DBP2(std::cerr<<"Ptr: WeakPtr calling free on _myRefCountPtr"<<std::endl);
                            Allocator::free(_myRefCountPtr);
                        }
                        _myNativePtr = 0;
                        _myRefCountPtr = 0;
                    }
                }

               private:
                    // do not allow comparison with other types and forbid comparison of bool results
                    // template<class D> bool operator==(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const;
                    // template<class D> bool operator!=(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const;
                    // template<class D> bool operator==(const Ptr<D, ThreadingModel, Allocator> & otherPtr) const;
                    // template<class D> bool operator!=(const Ptr<D, ThreadingModel, Allocator> & otherPtr) const;
        };

        template<class T, class ThreadingModel, class Allocator >
        inline bool Ptr<T,ThreadingModel,Allocator>::operator==(const asl::WeakPtr<T,ThreadingModel,Allocator>& rhs) const {
            return getNativePtr() == rhs._myNativePtr;
        }

        template<class T, class ThreadingModel, class Allocator >
        inline bool Ptr<T,ThreadingModel,Allocator>::operator!=(const asl::WeakPtr<T,ThreadingModel,Allocator>& rhs) const {
            return !(*this==rhs);
        }

        template<class T, class ThreadingModel, class Allocator >
        inline bool Ptr<T,ThreadingModel,Allocator>::operator<(const asl::WeakPtr<T,ThreadingModel,Allocator>& rhs) const {
            return getNativePtr() < rhs._myNativePtr;
        }

        template<class T, class ThreadingModel, class Allocator >
        inline bool Ptr<T,ThreadingModel,Allocator>::operator>(const asl::WeakPtr<T,ThreadingModel,Allocator>& rhs) const {
            return rhs < *this;
        }

        template<class T, class ThreadingModel, class Allocator >
        inline bool Ptr<T,ThreadingModel,Allocator>::operator<=(const asl::WeakPtr<T,ThreadingModel,Allocator>& rhs) const {
            return !(*this>rhs);
        }

        template<class T, class ThreadingModel, class Allocator >
        inline bool Ptr<T,ThreadingModel,Allocator>::operator>=(const asl::WeakPtr<T,ThreadingModel,Allocator>& rhs) const {
            return !(*this<rhs);
        }

#   endif //defined(AC_USE_BOOST_PTR)

} //namespace

template<typename T, typename U, class ThreadingModel, class Allocator>
asl::Ptr<T,ThreadingModel,Allocator> static_cast_Ptr(asl::Ptr<U,ThreadingModel,Allocator> const & r)
{
#   if defined(AC_USE_BOOST_PTR)
        return boost::static_pointer_cast<T>(r);
#   else //defined(AC_USE_BOOST_PTR)
        return asl::Ptr<T,ThreadingModel,Allocator>(static_cast<T*>(r.getNativePtr()),r.getRefCountPtr());
#   endif //defined(AC_USE_BOOST_PTR)
}

template<typename T, typename U, class ThreadingModel, class Allocator>
asl::Ptr<T,ThreadingModel,Allocator> dynamic_cast_Ptr(asl::Ptr<U,ThreadingModel,Allocator> const & r)
{
#   if defined(AC_USE_BOOST_PTR)
        return boost::dynamic_pointer_cast<T>(r);
#   else //defined(AC_USE_BOOST_PTR)
        try {
            T * myCastedNative = dynamic_cast<T*>(r.getNativePtr());
            if (myCastedNative) {
                return asl::Ptr<T,ThreadingModel,Allocator>(myCastedNative, r.getRefCountPtr());
            } else {
                return asl::Ptr<T,ThreadingModel,Allocator>(0);
            }
        } catch (asl::BadWeakPtrException&) {
            return asl::Ptr<T,ThreadingModel,Allocator>(0);
        }
#   endif //defined(AC_USE_BOOST_PTR)
}

/* @} */

#undef DBP2

#endif
