//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "CacheReader.h"

#include <asl/Auto.h>
#include <asl/Pump.h>
#include <asl/Assure.h>

using namespace std;
using namespace asl;

namespace y60 {

CacheReader::CacheReader (SoundCacheItemPtr myCacheItem)
    : _myURI (myCacheItem->getURI()),
      _myCacheItem(myCacheItem),
      _myCurFrame(0),
      _mySampleSink(0)
{
    AC_DEBUG << "CacheReader::CacheReader";
    // We assume that the cache item is ready for playback.
    ASSURE(_myCacheItem->isFull());
    _myCacheItem->incInUseCount();
}

CacheReader::~CacheReader() {
    AC_DEBUG << "CacheReader::~CacheReader (" << _myURI << ")";
    _myCacheItem->decInUseCount();
}

bool CacheReader::isSyncDecoder() const {
    return true;
}

std::string CacheReader::getName() const {
    return _myURI;
}

void CacheReader::setSampleSink(asl::ISampleSink* mySampleSink) {
    _mySampleSink = mySampleSink;
}

Time CacheReader::getDuration() const {
    return _myCacheItem->getNumFrames()/_myCacheItem->getSampleRate();
}

void CacheReader::seek (Time thePosition)
{
    _myCurFrame = int(thePosition*_myCacheItem->getSampleRate());
    AC_DEBUG << "CacheReader::seek(" << _myCurFrame << ")";
}

bool CacheReader::decode() {
    AC_TRACE << "CacheReader::decode(" << _myCurFrame << " of "
            << _myCacheItem->getNumFrames() << "frames )";
    if (_myCurFrame == _myCacheItem->getNumFrames()) {
        return true;
    }
    AudioBufferPtr myBuffer = _myCacheItem->getBuffer(_myCurFrame);
    _myCurFrame += myBuffer->getNumFrames();
    _mySampleSink->queueSamples(myBuffer);
    return (_myCurFrame == _myCacheItem->getNumFrames());
}

unsigned CacheReader::getSampleRate() {
    return _myCacheItem->getSampleRate();
}

unsigned CacheReader::getNumChannels() {
    return _myCacheItem->getNumChannels();
}

unsigned CacheReader::getCurFrame() const {
    return _myCurFrame;
}

void CacheReader::decodeEverything() {
    AC_WARNING << "CacheReader::decodeEverything was called.";
}

}

