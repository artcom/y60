/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
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

#ifndef _TESTGRAINSOURCE_H_
#define _TESTGRAINSOURCE_H_

#include "y60_sound_settings.h"

#include <asl/base/UnitTest.h>
#include <asl/audio/Pump.h>
#include <y60/sound/FFMpegAudioDecoder.h>

#include "GrainSource.h"

using namespace y60;
using namespace asl;

#include <acmake/y60sound_paths.h>
#define TEST_FILES CMAKE_CURRENT_SOURCE_DIR"/testfiles"

class GrainSourceStressTest : public UnitTest {

public:

    GrainSourceStressTest() :
      UnitTest("GrainSourceStressTest")
      {
      }

      void run() {

          unsigned mySampleRate = Pump::get().getNativeSampleRate();
          asl::SampleFormat mySampleFormat = Pump::get().getNativeSampleFormat();

          for (unsigned i = 1; i <= 10; i++) {
              unsigned myGrainSize = 2*i;
              //unsigned myGrainRate = i;

              std::string mySoundFile = TEST_FILES "/ShutterClick.wav";

              GrainSourcePtr myGrainSource =
                  GrainSourcePtr(new GrainSource("aGrainSource",
                  mySampleFormat,
                  mySampleRate, 2,
                  myGrainSize));
    
              FFMpegAudioDecoderFactory myDecoderFactory;

              IAudioDecoder* myDecoder = myDecoderFactory.tryCreateDecoder(mySoundFile);
              myDecoder->setSampleSink(myGrainSource.get());
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
