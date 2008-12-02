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
//
// Description:  A fast allocator for small objects of same size
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : ok
//    formatting             : ok
//    documentation          : poor
//    test coverage          : fair (threading tests currently disabled)
//    names                  : ok
//    style guide conformance: ok
//    technical soundness    : ok
//    dead code              : fair
//    readability            : ok
//    understandabilty       : ok
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendations:
//       - remove dead code
//       - re-enable or discard thread-safety tests
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_asl_MemoryPool_
#define _included_asl_MemoryPool_

#include "settings.h"
#include "Exception.h"
#include "string_functions.h"

#include "AtomicCount.h"
#ifdef WIN32
#include <stddef.h>
#endif

#include <assert.h>
#include <pthread.h>
#ifdef OSX
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#define DB2(x) // x

namespace asl {

typedef unsigned long Size;

/*! \addtogroup aslbase */
/* @{ */

// see Efficient C++ Pg. 85
template <class T>
class MemoryPool {
public:
    MemoryPool(Size theSize = EXPANSION_SIZE);
    ~MemoryPool();
    inline void * alloc(Size theSize);
    inline void free(void *theElement);
private:
    MemoryPool<T> * _myNextPool;
    enum { EXPANSION_SIZE = 64 };
    void expandTheFreeList(int theCount = EXPANSION_SIZE);
};

template <class T>
MemoryPool<T>::MemoryPool(Size theSize) {
    expandTheFreeList(theSize);
}

template <class T>
MemoryPool<T>::~MemoryPool() {
    MemoryPool<T> * myPool = _myNextPool;
    for (myPool = _myNextPool; myPool != 0; myPool = _myNextPool) {
        _myNextPool = _myNextPool->_myNextPool;
        //delete [] myPool;
        //free(myPool);
    }
}

DEFINE_EXCEPTION(BadAllocSize,asl::Exception);

template <class T>
inline
void *
MemoryPool<T>::alloc(Size theSize) {
    if (theSize > sizeof(T)) {
        return malloc(theSize);
        //throw BadAllocSize(std::string("Required ") +asl::as_string(theSize)+"expected "+asl::as_string(sizeof(T)),PLUS_FILE_LINE);
    }
    //MemoryPool<T> * myPool = _myNextPool;
    if (!_myNextPool) {
        expandTheFreeList();
    }
    MemoryPool<T> * myHead = _myNextPool;
    _myNextPool = myHead->_myNextPool;
    return myHead;
}

template <class T>
inline
void
MemoryPool<T>::free(void * theDoomed) {
    MemoryPool<T> * myHead = static_cast<MemoryPool<T>*>(theDoomed);
    myHead->_myNextPool = _myNextPool;
    _myNextPool = myHead;
}

template <class T>
inline
void 
MemoryPool<T>::expandTheFreeList(int theCount) {
    Size mySize = sizeof(T);
    if (mySize < sizeof(MemoryPool<T>*)) {
        mySize = sizeof(MemoryPool<T>*);
    }
    //MemoryPool <T> * myPool = reinterpret_cast<MemoryPool<T>*>(new char[mySize]);
    MemoryPool <T> * myPool = reinterpret_cast<MemoryPool<T>*>(malloc(mySize*theCount));
    unsigned char * myChunk = reinterpret_cast<unsigned char*>(myPool);
    _myNextPool = myPool;
    for (int i = 0; i < theCount; ++i) {
        //myPool->_myNextPool = reinterpret_cast<MemoryPool<T>*>(new char[mySize]);
        myPool->_myNextPool = reinterpret_cast<MemoryPool<T>*>(myChunk + i * mySize);
        myPool = myPool->_myNextPool;
    }
    myPool->_myNextPool = 0;
}

template <class T>
class PoolAllocator {
public:
    void * operator new(size_t theSize) {
        ++getAllocatedObjectsCounter();
        return getMemoryPool()->alloc(theSize);
    }
    void operator delete(void * theDoomed, size_t theSize) {
        ++getFreedObjectsCounter();
        if (theSize <= sizeof(T)) {
            getMemoryPool()->free(theDoomed);
        } else {
            ::free(theDoomed);
        }
    }
    static unsigned long getAllocatedObjectsCount() {
        return getAllocatedObjectsCounter();
    }    
    static unsigned long getFreedObjectsCount() {
        return getFreedObjectsCounter();
    }
private:
    static MemoryPool<T> * getMemoryPool() {
        static MemoryPool<T> * myPool = new MemoryPool<T>;
        return myPool;
    }
    static unsigned long & getAllocatedObjectsCounter() {
        static unsigned long ourAllocatedObjectsCounter = 0;
        return ourAllocatedObjectsCounter;
    }
    static unsigned long & getFreedObjectsCounter() {
        static unsigned long ourFreedObjectsCounter = 0;
        return ourFreedObjectsCounter;
    }
};

template <class T>
class StandardAllocator {
public:
    void * operator new(size_t theSize) {
        ++getAllocatedObjectsCounter();
        return ::operator new(theSize);
    }
    void operator delete(void * theDoomed, size_t theSize) {
        ++getFreedObjectsCounter();
        ::operator delete(theDoomed);
    }
    static unsigned long getAllocatedObjectsCount() {
        return getAllocatedObjectsCounter();
    }    
    static unsigned long getFreedObjectsCount() {
        return getFreedObjectsCounter();
    }
private:
    static unsigned long & getAllocatedObjectsCounter() {
        static unsigned long ourAllocatedObjectsCounter = 0;
        return ourAllocatedObjectsCounter;
    }
    static unsigned long & getFreedObjectsCounter() {
        static unsigned long ourFreedObjectsCounter = 0;
        return ourFreedObjectsCounter;
    }
};

template <class T>
class DummyAllocator {
    // This will cause global new and delete beeing called
};

#undef DB2


/*

template <class Threading, class T> class ObjectAllocator;

template <class Threading, class T>
class ObjectAllocator<SingleThreaded,T>  {};

template<class T>
class ObjectAllocator<SingleProcessor,T> : public StandardAllocator<T> {};

template<class T>
class ObjectAllocator<MultiProcessor,T> : public StandardAllocator<T> {};
*/
} //namespace



/* @} */

#endif
