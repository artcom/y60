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
//    $RCSfile: ThreadFifo.h,v $
//
//   $Revision: 1.5 $
//
//      Author: Axel Schmidt
//
// Description: threadsafe fifo
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _included_ThreadFifo_h_
#define _included_ThreadFifo_h_

#include "settings.h"
#include "ReadWriteLock.h"

#include "Exception.h"

#include <deque>

// #include <sched.h>
// #include <cassert>

namespace asl {


/*! \addtogroup aslbase */
/* @{ */
    
DEFINE_EXCEPTION(ThreadFifoException, Exception);
    
class ThreadFifoBase {
public:
    ThreadFifoBase() {};
    virtual int size() const = 0;
    virtual bool empty() const = 0;
    virtual ~ThreadFifoBase() {};
};

template <class T>
class ThreadFifo : public ThreadFifoBase 
{
public:
    ThreadFifo();
    virtual ~ThreadFifo();

    void push(const T& val);
    T pop();
    T front() const;
    // bool pop_nonblocking(T& val);
    // bool pop_blocking(T& val);
    // bool pop_waiting(T& val);
    // bool getFirst_waiting(T& val); // get first (wait if necessary, do not remove from queue)
    int  size() const;
    bool empty() const;
    void clear();
 
    /*
    // remember to lock/unlock the FIFO whe you peek into it!
    const T &   peekElement(unsigned int theIndex);
    void        lock();
    void        unlock();
    */
private:
    // ThreadSem * _protectionSemaphore;
    mutable PriorityCeilingRWLock _myLock;
    // ThreadSem * _countSemaphore;
    std::deque<T>    _queue;
};

template <class T>
ThreadFifo<T>::ThreadFifo() {
    // init the semaphores
    /*
    if ((_protectionSemaphore = ThreadSemFactory::instance()->newLock()) == 0) {
        assert(_protectionSemaphore);
    }
    if ((_countSemaphore = ThreadSemFactory::instance()->newLock(0)) == 0) {
        assert(_countSemaphore);
    }
    */
}

template <class T>
ThreadFifo<T>::~ThreadFifo()
{
    /*
    ThreadSemFactory::instance()->freeLock( _protectionSemaphore );
    ThreadSemFactory::instance()->freeLock( _countSemaphore );
    */
}

template <class T>
T
ThreadFifo<T>::pop()
{
    ScopeLocker L(_myLock, true);
    if (_queue.empty()) {
        throw ThreadFifoException("Queue is empty, can't pop",PLUS_FILE_LINE);
    }
    T val(_queue.front());
    _queue.pop_front();

    return val;
}


template <class T>
void
ThreadFifo<T>::push(const T& val)
{
    ScopeLocker L(_myLock, true);
    _queue.push_back( val );
}

template <class T>
void
ThreadFifo<T>::clear()
{
    ScopeLocker L(_myLock, true);
    _queue.clear();
}

template <class T>
T  
ThreadFifo<T>::front() const
{
    ScopeLocker L(_myLock, false);
    if (_queue.empty()) {
        throw ThreadFifoException("Queue is empty, can't front",PLUS_FILE_LINE);
    }
    return _queue.front();
}

template <class T>
bool  
ThreadFifo<T>::empty() const
{
    ScopeLocker L(_myLock, false);

    return _queue.empty();
}

template <class T>
int  
ThreadFifo<T>::size() const
{
    ScopeLocker L(_myLock, false);

    return _queue.size();
}


#if 0
template <class T>
bool
ThreadFifo<T>::pop_blocking(T& val)
{
    bool retval = false;

    int error = _protectionSemaphore->lock();
    assert(error != -1);
    
    if (_signalToStop) {
        _countSemaphore->setValue(1);
        error = _protectionSemaphore->unlock();
        assert(error != -1);
        return false;
    }

    if ( ! _queue.empty()) {
        
        val = _queue.front();
        _queue.pop_front();
        
        if (_queue.size() > 0) {
            _countSemaphore->setValue(1);
        } else {
            _countSemaphore->setValue(0);
        }
        retval = true;
    }

    error = _protectionSemaphore->unlock();
    assert(error != -1);
    return retval;
}

template <class T>
bool
ThreadFifo<T>::pop_waiting(T& val)
{
    bool retval = false;

    // wait until the queue is non-empty and we can access our lock
    int error = _protectionSemaphore->lock(*_countSemaphore);
    assert(error == 0);

    if (_signalToStop) {
        _countSemaphore->setValue(1);
        error = _protectionSemaphore->unlock();
        assert(error != -1);
        return false;
    }

    //if (_queue.empty()) {
    //    cerr << "_protectionSemaphore = " << _protectionSemaphore->getValue() << endl;
    //    cerr << "_countSemaphore= " << _countSemaphore->getValue() << endl;
    //}
    assert(! _queue.empty());
    
    val = _queue.front();
    _queue.pop_front();
    retval = true;
    if (_queue.size() > 0) {
        _countSemaphore->setValue(1);
    } else {
        _countSemaphore->setValue(0);
    }

    error = _protectionSemaphore->unlock();
    assert(error != -1);
    return retval;
}


template <class T>
bool
ThreadFifo<T>::getFirst_waiting(T& val) {
    bool retval = false;

    // wait until the queue is non-empty and we can access our lock
    int error = _protectionSemaphore->lock(*_countSemaphore);
    assert(error == 0);

    if (_signalToStop) {
        _countSemaphore->setValue(1);
        error = _protectionSemaphore->unlock();
        assert(error != -1);
        return false;
    }

    assert(! _queue.empty());
    
    val = _queue.front();
    retval = true;

    error = _protectionSemaphore->unlock();
    assert(error != -1);
    return retval;
}


template <class T>
const T &
ThreadFifo<T>::peekElement(unsigned int theIndex)
{
    typedef typename std::deque<T>::iterator DequeIterator;
    if (theIndex >= _queue.size()) {
        return _queue.front();
    }
    DequeIterator iter(_queue.begin());
    while (theIndex > 0) {
        ++iter;
        --theIndex;
    }
    return *iter;
}

#endif

/* @} */


}; // namespace asl

#endif
