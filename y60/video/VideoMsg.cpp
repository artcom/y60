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

#include "VideoMsg.h"

namespace y60 {

using namespace dom;

VideoMsg::VideoMsg(VideoMsgType theType, double theTime, std::vector<unsigned> theFrameSizes)
    : _myType(theType),
      _myTime(theTime),
      _myFramesSizes(theFrameSizes)
{
    for (unsigned i = 0; i < _myFramesSizes.size(); i++) {
        _myFrames.push_back(new unsigned char[_myFramesSizes[i]]);

    }
}

VideoMsg::~VideoMsg() {
    for (unsigned i = 0; i < _myFrames.size(); i++) {
        delete[] _myFrames[i];
    }
}

VideoMsg::VideoMsgType VideoMsg::getType() const {
    return _myType;
}

double VideoMsg::getTime() const {
    return _myTime;
}

unsigned VideoMsg::getSize(unsigned theBufferNum) const {
    return _myFramesSizes[theBufferNum];
}
unsigned char * VideoMsg::getBuffer(unsigned theBufferNum) const {
    return _myFrames[theBufferNum];
}

}
