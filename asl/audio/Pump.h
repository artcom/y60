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

        // Latency and Device name are system-specific (and should ideally be read from a 
        // configuration file), while the other parameters are all application-specific. 
        static void setSysConfig(const Time& myLatency, const std::string& myDeviceName = "");
        static void setAppConfig(unsigned mySampleRate, unsigned numOutputChannels = 2, 
                bool useDummy = false);
        static Pump& get();
        
        HWSampleSinkPtr createSampleSink 
                (const std::string & theName);
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

        bool isRunning() const;

        // Interface to HWSampleSink
        void lock();
        void unlock();

    protected:
        void start();
        void stop();

        HWSampleSinkPtr getSink(unsigned i);
        void addSink(HWSampleSinkPtr theSink);

        void setDeviceName(const std::string& theName);
        const std::string& getDeviceName() const;
        
        void setCardName(const std::string& theName);
        const std::string& getCardName() const;
            
        void addUnderrun();
        void mix(AudioBufferBase& theOutputBuffer, unsigned numFramesToDeliver);

        // The following are used only as initializers - set by setConfig, read 
        // by Pump::Pump and derived constructors.
        // TODO: Anyone know of a better way to do this?
        static double      _myLatency_Init;
        static std::string _myDeviceName_Init;
        static unsigned    _mySampleRate_Init;
        static unsigned    _numOutputChannels_Init;
        static bool        _useDummy_Init;

    private:
        void run();
        virtual void pump() = 0;
        void removeDeadSinks();

        std::vector < HWSampleSinkWeakPtr > _mySampleSinks;
        asl::ThreadLock _mySinkLock;
        bool _myRunning;
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
};

}

#endif 
