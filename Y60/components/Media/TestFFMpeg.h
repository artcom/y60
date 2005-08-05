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

#ifndef INCL_TESTFFMPEG
#define INCL_TESTFFMPEG

#include <asl/UnitTest.h>

#include "Media.h"

#include <asl/numeric_functions.h>
#include <asl/PlugInManager.h>
#include <asl/Pump.h>

#include <asl/Time.h>

using namespace std;
using namespace asl;
using namespace y60;

class TestPlay : public UnitTest {
    public:
        TestPlay(Ptr<Media> myMedia) 
            : UnitTest("TestPlay"),
              _myMedia(myMedia)
        {  
        }

        void run() {
            play("../../testfiles/aussentuer.mp3");
            play("../../testfiles/sz5-1_c-beam Warnung_Time_1.WAV");
            play("../../testfiles/stereotest441.wav");
//            play("../../testfiles/stereotest480.wav");
        }

    private:
        void play(const std::string & theURI) {
            SoundPtr mySound = _myMedia->createSound(theURI);
            mySound->play();
            while(mySound->isPlaying()) {
                msleep(100);
            }
        }
    
        Ptr<Media> _myMedia;
};

class TestFireAndForget: public UnitTest {
    public:
        TestFireAndForget(Ptr<Media> myMedia) 
            : UnitTest("TestFireAndForget"),
              _myMedia(myMedia)
        {  
        }

        void run() {
            Time myDuration;
            {
                SoundPtr mySound = 
                        _myMedia->createSound("../../testfiles/stereotest441.wav");
                mySound->play();
                myDuration = mySound->getDuration();
            }
            msleep(unsigned(myDuration*1000)+200);
            ENSURE(_myMedia->getNumSounds() == 0);
        }

    private:
        Ptr<Media> _myMedia;
};


class TestTwoSounds: public UnitTest {
    public:
        TestTwoSounds(Ptr<Media> myMedia) 
            : UnitTest("TestTwoSounds"),
              _myMedia(myMedia)
        {  
        }

        void run() {
            Time myDuration;
            SoundPtr mySound = _myMedia->createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            myDuration = mySound->getDuration();
            mySound = _myMedia->createSound("../../testfiles/stereotest441.wav");
            mySound->play();
            myDuration = maximum(myDuration, mySound->getDuration());
            msleep(unsigned(myDuration*1000));
        }

    private:
        Ptr<Media> _myMedia;
};

class TestStopAll: public UnitTest {
    public:
        TestStopAll(Ptr<Media> myMedia) 
            : UnitTest("TestStopAll"),
              _myMedia(myMedia)
        {  
        }

        void run() {
            {
                SoundPtr mySound = _myMedia->createSound("../../testfiles/aussentuer.mp3");
                mySound->play();
                mySound = _myMedia->createSound("../../testfiles/stereotest441.wav");
                mySound->play();
            }
            msleep(700);
            _myMedia->stopAll();
            ENSURE(_myMedia->getNumSounds() == 0);
        }

    private:
        Ptr<Media> _myMedia;
};

class TestStop: public UnitTest {
    public:
        TestStop(Ptr<Media> myMedia) 
            : UnitTest("TestStop"),
              _myMedia(myMedia)
        {  
        }

        void run() {
            SoundPtr mySound = _myMedia->createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            msleep(200);
            double myTime = mySound->getCurrentTime();
            ENSURE(myTime > 0.1 && myTime < 0.3);
            
            mySound->stop();
            msleep(100);
            myTime = mySound->getCurrentTime();
            ENSURE(myTime < 0.001);
            
            mySound->play();
            myTime = mySound->getCurrentTime();
            msleep(200);
            myTime = mySound->getCurrentTime();
            ENSURE(myTime > 0.1 && myTime < 0.3);

            mySound->stop();
        }

     private:
        Ptr<Media> _myMedia;
};

class TestPause: public UnitTest {
    public:
        TestPause(Ptr<Media> myMedia) 
            : UnitTest("TestPause"),
              _myMedia(myMedia)
        {  
        }
       
        void run() {
            SoundPtr mySound = _myMedia->createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            msleep(200);
            double myTime = mySound->getCurrentTime();
            ENSURE(myTime > 0.1 && myTime < 0.3);
            mySound->pause();
            msleep(400);
            myTime = mySound->getCurrentTime();
            ENSURE(myTime > 0.1 && myTime < 0.3);
            mySound->play();
            myTime = mySound->getCurrentTime();
            ENSURE(myTime > 0.1 && myTime < 0.3);
            while(mySound->isPlaying()) {
                msleep(100);
            }
        }

    private:
        Ptr<Media> _myMedia;
};

class TestLoop: public UnitTest {
    public:
        TestLoop(Ptr<Media> myMedia) 
            : UnitTest("TestLoop"),
              _myMedia(myMedia)
        {  
        }
       
        void run() {
            SoundPtr mySound = _myMedia->createSound("../../testfiles/aussentuer.mp3", true);
            mySound->play();
            msleep(3000);
            double myTime = mySound->getCurrentTime();
            ENSURE(myTime > 2.5 && myTime < 4.5);
            mySound->pause();
            msleep(500);
            myTime = mySound->getCurrentTime();
            ENSURE(myTime > 2.5 && myTime < 4.5);
            mySound->play();
            msleep(1000);
            myTime = mySound->getCurrentTime();
            ENSURE(myTime > 3.5 && myTime < 5.5);
            mySound->stop();
        }

    private:
        Ptr<Media> _myMedia;
};

class TestVolume: public UnitTest {
    public:
        TestVolume(Ptr<Media> myMedia) 
            : UnitTest("TestVolume"),
              _myMedia(myMedia)
        {  
        }
       
        void run() {
            {   // Sound volume 
                SoundPtr mySound = _myMedia->createSound("../../testfiles/aussentuer.mp3");
                mySound->setVolume(0.2f);
                mySound->play();
                msleep(200);
                mySound->setVolume(0.5f);
                msleep(200);
                ENSURE(almostEqual(mySound->getVolume(), 0.5));
                mySound->fadeToVolume(1, 0.3);
                msleep(400);
                ENSURE(almostEqual(mySound->getVolume(), 1));
                mySound->stop();
            }
            {   // Global volume
                SoundPtr mySound = _myMedia->createSound("../../testfiles/aussentuer.mp3");
                _myMedia->setVolume(0.2f);
                mySound->play();
                msleep(200);
                _myMedia->setVolume(0.5f);
                msleep(200);
                ENSURE(almostEqual(_myMedia->getVolume(), 0.5));
                _myMedia->fadeToVolume(1, 0.3f);
                msleep(400);
                ENSURE(almostEqual(_myMedia->getVolume(), 1));
                mySound->stop();
            }
            
        }

    private:
        Ptr<Media> _myMedia;
};

class StressTest: public UnitTest {
    public:
        StressTest(Ptr<Media> myMedia, double myDuration) 
            : UnitTest("StressTest"),
              _myMedia(myMedia),
              _myDuration(myDuration)
        {  
        }
       
        void run() {
            Time myStartTime;
            while(double(Time())-myStartTime < _myDuration) { 
                SoundPtr mySound = _myMedia->createSound
                        ("../../testfiles/stereotest441.wav", false);
                mySound->setVolume(0.2f);
                mySound->play();
                double r1 = rand()/double(RAND_MAX);
                unsigned myTime = unsigned(50*r1);
                msleep(myTime);
            }
            SUCCESS("StressTest");
        }

    private:
        double _myDuration;
        Ptr<Media> _myMedia;
};

class FFMpegTestSuite : public UnitTestSuite {
    public:
        FFMpegTestSuite(const char * myName, bool myUseDummyPump) 
            : UnitTestSuite(myName),
              _myUseDummyPump (myUseDummyPump)
        {}

        void setup() {
            UnitTestSuite::setup();

            PlugInManager::get().setSearchPath("${PRO}/lib");
            PlugInBasePtr myPlugIn = PlugInManager::get().getPlugIn("y60Media");
            Ptr<Media> myMedia = dynamic_cast_Ptr<Media>(myPlugIn);
            ENSURE(myMedia);
#ifdef WIN32
            myMedia->setSysConfig(0.05, "");
#else
            myMedia->setSysConfig(0.02, "");
#endif
            myMedia->setAppConfig(44100, 2, _myUseDummyPump);

            addTest(new TestPlay(myMedia));
            addTest(new TestFireAndForget(myMedia));
            addTest(new TestTwoSounds(myMedia));
            addTest(new TestPause(myMedia));
            addTest(new TestStop(myMedia));
            addTest(new TestStopAll(myMedia));
            addTest(new TestLoop(myMedia));
            addTest(new TestVolume(myMedia));
            addTest(new StressTest(myMedia, 5));

            // 24 Hour test :-).
//            addTest(new StressTest(myMedia, 60*60*24));
        }

    private:
        bool _myUseDummyPump;
};

#endif
