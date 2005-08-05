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
//    $RCSfile: TestWAVWriter.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.14 $
//
//
// Description:
//
//=============================================================================

#include "TestWAVWriter.h"
#include "WAVWriter.h"
#include "SoundException.h"

#include "SoundIn.h"
#include "SoundOut.h"
#include "SineGenerator.h"
#include "AudioScheduler.h"
#include "SoundCardManager.h"
#include "SoundCard.h"
#include "AudioBuffer.h"

#include <asl/string_functions.h>

#include <asl/Time.h>
#include <stdlib.h>

using namespace std;
using namespace asl;
using namespace AudioBase;


void
TestWAVWriter::testSoundIn (const std::string & myFName, bool isStereo){
    AudioBuffer::setSize(2048);
    SoundIn * mySoundin = new SoundIn (SoundCardManager::get().getCard(0), isStereo ? 2:1, 44100, 1.0);
    mySoundin->init();

    WAVWriter myWriter( myFName, 44100, isStereo ? 2:1);
    myWriter.init();

    AudioScheduler & myScheduler = AudioScheduler::get();
    myScheduler.connect (mySoundin->getOutput(0), myWriter.getInput(0));
    if (isStereo) {
        myScheduler.connect (mySoundin->getOutput(1), myWriter.getInput(1));
    }

    myScheduler.start();
    if (isStereo) {
        ENSURE_MSG (true, "WAVWriter saving soundin stereo, started");
    } else {
        ENSURE_MSG (true, "WAVWriter saving soundin mono, started");
    }
    msleep(3000);
    myScheduler.stop();
    ENSURE_MSG (true, "Scheduler stopped.");
}

void
TestWAVWriter::testSineGenerator(const std::string & myFName, bool isStereo) {
    AudioScheduler & myScheduler = AudioScheduler::get();
    AudioBuffer::setSize(2048);

    WAVWriter myWriter( myFName, 44100, isStereo ? 2:1);
    myWriter.init();

    // XXX Are we leaking these pointers?
    SineGenerator * myGenerator = new SineGenerator(44100);
    myGenerator->init();

    SineGenerator * myGenerator2 = 0;
    if (isStereo) {
        // XXX Are we leaking these pointers?
        myGenerator2 = new SineGenerator(44100);
        myGenerator2->init();
    }

    myScheduler.connect (myGenerator->_myOutput, myWriter.getInput(0));
    if (isStereo) {
        myScheduler.connect (myGenerator2->_myOutput, myWriter.getInput(1));
    }

    myScheduler.start();
    if (isStereo) {
        ENSURE_MSG (true, "WAVWriter saving a sine curve stereo, started");
    } else {
        ENSURE_MSG (true, "WAVWriter saving a sine curve mono, started");
    }

    msleep(3000);
    myScheduler.stop();
    ENSURE_MSG (true, "Scheduler stopped.");
}

void
TestWAVWriter::testMultiChannel(const std::string & myFName, unsigned numChannels) {
    AudioScheduler & myScheduler = AudioScheduler::get();
    AudioBuffer::setSize(2048);
    vector<SineGenerator *> myGenerators;

    WAVWriter myWriter( myFName, 48000, numChannels);
    myWriter.init();

    unsigned i;
    for (i = 0; i < numChannels; ++i) {
        // XXX Are we leaking these pointers?
        SineGenerator * generator = new SineGenerator(48000);
        generator->init();
        myScheduler.connect (generator->_myOutput, myWriter.getInput(i));
        myGenerators.push_back(generator);
    }

    myScheduler.start();
    ENSURE_MSG (true, "WAVWriter saving a sine curve multichannel, started");

    msleep(3000);
    myScheduler.stop();
    myGenerators.clear();
    ENSURE_MSG (true, "Scheduler stopped.");
}


void
TestWAVWriter::run() {
    try {
        testSineGenerator ("../../testfiles/testsinegenerator_Mono.wav", false);
        testSineGenerator ("../../testfiles/testsinegenerator_Stereo.wav", true);

        testSoundIn ("../../testfiles/testinput_Mono.wav",  false);
        testSoundIn ("../../testfiles/testinput_Stereo.wav",  true);

        testMultiChannel("../../testfiles/testsinegenerator_Multichannel.wav", 6);

    } catch (SoundException & e) {
        ENSURE_MSG (false, asl::as_string (e).c_str());
    }
}


