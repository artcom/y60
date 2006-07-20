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
//
//   $RCSfile: FrameCache.h,v $
//
//   $Author: jens $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _ac_video_FrameCache_h_
#define _ac_video_FrameCache_h_

#include <asl/Ptr.h>
#include <asl/ThreadSemaphore.h>
#include <asl/ThreadLock.h>
#include <list>

namespace y60 {
    
    /**
     * @ingroup Y60video
     * Universal locking cache of timestamped Video frames. It has basically the same 
     * interface as stl lists but blocks if pop_back or pop_front is called for an empty
     * container.
     *
     * @bug [TS] operations involving a semaphore and a ThreadLock are not atomic.
     *           This may be a potential spinlock-bug
     *
     */
    class FrameCache {
    public:
        class VideoFrame {
        public:
            VideoFrame(unsigned theBufferSize);
            ~VideoFrame();
    
            void setTimestamp(double theTimestamp);
            double getTimestamp() const;
            unsigned char* getBuffer();

        private:
            double _myTimestamp;
            unsigned char* _myBuffer;
            bool _myFree;
        };
    
        typedef asl::Ptr<VideoFrame> VideoFramePtr;
    
    public:
        /**
         * Creates a FrameCache. 
         * @param theErrorTimeout timeout for the blocking pop-operations on the cache.          
         */
        FrameCache(long theErrorTimeout = asl::ThreadSemaphore::WAIT_INFINITE);
        ~FrameCache();
        /**
         * adds theFrame to the cache. Wakes up thread waiting in pop_back or pop_front.
         * @param theFrame frame to add to the cache
         */
        void push_back(VideoFramePtr theFrame);
        /**
         * Retrieves the top element of the cache and removes it. If the cache
         * is empty, blocks until an element is there.
         * @throws TimeoutException if the waiting timed out in the time given at
         *         construction of this object.
         * @throws asl::ThreadSemaphore::ClosedException if close is called while in wait.
         * @return popped VideoFrame
         */
        VideoFramePtr pop_front();
        /**
         * @return Item in the front of the cache or VideoFramePtr(NULL) if the cache is empty.
         */
        VideoFramePtr front() const;
        /**
         * @return Item in the back of the cache or VideoFramePtr(NULL) if the cache is empty.
         */
        VideoFramePtr back() const;
        unsigned size() const;
        /**
         * Closes this Cache. No further reading or writing is possible. Threads pending in 
         * a pop_back or pop_front are woken up with a asl::ThreadSemaphore::ClosedException.
         */
        void close();
        /**
         * Resets a previously cleared cache.
         */
        void reset();
        /**
         * Clears the cache. Threads pending in a pop_back or pop_front are woken up
         * with a asl::ThreadSemaphore::ClosedException.
         */
        void clear();
        
        DEFINE_EXCEPTION(TimeoutException, asl::Exception);
        /**
         * @return the timeout for pop_back and pop_front operations.
         */
        long getErrorTimeout() const;
        /**
         * @param theTimeout timeout for pop_back and pop_front operations.
         */
        void setErrorTimeout(long theTimeout);

    private:
        asl::ThreadSemaphore _mySemaphore;
        mutable asl::ThreadLock _myListLock;
        std::list<VideoFramePtr> _myList;
        long _myErrorTimeout;
    };
}

#endif // _ac_video_FrameCache_h_
