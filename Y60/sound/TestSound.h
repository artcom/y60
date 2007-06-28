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

#include "SoundManager.h"
#include "TestGrainSource.h"
#include <asl/Pump.h>

#include <asl/numeric_functions.h>
#include <asl/string_functions.h>
#include <asl/PlugInManager.h>
#include <asl/proc_functions.h>
#include <asl/os_functions.h>

#include <asl/Time.h>

using namespace std;
using namespace asl;
using namespace y60;

class SoundTestBase: public UnitTest {
    public:
        SoundTestBase(SoundManager& mySoundManager, const char * myName) 
            : UnitTest(myName),
              _mySoundManager(mySoundManager)
        {  
        }

    protected:
        SoundManager& getSoundManager() {
            return _mySoundManager;
        }
 
        void checkTime(SoundPtr theSound, asl::Time theTime) {
            double myTime = theSound->getCurrentTime();
            ENSURE(myTime > theTime-0.1 && myTime < theTime+0.2);
            if (myTime <= theTime-0.1 || myTime >= theTime+0.2) {
                AC_WARNING << "Time measured: " << myTime << ", time expected: " 
                        << theTime;
            }
        }
        
    private:
        SoundManager& _mySoundManager;
};

class TestLeak : public SoundTestBase {
    public:
        TestLeak(SoundManager & theSoundManager)
            : SoundTestBase(theSoundManager, "TestLeak")
        {
        }

        void run() {

            AC_PRINT << "### INITIAL USAGE:" << getProcessMemoryUsage();
            unsigned myStartUsage = 0;

            const char * mySoundFile = "../../testfiles/Plopp_2a.wav";
            int myDiff;
            
            for (unsigned j=0; j<20; ++j) {
                std::vector<SoundPtr> mySounds;
                for (unsigned i = 0; i < 20; ++i) {
                    SoundPtr mySound = getSoundManager().createSound(mySoundFile, false, false);
                    mySounds.push_back(mySound);
                    mySound->play();

                    while (mySound->isPlaying()) {
                        asl::msleep(100);
                    }

                    unsigned myUsage = getProcessMemoryUsage();
                    if (i == 0 && j == 0) {
                        myStartUsage = myUsage;
                        AC_PRINT << "### START USAGE: " << myStartUsage;
                    }
                    myDiff = myUsage - myStartUsage;
                    AC_PRINT << "### TEST " << i << " USAGE:" << myUsage << " DIFF:" 
                            << myDiff << " SOUNDCOUNT:" << getSoundManager().getNumSounds();
                }

                mySounds.clear();
                asl::msleep(1000);

                unsigned myFiniUsage = getProcessMemoryUsage();
                myDiff = myFiniUsage - myStartUsage;
                AC_PRINT << "### FINI USAGE:" << myFiniUsage << " DIFF:" 
                        << myDiff << " SOUNDCOUNT:" << getSoundManager().getNumSounds();
                ENSURE(getSoundManager().getNumSounds() == 0);
            }
            asl::msleep(2000);
            unsigned myFiniUsage = getProcessMemoryUsage();
            myDiff = myFiniUsage - myStartUsage;
            AC_PRINT << "### FINI USAGE:" << myFiniUsage << " DIFF:" << myDiff << 
                    " SOUNDCOUNT:" << getSoundManager().getNumSounds();
            AC_PRINT << "Cache items: " << getSoundManager().getNumItemsInCache() 
                    << ", memory: " << getSoundManager().getCacheMemUsed();
        }
};

class TestPlay : public SoundTestBase {
    public:
        TestPlay(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestPlay")
        {  
        }

        void run() {
            play("../../testfiles/aussentuer.mp3");
            play("../../testfiles/sz5-1_c-beam Warnung_Time_1.WAV");
            play("../../testfiles/stereotest441.wav");
            play("../../testfiles/stereotest480.wav");
            play("../../testfiles/Plopp_2a.wav");
            msleep(100);
            ENSURE(getSoundManager().getNumSounds() == 0);
        }

    private:
        void play(const std::string & theURI) {
            SoundPtr mySound = getSoundManager().createSound(theURI);
            mySound->play();
            while(mySound->isPlaying()) {
                msleep(100);
            }
        }
};

class TestBroken : public SoundTestBase {
    public:
        TestBroken(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestBroken")
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
                SoundPtr mySound = getSoundManager().createSound(theURI);
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
        TestFireAndForget(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestFireAndForget")
        {  
        }

        void run() {
            Time myDuration;
            {
                SoundPtr mySound = 
                        getSoundManager().createSound("../../testfiles/stereotest441.wav");
                mySound->play();
                myDuration = mySound->getDuration();
            }
            msleep(unsigned(myDuration*1000)+200);
            ENSURE(getSoundManager().getNumSounds() == 0);
        }
};


class TestTwoSounds: public SoundTestBase {
    public:
        TestTwoSounds(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestTwoSounds")
        {  
        }

        void run() {
            Time myDuration;
            SoundPtr mySound = getSoundManager().createSound("../../testfiles/aussentuer.mp3");
            mySound->play();
            myDuration = mySound->getDuration();
            ENSURE(fabs(myDuration-1.619) < 0.01);
            mySound = getSoundManager().createSound("../../testfiles/stereotest441.wav");
            mySound->play();
            myDuration = maximum(myDuration, mySound->getDuration());
            msleep(unsigned(myDuration*1000));
        }
};

class TestStopAll: public SoundTestBase {
    public:
        TestStopAll(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestStopAll")
        {  
        }

        void run() {
            ENSURE(getSoundManager().getNumSounds() == 0);
            {
                SoundPtr mySound = getSoundManager().createSound(
                        "../../testfiles/aussentuer.mp3");
                mySound->play();
                mySound = getSoundManager().createSound("../../testfiles/stereotest441.wav");
                mySound->play();
            }
            msleep(700);
            getSoundManager().stopAll();
            ENSURE(getSoundManager().getNumSounds() == 0);
        }
};

class TestStop: public SoundTestBase {
    public:
        TestStop(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestStop")
        {  
        }

        void run() {
            runLoop(false);
            runLoop(true);
            msleep(100);
            ENSURE(getSoundManager().getNumSounds() == 0);
        }
        
    private:
        void runLoop(bool theLoop) {
            SoundPtr mySound = getSoundManager().createSound(
                    "../../testfiles/aussentuer.mp3", theLoop);
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

class TestCache: public SoundTestBase {
    public:
        TestCache(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestCache")
        {  
        }

        void run() {
            getSoundManager().setCacheSize(0,24*1024*1024);
            ENSURE(getSoundManager().getCacheMemUsed() == 0);
            ENSURE(getSoundManager().getMaxCacheSize() == 0);
            {
                play("../../testfiles/aussentuer.mp3");
                play("../../testfiles/stereotest441.wav");
            }
            // We'll have one cached item anyway because the check for cache size
            // limits is done when a new sound is played, not before.
            ENSURE(getSoundManager().getNumItemsInCache() == 1);
            getSoundManager().deleteCacheItem("../../testfiles/stereotest441.wav");
            ENSURE(getSoundManager().getNumItemsInCache() == 0);
            AC_PRINT << "Testing warning code. Ignore warnings about cache memory usage following this line.";
            getSoundManager().setCacheSize(0,0);
            play("../../testfiles/aussentuer.mp3");
            ENSURE(getSoundManager().getNumItemsInCache() == 0);
            getSoundManager().setCacheSize(128*1024*1024, 24*1024*1024);
            getSoundManager().preloadSound("../../testfiles/stereotest441.wav");
            ENSURE(getSoundManager().getNumItemsInCache() == 1);
            play("../../testfiles/stereotest441.wav");
        }
        
    private:
        void play(const std::string & theURI) {
            SoundPtr mySound = getSoundManager().createSound(theURI);
            mySound->play();
            while(mySound->isPlaying()) {
                msleep(100);
            }
        }
};

class TestStopByItself: public SoundTestBase {
    public:
        TestStopByItself(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestStopByItself")
        {  
        }

        void run() {
            playFile("../../testfiles/ShutterClick.wav");

            playFile("../../testfiles/aussentuer.mp3");
            
            {
                SoundPtr mySound = getSoundManager().createSound
                        ("../../testfiles/aussentuer.mp3");
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
            ENSURE(getSoundManager().getNumSounds() == 0);

        }

    private:
        void playFile(std::string theFilename) {
            {
                SoundPtr mySound = getSoundManager().createSound(theFilename);
                msleep(500);
                mySound->play();
                while(mySound->isPlaying()) {
                    msleep(100);
                }
                
                msleep(100);
                ENSURE(mySound->getNumUnderruns() == 0);
            }
            msleep(100);
            ENSURE(getSoundManager().getNumSounds() == 0);
        }

};

class TestPause: public SoundTestBase {
    public:
        TestPause(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestPause")
        {  
        }
       
        void run() {
            {
                SoundPtr mySound = getSoundManager().createSound
                        ("../../testfiles/aussentuer.mp3");
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
                SoundPtr mySound = getSoundManager().createSound
                        ("../../testfiles/aussentuer.mp3", theLoop);
                mySound->play();
                msleep(200);
                mySound->pause();
                msleep(100);
                checkTime(mySound, 0.2);
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
            ENSURE(getSoundManager().getNumSounds() == 0);
        }
};

class TestLoop: public SoundTestBase {
    public:
        TestLoop(SoundManager& mySoundManager) 
            : SoundTestBase(mySoundManager, "TestLoop")
        {  
        }
       
        void run() {
            SoundPtr mySound = getSoundManager().createSound("../../testfiles/crash.wav", true);
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
        TestVolume(SoundManager& mySoundManager, bool myTestGlobalVolume)
            : SoundTestBase(mySoundManager, "TestVolume"),
              _myTestGlobalVolume(myTestGlobalVolume)
        {  
        }
       
        void run() {
            {   // Sound volume 
                SoundPtr mySound = getSoundManager().createSound
                        ("../../testfiles/aussentuer.mp3");
                mySound->setVolume(0.2f);
                ENSURE(almostEqual(mySound->getVolume(), 0.2));
                mySound->play();
                msleep(200);
                mySound->setVolume(0.5f);
                msleep(200);
                ENSURE(almostEqual(mySound->getVolume(), 0.5));
                mySound->fadeToVolume(1, 0.2);
                msleep(400);
                ENSURE(almostEqual(mySound->getVolume(), 1));
                mySound->stop();
            }
            if (_myTestGlobalVolume) {
                // Global volume
                SoundPtr mySound = getSoundManager().createSound
                        ("../../testfiles/aussentuer.mp3");
                getSoundManager().setVolume(0.2f);
                mySound->play();
                msleep(200);
                getSoundManager().setVolume(0.5f);
                msleep(200);
                ENSURE(almostEqual(getSoundManager().getVolume(), 0.5));
                getSoundManager().fadeToVolume(1, 0.3f);
                msleep(400);
                ENSURE(almostEqual(getSoundManager().getVolume(), 1));
                mySound->stop();
            }
        }

    private:
        bool _myTestGlobalVolume;
};

class TestSeek: public SoundTestBase {
    public:
        TestSeek(SoundManager& mySoundManager)
            : SoundTestBase(mySoundManager, "TestSeek")
        {
        }

        void run() {
            SoundPtr mySound = getSoundManager().createSound
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
        StressTest(SoundManager& mySoundManager, double myDuration) 
            : SoundTestBase(mySoundManager, "StressTest"),
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
            getSoundManager().stopAll();
            SUCCESS("StressTest");
        }

    protected:
        void doIteration(int i) {
            // No caching.
            SoundPtr mySound = getSoundManager().createSound
                ("../../testfiles/stereotest441.wav", false, false);
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
        MemLeakStressTest(SoundManager& mySoundManager, double myDuration) 
            : StressTest(mySoundManager, myDuration)
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
            getSoundManager().stopAll();
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
        SoundTestSuite(const char * myName, int argc, char *argv[], bool myUseDummyPump) 
            : UnitTestSuite(myName, argc, argv),
              _myUseDummyPump (myUseDummyPump)
        {}

        void setup() {
            UnitTestSuite::setup();
            Pump::setUseRealPump(!_myUseDummyPump);
            SoundManager& mySoundManager = Singleton<SoundManager>::get();

            bool myNoisy;
            string myVal;
            if (_myUseDummyPump || get_environment_var("Y60_NOISY_SOUND_TESTS", myVal)) {
                myNoisy = true;
                mySoundManager.setVolume(1);
            } else {
                myNoisy = false;
                mySoundManager.setVolume(0);
            }
            
            addTest(new TestPlay(mySoundManager));
            addTest(new TestStop(mySoundManager));
            addTest(new TestCache(mySoundManager));

            addTest(new TestBroken(mySoundManager));
            addTest(new TestFireAndForget(mySoundManager));
            addTest(new TestTwoSounds(mySoundManager));

            addTest(new TestStopByItself(mySoundManager));

            addTest(new TestPause(mySoundManager));
            addTest(new TestStopAll(mySoundManager));
            addTest(new TestSeek(mySoundManager));
          
            addTest(new TestLoop(mySoundManager));
            addTest(new TestVolume(mySoundManager, myNoisy));

#ifndef DEBUG_VARIANT
            // In debug mode, the program runs too slowly for this test.
            addTest(new StressTest(mySoundManager, 5));
#endif
//            addTest(new TestLeak(mySoundManager));
//            addTest(new MemLeakStressTest(mySoundManager, 5*60));

        }

    private:
        bool _myUseDummyPump;
};

#endif
