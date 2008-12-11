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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ac_video_VideoMsg_h_
#define _ac_video_VideoMsg_h_

#include <asl/base/Ptr.h>
#include <asl/dom/Value.h>

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
