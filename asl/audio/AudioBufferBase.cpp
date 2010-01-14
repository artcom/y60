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

#include "AudioBufferBase.h"

#include <asl/base/Auto.h>
#include <asl/base/Assure.h>

namespace asl {

asl::ThreadLock AudioBufferBase::_myBuffersAllocatedLock;

int AudioBufferBase::_myNumBuffersAllocated = 0;

AudioBufferBase::AudioBufferBase() {
    AutoLocker<ThreadLock> myLocker(_myBuffersAllocatedLock);
    _myNumBuffersAllocated++;
}

AudioBufferBase::~AudioBufferBase() {
    AutoLocker<ThreadLock> myLocker(_myBuffersAllocatedLock);
    _myNumBuffersAllocated--;
}

void AudioBufferBase::setStartFrame(int myStartFrame) {
    _myStartFrame = myStartFrame;
}

int AudioBufferBase::getStartFrame() const {
    ASSURE_MSG(_myStartFrame != -1, "Start frame not set in getStartFrame(). Call init()!");
    return _myStartFrame;
}

int AudioBufferBase::getEndFrame() const {
    ASSURE_MSG(_myStartFrame != -1, "Start frame not set in getEndFrame(). Call init()!");
    return _myStartFrame+getNumFrames()-1;
}

int AudioBufferBase::getNumBuffersAllocated() {
    return _myNumBuffersAllocated;
}

std::ostream & operator<<(std::ostream & s, const AudioBufferBase& theBuffer) {
    return theBuffer.print(s);
}

}
