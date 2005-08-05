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
//    $RCSfile: testSound.tst.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.15 $
//
//
// Description:
//
//
//=============================================================================

#include <asl/settings.h>

#include "TestSineGenerator.h"
#include "TestWAVReader.h"
#include "TestSoundOut.h"
#include "TestMixer.h"
#include "TestWAVWriter.h"
#include "TestBandRejectFilter.h"
#include "Lowpass.h"
#include "AudioEater.h"
#include "Difference.h"
#include "TFilter.h"
#include "SineGenerator.h"
#include "AudioScheduler.h"
#include "AudioBuffer.h"
#include "IAudioBufferListener.h"

#include <asl/UnitTest.h>
#include <asl/Time.h>

#ifdef LINUX
#include <unistd.h>
#endif
#include <cmath>

using namespace std;
using namespace asl;

using namespace AudioBase;

class TestLowpass: public UnitTest {
public:
    explicit TestLowpass(): UnitTest ("TestLowpass") {}
    void runWithFilter (Lowpass * myLowpass, double expectedVolume) {
        AudioScheduler & myScheduler = AudioScheduler::get();
        AudioEater * myEater = new AudioEater(1, 44100);
        SineGenerator * myGenerator = new SineGenerator(44100);
        myGenerator->init();

        myScheduler.connect (myGenerator->_myOutput, myLowpass->getInput(0));
        myScheduler.connect (myLowpass->getOutput(0), myEater->getInput(0));
        myScheduler.start();
        msleep(200);
        myScheduler.stop();
        DPRINT2("Lowpass test - volume expected: ", expectedVolume);
        DPRINT2("Lowpass test - volume got: ", myEater->getMaxVolume());
        ENSURE(myEater->getMaxVolume() > expectedVolume-0.1);
        ENSURE(myEater->getMaxVolume() < expectedVolume+0.1);

        delete myEater;
    }

    virtual void run() {
        AudioBuffer::setSize(3*1536);
        Lowpass * myLowpass = new Lowpass(1500, 44100, 1);
        runWithFilter (myLowpass, 0.9);
        delete myLowpass;

        myLowpass = new Lowpass(100, 44100, 1);
        runWithFilter (myLowpass, 0.0);
        delete myLowpass;
    }
};

class TestDifference: public UnitTest, public IAudioBufferListener {
public:
    TestDifference(): UnitTest ("TestDifference") {}

    virtual void run() {
        AudioScheduler & myScheduler = AudioScheduler::get();
        _isKaputt = false;
        AudioBuffer::setSize(3*1536);

        SineGenerator * myGenerator = new SineGenerator(44100);
        myGenerator->init();
        TFilter * myT = new TFilter(2, 44100);
        Difference * myDiff = new Difference(44100);
        AudioEater * myUplink = new AudioEater(1, 44100);
        myUplink->registerListener (this);

        myScheduler.connect (myGenerator->_myOutput, myT->getInput(0));
        myScheduler.connect (myT->getOutput(0), myDiff->getInput(0));
        myScheduler.connect (myT->getOutput(1), myDiff->getInput(1));
        myScheduler.connect (myDiff->getOutput(0), myUplink->getInput(0));

        myScheduler.start();
        msleep(200);
        myScheduler.stop();
        ENSURE(!_isKaputt);

        delete myUplink;
        delete myT;
        delete myDiff;
    }

    virtual void onNewData(AudioModule* theModule,
            int theOutputIndex,
            AudioBuffer * theBuffer)
    {
        if(theBuffer->getMax() > 0.000001) {
            _isKaputt = true;
        }
    }

private:
    bool _isKaputt;
};


class TestUplink: public UnitTest, public IAudioBufferListener {
public:
    TestUplink()
        : UnitTest ("TestUplink"),
          _myOutputsFound(2, false)
    {}

    virtual void run() {
        AudioScheduler & myScheduler = AudioScheduler::get();
        AudioBuffer::setSize(2048);
        AudioEater * myUplink = new AudioEater(2, 44100); // two channels
        myUplink->registerListener (this);
        SineGenerator * myGenerator0 = new SineGenerator(44100);
        myGenerator0->init();
        SineGenerator * myGenerator1 = new SineGenerator(44100);
        myGenerator1->init();

        myScheduler.connect (myGenerator0->_myOutput, myUplink->getInput(0));
        myScheduler.connect (myGenerator1->_myOutput, myUplink->getInput(1));
        myScheduler.start();
        msleep(200);
        myScheduler.stop();
        ENSURE(_myOutputsFound[0] && _myOutputsFound[1]);

        delete myUplink;
    }

    virtual void onNewData(AudioModule* theModule,
            int theOutputIndex,
            AudioBuffer * theBuffer)
    {
        _myOutputsFound[theOutputIndex] = true;
    }
private:
    vector<bool> _myOutputsFound;
};

int main( int argc, char *argv[] ) {
    UnitTestSuite mySuite ("Sound tests");
    AudioBuffer::setSize(3*1536);
    mySuite.addTest(new TestSoundOut);
    mySuite.addTest(new TestWAVReader);
    mySuite.addTest(new TestSineGenerator);
    mySuite.addTest(new TestUplink);
    mySuite.addTest(new TestLowpass);
    mySuite.addTest(new TestBandRejectFilter);
    mySuite.addTest(new TestDifference);
    mySuite.addTest(new TestWAVWriter);
    mySuite.addTest(new TestMixer);

    mySuite.run();

    cerr << ">> Finished test '" << argv[0] << "'"
          << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();
}

