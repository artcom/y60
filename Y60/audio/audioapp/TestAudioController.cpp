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
//    $RCSfile: TestAudioController.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.25 $
//
//
// Description:
//
//=============================================================================

#include "TestAudioController.h"
#include "AudioController.h"

#include <audio/SoundException.h>
#include <audio/WAVReader.h>

#include <asl/file_functions.h>
#include <asl/Exception.h>

#include <asl/Time.h>
#include <asl/Logger.h>
#include <dom/Nodes.h>

#ifdef LINUX
#include <unistd.h>
#endif


using namespace std;
using namespace asl;
using namespace AudioApp;
using namespace AudioBase;

TestAudioController::TestAudioController (const string & myDataDir)
        : UnitTest ("TestAudioController"),
          _myDataDir(myDataDir)
{

}

void TestAudioController::runWithMixer(const string& myMixer) {
    try {
        AudioController & myController = AudioController::get();
        string myReaderID1 = myController.play(_myDataDir+"/stereotest480.wav",
                myMixer, 1, 0, false);
        ENSURE_MSG (true, "Initial play is ok.");

        msleep (2000);
        string myReaderID2 = myController.play
            (_myDataDir+"/pitchbend.wav", myMixer, 1, 0, false);
        ENSURE_MSG (true, "Play after pause is ok.");

        myController.setVolume (myMixer, 0.23);
        SUCCESS ("Set Mixer Volume worked.");

        msleep(500);
        DPRINT(fabs(myController.getVolume(myMixer)));
        ENSURE (fabs(myController.getVolume(myMixer) - 0.23) < 0.01);

        msleep (100);
        string myReaderID3 = myController.play(_myDataDir+"/A440-48000-16-1.wav", myMixer, 0.5, 0, false);
        ENSURE_MSG (true, "Two plays at once work.");

        // UH: these timed tests simply *DO*NOT*WORK*RELIABLY*
#if 0
        msleep (100);
        double d = myController.getDuration(myReaderID2);
        ENSURE (d<3.0);
#endif

        if (myController.isPlaying(myReaderID2)) {
            myController.stop(myReaderID2);
        } else {
            DPRINT("ID2 already stopped");
        }
        if (myController.isPlaying(myReaderID3)) {
            myController.stop(myReaderID3);
        } else {
            DPRINT("ID3 already stopped");
        }
        ENSURE_MSG (true, "Stop works.");

        msleep (100);
        myReaderID1 = myController.play (_myDataDir+"/A440-48000-16-2.wav",
                myMixer, 1, 0, true);
        SUCCESS ("loop play works.");
        msleep (1000);
        myController.stop(myReaderID1);
        SUCCESS ("loop stop works.");
        msleep (100);

        myReaderID1 = myController.play (_myDataDir+"/A440-48000-16-2.wav",
                myMixer, 1, 0, false);
        ENSURE(myReaderID1 != "");
        msleep (80);

        myController.setVolume (myReaderID1, 0.2);
        //msleep (80);

        myController.setVolume (myReaderID1, 0.8);
        //msleep (80);

        myController.fadeToVolume (myReaderID1, 0.2, 0.2);
        //msleep (80);

        DPRINT(fabs(myController.getVolume(myMixer)));
        DPRINT(fabs(myController.getVolume(myMixer)));
        myController.stop(myReaderID1);
        SUCCESS ("set reader volume works.");

        myReaderID1 = myController.play(_myDataDir+"/A440-48000-16-2.wav",
                myMixer, 1, 0, false);
        ENSURE(myReaderID1 != "");
        msleep(80);
        ENSURE(myController.isPlaying(myReaderID1));
        myController.stop(myReaderID1);
        ENSURE(!myController.isPlaying(myReaderID1));

        myReaderID1 = myController.play(_myDataDir+"/A440-48000-16-2.wav",
                myMixer, 1, 0, false);
        msleep(1000);
        ENSURE(!myController.isPlaying(myReaderID1));

        myReaderID1 = myController.play(_myDataDir+"/A440-48000-16-2.wav",
                myMixer, 1, 0, false);
        msleep(50);
        myController.stopAllSounds();
        ENSURE(!myController.isPlaying(myReaderID1));

        msleep(50);
        myController.stopAllSounds();
        ENSURE(!myController.isPlaying(myReaderID1));

    } catch (const SoundException & e) {
        AC_ERROR << e << endl;
        ENSURE (false);
    }
}

void
TestAudioController::testThreading() {
    string myReaderID;
    for (int j=0; j<1000000; j++) {
        myReaderID = AudioController::get().play(_myDataDir+"/stereotest480.wav",
                "Mixer", 0.05, 0, false);
        msleep (50);
        AudioController::get().stop(myReaderID);
        msleep ((50*rand())/RAND_MAX);
        if (j%100 == 0) {
            cerr << j << endl;
        }
    }
    msleep (4000);
    cerr << "Final # sounds:" << AudioController::get().getNumSounds()<<endl;
}

void
TestAudioController::testLotsOfSounds() {
    string myReaderIDs[16];
    for (int j=0; j<100000; j++) {
        myReaderIDs[0] = AudioController::get().play(_myDataDir+"/stereotest480.wav",
                "Music", 0.05, 0, false);
        //        myReaderID = theController.play("",_myDataDir+"/explode.wav",
        //                "SFX1", 0.05, 0, false, true);
        cerr << "# sounds:" << AudioController::get().getNumSounds() << endl;
        msleep (20);
        for (int i=14; i>=0; i--) {
            myReaderIDs[i+1] = myReaderIDs[i];
        }
        if (j>14) {
            cerr << "stop: " << myReaderIDs[15] << endl;
            AudioController::get().stop(myReaderIDs[15]);
        }
    }
    for (int i=1; i<15; i++) {
        cerr << "end: " << myReaderIDs[i] << endl;
        AudioController::get().stop(myReaderIDs[i]);
    }
    msleep (4000);
    cerr << "Final # sounds:" << AudioController::get().getNumSounds() << endl;
}

void TestAudioController::playAtPos(const asl::Point2d & thePos) {
//    string myReaderID1 = getAudioController().play2d("",_myDataDir+"/explode.wav",
//                "2dMixer", thePos, 0.5, 0, false, false);
    string myReaderID1 = AudioController::get().play2d(_myDataDir+"/A440-48000-16-2.wav",
                "2dMixer", thePos, 1, 0, false);
    msleep (500);
    ENSURE_MSG(true, "playAtPos worked");
    AudioController::get().stop(myReaderID1);
}

void TestAudioController::testSpatialMixer() {
//    for (double x =0.0; x<1; x+=0.02) {
//        playAtPos(Coord2dD(x,0));
//    }
//     for (int i=0; i<20; i++) {
        playAtPos(Point2d(0.0,0.0));
        playAtPos(Point2d(0.0,1.0));
        playAtPos(Point2d(1.0,0.0));
        playAtPos(Point2d(1.0,1.0));
        playAtPos(Point2d(0.3,0.5));
//    }
    msleep (100);
}

void TestAudioController::runLatencyTests (const dom::Node& myConfig) {
    cerr << "Running latency tests..." << endl;
    AudioController & theController = AudioController::get();
    map<double,int> myUnderrunMap;
    double i;
    for (i=0.1; i>0.001; i/=2) {
        theController.init(myConfig, i);
        cerr << theController.getLatency()<< endl;
        string myReaderID1 = theController.play (_myDataDir+"/stereotest480.wav",
			   "Mixer", 1, 0, 1);
        msleep (20000);
        theController.stop(myReaderID1);

        myUnderrunMap[theController.getLatency()] = theController.getNumUnderruns();
    }

    cerr << "Latency test results: "<<endl;
    map<double,int>::iterator it;
    for (it=myUnderrunMap.begin(); it!=myUnderrunMap.end(); it++) {
        cerr<<"  Latency: " << (*it).first
              << " sec., underruns: " << (*it).second<<endl;
    }
}

void TestAudioController::runMultiChannel() {
    runWithMixer("Music");
    runWithMixer("PanSFX");
    runWithMixer("SFX1");
    runWithMixer("SFX2");

/*    IAudioController& myController = AudioController::get();
    string myReaderID1 = myController.play ("",_myDataDir+"/stereotest480.wav",
            myMixer, 1, 0, false);
    myController.flush();
    ENSURE_MSG (true, "Initial play is ok.");
*/
}

void
TestAudioController::playFileOnMixer(const string & theMixerID) {
    AudioController& theController = AudioController::get();
    string myReaderID = theController.play(_myDataDir+"/stereotest480.wav",
            theMixerID, 1, 0, false);
    msleep (500);
}

void
TestAudioController::runSimple() {
    playFileOnMixer("Music");
    playFileOnMixer("SFX1");
    playFileOnMixer("SFX2");
    msleep (1800);
}

void
TestAudioController::testAllChannels() {
    AudioController::get().init("../../testfiles/AllChannelsConfig.xml");
    AudioController::get().play(_myDataDir+"/stereotest480.wav", "Mixer", 1, 0, true);
//    AudioController::get().play(_myDataDir+"/explode.wav", "Mixer", 1, 0, true);
    while (true) {
        msleep (6000000);
    }
}

void TestAudioController::runBase(bool hasDefaultConfig) {
    runWithMixer("Mixer");

    AudioController& myController = AudioController::get();
    try {
 	    DPRINT(myController.getVolume("SoundOut"));
        myController.setVolume ("SoundOut", 0.666);
        SUCCESS ("Set SoundOut Volume worked.");
        msleep(1000);

        DPRINT(fabs(myController.getVolume("SoundOut") - 0.666));
        ENSURE (fabs(myController.getVolume("SoundOut") - 0.666) < 0.05);
        DPRINT(myController.getVolume("SoundOut"));
    } catch (SoundException e) {
        AC_ERROR << e << endl;
        ENSURE (false);
    }

    if (!hasDefaultConfig) {
        bool myExceptionFired = false;
        try {
            myController.play (_myDataDir+"/A440-48000-16-2.wav",
                    "MixerDoesntExist", 1, 0, true);
        } catch (SoundException e) {
            myExceptionFired = true;
        }
        ENSURE_MSG(myExceptionFired, "No mixer exception works.");
    }
}

void TestAudioController::run() {
    try {
        //testAllChannels();
        //AudioController::get().init("../../testfiles/ComplexAudioConfig.xml");
        //testLotsOfSounds();

#ifndef WIN32
        //AudioController::get().init("../../testfiles/AllModulesConfig.xml");
        //runSimple();
#endif

        AudioController::get().init("../../testfiles/StereoAudioConfig.xml");
        runBase(false);
        //testThreading();
        //runLatencyTests(*theAudioConfig);

        //AudioController::get().init();
        runBase(true);

#if 0
        ENSURE_MSG(true, "ComplexAudioConfig.xml");
        AudioController::get().init("../../testfiles/ComplexAudioConfig.xml");
        runMultiChannel();
        testSpatialMixer();
#endif

#if 0
        ENSURE_MSG(true, "StereoAudioInConfig.xml");
        AudioController::get().init("../../testfiles/StereoAudioInConfig.xml");
        msleep (3000);
#endif
    } catch (asl::Exception e) {
        AC_ERROR << e << endl ;
        ENSURE (false);
    }
}

