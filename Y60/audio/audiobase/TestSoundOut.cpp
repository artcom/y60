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
//    $RCSfile: TestSoundOut.cpp,v $
//
//     $Author: build $
//
//   $Revision: 1.24 $
//
//
// Description:
//
//=============================================================================

#include "TestSoundOut.h"
#include "SoundOut.h"
#include "SineGenerator.h"
#include "AudioDefs.h"
#include "AudioScheduler.h"
#include "SoundCardManager.h"
#include "SoundCard.h"
#ifdef WIN32
#include "WinMMSoundCard.h"
#else
#include "ALSASoundCard.h"
#endif
#include "AudioBuffer.h"

#include <asl/Time.h>

#include <math.h>

using namespace asl;
using namespace std;
using namespace AudioBase;


void
TestSoundOut::infoTest() {
    SoundCardManager & theManager = SoundCardManager::get();
    cerr << theManager;
}


SineGenerator *
createAndConnectSineGenerator(AudioScheduler & myScheduler,
        SoundOut* theWriter, int theChannel)
{
    SineGenerator * myGenerator = new SineGenerator(48000);
    myGenerator->init();
    myScheduler.connect (myGenerator->_myOutput, theWriter->getInput(theChannel));
    return myGenerator;

}

void
TestSoundOut::sineTest () {
    AudioScheduler & myScheduler = AudioScheduler::get();
    SoundOut * myWriter = new SoundOut(SoundCardManager::get().getCard(0), 2, 48000);

    SineGenerator * myLGenerator = createAndConnectSineGenerator(myScheduler,myWriter,0);
    SineGenerator * myRGenerator = createAndConnectSineGenerator(myScheduler,myWriter,1);

    myScheduler.start();
    msleep(2000);
    myScheduler.stop();

    ENSURE (!myWriter->hasClicks());
    ENSURE_MSG (true, "Scheduler stopped.");

    delete myWriter;
}

// TODO: TestTimeSource should be added to a test suite. The way it is now, it can
// fail without causing a test suite failure!
void
TestSoundOut::timeTest() {
    AudioScheduler & myScheduler = AudioScheduler::get();
    SoundOut * myWriter = new SoundOut(SoundCardManager::get().getCard(0), 2, 48000);

    SineGenerator * myLGenerator = createAndConnectSineGenerator(myScheduler,myWriter,0);
    SineGenerator * myRGenerator = createAndConnectSineGenerator(myScheduler,myWriter,1);
    myScheduler.start();

    // XXX
    //TestTimeSource timeTest (myCard);
    //timeTest.run();
    msleep (300);
    myScheduler.stop();
    delete myWriter;
}

void
TestSoundOut::volumeTest() {
    AudioScheduler & myScheduler = AudioScheduler::get();
    SoundOut * myWriter = new SoundOut(SoundCardManager::get().getCard(0), 2, 48000);

    SineGenerator * myLGenerator = createAndConnectSineGenerator(myScheduler,myWriter,0);
    SineGenerator * myRGenerator = createAndConnectSineGenerator(myScheduler,myWriter,1);
    myScheduler.start();

    for(int i=0; i<100; i++) {
        msleep (10);
        myWriter->setVolume((100.0-i)/100.0);
    }
    bool isLoud = true;
    for(int i=0; i<100; i++) {
        msleep (10);
        if (isLoud) {
            myWriter->setVolume(1);
        } else {
            myWriter->setVolume(0);
        }
        isLoud = !isLoud;
    }
    myWriter->setVolume(0.9);
    msleep(300);
    DPRINT(myWriter->getVolume());
    //ENSURE(myWriter->getVolume() >= 0.8);
    myWriter->fadeToVolume(0.1,1);
    msleep(300);
    DPRINT(myWriter->getVolume());
    // ENSURE(fabs(myWriter->getVolume() - 0.5) < 0.3); // FIXME - this tests fails every 10 calls.
    msleep(800); // Wait a total of 1 secdd+at least one buffer length.
    DPRINT(myWriter->getVolume());
    //ENSURE(fabs(myWriter->getVolume()-0.1) < 0.1);
    //ENSURE (!myWriter->hasClicks()); FIXME!!!

    myScheduler.stop();
    delete myWriter;
}

void
TestSoundOut::run() {
    SoundCard * myCard = SoundCardManager::get().getCard(0);
    myCard->setParams(0.2, 44100, 16, 2);
    infoTest();
    volumeTest();
    sineTest();
    myCard->setParams(0.2, 44100, 16, 2);
    sineTest();
    timeTest();

}


