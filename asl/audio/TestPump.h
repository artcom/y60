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

#ifndef INCL_TESTPUMP
#define INCL_TESTPUMP

#include "asl_audio_settings.h"

#ifdef _MSC_VER
// For M_PI
#define _USE_MATH_DEFINES 
#endif

#include "HWSampleSink.h"
#include "AudioBuffer.h"
#include "Pump.h"

#include <string>
#include <asl/base/UnitTest.h>

class ASL_AUDIO_EXPORT TestPump: public UnitTest{
    public:
        TestPump (const char * myName)
            : UnitTest(myName) {}

    protected:
        virtual void runWithPump(bool useDummyPump);
        void stressTest(double myDuration);

    private:
        void testBufferAlloc();
        void playSingleSound(unsigned theFramesPerBuffer, unsigned theDuration = 100);
        void testUnderrun(); 
        void testMix(); 
        void testMultiplePlay();
        void testSimultaneousPlay();
        void testConversions();
        void testRunUntilEmpty(); 
        void testVolume();
        void testPumpTimer();
        void testSinkTimer();
        void testDelayed();
        asl::HWSampleSinkPtr createSampleSink (const std::string & theName, 
                unsigned theSampleRate, unsigned NumChannels);
        
        void queueSineBuffers(asl::HWSampleSinkPtr& mySampleSink, asl::SampleFormat mySF,
                unsigned numFramesPerBuffer, unsigned numChannels, unsigned myFrequency,
                unsigned mySampleRate, double myDuration, double myVolume=1.0);

        template<class SAMPLE>
        void queueSineBuffers(asl::HWSampleSinkPtr& mySampleSink, asl::SampleFormat mySF,
                unsigned numFramesPerBuffer, unsigned numChannels, unsigned myFrequency,
                unsigned mySampleRate, double myDuration, double myVolume=1.0)
        {
            unsigned numFrames = unsigned(myDuration*mySampleRate);

            // The next two lines make sure the sound ends at a zero crossing.
            // This avoids a click when played to the end.
            float numFramesPerSine = float(mySampleRate)/myFrequency;
            numFrames = unsigned(numFramesPerSine*round(numFrames/numFramesPerSine))+1;
            SAMPLE * mySamples = new SAMPLE[numFramesPerBuffer*numChannels];
            SAMPLE * curSample = mySamples;
            for (unsigned i=0; i<numFrames; ++i) {
                for (unsigned curChannel=0; curChannel<numChannels; ++curChannel) {
                    *curSample = asl::AudioBuffer<SAMPLE>::floatToSample
                            (float(sin((2*M_PI*i*myFrequency)/mySampleRate)*myVolume));
                    curSample++;
                }
                if (curSample == mySamples+numFramesPerBuffer*numChannels) {
                    asl::AudioBufferPtr myBuffer = asl::Pump::get().
                            createBuffer(numFramesPerBuffer);
                    myBuffer->convert(mySamples, mySF, numChannels);
                    mySampleSink->queueSamples(myBuffer);
                    curSample = mySamples;
                }
            }
            asl::AudioBufferPtr myBuffer = asl::Pump::get().createBuffer(
                    (curSample-mySamples)/numChannels);
            myBuffer->convert(mySamples, mySF, numChannels);
            mySampleSink->queueSamples(myBuffer);
        };
        
        bool _myNoisy;
};


#endif

