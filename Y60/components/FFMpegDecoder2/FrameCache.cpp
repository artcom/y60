#include "FrameCache.h"

#include <asl/Logger.h>

using namespace asl;

namespace y60 {

FrameCache::FrameCache(long theErrorTimeout) : _myErrorTimeout(theErrorTimeout) {
}

FrameCache::~FrameCache() {
}

void FrameCache::push_back(VideoFramePtr theFrame) {
    AC_DEBUG << "push_back {";
    _myListLock.lock();
    _myList.push_back(theFrame);
    _mySemaphore.post();
    _myListLock.unlock();
    AC_DEBUG << "} push_back";
}

FrameCache::VideoFramePtr FrameCache::pop_front() {
    AC_DEBUG << "pop_front {";
    if (!_mySemaphore.wait(_myErrorTimeout)) {
        throw FrameCache::TimeoutException("Timeout", PLUS_FILE_LINE);
    }
    // XXX We should lock atomically in the wait here. This does not work if multiple threads
    // call pop_xxx on the Cache!
    _myListLock.lock();
    VideoFramePtr myPopper = _myList.front();
    _myList.pop_front();
    _myListLock.unlock();
    AC_DEBUG << "} pop_front";
    return myPopper;
}

FrameCache::VideoFramePtr FrameCache::front() const {
    _myListLock.lock();
    VideoFramePtr myValue = _myList.front();
    _myListLock.unlock();
    return myValue;
}

FrameCache::VideoFramePtr FrameCache::back() const {
    _myListLock.lock();
    VideoFramePtr myValue = _myList.back();
    _myListLock.unlock();
    return myValue;
}

unsigned FrameCache::size() const {
    _myListLock.lock();
    unsigned myListSize = _myList.size();
    _myListLock.unlock();
    return myListSize;
    //return _myList.size(); 
}
        
void FrameCache::close() { 
    AC_DEBUG << "FrameCache::close";
    _mySemaphore.close(); 
}

void FrameCache::reset() {
    AC_DEBUG << "FrameCache::reset";
    _mySemaphore.reset(_myList.size()); 
}

void FrameCache::clear() {
    AC_DEBUG << "FrameCache::clear";
    _mySemaphore.close(); 
    _myList.clear(); 
    _mySemaphore.reset(); 
}

long FrameCache::getErrorTimeout() const {
    return _myErrorTimeout; 
}

void FrameCache::setErrorTimeout(long theTimeout) { 
    _myErrorTimeout = theTimeout; 
}

FrameCache::VideoFrame::VideoFrame(unsigned theBufferSize) {
    _myBuffer = new unsigned char[theBufferSize];
    _myFree = true;
    _myTimestamp = 0;
}

FrameCache::VideoFrame::~VideoFrame() {
    delete[] _myBuffer;
    _myBuffer = 0;
}

void FrameCache::VideoFrame::setTimestamp(double theTimestamp) {
    _myTimestamp = theTimestamp;
}

double FrameCache::VideoFrame::getTimestamp() const {
    return _myTimestamp;
}

unsigned char* FrameCache::VideoFrame::getBuffer() {
    return _myBuffer;
}



}

