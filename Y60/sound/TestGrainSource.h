//=============================================================================
//
// Copyright (C) 2007, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _TESTGRAINSOURCE_H_
#define _TESTGRAINSOURCE_H_

#include <asl/UnitTest.h>
#include <asl/Pump.h>
#include <y60/FFMpegAudioDecoder.h>

#include "GrainSource.h"

using namespace y60;
using namespace asl;

class GrainSourceStressTest : public UnitTest {

 public: 

  GrainSourceStressTest() :
    UnitTest("GrainSourceStressTest") 
    {
    }

  void run() {

    unsigned mySampleRate = Pump::get().getNativeSampleRate();
    asl::SampleFormat mySampleFormat = Pump::get().getNativeSampleFormat();

    av_register_all(); // never forget :)

    for (unsigned i = 1; i <= 10; i++) {
      unsigned myGrainSize = 2*i;
      unsigned myGrainRate = i;

      std::string mySoundFile = "../../testfiles/ShutterClick.wav";

	GrainSourcePtr myGrainSource = 
	  GrainSourcePtr(new GrainSource("aGrainSource", 
					 mySampleFormat,
					 mySampleRate, 2, 
					 myGrainSize));

	IAudioDecoder* myDecoder = 
	  new FFMpegAudioDecoder(mySoundFile);
	myDecoder->setSampleSink(&(*myGrainSource));
	myGrainSource->clearAudioData();
	myDecoder->decodeEverything();
	delete myDecoder;

	Pump::get().addSampleSource(myGrainSource);
	
	myGrainSource->enable();
	msleep(10);
	myGrainSource->disable();
	ENSURE(true);
    }
  }
};

class GrainSourceTestSuite : public UnitTestSuite {
 public:

  GrainSourceTestSuite(const char * myName, int argc, char * argv[])
    : UnitTestSuite(myName, argc, argv)
    {}

  void setup() {
    UnitTestSuite::setup();
    Pump::setUseRealPump(true);
        
    // GRAINSOURCE specific tests
    addTest(new GrainSourceStressTest());
  }
    
};



#endif
