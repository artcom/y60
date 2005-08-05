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
//    $RCSfile: testAudioApp.tst.cpp,v $
//
//     $Author: thomas $
//
//   $Revision: 1.12 $
//
//
// Description:
//
//
//=============================================================================

#include <asl/Time.h>
#include <asl/proc_functions.h>

#include "AudioController.h"
#include "TestAudioController.h"

using namespace std;

class TestWAVLeak : public UnitTest {
public:
    TestWAVLeak(const std::string & theBaseDirectory)
        : UnitTest("TestWAVLeak"), _myBaseDirectory(theBaseDirectory) {
    }

    void run() {
        const std::string & myFilename = "pitchbend.wav";
        AudioApp::AudioController & myController = AudioApp::AudioController::get();
        myController.init(48000);

        unsigned myInitialMemUsage = asl::getProcessMemoryUsage();
        DPRINT(myInitialMemUsage);

        unsigned myDiffMemLimit = 0;

        for (unsigned i = 0; i < 8; ++i) {
            std::string myId = myController.play(getBaseDirectory() + myFilename, "Mixer", 1.0, 0.0, false);
            cerr << "Playing " << myId << endl;
            while (myController.isPlaying(myId)) {
                asl::msleep(100);
            }
            unsigned myMemUsage = asl::getProcessMemoryUsage();
            if (myDiffMemLimit == 0) {
                // allow 10% more mem than the first iteration
                myDiffMemLimit = (unsigned)((myMemUsage - myInitialMemUsage) * 1.1f);
                DPRINT(myDiffMemLimit);
            }
            DPRINT(myMemUsage);
        }
        unsigned myFinalMemUsage = asl::getProcessMemoryUsage();
        DPRINT(myFinalMemUsage);

        unsigned myDiffMemUsage = myFinalMemUsage - myInitialMemUsage;
        DPRINT(myDiffMemUsage);
//        ENSURE(myDiffMemUsage <= myDiffMemLimit);

        myController.stopAllSounds();
    }

private:
    std::string _myBaseDirectory;

    const std::string & getBaseDirectory() const {
        return _myBaseDirectory;
    }
};

class TestFFMpegAudioReader : public UnitTest {
public:
    TestFFMpegAudioReader(const std::string & theBaseDirectory)
        : UnitTest("TestFFMpegAudioReader"), _myBaseDirectory(theBaseDirectory) {
    }

    void play(const std::string & theFilename, unsigned theSampleRate) {

        AudioApp::AudioController & myController = AudioApp::AudioController::get();
        myController.init(theSampleRate);

        std::string myId = myController.play(getBaseDirectory() + theFilename, "Mixer", 1.0, 0.0, false);
        ENSURE(myId != "");
        cerr << "playing id=" << myId << endl;
        asl::msleep(1500);

        double mySeekOffset = -1.0;
        cerr << "seek " << myId << " " << mySeekOffset << endl;
        myController.setSeekOffset(myId, mySeekOffset);
        asl::msleep(1500);

        cerr << "pause id=" << myId << endl;
        myController.pause(myId);
        asl::msleep(1500);
        cerr << "unpause id=" << myId << endl;
        myController.pause(myId);
        asl::msleep(1500);

        for (unsigned int i = 0; i < 5; ++i) {
            cerr << "seek " << myId << " " << mySeekOffset << endl;
            myController.setSeekOffset(myId, mySeekOffset);
            asl::msleep(1000);
        }

        cerr << "stop id=" << myId << endl;
        myController.stop(myId);
        cerr << "stopped id=" << myId << endl;
        asl::msleep(1500);

        SUCCESS("FFMpegAudioReader");
    }

    void run() {
	    // soundcard and file have same samplerate
        play("track2.mp3", 22050);

        // downsampling
        play("track2.mp3", 8000);

        // samplerate different (upsampling)
        play("One_Better_Day.mp3", 44100);

        // wma test disabled for now since it is b0rken in current ffmpeg
        // play("Leben_cut_160_stereo_wma7.wma", 44100);
    }

private:
    std::string _myBaseDirectory;

    const std::string & getBaseDirectory() const {
        return _myBaseDirectory;
    }
};

int main( int argc, char *argv[] ) {
    try  {
        UnitTestSuite mySuite ("Sound tests");

        mySuite.addTest (new TestWAVLeak("../../../audiobase/testfiles/"));
        mySuite.addTest (new TestFFMpegAudioReader("../../../audiobase/testfiles/"));
        mySuite.addTest (new TestAudioController("../../../audiobase/testfiles"));
        mySuite.run();

        return mySuite.returnStatus();
    } catch (asl::Exception & ex) {
        cerr << ex << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown Exception" << endl;
        return 1;
    }
}

