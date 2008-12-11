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

//#define ASSURE_POLICY AssurePolicy::Abort
#include "Pump.h"
#include "DummyPump.h"
#include "AudioException.h"
#ifdef WIN32
#include "DirectSoundPump.h"
#elif LINUX
#include "ALSAPump.h"
#elif OSX
#include "CoreAudioPump.h"
#endif

#include <asl/base/Auto.h>
#include <asl/base/os_functions.h>
#include <asl/base/string_functions.h>

#ifdef USE_DASHBOARD
#include <asl/base/Dashboard.h>
#endif


using namespace std;

namespace asl {

bool Pump::_myUseRealPump = true;

Pump::Pump (SampleFormat mySF, unsigned myTimeStartDelay) 
    : AudioTimeSource(myTimeStartDelay, getSampleRateConfig()),
      _myRunning(false),
      _myNumUnderruns(0),
      _mySF(mySF),
      _myVolumeFader(mySF),
      _doBritzelTest(false),
      _numClicks(0),
      _myLastFrame(0)
{
    AC_INFO << "Pump::Pump";

#ifdef WIN32
    double myLatency= 0.05;
#else
    double myLatency= 0.05; // 0.04 causes hundreds of underruns in the tests, 0.05 causes none (on caspar64)
#endif
    get_environment_var_as("Y60_SOUND_LATENCY", myLatency);
    _myLatency = myLatency;
    _numOutputChannels = 2;
    get_environment_var_as("Y60_SOUND_NUM_OUTPUT_CHANNELS", _numOutputChannels);
    _mySampleRate = 44100;
    get_environment_var_as("Y60_SOUND_SAMPLE_RATE", _mySampleRate);
    _curFrame = 0;

    _myTempBuffer = AudioBufferPtr(createAudioBuffer(getNativeSampleFormat(), 
            0, getNumOutputChannels(), getNativeSampleRate()));
    _myVolumeFader.setVolume(1.0);
}

Pump::~Pump() {
    AC_INFO << "Pump::~Pump";
    for (unsigned i=0; i<_mySampleSinks.size(); ++i) {
        SampleSourcePtr mySink = _mySampleSinks[i].lock();
        if (mySink) {
            if (mySink->isEnabled())
            {
                AC_WARNING << "Audio Pump being deleted, but " << 
                    mySink->getName() << " is still active (state: ";
            }
        }
    }
    // Check if there are buffers other than _myTempBuffer allocated
    if (_numClicks > 0) {
        AC_WARNING << "Audio pump: " << _numClicks << " buffers with clicks detected.";
    }
}

HWSampleSinkPtr Pump::createSampleSink (const string & theName) 
{
    HWSampleSinkPtr mySink = HWSampleSinkPtr(new HWSampleSink(theName, 
                                                              getNativeSampleFormat(), getNativeSampleRate(), getNumOutputChannels()));
    mySink->setSelf(mySink);
    addSink(mySink);
    return mySink;
}


void Pump::addSampleSource(const SampleSourcePtr& theSampleSource) {

    addSink(theSampleSource);

}

AudioBufferPtr Pump::createBuffer(unsigned theNumFrames)
{
    return AudioBufferPtr(createAudioBuffer(getNativeSampleFormat(), theNumFrames, 
            getNumOutputChannels(), getNativeSampleRate()));
}

Pump& Pump::get() {
    // TODO: The way this is now, useRealPump gets instantiated once per plugin.
    // To change this, move useRealPump to ALSAPump and DirectSoundPump.

    string myWhoCares;
    if (get_environment_var("Y60_SOUND_USE_DUMMY", myWhoCares)) {
        _myUseRealPump = false;
    }
    if (_myUseRealPump) {
        try {
#ifdef WIN32          
            return Singleton<DirectSoundPump>::get();
#elif LINUX
            return Singleton<ALSAPump>::get();
#else
            return Singleton<CoreAudioPump>::get();
#endif
        } catch (const AudioException& e) {
            AC_WARNING << "Could not create NativePump.";
            AC_WARNING << "Error was: " << e;
            _myUseRealPump = false;
        }
    }
    return Singleton<DummyPump>::get();
    
}

void 
Pump::setUseRealPump(bool theRealPumpFlag) {
    _myUseRealPump = theRealPumpFlag;
}

void Pump::setVolume (float theVolume) {
    _myVolumeFader.setVolume(theVolume);
}
        
void Pump::fadeToVolume(float theVolume, float theTime) {
    _myVolumeFader.setVolume(theVolume, unsigned(theTime*getNativeSampleRate()));
}
    
float Pump::getVolume() const {
    return _myVolumeFader.getVolume();
}

void Pump::setBritzelTest(bool doBritzelTest) {
    _doBritzelTest = doBritzelTest;
}

Time Pump::getLatency () const {
    return _myLatency;
}

unsigned Pump::getNumUnderruns () const
{
    return _myNumUnderruns;
}

unsigned Pump::getNumSinks () const {
    AutoLocker<Pump> myLocker(*(const_cast<Pump*>(this)));
    const_cast<Pump*>(this)->removeDeadSinks();
    return _mySampleSinks.size();
}

unsigned Pump::getNativeSampleRate () const {
    return _mySampleRate;
}

SampleFormat Pump::getNativeSampleFormat () const {
    return _mySF;
}

unsigned Pump::getNumOutputChannels () const {
    return _numOutputChannels;
}

unsigned Pump::getOutputBytesPerFrame() const {
    return _numOutputChannels*getBytesPerSample(_mySF);
}

unsigned Pump::getNumClicks() const {
    return _numClicks;
}

void Pump::dumpState () const {
    AC_DEBUG << "Audio Device: " << _myDeviceName;
    AC_DEBUG << "  Card name: " << _myCardName;
}

void Pump::lock() {
    _mySinkLock.lock();
}

void Pump::unlock()
{
    _mySinkLock.unlock();
}

void Pump::start() {
    if(!_myRunning) {
        AudioTimeSource::run();
        _myRunning = true;
        fork();
#ifdef WIN32
        int myPriorityClass = SCHED_OTHER;
        int myPriority = THREAD_PRIORITY_TIME_CRITICAL;
#else
        int myPriorityClass = SCHED_RR;
        int myPriority = PosixThread::getMaxPriority(SCHED_RR);
#endif
        bool amIRoot = setPriority (myPriorityClass, myPriority);
        if (!amIRoot) {
            setPriority (SCHED_OTHER, PosixThread::getMaxPriority(SCHED_OTHER));
            AC_WARNING << "Audio running at normal priority.";
#ifndef WIN32
            AC_WARNING << "To get realtime priority, start as root.";
#endif
        } else {
            AC_INFO << "Audio running with realtime priority.";
        }
    } else {
        AC_WARNING << "Pump::start called twice - ignored.";
    }
}

void
Pump::stop() {
    AC_DEBUG << "Pump::stop";
    AudioTimeSource::stop();
    if (_myRunning) {
        _myRunning = false;
        join();
        if (_myNumUnderruns > 0) {
            AC_WARNING << "Pump::stop: Number of buffer underruns: " << _myNumUnderruns << endl;
        }
    }
}

SampleSourcePtr Pump::getSink(unsigned i) {
    return _mySampleSinks[i].lock();
}

void Pump::addSink(SampleSourcePtr theSink) {
    AutoLocker<Pump> myLocker(*this);
    _mySampleSinks.push_back(theSink);
}


void Pump::setDeviceName(const string& theName) {
    _myDeviceName = theName;
}

const std::string& Pump::getDeviceName() const {
    return _myDeviceName;
}

void Pump::setCardName(const string& theName) {
    _myCardName = theName;
}

const std::string& Pump::getCardName() const {
    return _myCardName;
}

bool 
Pump::isRunning() const {
    return _myRunning;
}

void Pump::addUnderrun() {
    _myNumUnderruns++;
}

void
Pump::run() {
    try {
        while(_myRunning) {
            pump();
//            Dashboard::get().cycle();
        }
    } catch (const Exception & e) {
        AC_ERROR << "Pump::run: Unhandled asl::exception.";
        AC_ERROR << e;
    } catch (const std::exception & e) {
        AC_ERROR << "Pump::run: Unhandled std::exception.";
        AC_ERROR << e.what();
    } catch (...) {
        try {
            AC_ERROR << "Pump::run: Unhandled exception.";
        } catch (...) {
            AC_ERROR << "Pump::run: Exception during exception handling.";
        }
        throw; // rethrow to get some more information about the exception
    }
}

void Pump::mix(AudioBufferBase& theOutputBuffer, unsigned numFramesToDeliver) {
    //    AC_TRACE << "Pump::mix";
#ifdef USE_DASHBOARD
    MAKE_SCOPE_TIMER(Mix);
#endif
    theOutputBuffer.resize(numFramesToDeliver);
    theOutputBuffer.clear();
    _myTempBuffer->resize(numFramesToDeliver);
    {
        AutoLocker<Pump> myLocker(*this);
#ifdef USE_DASHBOARD
        MAKE_SCOPE_TIMER(Mix_Pump_Lock);
#endif        
        std::vector <SampleSourceWeakPtr>::iterator it;
        removeDeadSinks();
        for (it = _mySampleSinks.begin(); it != _mySampleSinks.end();) {
            SampleSourcePtr curSampleSink = (*it).lock();
            if (curSampleSink) {
                AutoLocker<SampleSource> mySinkLocker(*curSampleSink);
                if (curSampleSink->isEnabled())
                {
                    curSampleSink->deliverData(*_myTempBuffer);
                    {
#ifdef USE_DASHBOARD
                        MAKE_SCOPE_TIMER(Add_Buffers);
#endif
                        theOutputBuffer += *_myTempBuffer;
                    }
                }
            }
            ++it;
        }
        
        {
#ifdef USE_DASHBOARD
            MAKE_SCOPE_TIMER(Pump_Volume_Fader);
#endif
            _myVolumeFader.apply(theOutputBuffer, _curFrame);
        }
//        theOutputBuffer.setMarker(1);
        if (_doBritzelTest) {
            if (theOutputBuffer.hasClicks(_myLastFrame)) {
                ++_numClicks;
                if (_numClicks == 1) {
                    AC_WARNING << "Pump::mix: Click detected";
                } else {
                    AC_TRACE << "Pump::mix: Click detected";
                }
            }
            _myLastFrame = theOutputBuffer.getSampleAsFloat(
                    theOutputBuffer.getNumFrames()-1, 0);
        }
    }
    _curFrame += numFramesToDeliver;
}

void Pump::removeDeadSinks() {
    std::vector <SampleSourceWeakPtr >::iterator it;
    for (it = _mySampleSinks.begin(); it != _mySampleSinks.end();) {
        SampleSourcePtr curSampleSink = (*it).lock();
        if (!curSampleSink) {
            it = _mySampleSinks.erase(it);
        } else {
            ++it;
        }
    }
}

int Pump::getSampleRateConfig() {
    string myRateString = "44100";
    get_environment_var("Y60_SOUND_SAMPLE_RATE", myRateString);
    return as<int>(myRateString);
}


} // namespace

