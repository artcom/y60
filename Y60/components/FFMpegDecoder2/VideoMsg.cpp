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

#include "VideoMsg.h"

namespace y60 {

using namespace dom;
    
VideoMsg::VideoMsg(VideoMsgType theType, double theTimestamp, 
            unsigned theFrameSize)
    : _myType(theType),
      _myTimestamp(theTimestamp),
      _myFrameSize(theFrameSize)
{
    if (_myFrameSize) {
        _myFrame = new unsigned char[_myFrameSize];
    } else {
        _myFrame = 0;
    }
}

VideoMsg::~VideoMsg() {
    delete[] _myFrame;
}

VideoMsg::VideoMsgType VideoMsg::getType() const {
    return _myType;
}

double VideoMsg::getTimestamp() const {
    return _myTimestamp;
}

unsigned char * VideoMsg::getBuffer() const {
    return _myFrame;
}

}
