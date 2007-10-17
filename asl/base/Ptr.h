/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
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

#include "settings.h"
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

#define DB2(x) // x
#define DBP2(x) // x

namespace asl {

    /*! \addtogroup aslbase */
    /* @{ */
    
    DEFINE_EXCEPTION(Forbidden, Exception);
    DEFINE_EXCEPTION(BadWeakPtrException, Exception);

    template <class ThreadingModel>
    struct ReferenceCounter {
        ReferenceCounter(long theSmartCount = 1, long theWeakCount = 1) :
            smartCount(theSmartCount),
            weakCount(theWeakCount)
        {}

        inline ReferenceCounter<ThreadingModel>* getNextPtr() const {
            return reinterpret_cast<ReferenceCounter<ThreadingModel>*>((long)smartCount);
        }

        inline void setNextPtr(ReferenceCounter<ThreadingModel> * theNext) {
            smartCount.set(reinterpret_cast<long>(theNext));
        }

        inline void init() {
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

            inline static ReferenceCounter<ThreadingModel> * allocate() {

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
            inline static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
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
            static ReferenceCounter<ThreadingModel> * volatile _theFreeListHead_;
            static AtomicCount<MultiProcessor> _theThreadLock_;
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
            inline static ReferenceCounter<ThreadingModel> * allocate() {
                // make sure pthread_key_create is called only once
                static int _theKeyStatus_ = pthread_key_create(&_theKey_,0);
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
            inline static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
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
            inline static RefCount * allocate() {
                // make sure pthread_key_create is called only once
                static int _theKeyStatus_ = pthread_key_create(&_theKey_,0);
                RefCount * myHead = reinterpret_cast<RefCount *>(pthread_getspecific(_theKey_));
                if (myHead) {
                    int status = pthread_setspecific(_theKey_,myHead->getNextPtr());
                    myHead->init();
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
                    return myNewChunk;
                }
            }
            inline static void free(RefCount * anOldPtr) {
                anOldPtr->setNextPtr(reinterpret_cast<RefCount*>(pthread_getspecific(_theKey_)));
                int status = pthread_setspecific(_theKey_,anOldPtr);
            }
        private:
            static pthread_key_t _theKey_;
    };

    // This is thread-safe free-list version of the allocator using pthread mutexes
    class MutexPtrFreeListAllocator {
        public:
            typedef MultiProcessor ThreadingModel;
            inline static ReferenceCounter<ThreadingModel> * allocate() {
                static int myStatus = pthread_mutex_init(&_theMutex_,0);
                pthread_mutex_lock(&_theMutex_);
                if (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * result = _theFreeListHead_;
                    _theFreeListHead_ = result->getNextPtr();
                    pthread_mutex_unlock(&_theMutex_);
                    result->init();
                    return result;
                } else {
                    pthread_mutex_unlock(&_theMutex_);
                    return new ReferenceCounter<ThreadingModel>;
                }
            }
            inline static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
                pthread_mutex_lock(&_theMutex_);
                anOldPtr->setNextPtr(_theFreeListHead_);
                _theFreeListHead_ = anOldPtr;
                pthread_mutex_unlock(&_theMutex_);
            }
        private:
            static ReferenceCounter<ThreadingModel> * _theFreeListHead_;
            static pthread_mutex_t _theMutex_;
};

    // PtrMultiFreeListAllocator is not really thread-safe by using multiple free list heads
    // so you should avoid it; it is here just for experimental purposes
    class PtrMultiFreeListAllocator {
        public:
           typedef MultiProcessor ThreadingModel;
           inline static ReferenceCounter<ThreadingModel> * allocate() {
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
            inline static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
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
            inline static ReferenceCounter<ThreadingModel> * allocate() {
                if (_theFreeListHead_) {
                    ReferenceCounter<ThreadingModel> * result = _theFreeListHead_;
                    _theFreeListHead_ = result->getNextPtr();
                    result->init();
                    return result;
                } else {
                    return new ReferenceCounter<ThreadingModel>;
                }
            }
            inline static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
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
            static ReferenceCounter<ThreadingModel> * _theFreeListHead_;
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
            inline static ReferenceCounter<ThreadingModel> * allocate() {
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
            inline static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
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
            static ReferenceCounter<ThreadingModel> * _theFreeListHead_;
    };

    template <class ThreadingModel>
    class PtrHeapAllocator {
        public:
            inline static ReferenceCounter<ThreadingModel> * allocate() {
                return new ReferenceCounter<ThreadingModel>;
            }
            inline static void free(ReferenceCounter<ThreadingModel> * anOldPtr) {
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
    

    template <class T,
              class ThreadingModel=MultiProcessor,
              class Allocator=PtrAllocator<ThreadingModel> >
    class WeakPtr;
 

    // Use this class almost exactly as you would use a pointer
    template<class C,
             class ThreadingModel=MultiProcessor,
             class Allocator=PtrAllocator<ThreadingModel> >
    class Ptr {
        friend class WeakPtr<C,ThreadingModel,Allocator>;
        
    public:
        typedef ThreadingModel ThreadingModelType;
        typedef Allocator AllocatorType;

        explicit Ptr(C * nativePtr = 0) : _myNativePtr(nativePtr), _myRefCountPtr(0) {
            if (_myNativePtr) {
                _myRefCountPtr = reinterpret_cast<asl::ReferenceCounter<ThreadingModel>*>(Allocator::allocate());
            }
        }

        explicit Ptr(C * nativePtr, ReferenceCounter<ThreadingModel> * refCountPtr)
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
        const Ptr<C,ThreadingModel,Allocator> & operator=(const Ptr<D,ThreadingModel,Allocator> & otherType) 
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

        inline const Ptr& operator=(const Ptr & otherPtr) {
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

        inline bool operator==(const Ptr& otherPtr) const {
            return _myNativePtr == otherPtr._myNativePtr;
        }

        inline bool operator!=(const Ptr& otherPtr) const {
            return !operator==(otherPtr);
        }

        inline operator bool() const {
            return (_myNativePtr != 0);
        }

        inline C & operator*() {
            return *_myNativePtr;
        }

        inline const C& operator*() const {
            return *_myNativePtr;
        }

        inline C * operator->() {
            return _myNativePtr;
        }

        inline const C * operator->() const {
            return _myNativePtr;
        }

        inline C * getNativePtr() const {
            return _myNativePtr;
        }

        inline ReferenceCounter<ThreadingModel> * getRefCountPtr() const {
            return _myRefCountPtr;
        }
        
        inline long getRefCount() const {
            if (_myRefCountPtr) {
                return _myRefCountPtr->smartCount;
            }
            // XXX DS: huh? why is the refcount 1 if there is no reference counter?
            // PM: The refcount is undefined if there is no reference counter
            //     so we can return what we want; 1 is as wrong as any other number
            return 1;
        }
        
    private:
        C *    _myNativePtr;
        ReferenceCounter<ThreadingModel> * _myRefCountPtr;

        inline bool reference() {
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
        inline void unreference() {
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
    
    public:
        bool operator==(const WeakPtr<C, ThreadingModel, Allocator> & otherPtr) const {
            return _myRefCountPtr == otherPtr._myRefCountPtr;
        }

        bool operator!=(const WeakPtr<C, ThreadingModel, Allocator> & otherPtr) const {
            return _myRefCountPtr != otherPtr._myRefCountPtr;
        }
    
    private:
        // do not allow comparison with other types and forbid comparison of bool results
        template<class D>
        bool operator==(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const {
            throw Forbidden(JUST_FILE_LINE);
        }

        template<class D>
        bool operator!=(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const {
            throw Forbidden(JUST_FILE_LINE);
        }
    };


    template <class T,
              class ThreadingModel,
              class Allocator>
    class WeakPtr {
        friend class Ptr<T, ThreadingModel, Allocator>;
        public:
            WeakPtr() :
                _myNativePtr(0),
                _myRefCountPtr(0)
            {};
            WeakPtr(const Ptr<T, ThreadingModel, Allocator> & theSmartPtr) :
                _myNativePtr(theSmartPtr.getNativePtr()),
                _myRefCountPtr(theSmartPtr.getRefCountPtr())
            {
                reference();
            };

            WeakPtr(const WeakPtr & otherWeakPtr) :
                _myNativePtr(otherWeakPtr._myNativePtr),
                _myRefCountPtr(otherWeakPtr._myRefCountPtr)
            {
                reference();
            }

            ~WeakPtr() {
                unreference();
            }

            inline const WeakPtr & operator=(const WeakPtr & otherPtr) {
                if (this != & otherPtr) {
                    unreference();
                    _myNativePtr = otherPtr._myNativePtr;
                    _myRefCountPtr = otherPtr._myRefCountPtr;
                    reference();
                }
                return *this;
            }

            operator bool() const {
                return isValid();
            }

            inline ReferenceCounter<ThreadingModel> * getRefCountPtr() const {
                return _myRefCountPtr;
            }

            inline Ptr<T, ThreadingModel, Allocator> lock() {
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

            inline const Ptr<T, ThreadingModel, Allocator> lock() const {
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

            // only use for debugging and tests
            inline long getRefCount() const {
                if (_myRefCountPtr) {
                    return _myRefCountPtr->smartCount;
                }
                // XXX DS: huh? why is the refcount 1 if there is no reference counter?
                // PM: The refcount is undefined if there is no reference counter
                //     so we can return what we want; 1 is as wrong as any other number
                return 1;
            }
            inline long getWeakCount() const {
                if (_myRefCountPtr) {
                    return _myRefCountPtr->weakCount;
                }
                // XXX DS: huh? why is the refcount 1 if there is no reference counter?
                // PM: The refcount is undefined if there is no reference counter
                //     so we can return what we want; 1 is as wrong as any other number
                return 1;
            }

        private:
            T *                                _myNativePtr;
            ReferenceCounter<ThreadingModel> * _myRefCountPtr;

            inline bool isValid() const {
                if (_myRefCountPtr && _myRefCountPtr->smartCount != 0) {
                    return true;
                }
                return false;
            }


            inline void reference() {
                DBP2(std::cerr<<"WeakPtr::reference _myRefCountPtr = "<<_myRefCountPtr<<std::endl);
                if (_myNativePtr) {
                    _myRefCountPtr->weakCount.increment();
                }
            }

            // WeakPtr::
            inline void unreference() {
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

           public:
                bool operator==(const WeakPtr & otherPtr) const {
                    return _myRefCountPtr == otherPtr._myRefCountPtr;
                }
                bool operator!=(const WeakPtr & otherPtr) const {
                    return _myRefCountPtr != otherPtr._myRefCountPtr;
                }
                bool operator==(const Ptr<T, ThreadingModel, Allocator> & otherPtr) const {
                    return _myRefCountPtr == otherPtr._myRefCountPtr;
                }
                bool operator!=(const Ptr<T, ThreadingModel, Allocator> & otherPtr) const {
                    return _myRefCountPtr != otherPtr._myRefCountPtr;
                }
           private:
                // do not allow comparison with other types and forbid comparison of bool results
                template<class D>
                bool operator==(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
                template<class D>
                bool operator!=(const WeakPtr<D, ThreadingModel, Allocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
                template<class D>
                bool operator==(const Ptr<D, ThreadingModel, Allocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
                template<class D>
                bool operator!=(const Ptr<D, ThreadingModel, Allocator> & otherPtr) const {
                    throw Forbidden(JUST_FILE_LINE);
                }
    };

} //namespace

template<typename T, typename U, class ThreadingModel, class Allocator>
asl::Ptr<T,ThreadingModel,Allocator> static_cast_Ptr(asl::Ptr<U,ThreadingModel,Allocator> const & r)
{
    return asl::Ptr<T,ThreadingModel,Allocator>(static_cast<T*>(r.getNativePtr()),r.getRefCountPtr());
}

template<typename T, typename U, class ThreadingModel, class Allocator>
asl::Ptr<T,ThreadingModel,Allocator> dynamic_cast_Ptr(asl::Ptr<U,ThreadingModel,Allocator> const & r)
{
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
}

/* @} */


#undef DB2
#undef DBP2

#endif
