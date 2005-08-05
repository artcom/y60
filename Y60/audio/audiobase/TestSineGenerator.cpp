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
//    $RCSfile: TestSineGenerator.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.10 $
//
//
// Description:
//
//=============================================================================

#include "TestSineGenerator.h"
#include "SineGenerator.h"
#include "AudioScheduler.h"
#include "AudioSyncModule.h"
#include "AudioInput.h"
#include "AudioEater.h"
#include "AudioBuffer.h"
#ifdef LINUX
#include <unistd.h>
#endif
#include <asl/Time.h>

using namespace std;

using namespace AudioBase;


void TestSineGenerator::run() {
    AudioScheduler & myScheduler = AudioScheduler::get();
    AudioEater * myEater = new AudioEater(1, 48000);
    AudioScheduler::get().setBufferSize(2048);
    SineGenerator * myGenerator = new SineGenerator(48000);
    myGenerator->init();

    myScheduler.connect (myGenerator->_myOutput, myEater->getInput(0));
    myScheduler.start();
    asl::msleep(200);
    myScheduler.lock();
    ENSURE_MSG (myGenerator->getBuffersGenerated() > 2,
            "Something got sent");
    ENSURE_MSG (myEater->getBuffersEaten() == myGenerator->getBuffersGenerated(),
            "Everything sent arrived at the destination" );
    myScheduler.unlock();            
    myScheduler.stop();
    ENSURE_MSG (true, "Scheduler stopped.");

    delete myEater;
}


