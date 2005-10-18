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
  
//#define ASSURE_POLICY AssurePolicy::Abort
#include "Pump.h"
#include "DummyPump.h"
#include "AudioException.h"
#ifdef LINUX
#include "ALSAPump.h"
#else
#include "DirectSoundPump.h"
#endif

#include <asl/Auto.h>

#ifdef USE_DASHBOARD
#include <asl/Dashboard.h>
#endif

using namespace std;

namespace asl {

#ifdef WIN32
double Pump::_myLatency_Init = 0.05;
#else
double Pump::_myLatency_Init = 0.02;
#endif
std::string Pump::_myDeviceName_Init = "";
unsigned Pump::_mySampleRate_Init = 44100;
unsigned Pump::_numOutputChannels_Init = 2;
bool Pump::_useDummy_Init = false;
    
Pump::Pump (SampleFormat mySF, unsigned myTimeStartDelay) 
    : AudioTimeSource(myTimeStartDelay, _mySampleRate_Init),
      _myRunning(false),
      _myNumUnderruns(0),
      _mySF(mySF),
      _myVolumeFader(mySF),
      _doBritzelTest(false),
      _numClicks(0),
      _myLastFrame(0)
{
    AC_INFO << "Pump::Pump";
    
    _myLatency = _myLatency_Init;
    _numOutputChannels = _numOutputChannels_Init;
    _mySampleRate = _mySampleRate_Init;
    _curFrame = 0;

    _myTempBuffer = AudioBufferPtr(createAudioBuffer(getNativeSampleFormat(), 
            0, getNumOutputChannels(), getNativeSampleRate()));
    _myVolumeFader.setVolume(1.0);
}

Pump::~Pump() {
    AC_INFO << "Pump::~Pump";
    for (unsigned i=0; i<_mySampleSinks.size(); ++i) {
        HWSampleSinkPtr mySink = _mySampleSinks[i].lock();
        if (mySink) {
            if (mySink->getState() != HWSampleSink::STOPPED &&
                 mySink->getState() != HWSampleSink::PAUSED)
            {
                AC_WARNING << "Audio Pump being deleted, but " << 
                    mySink->getName() << " is still active (state: " << 
                    HWSampleSink::stateToString(mySink->getState()) << ")";
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

AudioBufferPtr Pump::createBuffer(unsigned theNumFrames)
{
    return AudioBufferPtr(createAudioBuffer(getNativeSampleFormat(), theNumFrames, 
            getNumOutputChannels(), getNativeSampleRate()));
}

void Pump::setSysConfig(const Time& myLatency, const std::string& myDeviceName)
{
    AC_INFO << "Pump::setSysConfig";
    _myLatency_Init = myLatency;
    _myDeviceName_Init = myDeviceName;
    
}

void Pump::setAppConfig(unsigned mySampleRate, unsigned numOutputChannels, bool useDummy)
{
    AC_INFO << "Pump::setAppConfig";
    _mySampleRate_Init = mySampleRate;
    _numOutputChannels_Init = numOutputChannels;
    _useDummy_Init = useDummy;
    
}

Pump& Pump::get() {
    // TODO: The way this is now, useRealPump gets instantiated once per plugin.
    // To change this, move useRealPump to ALSAPump and DirectSoundPump.

    static bool useRealPump = true;
    if (_useDummy_Init) {
        useRealPump = false;
    }
    if (useRealPump) {
#ifdef LINUX
        try {
            return Singleton<ALSAPump>::get();
        } catch (const AudioException& e) {
            AC_WARNING << "Could not create ALSAPump.";
            AC_WARNING << "Error was: " << e;
            useRealPump = false;
        }
#else
        try {
            return Singleton<DirectSoundPump>::get();
        } catch (const AudioException& e) {
            AC_WARNING << "Could not create DirectSoundPump.";
            AC_WARNING << "Error was: " << e;
            useRealPump = false;
        }
#endif
    }
    return Singleton<DummyPump>::get();
    
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
#ifdef LINUX
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
            AC_ERROR << "Pump::stop: Number of buffer underruns: " << _myNumUnderruns << endl;
        }
    }
}

HWSampleSinkPtr Pump::getSink(unsigned i) {
    return _mySampleSinks[i].lock();
}

void Pump::addSink(HWSampleSinkPtr theSink) {
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
        AC_ERROR << "Pump::run: Unhandled exception.";
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
    }
}

void Pump::mix(AudioBufferBase& theOutputBuffer, unsigned numFramesToDeliver) {
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
        std::vector <HWSampleSinkWeakPtr >::iterator it;
        removeDeadSinks();
        for (it = _mySampleSinks.begin(); it != _mySampleSinks.end();) {
            HWSampleSinkPtr curSampleSink = (*it).lock();
            if (curSampleSink) {
                AutoLocker<HWSampleSink> mySinkLocker(*curSampleSink);
                if (curSampleSink->getState() != HWSampleSink::STOPPED &&
                    curSampleSink->getState() != HWSampleSink::PAUSED) 
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
    std::vector <HWSampleSinkWeakPtr >::iterator it;
    for (it = _mySampleSinks.begin(); it != _mySampleSinks.end();) {
        HWSampleSinkPtr curSampleSink = (*it).lock();
        if (!curSampleSink) {
            it = _mySampleSinks.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace

