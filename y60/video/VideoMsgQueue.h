/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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
#include "y60_video_settings.h"

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
    class Y60_VIDEO_DECL VideoMsgQueue {
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
         * Do we have a eof in queue
         */
        bool hasEOF();

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
