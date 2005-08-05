//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#include "AudioBufferBase.h"

#include <asl/Auto.h>

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

int AudioBufferBase::getNumBuffersAllocated() {
        return _myNumBuffersAllocated;
}

std::ostream & operator<<(std::ostream & s, const AudioBufferBase& theBuffer) {
    return theBuffer.print(s);
}

}
