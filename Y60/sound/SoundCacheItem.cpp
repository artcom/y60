//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "SoundCacheItem.h"
#include "SoundManager.h"

#include <asl/Assure.h>

using namespace std;
using namespace asl;

namespace y60 {

SoundCacheItem::SoundCacheItem (const string& myURI)
    : _myURI (myURI),
      _myFramesDecoded(0),
      _myTotalFrames(-1),
      _myInUseCount(0)
{
    AC_DEBUG << "SoundCacheItem::SoundCacheItem";
}

SoundCacheItem::~SoundCacheItem() {
    AC_DEBUG << "SoundCacheItem::~SoundCacheItem";
}

std::string SoundCacheItem::getURI() const {
    return _myURI;
}

unsigned SoundCacheItem::getMemUsed() const {
    if (_myFramesDecoded == 0) {
        return 0;
    } else {
        AudioBufferPtr myBuffer = (*_myBuffers.begin()).second;
        return _myFramesDecoded*myBuffer->getBytesPerFrame();
    }
}

bool SoundCacheItem::isFull() const {
    AC_TRACE << "SoundCacheItem::isFull: _myTotalFrames= " << _myTotalFrames 
            << ", _myFramesDecoded= " << _myFramesDecoded;
    return (_myTotalFrames == _myFramesDecoded);
}

void SoundCacheItem::doneCaching(int theTotalFrames) {
    if (theTotalFrames != -1) {
        _myTotalFrames = theTotalFrames;
        if (_myTotalFrames < _myFramesDecoded) {
            AC_WARNING << "Decoded frames > total frames. That doesn't make sense.";
            AC_WARNING << "_myTotalFrames = " << _myTotalFrames << 
                ", _myFramesDecoded = " << _myFramesDecoded;
        }
    }
    AC_DEBUG << "SoundCacheItem::doneCaching: _myTotalFrames= " << _myTotalFrames 
            << ", _myFramesDecoded= " << _myFramesDecoded;
    if (!isFull()) {
        SoundManager::get().deleteCacheItem(_myURI);
    }
}

AudioBufferPtr SoundCacheItem::getBuffer(int theStartFrame) const {
    ASSURE(isFull());
    BufferMap::const_iterator it = _myBuffers.lower_bound(theStartFrame);
    AudioBufferPtr myBuffer = it->second;
    if (myBuffer->getStartFrame() == theStartFrame) {
        AC_TRACE << "SoundCacheItem::getBuffer: entire buffer";
        return myBuffer;
    } else {
        AC_DEBUG << "SoundCacheItem::getBuffer: partial buffer";
        --it;
        AudioBufferPtr myOrigBuffer = it->second;
        unsigned myRelStartFrame = theStartFrame-myOrigBuffer->getStartFrame();
        AudioBufferPtr myPartialBuffer = AudioBufferPtr(myOrigBuffer->partialClone
                (myRelStartFrame, myOrigBuffer->getNumFrames()));
        return myPartialBuffer;
    }
}

int SoundCacheItem::getNumFrames() const {
    return _myFramesDecoded;
}

unsigned SoundCacheItem::getSampleRate() const {
    return _myBuffers.find(0)->second->getSampleRate();
}

unsigned SoundCacheItem::getNumChannels() const {
    return _myBuffers.find(0)->second->getNumChannels();
}

asl::Time SoundCacheItem::getLastUsedTime() const {
    return _myLastUseTime;
}

void SoundCacheItem::incInUseCount() {
    _myInUseCount++;
}

void SoundCacheItem::decInUseCount() {
    _myInUseCount--;
    if (_myInUseCount == 0) {
        _myLastUseTime = Time();
    }
    ASSURE(_myInUseCount >= 0);
}

bool SoundCacheItem::isInUse() const {
    return _myInUseCount > 0;
}

bool SoundCacheItem::queueSamples(asl::AudioBufferPtr& theBuffer) {
    AC_TRACE << "SoundCacheItem::addBuffer (frame " << theBuffer->getStartFrame() << ")";
    if (!_myBuffers.empty()) {
        BufferMap::iterator it = _myBuffers.end();
        --it;
        AudioBufferPtr myLastBuffer = it->second;
        int myLastFrame = myLastBuffer->getEndFrame();
        ASSURE_MSG(theBuffer->getStartFrame() == myLastBuffer->getEndFrame()+1, 
                "Adding buffer to SoundCacheItem that doesn't fit to end of current buffer list.");
    }
    
    // Actually append the buffer.
    _myBuffers[theBuffer->getStartFrame()] = theBuffer;
    _myFramesDecoded += theBuffer->getNumFrames();

    if (getMemUsed() > SoundManager::get().getMaxCacheItemSize()) {
        AC_WARNING << "SoundCacheItem::addBuffer: Max. memory usage per cache item exceeded.";
        AC_WARNING << "    Sound: " << _myURI << ", Max. mem: " 
                << SoundManager::get().getMaxCacheItemSize();
        AC_WARNING << "    Disabling cache for this item.";
        SoundManager::get().deleteCacheItem(_myURI);
        return false;
    } else {
        return true;
    }
}

}

