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
//
//    $RCSfile: TestMixer.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.19 $
//
//
// Description:
//
//=============================================================================

#include "TestMixer.h"
#include "SoundOut.h"
#include "SineGenerator.h"
#include "Mixer.h"
#include "AudioDefs.h"
#include "AudioScheduler.h"
#include "NullSource.h"

#include "SoundCardManager.h"
#include "SoundCard.h"
//#ifdef WIN32
//#include "DummySoundCard.h"
//#else
//#include "ALSASoundCard.h"
//#endif

#include <asl/Logger.h>
#include <asl/Time.h>

using namespace std;
using namespace asl;
using namespace AudioBase;


void TestMixer::setupMixer(AudioScheduler& myScheduler, SoundOut * myWriter, bool myStereo) {
    if (myStereo) {
        _myMixer = new Mixer(2, 48000, 0.5);
        myScheduler.connect (_myMixer->getOutput(Left), myWriter->getInput(0));
        myScheduler.connect (_myMixer->getOutput(Right), myWriter->getInput(1));
        _myNull = 0;
    } else {
        _myMixer = new Mixer(1, 48000, 0.5);
        myScheduler.connect (_myMixer->getOutput(Mono), myWriter->getInput(0));
        _myNull = new NullSource(48000);
        myScheduler.connect (_myNull->getOutput(), myWriter->getInput(1));
    }
    ENSURE_MSG (true, "Mixer connected");
}

SineGenerator *
TestMixer::createAndConnectSineGenerator(AudioScheduler& myScheduler, ChannelType theType) {
    myScheduler.lock();
    SineGenerator * myGenerator = new SineGenerator(48000);
    AudioInput *myMixerInput = _myMixer->newInput(theType);
    myGenerator->init();
    myScheduler.connect (myGenerator->_myOutput, myMixerInput);
    myScheduler.unlock();
    ENSURE_MSG (true, "Sine generator started.");

    return myGenerator;
}

void
TestMixer::initModules() {
    _myMixer->init();
    if (_myNull) {
        _myNull->init();
    }
}

void TestMixer::simpleTest(bool myStereo)
{
    ENSURE_MSG (true, (myStereo ? "stereo" : "mono"));

    AudioScheduler & myScheduler = AudioScheduler::get();
    SoundCard * myCard = SoundCardManager::get().getCard(0);
    myCard->setParams(0.1, 48000, 16, 2);
    SoundOut * myWriter = new SoundOut(myCard, 2, 48000);
    setupMixer(myScheduler, myWriter, myStereo);
    initModules();

    myScheduler.start();
    msleep(100);
    ENSURE_MSG (true, "Scheduler started");

    SineGenerator * myGenerator = createAndConnectSineGenerator(myScheduler);

    msleep(500);
    myScheduler.stop();
    ENSURE_MSG (true, "Scheduler stopped.");

    AC_DEBUG << getTracePrefix() << " Info:  time needed: "
            << SoundCardManager::get().getCard(0)->getCurrentTime()
            << " sec, expected: 0,6 sec";

    delete myWriter;
}

void TestMixer::complexTest(bool myStereo) {
    AudioScheduler & myScheduler = AudioScheduler::get();
    SoundOut * myWriter = new SoundOut(SoundCardManager::get().getCard(0), 2, 48000);
    setupMixer(myScheduler, myWriter, myStereo);
    initModules();

    myScheduler.start();
    ENSURE_MSG (true, "Scheduler started");

    SineGenerator * myGenerator = createAndConnectSineGenerator(myScheduler);

    msleep(600);
    ENSURE_MSG (true, "After sleep");

    myGenerator = createAndConnectSineGenerator(myScheduler, Left);
    msleep(200);

    // connect second generator.
    SineGenerator * myGenerator2 = createAndConnectSineGenerator(myScheduler, Right);
    ENSURE_MSG (true, "Sine generator 2 started");

    msleep (500);
    myScheduler.stop();

    ENSURE_MSG (true, "Scheduler stopped.");

    delete myWriter;
}

void TestMixer::threadSafetyTest (bool myStereo) {
    AudioScheduler & myScheduler = AudioScheduler::get();

    SoundOut * myWriter = new SoundOut(SoundCardManager::get().getCard(0), 2, 48000);
    setupMixer(myScheduler, myWriter, myStereo);
    _myMixer->setVolume(0.01);
    initModules();

    myScheduler.start();

    int i;
    for (i=0; i<500; i++) {
        createAndConnectSineGenerator(myScheduler);
        // Memory leak expected.

        double myDelay = double(rand())/(RAND_MAX*300.0);
        msleep (int(myDelay));
    }

    myScheduler.stop();
    ENSURE_MSG (true, "Scheduler stopped.");

    delete myWriter;
}

void TestMixer::volumeTest(bool myStereo) {
    AudioScheduler & myScheduler = AudioScheduler::get();

    SoundOut * myWriter = new SoundOut(SoundCardManager::get().getCard(0), 2, 48000);
    setupMixer(myScheduler, myWriter, myStereo);
    initModules();

    myScheduler.start();
    ENSURE_MSG (true, "Scheduler started");

    SineGenerator * myGenerator1 = createAndConnectSineGenerator(myScheduler);

    msleep(50);
    _myMixer->setVolume(0.1);
    msleep(50);
    _myMixer->setVolume(1);
    msleep(50);
    _myMixer->setVolume(0.1);
    msleep(10);
    _myMixer->setVolume(1);
    msleep(300);

    myScheduler.stop();

    ENSURE_MSG (true, "Scheduler stopped.");

    delete myWriter;
}

void TestMixer::run() {
    simpleTest (false);
    simpleTest (true);

    complexTest (true);
    complexTest (false);

    volumeTest (true);
    volumeTest (false);
/*
    threadSafetyTest (true);
    threadSafetyTest (false);
    */
}


