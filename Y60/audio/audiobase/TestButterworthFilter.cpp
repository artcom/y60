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
//    $RCSfile: TestButterworthFilter.cpp,v $
//
//     $Author: christian $
//
//   $Revision: 1.9 $
//
//=============================================================================

#include "TestButterworthFilter.h"
#include "ButterworthFilter.h"
#include "AudioEater.h"
#include "SineGenerator.h"
#include "AudioScheduler.h"

#include <asl/Time.h>
#ifdef LINUX
#include <unistd.h>
#endif


using namespace AudioBase;

void 
TestButterworthFilter::runWithFilter (ButterworthFilter * theFilter, 
                                      double expectedVolume)
{
    AudioScheduler & myScheduler = AudioScheduler::get();
    AudioEater * myEater = new AudioEater(1, 44100);
    SineGenerator * myGenerator = new SineGenerator(44100);
    myGenerator->init();

    myScheduler.connect (myGenerator->_myOutput, theFilter->getInput(0));
    myScheduler.connect (theFilter->getOutput(0), myEater->getInput(0));
    myScheduler.start();
    asl::msleep(200);
    myScheduler.stop();
    DPRINT2("Butterworth test - volume expected: ", expectedVolume);
    DPRINT2("Butterworth test - volume got: ", myEater->getMaxVolume());
    ENSURE(myEater->getMaxVolume() > expectedVolume-0.1);
    ENSURE(myEater->getMaxVolume() < expectedVolume+0.1);

    delete myEater;
}


