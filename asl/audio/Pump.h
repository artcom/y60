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

#ifndef INCL_AUDIO_PUMP
#define INCL_AUDIO_PUMP

#include "asl_audio_settings.h"

#include "SampleFormat.h"
#include "SampleSource.h"
#include "HWSampleSink.h"
#include "AudioBuffer.h"
#include "AudioTimeSource.h"

#include <asl/base/PosixThread.h>
#include <asl/base/Time.h>
#include <asl/base/ThreadLock.h>

#include <string>
#include <vector>

namespace asl {

class ASL_AUDIO_DECL Pump : public AudioTimeSource, private PosixThread 
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
