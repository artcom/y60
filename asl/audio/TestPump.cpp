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

#include "TestPump.h"

#include "Pump.h"

#include <asl/proc_functions.h>
#include <asl/Dashboard.h>

using namespace asl;

const SampleFormat ourNativeSampleFormat = SF_F32;

void TestPump::runWithPump(bool useDummyPump) {
        unsigned myBeginMemory = getProcessMemoryUsage(); 
#ifdef WIN32
        Pump::setSysConfig(0.05, "");
#else
        Pump::setSysConfig(0.02, "");
#endif
        Pump::setAppConfig(44100, 2, useDummyPump);

        Pump::get().setBritzelTest(true);
        
        testBufferAlloc();
        testPumpTimer();
        testSinkTimer();

        // Test different buffer sizes.
//        playSingleSound(32768);

        playSingleSound(8);            
        playSingleSound(19);   
        playSingleSound(256);  
        playSingleSound(1024); 
        playSingleSound(1472); 
        playSingleSound(8096); 

        testMix();
        testMultiplePlay();
        testVolume();
        testSimultaneousPlay();
        testConversions();
        testRunUntilEmpty();
        
        stressTest(3);

        ENSURE(Pump::get().getNumClicks() == 0);

        testUnderrun();

        AC_DEBUG << "Memory at start: " << myBeginMemory << endl;
        AC_DEBUG << "Memory at end: " << getProcessMemoryUsage() << endl;
        AC_DEBUG << "Allocated buffers: " << AudioBufferBase::getNumBuffersAllocated() 
                 << endl;

        Dashboard::get().print(cerr);
    }

void TestPump::testBufferAlloc() {
    // Test basic buffer allocation.
    AudioBufferBase * myBuffer1 = createAudioBuffer(SF_F32, 1024, 2, 44100);
    delete myBuffer1;
    {
        AudioBufferPtr myBuffer2(createAudioBuffer(SF_F32, 1024, 2, 44100));
    }
}

void TestPump::playSingleSound(unsigned theFramesPerBuffer, unsigned theDuration) {
    Pump & myPump = Pump::get();
    msleep(theDuration);
    HWSampleSinkPtr mySampleSink = 
        createSampleSink("TestSink", 44100, 2);
    ENSURE(myPump.getNumSinks() == 1);
    msleep(theDuration);
    queueSineBuffers(mySampleSink, SF_F32, theFramesPerBuffer, 2, 440, 44100, 0.0015*theDuration);
    mySampleSink->play();
    msleep(theDuration);
    mySampleSink->stop();
    mySampleSink = HWSampleSinkPtr(0);
    msleep(theDuration);
    ENSURE(myPump.getNumSinks() == 0);
}

void TestPump::testUnderrun() {
    Pump & myPump = Pump::get();
    HWSampleSinkPtr mySampleSink = 
        createSampleSink("TestSink", 44100, 2);
    mySampleSink->play();
    msleep(100);
    queueSineBuffers(mySampleSink, SF_F32, 256, 2, 440, 44100, 0.1);
    msleep(100);
    mySampleSink->stop();
    msleep(100);
}

void TestPump::testMix() {
    Pump & myPump = Pump::get();

    // Mix two sounds.
    HWSampleSinkPtr mySampleSink1 = 
        createSampleSink("TestSinkDouble1", 44100, 2);
    HWSampleSinkPtr mySampleSink2 = 
        createSampleSink("TestSinkDouble2", 44100, 2);
    queueSineBuffers(mySampleSink1, SF_F32, 32, 2, 440, 44100, 2, 0.5);
    queueSineBuffers(mySampleSink2, SF_F32, 33, 2, 440, 44100, 2, 0.5);

    mySampleSink1->play();
    msleep(100);
    mySampleSink2->play();
    ENSURE(myPump.getNumSinks() == 2);
    msleep(100);
    mySampleSink1->stop();
    msleep(100);
    mySampleSink2->stop();
    msleep(100);
    mySampleSink1 = HWSampleSinkPtr(0);
    mySampleSink2 = HWSampleSinkPtr(0);
    ENSURE(myPump.getNumSinks() == 0);
}

void TestPump::testMultiplePlay() {
    Pump & myPump = Pump::get();

    // Stop and play again.
    HWSampleSinkPtr myStopSampleSink = 
        createSampleSink("TestStopPlaySink", 44100, 2);
    queueSineBuffers(myStopSampleSink, SF_F32, 1024, 2, 440, 44100, 2, 0.5);
    ENSURE(fabs(myStopSampleSink->getBufferedTime()-2) < 0.1);
    myStopSampleSink->play();
    msleep(100);
    myStopSampleSink->stop();
    msleep(100);
    ENSURE(fabs(double(myStopSampleSink->getBufferedTime())) < 0.001);
    queueSineBuffers(myStopSampleSink, SF_F32, 1024, 2, 440, 44100, 2, 0.5);
    myStopSampleSink->play();
    msleep(100);
    myStopSampleSink->pause();
    msleep(100);
    ENSURE(fabs(myStopSampleSink->getBufferedTime()-1.9) < 0.1);
    myStopSampleSink->play();
    msleep(100);
    myStopSampleSink->stop();
    msleep(100);
}

void TestPump::testSimultaneousPlay() {
    Pump & myPump = Pump::get();

    // Play two sounds and stop them at the same time.
    HWSampleSinkPtr myStopSampleSink1 = 
        createSampleSink("TestSimultaneousStopSink1", 44100, 2);
    HWSampleSinkPtr myStopSampleSink2 = 
        createSampleSink("TestSimultaneousStopSink2", 44100, 2);
    queueSineBuffers(myStopSampleSink1, SF_F32, 1024, 2, 440, 44100, 2, 0.5);
    queueSineBuffers(myStopSampleSink2, SF_F32, 1024, 2, 330, 44100, 2, 0.5);
    myStopSampleSink1->play();
    myStopSampleSink2->play();
    msleep(100);
    myStopSampleSink1->stop();
    myStopSampleSink2->stop();
    msleep(100);
}

void TestPump::testConversions() {
    Pump & myPump = Pump::get();

    HWSampleSinkPtr myMonoSampleSink = 
        createSampleSink("TestMonoSink", 44100, 1);
    queueSineBuffers(myMonoSampleSink, SF_F32, 1024, 1, 440, 44100, 2, 1);
    myMonoSampleSink->play();
    msleep(100);
    myMonoSampleSink->stop();
    msleep(100);
    SUCCESS("Played mono sound on stereo device.");

    // Play a sound with signed 16-bit samples.
    HWSampleSinkPtr myS16SampleSink = 
        createSampleSink("TestS16Sink", 44100, 2);
    queueSineBuffers(myS16SampleSink, SF_S16, 1024, 2, 440, 44100, 2, 1);
    myS16SampleSink->play();
    msleep(100);
    myS16SampleSink->stop();
    msleep(100);
    SUCCESS("Played signed 16-bit sound.");
}

void TestPump::testRunUntilEmpty() {
    Pump & myPump = Pump::get();

    HWSampleSinkPtr mySink = 
        createSampleSink("TestRunUntilEmptySink", 44100, 2);
    queueSineBuffers(mySink, SF_F32, 1024, 2, 440, 44100, 0.4, 1);
    mySink->play();
    msleep(100);
    mySink->stop(true);
    msleep(100);
    ENSURE(mySink->getState() == HWSampleSink::RUNNING);
    msleep(500);
    ENSURE(mySink->getState() == HWSampleSink::STOPPED);
}

void TestPump::testVolume() {
    Pump & myPump = Pump::get();

    // Test sample sink volume control.
    HWSampleSinkPtr myVolumeSampleSink = 
        createSampleSink("TestVolumeSink1", 44100, 2);
    queueSineBuffers(myVolumeSampleSink, SF_F32, 1024, 2, 440, 44100, 2, 1);
    myVolumeSampleSink->play();
    msleep(100);
    myVolumeSampleSink->setVolume(0.5);
    // Setting volume doesn't take effect immediately
    ENSURE(almostEqual(myVolumeSampleSink->getVolume(),1));
    msleep(100);
    ENSURE(almostEqual(myVolumeSampleSink->getVolume(),0.5));
    myVolumeSampleSink->fadeToVolume(1, 0.2f);
    ENSURE(almostEqual(myVolumeSampleSink->getVolume(),0.5));
    msleep(300);
    ENSURE(almostEqual(myVolumeSampleSink->getVolume(),1));
    myVolumeSampleSink->stop();
    msleep(100);

    // Test global volume control.
    myVolumeSampleSink = 
        createSampleSink("TestVolumeSink2", 44100, 2);
    queueSineBuffers(myVolumeSampleSink, SF_F32, 1024, 2, 440, 44100, 2, 1);
    myVolumeSampleSink->play();
    msleep(100);
    myPump.setVolume(0.5);
    // Setting volume doesn't take effect immediately
    ENSURE(almostEqual(myPump.getVolume(),1));
    msleep(100);
    ENSURE(almostEqual(myPump.getVolume(),0.5));
    myPump.fadeToVolume(1, 0.2f);
    ENSURE(almostEqual(myPump.getVolume(),0.5));
    msleep(300);
    ENSURE(almostEqual(myPump.getVolume(),1));
    myVolumeSampleSink->stop();
}

void TestPump::testPumpTimer() {
#ifdef WIN32
    timeBeginPeriod(1);
#endif
    Time AudioStartTime = Pump::get().getCurrentTime();
#ifdef WIN32
    double SystemStartTime = timeGetTime()/1000.0;
#else
    double SystemStartTime = Time();
#endif
    double maxErr = 0.0, minErr = 1.0, avrgErr = 0.0;
    for(unsigned i=0; i<500; ++i) {
        Time AudioTime = Pump::get().getCurrentTime() - AudioStartTime;
#ifdef WIN32
        double SystemTime = timeGetTime()/1000.0-SystemStartTime;
#else
        double SystemTime = Time()-SystemStartTime;
#endif
        AC_TRACE << "Audio time: " << AudioTime << ", System time: " << SystemTime
                 << ", diff: " << AudioTime-SystemTime << endl;
        double myErr = fabs(AudioTime-SystemTime);
        
        if (myErr > maxErr) {
            maxErr = myErr;
        }
        if (myErr < minErr) {
            minErr = myErr;
        }
        avrgErr+=myErr;
        msleep(1);
    }
#ifdef WIN32
    timeEndPeriod(1);
#endif
    avrgErr /= 500;
    AC_DEBUG << "Timer error: MIN: " << minErr << ", MAX: " << maxErr
             << ", AVRG: " << avrgErr;
//    ENSURE_MSG(maxErr < 0.001, "Audio timer jitter < 1 ms");
}

void TestPump::testSinkTimer() {
    Pump & myPump = Pump::get();

    HWSampleSinkPtr mySampleSink = 
        createSampleSink("TestTimerSink", 44100, 2);
    queueSineBuffers(mySampleSink, SF_F32, 1024, 2, 440, 44100, 2, 0.5);
    Time curTime; 
    ENSURE(double(mySampleSink->getCurrentTime()) == 0.0);
    mySampleSink->play();
    msleep(100);
    curTime = mySampleSink->getCurrentTime();
    ENSURE(double(curTime) > 0.05 && double(curTime) < 0.2);
//    AC_PRINT << "Time: " << curTime;
    mySampleSink->stop();
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    msleep(100);
    ENSURE(double(mySampleSink->getCurrentTime()) == 0.0);
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    queueSineBuffers(mySampleSink, SF_F32, 1024, 2, 440, 44100, 2, 0.5);
    mySampleSink->play();
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    msleep(100);
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    mySampleSink->pause();
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    msleep(100);
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    mySampleSink->play();
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    msleep(100);
    curTime = mySampleSink->getCurrentTime();
    ENSURE(double(curTime) > 0.2 && double(curTime) < 0.4);
//    AC_PRINT << "Time: " << curTime;
    mySampleSink->stop();
//    AC_PRINT << "Time: " << mySampleSink->getCurrentTime();
    msleep(100);
    ENSURE(double(mySampleSink->getCurrentTime()) == 0.0);
}

HWSampleSinkPtr TestPump::createSampleSink (const string & theName, 
        unsigned theSampleRate, unsigned NumChannels)
{
    HWSampleSinkPtr mySampleSink = Pump::get().createSampleSink(theName);
    return mySampleSink;
}

void TestPump::stressTest(double myDuration) {
    Pump & myPump = Pump::get();

    vector<HWSampleSinkPtr> mySinks;
    vector<int> myTimeRunning;
    int numTestSounds = 10;
    for (int i=0; i<numTestSounds; i++) {
        HWSampleSinkPtr mySink = 
            createSampleSink((string("TestSink")+as_string(i)).c_str(), 44100, 2);
        mySinks.push_back(mySink);
        myTimeRunning.push_back(0);
    }
    int j=0;
    unsigned SLEEP_INTERVAL=20;
    while (++j<(1000/SLEEP_INTERVAL)*myDuration) {
        int i = int((float)numTestSounds*rand()/(RAND_MAX+1.0));
        msleep(SLEEP_INTERVAL);
        AC_DEBUG << "Mem: " << getProcessMemoryUsage() << ", allocated buffers: " 
            << AudioBufferBase::getNumBuffersAllocated() << endl;
        cerr << ".";
        if (mySinks[i]->getState() == HWSampleSink::STOPPED) {
            int myBufferSize = 16+int(1000.0*rand()/(RAND_MAX+1.0));
            queueSineBuffers(mySinks[i], SF_F32, 512, 2, 
                    unsigned(440*(0.5+i/(float)numTestSounds)), 44100, 3, 0.1);
            mySinks[i]->play();
        } else {
            mySinks[i]->stop();
        }
        for (int k=0; k<numTestSounds; ++k) {
            if (mySinks[k]->getState() == HWSampleSink::RUNNING) {
                myTimeRunning[k]++;
                if (myTimeRunning[k] > 19) {
                    myTimeRunning[k] = 0;
                    mySinks[k]->stop();
                }
            }
        }
    }
    cerr << endl;
    AC_DEBUG << "Mem1: " << getProcessMemoryUsage() << ", allocated buffers: " 
        << AudioBufferBase::getNumBuffersAllocated() << endl;
    for (int i=0; i<numTestSounds; i++) {
        HWSampleSinkPtr mySink = mySinks[0];
        mySinks.erase(mySinks.begin());
        if (mySink->getState() == HWSampleSink::RUNNING) {
            mySink->stop();
        }
    }
    msleep(100);
}


void TestPump::queueSineBuffers(asl::HWSampleSinkPtr& mySampleSink, SampleFormat mySF,
        unsigned numFramesPerBuffer, unsigned numChannels, unsigned myFrequency,
        unsigned mySampleRate, double myDuration, double myVolume)
{
    switch (mySF) {
        case SF_S16:
            queueSineBuffers<short>(mySampleSink, mySF, numFramesPerBuffer, numChannels, 
                    myFrequency, mySampleRate, myDuration, myVolume);
            return;
        case SF_F32:
            queueSineBuffers<float>(mySampleSink, mySF, numFramesPerBuffer, numChannels, 
                    myFrequency, mySampleRate, myDuration, myVolume);
            return;
        default:
            ASSURE(false);
    }
}

