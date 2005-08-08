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

#ifndef INCL_TESTSOUND
#define INCL_TESTSOUND

#include <asl/UnitTest.h>

#include "Media.h"

#include <asl/numeric_functions.h>
#include <asl/PlugInManager.h>
#include <asl/Pump.h>

#include <asl/Time.h>

using namespace std;
using namespace asl;
using namespace y60;

class SoundTestBase: public UnitTest {
    public:
        SoundTestBase(const char * myName) 
            : UnitTest(myName)
        {  
            PlugInBasePtr myPlugIn = PlugInManager::get().getPlugIn("y60Media");
            _myMedia = dynamic_cast_Ptr<Media>(myPlugIn);
        }

    protected:
        Ptr<Media> getMedia() {
            return _myMedia;
        }
       
        void checkTime(SoundPtr theSound, asl::Time theTime) {
            double myTime = theSound->getCurrentTime();
            ENSURE(myTime > theTime-0.1 && myTime < theTime+0.2);
            if (myTime <= theTime-0.1 || myTime >= theTime+0.2) {
                AC_WARNING << "Time measured: " << myTime << ", time expected: " << theTime;
            }
        }
        
    private:
        Ptr<Media> _myMedia;
};

class TestPlay : public SoundTestBase {
    public:
        TestPlay() 
            : SoundTestBase("TestPlay")
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
            SoundPtr mySound = getMedia()->createSound(theURI);
            mySound->play();
            while(mySound->isPlaying()) {
                msleep(100);
            }
        }
};

class TestFireAndForget: public SoundTestBase {
    public:
        TestFireAndForget() 
            : SoundTestBase("TestFireAndForget")
        {  
        }

        void run() {
            Time myDuration;
            {
                SoundPtr mySound = 
                        getMedia()->createSound("../../testfiles/stereotest441.wav");
                mySound->play();
                myDuration = mySound->getDuration();
            }
            msleep(unsigned(myDuration*1000)+200);
            ENSURE(getMedia()->getNumSounds() == 0);
        }
};


class TestTwoSounds: public SoundTestBase {
    public:
        TestTwoSounds() 
            : SoundTestBase("TestTwoSounds")
        {  
        }

        void run() {
            Time myDuration;
            SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            myDuration = mySound->getDuration();
            mySound = getMedia()->createSound("../../testfiles/stereotest441.wav");
            mySound->play();
            myDuration = maximum(myDuration, mySound->getDuration());
            msleep(unsigned(myDuration*1000));
        }
};

class TestStopAll: public SoundTestBase {
    public:
        TestStopAll() 
            : SoundTestBase("TestStopAll")
        {  
        }

        void run() {
            {
                SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
                mySound->play();
                mySound = getMedia()->createSound("../../testfiles/stereotest441.wav");
                mySound->play();
            }
            msleep(700);
            getMedia()->stopAll();
            ENSURE(getMedia()->getNumSounds() == 0);
        }
};

class TestStop: public SoundTestBase {
    public:
        TestStop() 
            : SoundTestBase("TestStop")
        {  
        }

        void run() {
            SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            msleep(200);
            checkTime(mySound, 0.2);
            
            mySound->stop();
            msleep(100);
            checkTime(mySound, 0);
            
            mySound->play();
            msleep(200);
            checkTime(mySound, 0.2);

            mySound->stop();

            mySound->play();
            mySound->stop();

            ENSURE(mySound->getNumUnderruns() == 0);
        }
};

class TestStopByItself: public SoundTestBase {
    public:
        TestStopByItself() 
            : SoundTestBase("TestStopByItself")
        {  
        }

        void run() {
            SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            while(mySound->isPlaying()) {
                msleep(100);
            }
            
            mySound->play();
            msleep(100);
            mySound->stop();

            ENSURE(mySound->getNumUnderruns() == 0);
        }
};

class TestPause: public SoundTestBase {
    public:
        TestPause() 
            : SoundTestBase("TestPause")
        {  
        }
       
        void run() {
            SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            msleep(200);
            checkTime(mySound, 0.2);
            mySound->pause();
            msleep(400);
            checkTime(mySound, 0.2);
            mySound->play();
            checkTime(mySound, 0.2);
            while(mySound->isPlaying()) {
                msleep(100);
            }
            ENSURE(mySound->getNumUnderruns() == 0);

            mySound->play();
            msleep(200);
            mySound->pause();
            msleep(100);
            checkTime(mySound, 0.3);
            mySound->stop();
            checkTime(mySound, 0);

            mySound->play();
            mySound->pause();
            msleep(100);
            checkTime(mySound, 0);
            mySound->stop();

            mySound->play();
            msleep(200);
            mySound->pause();
            mySound->stop();

            ENSURE(mySound->getNumUnderruns() == 0);
        }
};

class TestLoop: public SoundTestBase {
    public:
        TestLoop() 
            : SoundTestBase("TestLoop")
        {  
        }
       
        void run() {
            SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3", true);
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
};

class TestVolume: public SoundTestBase {
    public:
        TestVolume() 
            : SoundTestBase("TestVolume")
        {  
        }
       
        void run() {
            {   // Sound volume 
                SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
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
                SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
                getMedia()->setVolume(0.2f);
                mySound->play();
                msleep(200);
                getMedia()->setVolume(0.5f);
                msleep(200);
                ENSURE(almostEqual(getMedia()->getVolume(), 0.5));
                getMedia()->fadeToVolume(1, 0.3f);
                msleep(400);
                ENSURE(almostEqual(getMedia()->getVolume(), 1));
                mySound->stop();
            }
            
        }
};

class StressTest: public SoundTestBase {
    public:
        StressTest(double myDuration) 
            : SoundTestBase("StressTest"),
              _myDuration(myDuration)
        {  
        }
       
        void run() {
            Time myStartTime;
            while(double(Time())-myStartTime < _myDuration) { 
                SoundPtr mySound = getMedia()->createSound
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
};

class SoundTestSuite : public UnitTestSuite {
    public:
        SoundTestSuite(const char * myName, bool myUseDummyPump) 
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

            addTest(new TestPlay());
            addTest(new TestFireAndForget());
            addTest(new TestTwoSounds());
            addTest(new TestStop());
            addTest(new TestStopByItself());
            
            addTest(new TestPause());
            
            addTest(new TestStopAll());
            addTest(new TestLoop());
            addTest(new TestVolume());
            addTest(new StressTest(5));

            // 24 Hour test :-).
//            addTest(new StressTest(myMedia, 60*60*24));
        }

    private:
        bool _myUseDummyPump;
};

#endif
