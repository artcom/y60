//=============================================================================
//
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testFFMpegAudioReader.tst.cpp,v $
//   $Author: ulrich $
//   $Revision: 1.20 $
//   $Date: 2005/04/29 14:08:26 $
//
//  FFMpeg audio reader.
//
//=============================================================================

#include "FFMpegAudioReader.h"

#include "AudioScheduler.h"
#include "AudioBuffer.h"
#include "SoundCardManager.h"
#include "SoundCard.h"
#include "SoundOut.h"

#include <asl/UnitTest.h>
#include <asl/Time.h>

using namespace std;
using namespace AudioBase;

class FFMpegAudioReaderTest : public UnitTest {
    public:
        FFMpegAudioReaderTest() : UnitTest("FFMpegAudioReaderTest") {  }

        void play(const std::string & theFilename, unsigned theTimeout = 2000) {
            cout << "Playing " << theFilename << endl;

            AudioBase::SoundCard * myCard = AudioBase::SoundCardManager::get().getCard(0);
            ENSURE(myCard != 0);

            FFMpegAudioReader * myReader = new AudioBase::FFMpegAudioReader(theFilename, 1.0);
            ENSURE(myReader->getNumOutputs() == myReader->getNumChannels());
            ENSURE(myReader->getSampleRate() != -1);

#ifdef LINUX
            // TODO: Find out, why linux sound card does not work with only one channel
            myCard->setParams(0.1, 48000, 16, 2);
#else
            myCard->setParams(0.1, 48000, 16, myReader->getNumChannels());
#endif

            // writer
            SoundOut * myWriter = new SoundOut(myCard, myReader->getNumChannels(), 48000);
            ENSURE(myWriter->getNumInputs() == myReader->getNumOutputs());

            // connect all outputs
            AudioBase::AudioScheduler & myScheduler = AudioBase::AudioScheduler::get();
            for (unsigned i = 0; i < myReader->getNumChannels(); ++i) {
                myScheduler.connect(((AudioBase::AudioModule*)myReader)->getOutput(i), myWriter->getInput(i));
            }

            myScheduler.start();
            asl::msleep(theTimeout);
            myScheduler.stop();

            delete myWriter;
            /*
             * It is *FORBIDDEN*VERBOTEN*INTERDIT* to delete AudioAsyncModules
             * such as FFMpegAudioReader, since they're handled by
             * the AudioScheduler...
             */
        }

        void run() {
            AudioBase::AudioBuffer::setSize(2048);

            play("../../testfiles/pitchbend_mono.wav", 1000);
            //play("http://himmel/testfiles/track2.mp3", 3000);
            play("../../testfiles/One_Better_Day.mp3", 3000);
            // Don't test WMA for FFMpeg. It is b0rken.
            //play("../../testfiles/Leben_cut_160_stereo_wma7.wma", 3000);

            SUCCESS("FFMpegAudioReader");
        }
};


class MyTestSuite : public UnitTestSuite {
    public:
        MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
        void setup() {
            UnitTestSuite::setup();
            addTest(new FFMpegAudioReaderTest);
        }
};

int main(int argc, char *argv[]) {
    MyTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.run();
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}
