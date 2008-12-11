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

#ifdef _MSC_VER
// For M_PI
#define _USE_MATH_DEFINES 
#endif
#include "Effect.h"
#include "VolumeFader.h"

#include <asl/base/UnitTest.h>

#include <math.h>
#include <stdlib.h>

using namespace asl;

class HalfEffect: public Effect {
      
private:
    template <class SAMPLE>
        class HalfEffectFunctor : public EffectFunctor<SAMPLE> {
            protected:
                virtual void operator()(Effect* theEffect, AudioBuffer<SAMPLE> & theBuffer, 
                        Unsigned64 theAbsoluteFrame) {
                    SAMPLE * curSample = theBuffer.begin();
                    for (unsigned curFrame = 0; curFrame<theBuffer.getNumFrames(); ++curFrame)
                    {
                        for (unsigned myChannel = 0; myChannel<theBuffer.getNumChannels(); 
                                ++myChannel) 
                        {
                            *curSample = (SAMPLE)((*curSample) * 0.5);
                            curSample++;
                        }
                    }
                }
        };

public:
    HalfEffect(SampleFormat theSampleFormat) 
        : Effect(createEffectFunctor<HalfEffectFunctor>(theSampleFormat))
    {
    }
};

class TestEffect: public UnitTest {

public:
    explicit TestEffect(const char * myName): UnitTest (myName) {}

    virtual void run() {
        run(SF_S16);
        run(SF_F32);
    }

protected:
    void fillSineBuffer(AudioBufferBase* theAudioBuffer, SampleFormat theSampleFormat,
            unsigned theFrequency, double theVolume) 
    {
        switch (theSampleFormat) {
            case SF_S16:
                fillSineBuffer<short>(theAudioBuffer, theFrequency, theVolume);
                return;
            case SF_F32:
                fillSineBuffer<float>(theAudioBuffer, theFrequency, theVolume);
                return;
            default:
                ASSURE(false);
        }
    }

    template<class SAMPLE>
    void fillSineBuffer(AudioBufferBase* theBuffer, unsigned theFrequency, double theVolume)
    {
        SAMPLE * curSample = dynamic_cast<AudioBuffer<SAMPLE>*>(&(*theBuffer))->begin();
        for (unsigned i=0; i<theBuffer->getNumFrames(); ++i) {
            for (unsigned curChannel=0; curChannel<theBuffer->getNumChannels(); ++curChannel) {
                *curSample = AudioBuffer<SAMPLE>::floatToSample
                        (float(sin((2*M_PI*i*theFrequency)/theBuffer->getSampleRate())*theVolume));
                curSample++;
            }
        }
    }

private:
    virtual void run(SampleFormat theSampleFormat) =0;
};

class TestHalfEffect: public TestEffect {
public:
    explicit TestHalfEffect(): TestEffect ("TestHalfEffect") {}

private:
    virtual void run(SampleFormat theSampleFormat) {
        asl::Ptr<AudioBufferBase> myAudioBuffer = asl::Ptr<AudioBufferBase>(
                createAudioBuffer(theSampleFormat, 2048, 2, 44100));
        fillSineBuffer(&(*myAudioBuffer), theSampleFormat, 440, 1.0);
        asl::Ptr<AudioBufferBase> myBaselineBuffer = asl::Ptr<AudioBufferBase>(
                createAudioBuffer(theSampleFormat, 2048, 2, 44100));
        fillSineBuffer(&(*myBaselineBuffer), theSampleFormat, 440, 0.5);
        HalfEffect myEffect(theSampleFormat);
        myEffect.apply(*myAudioBuffer, 0);
        ENSURE(myBaselineBuffer->almostEqual(*myAudioBuffer, 0.000001));
    }
};

class TestNullEffect: public TestEffect {
public:
    explicit TestNullEffect(): TestEffect ("TestNullEffect") {}

private:
    virtual void run(SampleFormat theSampleFormat) {
        asl::Ptr<AudioBufferBase> myAudioBuffer = asl::Ptr<AudioBufferBase>(
                createAudioBuffer(theSampleFormat, 2048, 2, 44100));
        fillSineBuffer(&(*myAudioBuffer), theSampleFormat, 440, 1.0);
        asl::Ptr<AudioBufferBase> myBaselineBuffer = asl::Ptr<AudioBufferBase>(myAudioBuffer->clone());
        NullEffect myEffect(theSampleFormat);
        myEffect.apply(*myAudioBuffer, 0);
        ENSURE(*myBaselineBuffer == *myAudioBuffer);
    }
};

class TestVolumeFader: public TestEffect {
public:
    explicit TestVolumeFader(): TestEffect ("TestVolumeFader") {}
private:
    virtual void run(SampleFormat theSampleFormat) {
        asl::Ptr<AudioBufferBase> myAudioBuffer = asl::Ptr<AudioBufferBase>(
                createAudioBuffer(theSampleFormat, 2048, 2, 44100));
        fillSineBuffer(&(*myAudioBuffer), theSampleFormat, 440, 1.0);
        asl::Ptr<AudioBufferBase> myBaselineBuffer = asl::Ptr<AudioBufferBase>(myAudioBuffer->clone());
        VolumeFader myFader(theSampleFormat);
        myFader.setVolume(1.0);
        // One buffer fadein
        ENSURE(::almostEqual(myFader.getVolume(0), 0.0));
        myFader.apply(*myAudioBuffer, 0);
        ENSURE(::almostEqual(myAudioBuffer->getSampleAsFloat(0, 0), 0.0));
        ENSURE(!::almostEqual(myAudioBuffer->getSampleAsFloat(5, 0), 0.0));
        ENSURE(::almostEqual(myAudioBuffer->getSampleAsFloat(500, 0),
                myBaselineBuffer->getSampleAsFloat(500, 0)));
        ENSURE(::almostEqual(myFader.getVolume(myAudioBuffer->getNumFrames()), 1.0));
        unsigned curFrame = myAudioBuffer->getNumFrames();
        ENSURE(::almostEqual(myFader.getVolume(curFrame), 1.0));
        
        // One buffer full volume
        myAudioBuffer = asl::Ptr<AudioBufferBase>(myBaselineBuffer->clone());
        myFader.apply(*myAudioBuffer, curFrame);
        ENSURE(*myBaselineBuffer == *myAudioBuffer);
        
        // One buffer fadeout
        myFader.setVolume(0.5);
        curFrame += myAudioBuffer->getNumFrames();
        ENSURE(::almostEqual(myFader.getVolume(curFrame), 1.0));
        myAudioBuffer = asl::Ptr<AudioBufferBase>(myBaselineBuffer->clone());
        myFader.apply(*myAudioBuffer, curFrame);
        ENSURE(::almostEqual(myBaselineBuffer->getSampleAsFloat(VolumeFader::DEFAULT_FADE_FRAMES, 0)*0.5,
                myAudioBuffer->getSampleAsFloat(VolumeFader::DEFAULT_FADE_FRAMES, 0), 0.001));
        ENSURE(!::almostEqual(myBaselineBuffer->getSampleAsFloat(VolumeFader::DEFAULT_FADE_FRAMES-5, 0)*0.5,
                myAudioBuffer->getSampleAsFloat(VolumeFader::DEFAULT_FADE_FRAMES-5, 0), 0.001));
        curFrame += myAudioBuffer->getNumFrames();
        ENSURE(::almostEqual(myFader.getVolume(curFrame), 0.5));
    }    
};

int main( int argc, char *argv[] ) {
    UnitTestSuite mySuite ("Effect tests", argc, argv);
    mySuite.addTest(new TestNullEffect);
    mySuite.addTest(new TestHalfEffect);
    mySuite.addTest(new TestVolumeFader);

    mySuite.run();

    std::cerr << ">> Finished test '" << argv[0] << "'"
              << ", return status = " << mySuite.returnStatus() << std::endl;

    return mySuite.returnStatus();
}

