//=============================================================================
//
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#include "SampleSource.h"

namespace asl {

using namespace std;

    SampleSource::SampleSource(const std::string & theName, SampleFormat theSampleFormat, 
                               unsigned theSampleRate, unsigned theNumChannels) :
        _numChannels(theNumChannels),
        _mySampleRate(theSampleRate),
        _mySampleFormat(theSampleFormat),
        _myEnableState(false)
    {
        AC_DEBUG << "SampleSource::SampleSource (" << _myName << ")";
    }


    SampleSource::~SampleSource() {
        AC_DEBUG << "~SampleSource end (" << _myName << ")";
    }

    
    unsigned SampleSource::getNumChannels() const {
        return _numChannels;
    }

    
    unsigned SampleSource::getSampleRate() const {
        return _mySampleRate;         
    }           


    inline unsigned SampleSource::getBytesPerSecond() const {
        return _mySampleRate*getBytesPerFrame();
    }


    inline SampleFormat SampleSource::getSampleFormat() const {
        return _mySampleFormat;
    }


    inline unsigned SampleSource::getBytesPerFrame() const {
        return _numChannels*getBytesPerSample(getSampleFormat());        
    }

    
    std::string SampleSource::getName() const {
        return _myName;
    }


    void SampleSource::lock() {
        _mySampleSourceLock.lock();
    }


    void SampleSource::unlock() {
        _mySampleSourceLock.unlock();
    }


    void SampleSource::enable() {
        _myEnableState = true;
    }


    void SampleSource::disable() {
        _myEnableState = false;
    }


    bool SampleSource::isEnabled() {
        return _myEnableState;
    }

}

