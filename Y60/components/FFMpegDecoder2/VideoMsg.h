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

#ifndef _ac_video_VideoMsg_h_
#define _ac_video_VideoMsg_h_

#include <asl/Ptr.h>
#include <dom/Value.h>

namespace y60 {

class VideoMsg {
public:
    enum VideoMsgType {
        MSG_FRAME,
        MSG_EOF
    };
    
    // Time is in seconds.
    VideoMsg(VideoMsgType theType, double theTime = 0, 
            unsigned theFrameSize = 0);
    ~VideoMsg();

    VideoMsgType getType() const;
    double getTime() const;
    unsigned char * getBuffer() const;

private:
    VideoMsgType _myType;
    double _myTime;
    unsigned _myFrameSize;
    unsigned char * _myFrame;
};

// Needed since the default pointer class uses an allocator with a thread-local
// free list that breaks when all rasters are allocated in one thread and deallocated
// in another.
typedef asl::Ptr<VideoMsg, asl::MultiProcessor, 
        asl::PtrHeapAllocator<asl::MultiProcessor> > VideoMsgPtr;
    
}

#endif
