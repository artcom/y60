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

// own header
#include "SoundCacheItem.h"

#include "SoundManager.h"

#include <asl/base/Assure.h>

using namespace std;
using namespace asl;

namespace y60 {

SoundCacheItem::SoundCacheItem (const string& myURI)
    : _myURI (myURI),
      _myFramesDecoded(0),
      _myTotalFrames(-1),
      _myInUseCount(0)
{
    AC_TRACE << "SoundCacheItem::SoundCacheItem";
}

SoundCacheItem::~SoundCacheItem() {
    AC_TRACE << "SoundCacheItem::~SoundCacheItem";
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
    AC_TRACE << "SoundCacheItem::doneCaching: _myTotalFrames= " << _myTotalFrames
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
        AC_TRACE << "SoundCacheItem::getBuffer: partial buffer";
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
    AC_TRACE << "SoundCacheItem::queueSamples (frame " << theBuffer->getStartFrame() << ")";
    if (!_myBuffers.empty()) {
        BufferMap::iterator it = _myBuffers.end();
        --it;
        AudioBufferPtr myLastBuffer = it->second;
        //int myLastFrame = myLastBuffer->getEndFrame();
        ASSURE_MSG(theBuffer->getStartFrame() == myLastBuffer->getEndFrame()+1,
                "Adding buffer to SoundCacheItem that doesn't fit to end of current buffer list.");
    }

    // Actually append the buffer.
    _myBuffers[theBuffer->getStartFrame()] = theBuffer;
    _myFramesDecoded += theBuffer->getNumFrames();

    if (getMemUsed() > SoundManager::get().getMaxCacheItemSize()) {
        AC_WARNING << "SoundCacheItem::queueSamples: Max. memory usage per cache item exceeded.";
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

