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
//   $RCSfile: VideoMsgQueue.h,v $
//
//   $Author: jens $
//
//   $Revision: 1.2 $
//
//=============================================================================

#ifndef _ac_video_VideoMsgQueue_h_
#define _ac_video_VideoMsgQueue_h_

#include "VideoMsg.h"

#include <asl/base/ThreadSemaphore.h>
#include <asl/base/ThreadLock.h>
#include <list>

namespace y60 {
    
    /**
     * @ingroup Y60video
     * Message queue between a video decoder thread and a thread that wants the
     * decoded frames. Usually, each message contains a single frame of video.
     * There is also an 'end of file' message that gets put into the queue when
     * the decoder thread has decoded the last frame in the file.
     *
     * This queue is a single-consumer message queue. Multiple consumers are not
     * supported or expected.
     *
     */
    class VideoMsgQueue {
    public:
        /**
         * Creates a VideoMsgQueue. 
         */
        VideoMsgQueue();
        ~VideoMsgQueue();
        /**
         * adds theFrame to the queue. Wakes up thread waiting in pop_back or pop_front.
         * @param theFrame frame to add to the cache
         */
        void push_back(VideoMsgPtr theFrame);
        /**
         * adds theFrame to the front of the queue. Wakes up thread waiting in pop_back or pop_front.
         * @param theFrame frame to add at the front of the cache
         */
        void push_front(VideoMsgPtr theFrame);
        /**
         * Retrieves the top element of the queue and removes it. If the queue
         * is empty, blocks until an element is there.
         * @throws asl::ThreadSemaphore::ClosedException if close is called while in wait.
         * @return popped VideoMsg
         */
        VideoMsgPtr pop_front();
        /**
         * @return Item in the front of the queue or VideoMsgPtr(NULL) if the queue is empty.
         */
        VideoMsgPtr front() const;
        /**
         * @return Item in the back of the queue or VideoMsgPtr(NULL) if the queue is empty.
         */
        VideoMsgPtr back() const;

        unsigned size() const;
        /**
         * Closes this queue. No further reading or writing is possible. Threads pending in 
         * a pop_front are woken up with a asl::ThreadSemaphore::ClosedException.
         */
        void close();
        /**
         * Resets a previously cleared queue.
         */
        void reset();
        /**
         * Clears the queue. Threads pending in a pop_front are woken up
         * with a asl::ThreadSemaphore::ClosedException.
         */
        void clear();

        /**
         * Dumps the queue.
         */
        void dump();
        
    private:
        asl::ThreadSemaphore _mySemaphore;
        mutable asl::ThreadLock _myListLock;
        std::list<VideoMsgPtr> _myList;
    };
}

#endif // _ac_video_VideoMsgQueue_h_
