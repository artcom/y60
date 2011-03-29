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

#include "HWSampleSink.h"
#include "Pump.h"

#include <asl/base/Auto.h>
#include <asl/base/Logger.h>

#ifdef USE_DASHBOARD
#include <asl/base/Dashboard.h>
#endif

namespace asl {

using namespace std;

HWSampleSink::HWSampleSink(const string & myName, SampleFormat mySampleFormat,
                           unsigned mySampleRate, unsigned numChannels)
    : SampleSource(myName, mySampleFormat, mySampleRate, numChannels),
      AudioTimeSource(0, mySampleRate),
      _myState(STOPPED),
      _myStopWhenEmpty(false),
      _isDelayingPlay(false)
{
    AC_DEBUG << "HWSampleSink::HWSampleSink (" << _myName << ")";

    _myLockedSelf = HWSampleSinkPtr();
    _isUsingBackupBuffer = false;
    _myPosInInputBuffer = 0;
    _numUnderruns = 0;

    _myFrameCount = 0;
    _myVolume = 1;
    _myVolumeFader = VolumeFaderPtr(new VolumeFader(_mySampleFormat));
    _myVolumeFader->setVolume(1, 0);
    _myBackupBuffer = AudioBufferPtr(createAudioBuffer(_mySampleFormat, 32,
                                                       _numChannels, SampleSource::_mySampleRate));
    _myBackupBuffer->clear();


    _myPumpTimeSource = ExternalTimeSourcePtr(new ExternalTimeSource());
}

HWSampleSink::~HWSampleSink() {
    AC_DEBUG << "~HWSampleSink (" << _myName << ", ref count=" << _mySelf.use_count() << ")";
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
    _myLockedSelf = HWSampleSinkPtr();
}

void HWSampleSink::play() {
    AC_DEBUG << "HWSampleSink::play (" << _myName << ")";
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    if (_myState != RUNNING) {
        _myLockedSelf = _mySelf.lock();
        _myVolumeFader->setVolume(_myVolume);
        changeState(RUNNING);
        AudioTimeSource::run();
        _myPumpTimeSource->run();
    } else {
        AC_DEBUG << "HWSampleSink::play: Play received when already playing. Ignored.";
    }
}

void HWSampleSink::pause() {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    AC_DEBUG << "HWSampleSink::pause (" << _myName << ")";
    if (_myState == RUNNING) {
        changeState(PAUSING_FADE_OUT);
        _myVolumeFader->setVolume(0);
        _myPumpTimeSource->pause();

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
                AudioTimeSource::stop();
                //_myPumpTimeSource->stop();
                break;
            case PAUSING_FADE_OUT:
                changeState(STOPPING_FADE_OUT);
                _myStopWhenEmpty = false;
                break;
			case PAUSED:{
                changeState(STOPPED);
                AudioTimeSource::stop();
                AC_TRACE << "PAUSED: _myBufferQueue.clear();";
                _myBufferQueue.clear();
                _myPosInInputBuffer = 0;
                _myStopWhenEmpty = false;
                _myPumpTimeSource->stop();
				break;}
            case STOPPED:
                _myBufferQueue.clear();
                break;
			default:{
                AC_DEBUG << "HWSampleSink::stop: stop received in state " <<
					stateToString(getState()) << ". Ignored.";}
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
void HWSampleSink::setVolumes(const std::vector<float> & theVolumes) {
    ASSURE(theVolumes.size() > 0.0);
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    // no fading here
    _myVolumeFader->setVolumes(theVolumes);
    _myVolume = theVolumes[0];
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
    return _myVolumeFader->getVolume();
}

bool HWSampleSink::queueSamples(AudioBufferPtr& theBuffer) {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    AC_TRACE << "queueSamples: " << *theBuffer << endl;
    _myBufferQueue.push_back(theBuffer);
    return true;
}

asl::Time HWSampleSink::getPumpTime() const {
    return _myPumpTimeSource->getCurrentTime();
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
    AC_TRACE << "HWSampleSink '" << _myName << "' state:";
    AC_TRACE << "  Sample rate: " << SampleSource::_mySampleRate;
    AC_TRACE << "  Sample format: " << _mySampleFormat;
    AC_TRACE << "  Number of channels: " << _numChannels;
    AC_TRACE << "  State: " + stateToString(_myState);
}

void HWSampleSink::deliverData(AudioBufferBase& theBuffer) {
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
#ifdef USE_DASHBOARD
    MAKE_SCOPE_TIMER(Deliver_Data);
#endif

    AC_TRACE << "HWSampleSink::deliverData";
    dumpState();
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
    if (_myState != STOPPING_FADE_OUT) {
        // Keep sample-accurate track of time spent playing.
        // In state STOPPING_FADE_OUT, time is set to 0 immediately. It stays
        // that way while in that state unless set from the outside (e.g. a seek happens).
        addFramesToTime(theBuffer.getNumFrames());
    }
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
        _myLockedSelf = HWSampleSinkPtr();
        _myPumpTimeSource->stop();

    }
    if (_myState == PAUSING_FADE_OUT && almostEqual(_myVolumeFader->getVolume(), 0.0)) {
        changeState(PAUSED);
        _myLockedSelf = HWSampleSinkPtr();
        AudioTimeSource::pause();
    }
    if (_isDelayingPlay && _myTimeToStart <= getCurrentTime()) {
        // Delay is finished, audio data available.
        _isDelayingPlay = false;
    }
    AC_TRACE << "HWSampleSink::~deliverData";
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

    switch (_myState) {
    case RUNNING:
        enable();
        break;
    case STOPPED:
    case PAUSED:
        disable();
        break;
    case STOPPING_FADE_OUT: // avoid warning
    case PAUSING_FADE_OUT: // avoid warning
    case PLAYBACK_DONE: // avoid warning
    case DELAYING_PLAY: // avoid warning
        break;
    }
}

AudioBufferBase* HWSampleSink::getNextBuffer() {
    ASSURE_MSG(getState() != STOPPED,
               "HWSampleSink::getNextBuffer() should not be called when the sink isn't active.");
    AutoLocker<ThreadLock> myLocker(_myQueueLock);
    if (_isDelayingPlay) {
        _isUsingBackupBuffer = true;
        return _myBackupBuffer.get();
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
                //if ((_numUnderruns % 10) == 1) {
                    AC_WARNING << "Underrun for sample sink " << _myName << " num: " << _numUnderruns;
                //}
            }
        }
        _isUsingBackupBuffer = true;
        ASSURE_MSG(_myPosInInputBuffer == 0,
                "Position in input buffer should be zero on underrun.");
        return _myBackupBuffer.get();
    }
    _isUsingBackupBuffer = false;
    ASSURE_WITH(AssurePolicy::Abort,!_myBufferQueue.empty());
    AudioBufferBase* myBuffer = &(*_myBufferQueue.front());
    return myBuffer;
}

}

