//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef INCL_TESTPUMP
#define INCL_TESTPUMP

#ifdef _MSC_VER
// For M_PI
#define _USE_MATH_DEFINES 
#endif

#include "HWSampleSink.h"
#include "AudioBuffer.h"
#include "Pump.h"

#include <string>
#include <asl/UnitTest.h>

class TestPump: public UnitTest{
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

