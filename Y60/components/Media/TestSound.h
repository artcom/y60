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
#include <asl/string_functions.h>
#include <asl/PlugInManager.h>
#include <asl/proc_functions.h>

#include <asl/Time.h>

using namespace std;
using namespace asl;
using namespace y60;

class SoundTestBase: public UnitTest {
    public:
        SoundTestBase(Ptr<Media> myMedia, const char * myName) 
            : UnitTest(myName),
              _myMedia(myMedia)
        {  
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
        TestPlay(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestPlay")
        {  
        }

        void run() {
            play("../../testfiles/aussentuer.mp3");
            play("../../testfiles/sz5-1_c-beam Warnung_Time_1.WAV");
            play("../../testfiles/stereotest441.wav");
            play("../../testfiles/stereotest480.wav");
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

class TestBroken : public SoundTestBase {
    public:
        TestBroken(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestBroken")
        {  
        }

        void run() {
            play("../../testfiles/leer.wav");
            play("../../testfiles/broken.wav");
        }

    private:
        void play(const std::string & theURI) {
            bool myException = false;
            try {
                SoundPtr mySound = getMedia()->createSound(theURI);
                mySound->play();
                while(mySound->isPlaying()) {
                    msleep(100);
                }
            } catch (asl::Exception&) {
                myException = true;
            }
            ENSURE(myException);
        }
};

class TestFireAndForget: public SoundTestBase {
    public:
        TestFireAndForget(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestFireAndForget")
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
        TestTwoSounds(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestTwoSounds")
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
        TestStopAll(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestStopAll")
        {  
        }

        void run() {
            ENSURE(getMedia()->getNumSounds() == 0);
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
        TestStop(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestStop")
        {  
        }

        void run() {
            runLoop(false);
            runLoop(true);
            msleep(100);
            ENSURE(getMedia()->getNumSounds() == 0);
        }
        
    private:
        void runLoop(bool theLoop) {
            SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3", theLoop);
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
        TestStopByItself(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestStopByItself")
        {  
        }

        void run() {
            {
                SoundPtr mySound = getMedia()->createSound("../../testfiles/aussentuer.mp3");
                mySound->play();
                while(mySound->isPlaying()) {
                    msleep(100);
                }
                
                ENSURE(mySound->getNumUnderruns() == 0);
            }
            msleep(100);
            ENSURE(getMedia()->getNumSounds() == 0);

            {
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
            msleep(100);
            ENSURE(getMedia()->getNumSounds() == 0);
        }

};

class TestPause: public SoundTestBase {
    public:
        TestPause(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestPause")
        {  
        }
       
        void run() {
            {
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
            }
            runLoop(false);
            runLoop(true);
        }
        
    private:
        void runLoop(bool theLoop) {
            {
                SoundPtr mySound = getMedia()->createSound
                        ("../../testfiles/aussentuer.mp3", theLoop);
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
            msleep(100);
            ENSURE(getMedia()->getNumSounds() == 0);
        }
};

class TestLoop: public SoundTestBase {
    public:
        TestLoop(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestLoop")
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
        TestVolume(Ptr<Media> myMedia) 
            : SoundTestBase(myMedia, "TestVolume")
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

class TestSeek: public SoundTestBase {
    public:
        TestSeek(Ptr<Media> myMedia)
            : SoundTestBase(myMedia, "TestSeek")
        {
        }

        void run() {
            SoundPtr mySound = getMedia()->createSound
                    ("../../testfiles/sz5-1_c-beam Warnung_Time_1.WAV");
            // Seek at start
            mySound->seek(0.5);
            checkTime(mySound, 0.5);
            mySound->play();
            checkTime(mySound, 0.5);
            msleep(200);
            checkTime(mySound, 0.7);

            // Seek while playing
            mySound->seek(2.0);
            checkTime(mySound, 2.0);
            msleep(200);
            checkTime(mySound, 2.2);
            
            // Seek while paused
            mySound->pause();
            mySound->seek(4.0);
            checkTime(mySound, 4.0);
            mySound->play();
            checkTime(mySound, 4.0);
            msleep(200);

            // Seek while stopped
            mySound->stop();
            mySound->seek(4.0);
            checkTime(mySound, 4.0);
            mySound->play();
            checkTime(mySound, 4.0);
            msleep(200);
            checkTime(mySound, 4.2);
            mySound->stop();
        }
};

class StressTest: public SoundTestBase {
    public:
        StressTest(Ptr<Media> myMedia, double myDuration) 
            : SoundTestBase(myMedia, "StressTest"),
              _myDuration(myDuration)
        {  
        }
       
        virtual void run() {
            Time myStartTime;
            int i = 0;
            while(double(Time())-myStartTime < _myDuration) {
                doIteration(i);
                ++i;
            }
            getMedia()->stopAll();
            SUCCESS("StressTest");
        }

    protected:
        void doIteration(int i) {
            SoundPtr mySound = getMedia()->createSound
                ("../../testfiles/stereotest441.wav", false,
                 "../../testfiles/stereotest441.wav "+asl::as_string(i));
            mySound->setVolume(0.02f);
            mySound->play();
            double r1 = rand()/double(RAND_MAX);
            unsigned myTime = unsigned(3*r1);
            msleep(myTime);
        }
        
        double _myDuration;
};

class MemLeakStressTest: public StressTest {
    public:
        MemLeakStressTest(Ptr<Media> myMedia, double myDuration) 
            : StressTest(myMedia, myDuration)
        {
        }
       
        void run() {
            Time myStartTime;
            Time myLastMemCheckTime = 0;
            Time myLastMemIncreaseTime;
            unsigned myMaxMemoryUsage = 0;
            int i = 0;
            while(double(Time())-myStartTime < _myDuration) {
                doIteration(i);
                ++i;
                if (Time()-myLastMemCheckTime > 10) {
                    myLastMemCheckTime = Time();
                    unsigned myMemoryUsage = getProcessMemoryUsage();
                    if (myMemoryUsage > myMaxMemoryUsage) {
                        myMaxMemoryUsage = myMemoryUsage;
                        myLastMemIncreaseTime = myLastMemCheckTime;
                        AC_WARNING << "Max memory usage increased: " << myMaxMemoryUsage;
                    }
                }
            }
            getMedia()->stopAll();
            ENSURE(Time()-myLastMemIncreaseTime > 10*60);
            if (Time()-myLastMemIncreaseTime < 10*60) {
                AC_PRINT << "Last memory usage maximum was " << 
                    (Time()-myLastMemIncreaseTime)/60 << " min. ago.";
                AC_PRINT << "Maximum memory used: " << myMaxMemoryUsage;
            }
        }
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

            typedef y60::Media * (*GetMediaFunctionPtr)();
            y60::MediaPtr myMedia;
            GetMediaFunctionPtr myGetMediaFunction = (GetMediaFunctionPtr)PlugInManager::getFunction(myPlugIn->getDLHandle(), "createMedia");
            if (myGetMediaFunction) {
                myMedia = y60::MediaPtr((*myGetMediaFunction)());
            } else {
                cerr << "Could not get 'createMedia' fuction pointer from dll." << endl;
            }

            ENSURE(myMedia);
#ifdef WIN32
            myMedia->setSysConfig(0.05, "");
#else
            myMedia->setSysConfig(0.02, "");
#endif
            myMedia->setAppConfig(44100, 2, _myUseDummyPump);
            
            addTest(new TestPlay(myMedia));
            addTest(new TestBroken(myMedia));
            addTest(new TestFireAndForget(myMedia));
            addTest(new TestTwoSounds(myMedia));
            addTest(new TestStop(myMedia));
            addTest(new TestStopByItself(myMedia));
            addTest(new TestPause(myMedia));
            addTest(new TestStopAll(myMedia));           
            addTest(new TestLoop(myMedia));
            addTest(new TestVolume(myMedia));
            addTest(new TestSeek(myMedia));
          
//            addTest(new StressTest(myMedia, 5));

//            addTest(new MemLeakStressTest(myMedia, 60*60*24));
        }

    private:
        bool _myUseDummyPump;
};

#endif
