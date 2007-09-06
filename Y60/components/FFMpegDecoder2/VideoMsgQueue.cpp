//============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "VideoMsgQueue.h"

#include <asl/Logger.h>

using namespace asl;

namespace y60 {

VideoMsgQueue::VideoMsgQueue() {
}

VideoMsgQueue::~VideoMsgQueue() {
}

void VideoMsgQueue::push_back(VideoMsgPtr theFrame) {
    AC_DEBUG << "push_back {";
    _myListLock.lock();
    _myList.push_back(theFrame);
    _mySemaphore.post();
    _myListLock.unlock();
    AC_DEBUG << "} push_back";
}

void VideoMsgQueue::push_front(VideoMsgPtr theFrame) {
    AC_DEBUG << "push_front {";
    _myListLock.lock();
    _myList.push_front(theFrame);
    _mySemaphore.post();
    _myListLock.unlock();
    AC_DEBUG << "} push_front";
}

VideoMsgPtr VideoMsgQueue::pop_front() {
    AC_DEBUG << "pop_front {";
    _mySemaphore.wait(asl::ThreadSemaphore::WAIT_INFINITE);
    _myListLock.lock();
    VideoMsgPtr myPopper = _myList.front();
    _myList.pop_front();
    _myListLock.unlock();
    AC_DEBUG << "} pop_front";
    return myPopper;
}

VideoMsgPtr VideoMsgQueue::front() const {
    _myListLock.lock();
    VideoMsgPtr myValue = _myList.front();
    _myListLock.unlock();
    return myValue;
}

VideoMsgPtr VideoMsgQueue::back() const {
    _myListLock.lock();
    VideoMsgPtr myValue = _myList.back();
    _myListLock.unlock();
    return myValue;
}

unsigned VideoMsgQueue::size() const {
    _myListLock.lock();
    unsigned myListSize = _myList.size();
    _myListLock.unlock();
    return myListSize;
    //return _myList.size(); 
}
        
void VideoMsgQueue::close() { 
    AC_DEBUG << "VideoMsgQueue::close";
    _mySemaphore.close(); 
}

void VideoMsgQueue::reset() {
    AC_DEBUG << "VideoMsgQueue::reset";
    _mySemaphore.reset(_myList.size()); 
}

void VideoMsgQueue::clear() {
    AC_DEBUG << "VideoMsgQueue::clear";
    _mySemaphore.close(); 
    _myList.clear(); 
    _mySemaphore.reset(); 
}

}

