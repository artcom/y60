//=============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//
//=============================================================================

#include "HWSampleSink.h"
#include "Pump.h"

#include <asl/Auto.h>
#include <asl/Logger.h>

#ifdef USE_DASHBOARD
#include <asl/Dashboard.h>
#endif

namespace asl {

using namespace std;
using namespace asl;

HWSampleSink::HWSampleSink(const string & myName, SampleFormat mySampleFormat, 
                           unsigned mySampleRate, unsigned numChannels)
    : AudioTimeSource(0, mySampleRate),
      _myStopWhenEmpty(false),
      _numChannels(numChannels),
      _myState(STOPPED),
      _isDelayingPlay(false)
{
    AC_DEBUG << "HWSampleSink::HWSampleSink (" << _myName << ")";
    _myName = myName;
    _mySampleFormat = mySampleFormat;
    _mySampleRate = mySampleRate;

    _myLockedSelf = HWSampleSinkPtr(0);
    _isUsingBackupBuffer = false;
    _myPosInInputBuffer = 0;
    _numUnderruns = 0;

    _myFrameCount = 0;
    _myVolume = 1;
    _myVolumeFader = VolumeFaderPtr(new VolumeFader(_mySampleFormat));
    _numChannels = numChannels;
    _myBackupBuffer = AudioBufferPtr(createAudioBuffer(_mySampleFormat, 32, 
                _numChannels, _mySampleRate));
    _myBackupBuffer->clear();

}

HWSampleSink::~HWSampleSink() {
    AC_DEBUG << "~HWSampleSink (" << _myName << ", ref count=" << _mySelf.getRefCount() << ")";
    if (_myState != STOPPED) {
        AC_WARNING << "Deleting SampleSink that's still running!";
    }
    if (_numUnderruns > 0) {
        AC_WARNING << _numUnderruns << " underruns in SampleSink for " << _myName << endl;
    }
    AC_DEBUG << "~HWSampleSink end (" << _myName << ")";
}

void HWSampleSink::setSelf(const HWSampleSinkPtr& mySelf)
{
    _mySelf = mySelf;
    _myLockedSelf = HWSampleSinkPtr(0);
}

void HWSampleSink::play() {
    AC_DEBUG << "HWSampleSink::play (" << _myName << ")";
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    if (_myState != RUNNING) {
        _myLockedSelf = _mySelf.lock();
        _myVolumeFader->setVolume(_myVolume);
        changeState(RUNNING);
        AudioTimeSource::run();
    } else {
        AC_DEBUG << "HWSampleSink::play: Play received when alredy playing. Ignored.";
    }
}

void HWSampleSink::pause() {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    AC_DEBUG << "HWSampleSink::pause (" << _myName << ")";
    if (_myState == RUNNING) {
        changeState(PAUSING_FADE_OUT);
        _myVolumeFader->setVolume(0);
    } else {
        AC_DEBUG << "HWSampleSink::pause: Pause received in state " << 
                stateToString(getState()) << ". Ignored.";
    }
}

void HWSampleSink::stop(bool theRunUntilEmpty) {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    AC_DEBUG << "HWSampleSink::stop (" << _myName << ")";
    if (theRunUntilEmpty) {
//        cerr << "RunUntilEmpty" << endl;
        _myStopWhenEmpty = true;
    } else {
//        cerr << "!RunUntilEmpty" << endl;
        switch(_myState) {
            case RUNNING:
//                cerr << "Running" << endl;
                changeState(STOPPING_FADE_OUT);
                _myVolumeFader->setVolume(0);
                _myStopWhenEmpty = false;
                break;
            case PAUSING_FADE_OUT:
                changeState(STOPPING_FADE_OUT);
                _myStopWhenEmpty = false;
                break;
            case PAUSED:
                changeState(STOPPED);
                AudioTimeSource::stop();
                AC_TRACE << "PAUSED: _myBufferQueue.clear();";
                _myBufferQueue.clear();
                _myPosInInputBuffer = 0;
                _myStopWhenEmpty = false;
                break;
            default:
                AC_DEBUG << "HWSampleSink::stop: stop received in state " << 
                        stateToString(getState()) << ". Ignored.";
        }
    }
}

void HWSampleSink::delayedPlay(asl::Time theTimeToStart) {
    AC_DEBUG << "HWSampleSink::delayedPlay (" << _myName << ")";
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    // Theoretically, STOPPING_FADE_OUT or PAUSING_FADE_OUT could happen too.
    ASSURE(_myState == STOPPED || _myState == PAUSED); 
    _myTimeToStart = theTimeToStart;
    _myLockedSelf = _mySelf.lock();
    _myVolumeFader->setVolume(_myVolume);
    _isDelayingPlay = true;
    changeState(RUNNING);
    AudioTimeSource::run();
}

void HWSampleSink::setVolume(float theVolume) {
    ASSURE(theVolume <= 1.0);
    if (!asl::almostEqual(theVolume, _myVolume)) {
        AutoLocker<ThreadLock> myLocker(_myQueueLock);
        _myVolume = theVolume;
        if (_myState == STOPPED || _myState == PAUSED) {
            // No fade, immediate volume change.
            _myVolumeFader->setVolume(theVolume, 0);
        } else {
            _myVolumeFader->setVolume(theVolume);
        }
    }
}

void HWSampleSink::fadeToVolume(float theVolume, float theTime) {
    ASSURE(theVolume <= 1.0);
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    _myVolume = theVolume;
    _myVolumeFader->setVolume(theVolume, unsigned(theTime*getSampleRate()));
}

bool HWSampleSink::isPlaying() const {
    return getState() == RUNNING;
}

float HWSampleSink::getVolume() const {
    return _myVolumeFader->getVolume();//_myVolume;
}

bool HWSampleSink::queueSamples(AudioBufferPtr& theBuffer) {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    AC_TRACE << "queueSamples: " << *theBuffer << endl;
    _myBufferQueue.push_back(theBuffer);
    return true;
}

asl::Time HWSampleSink::getBufferedTime() const {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    unsigned myFramesBuffered = 0;
    list< AudioBufferPtr >::const_iterator it;
    for (it = _myBufferQueue.begin(); it != _myBufferQueue.end(); ++it) {
        myFramesBuffered += (*it)->getNumFrames();
    }
    myFramesBuffered -= _myPosInInputBuffer;
    return double(myFramesBuffered)/getSampleRate();
}

std::string HWSampleSink::getName() const {
    return _myName;
}

HWSampleSink::State HWSampleSink::getState() const {
    return _myState;
}

std::string HWSampleSink::stateToString(State theState) {
    switch (theState) {
        case STOPPED:
            return "STOPPED";
            break;
        case RUNNING:
            return "RUNNING";
            break;
        case STOPPING_FADE_OUT:
            return "STOPPING_FADE_OUT";
            break;
        case PAUSING_FADE_OUT:
            return "PAUSING_FADE_OUT";
            break;
        case PLAYBACK_DONE:
            return "PLAYBACK_DONE";
            break;
        case PAUSED:
            return "PAUSED";
            break;
        default:
            return "unknown HWSampleSink state";
    }
}

unsigned HWSampleSink::getNumUnderruns() const {
    return _numUnderruns;
}

void HWSampleSink::dumpState() {
    AC_DEBUG << "HWSampleSink '" << _myName << "' state:";
    AC_DEBUG << "  Sample rate: " << _mySampleRate;
    AC_DEBUG << "  Sample format: " << _mySampleFormat;
    AC_DEBUG << "  Number of channels: " << _numChannels;
    AC_DEBUG << string("State: ")+stateToString(_myState);
}

void HWSampleSink::deliverData(AudioBufferBase& theBuffer) {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
#ifdef USE_DASHBOARD
    MAKE_SCOPE_TIMER(Deliver_Data);
#endif
    
    AC_TRACE << "HWSampleSink::deliverData";
//    dumpState();
    unsigned curOutputFrame = 0;
    AudioBufferBase* mySourceBuffer = getNextBuffer();
    unsigned myFramesInBuffer = mySourceBuffer->getNumFrames()-_myPosInInputBuffer;
    unsigned numFramesToCopy = min(theBuffer.getNumFrames(), myFramesInBuffer);
    theBuffer.copyFrames(0, *mySourceBuffer, _myPosInInputBuffer, numFramesToCopy);
    curOutputFrame += numFramesToCopy;
    if (curOutputFrame == theBuffer.getNumFrames()) {
        _myPosInInputBuffer += numFramesToCopy;
    } else {
        _myPosInInputBuffer = 0;
        while (curOutputFrame < theBuffer.getNumFrames()) {
            if (!_isUsingBackupBuffer) {
                _myBufferQueue.pop_front();
            }
            mySourceBuffer = getNextBuffer();
            myFramesInBuffer = mySourceBuffer->getNumFrames();
            numFramesToCopy = min(theBuffer.getNumFrames()-curOutputFrame, myFramesInBuffer);
            theBuffer.copyFrames(curOutputFrame, *mySourceBuffer, 0, numFramesToCopy);
            curOutputFrame += numFramesToCopy;
        }
        if (curOutputFrame != theBuffer.getNumFrames()) {
            AC_ERROR << "Internal error in HWSampleSink::deliverData()";
        }
        _myPosInInputBuffer = numFramesToCopy;
    }
    if (_isUsingBackupBuffer) {
        _myPosInInputBuffer = 0;
    }
    if (_myPosInInputBuffer == mySourceBuffer->getNumFrames()) {
        _myPosInInputBuffer = 0;
        if (!_isUsingBackupBuffer) {
            _myBufferQueue.pop_front();
        }
    }

    {
#ifdef USE_DASHBOARD
        MAKE_SCOPE_TIMER(Volume_Fader);
#endif
        _myVolumeFader->apply(theBuffer, _myFrameCount);
    }
    _myFrameCount += theBuffer.getNumFrames();
    addFramesToTime(theBuffer.getNumFrames());
//    theBuffer.dumpSamples(cerr, 0, 512);
/*
    // This is code to insert debug samples into a stream. By enabling this, you can 
    // see where buffer boundaries are when you record the sound.
    switch(_myState) {
        case RUNNING:
            theBuffer.setMarker(1);
            break;
        case FADING_OUT:
            theBuffer.setMarker(-1);
//            theBuffer.dumpSamples(cerr, 0, 512);
            break;
        default:
            AC_WARNING << "Strange state...";
    }
*/
    // Check if fadeouts are finished.
    if ((_myState == STOPPING_FADE_OUT && almostEqual(_myVolumeFader->getVolume(), 0.0)) ||
            _myState == PLAYBACK_DONE)
    {
#ifdef USE_DASHBOARD
        MAKE_SCOPE_TIMER(Stop_Sink);
#endif
        _isDelayingPlay = false;
        changeState(STOPPED);
        AC_TRACE << "_myBufferQueue.clear();";
        _myBufferQueue.clear();
        _myPosInInputBuffer = 0;
        _myLockedSelf = HWSampleSinkPtr(0);
        AudioTimeSource::stop();
    }
    if (_myState == PAUSING_FADE_OUT && almostEqual(_myVolumeFader->getVolume(), 0.0)) {
        changeState(PAUSED);
        _myLockedSelf = HWSampleSinkPtr(0);
        AudioTimeSource::pause();
    }
    if (_isDelayingPlay && _myTimeToStart <= getCurrentTime()) {
        // Delay is finished, audio data available.
        _isDelayingPlay = false;
    }
    AC_TRACE << "HWSampleSink::~deliverData";
}

unsigned HWSampleSink::getNumChannels() const {
    return _numChannels;
}

unsigned HWSampleSink::getSampleRate() const {
    return _mySampleRate;
}

unsigned HWSampleSink::getBytesPerFrame() const {
    return _numChannels*getBytesPerSample(getSampleFormat());
}

unsigned HWSampleSink::getBytesPerSecond() const {
    return _mySampleRate*getBytesPerFrame();
}

SampleFormat HWSampleSink::getSampleFormat() const {
    return _mySampleFormat;
}

void HWSampleSink::lock() {
    _myQueueLock.lock();
}

void HWSampleSink::unlock() {
    _myQueueLock.unlock();
}
        
void HWSampleSink::changeState(State newState) {
    AC_DEBUG << "HWSampleSink ( " << _myName <<"): " << stateToString(_myState) << " -> "
        << stateToString(newState) << ", " << _isDelayingPlay;
    _myState = newState;
}

AudioBufferBase* HWSampleSink::getNextBuffer() {
    ASSURE_MSG(getState() != STOPPED,
            "HWSampleSink::getNextBuffer() should not be called when the sink isn't active.");
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    if (_isDelayingPlay) {
        _isUsingBackupBuffer = true;
        return &(*_myBackupBuffer);
    }
    if (_myBufferQueue.empty()) {
        if (_myState != PLAYBACK_DONE) {
            if (_myStopWhenEmpty || _myState == STOPPING_FADE_OUT)
            {
                _myStopWhenEmpty = false;
                changeState(PLAYBACK_DONE);
                AudioTimeSource::stop();
                
            } else if (_myState == PAUSING_FADE_OUT) {                
               ; //deliverData will hopeful-and-eventually put us into PAUSED state (dk)

            } else {
                _numUnderruns++;
                if (_numUnderruns == 1) {
                    AC_WARNING << "Underrun for sample sink " << _myName;
                }             
            }
        }
        _isUsingBackupBuffer = true;
        ASSURE_MSG(_myPosInInputBuffer == 0,
                "Position in input buffer should be zero on underrun.");
        return &(*_myBackupBuffer);
    }
    _isUsingBackupBuffer = false;
    AudioBufferBase* myBuffer = &(*_myBufferQueue.front());
    return myBuffer;
}

}

