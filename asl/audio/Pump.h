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

#ifndef INCL_AUDIO_PUMP
#define INCL_AUDIO_PUMP

#include "SampleFormat.h"
#include "SampleSource.h"
#include "HWSampleSink.h"
#include "AudioBuffer.h"
#include "AudioTimeSource.h"

#include <asl/PosixThread.h>
#include <asl/Time.h>
#include <asl/ThreadLock.h>

#include <string>
#include <vector>

namespace asl {

class Pump : public AudioTimeSource, private PosixThread 
{
    public:
        Pump(SampleFormat mySF, unsigned myTimeStartDelay);
        virtual ~Pump();

        static Pump& get();
        
        HWSampleSinkPtr createSampleSink(const std::string & theName);
        AudioBufferPtr createBuffer(unsigned theNumFrames);
        void setVolume(float theVolume);
        void fadeToVolume(float theVolume, float theTime);
        float getVolume() const;
        void setBritzelTest(bool doBritzelTest);
        Time getLatency() const;
        unsigned getNumUnderruns() const;
        unsigned getNumSinks() const;
        unsigned getNativeSampleRate() const;
        SampleFormat getNativeSampleFormat() const;
        unsigned getNumOutputChannels() const;
        unsigned getOutputBytesPerFrame() const;
        unsigned getNumClicks() const;
        virtual void dumpState() const;
        static void setUseRealPump(bool theRealPumpFlag);
        bool isRunning() const;

        void addSampleSource(const SampleSourcePtr& theSampleSource);

        // Interface to HWSampleSink
        void lock();
        void unlock();

    protected:
        void start();
        void stop();

    //        HWSampleSinkPtr getSink(unsigned i);
        SampleSourcePtr getSink(unsigned i);
    //        void addSink(HWSampleSinkPtr theSink);
        void addSink(SampleSourcePtr theSink);

        void setDeviceName(const std::string& theName);
        const std::string& getDeviceName() const;
        
        void setCardName(const std::string& theName);
        const std::string& getCardName() const;
            
        void addUnderrun();
        void mix(AudioBufferBase& theOutputBuffer, unsigned numFramesToDeliver);

        bool _myRunning;

    private:
        void run();
        virtual void pump() {};
        void removeDeadSinks();
        int getSampleRateConfig();

    //         std::vector < HWSampleSinkWeakPtr > _mySampleSinks;
        std::vector < SampleSourceWeakPtr > _mySampleSinks;
        asl::ThreadLock _mySinkLock;
        Time _myLatency;

        SampleFormat _mySF;
        unsigned     _myNumUnderruns;
        std::string  _myDeviceName;
        std::string  _myCardName;
        unsigned     _numOutputChannels;
        unsigned     _mySampleRate;

        AudioBufferPtr _myTempBuffer;

        VolumeFader _myVolumeFader;
        unsigned    _curFrame;
        bool        _doBritzelTest;
        unsigned    _numClicks;
        float       _myLastFrame;
        static bool _myUseRealPump;

};

}

#endif 
