//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: TestWAVReader.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.17 $
//
//
// Description: 
//
//=============================================================================

#include "TestWAVReader.h"
#include "WAVReader.h"
#include "SoundException.h"
#include "SoundOut.h"
#include "AudioScheduler.h"
#include "NullSource.h"
#include "SoundCardManager.h"
#include "SoundCard.h"

#include <asl/string_functions.h>
#include <asl/Time.h>

#include <dom/Nodes.h>

#include <stdlib.h>

using namespace asl;
using namespace std;
using namespace AudioBase;

void 
TestWAVReader::testReader (AudioScheduler & theScheduler, SoundOut& theWriter, 
        WAVReader & theReader, bool isStereo, double expectedDuration) 
{
    NullSource * nothing = 0;
    if (isStereo) {
        theScheduler.connect (theReader.getOutput (Left), theWriter.getInput(0));
        theScheduler.connect (theReader.getOutput (Right), theWriter.getInput(1));
    } else {
        theScheduler.connect (theReader.getOutput (Mono), theWriter.getInput(0));
        nothing = new NullSource(48000);
        nothing->init();
        theScheduler.connect (nothing->getOutput(), theWriter.getInput(1));
    }
    if (isStereo) {
        ENSURE (theReader.getNumChannels() == 2);
    } else {
        ENSURE (theReader.getNumChannels() == 1);
    }
    theScheduler.start();
    asl::msleep(int (expectedDuration*1000));
    theScheduler.stop();
    DPRINT(theWriter.hasClicks());
    ENSURE (!theWriter.hasClicks());

    if (nothing) {
        // UH: shouldn't we delete nothing?
    }
}

void 
TestWAVReader::testFile (const string & myFName, bool isStereo, 
                         double expectedDuration) 
{
cerr << "4" << endl;    
    AudioScheduler & myScheduler = AudioScheduler::get();
cerr << "5" << endl;    
    SoundCard * myCard = SoundCardManager::get().getCard(0);
cerr << "6" << endl;    
    myCard->setParams(0.1, 48000, 16, 2);
cerr << "7" << endl;    
    SoundOut myWriter(myCard, 2, 48000);
cerr << "8" << endl;    

    WAVReader *myReader = new WAVReader (myFName, 48000, 1, 0, false);
cerr << "9" << endl;    
    ENSURE_MSG (fabs(myReader->getDuration()-expectedDuration) < 0.1,
                "Duration is correct.");
    if (fabs(myReader->getDuration()-expectedDuration) > 0.1) {
        cerr << "Reported duration: " << myReader->getDuration();
    }
    testReader (myScheduler, myWriter, *myReader, isStereo, expectedDuration);
}

void 
TestWAVReader::testLoop () {
    AudioScheduler & myScheduler = AudioScheduler::get();
    SoundCard * myCard = SoundCardManager::get().getCard(0);
    myCard->setParams(0.1, 48000, 16, 2);
    SoundOut myWriter(myCard, 2, 48000);

    WAVReader *myReader = new WAVReader ("../../testfiles/A440-48000-24-2.wav",
            48000, 1, 0, true);

    myScheduler.connect (myReader->getOutput (Left), myWriter.getInput(0));
    myScheduler.connect (myReader->getOutput (Right), myWriter.getInput(1));
    myScheduler.start();
    msleep(1000);
    ENSURE (!myWriter.hasClicks());

    myReader->stop();
    myScheduler.stop();
    SUCCESS("Loop worked.");
}

void 
TestWAVReader::testException (const string & myFName, const string & myTestName) {
    bool isOK = false;
    WAVReader *myReader = 0;
    try {
        myReader = new WAVReader(myFName, 44100, 1, 0, false);
    } catch (SoundException & e) {
        isOK = true;
        cerr << getTracePrefix() << " OK   " << e;
    }
    ENSURE_MSG (isOK, myTestName.c_str());
}

void 
TestWAVReader::testVolume() {
    AudioScheduler & myScheduler = AudioScheduler::get();
    SoundCard * myCard = SoundCardManager::get().getCard(0);
    myCard->setParams(0.1, 44100, 16, 2);
    SoundOut myWriter(myCard, 2, 44100);

    WAVReader * myReader = new WAVReader 
            ("../../testfiles/stereotest.wav", 44100, 1, 0, true);

    myScheduler.connect (myReader->getOutput (Left), myWriter.getInput(0));
    myScheduler.connect (myReader->getOutput (Right), myWriter.getInput(1));
    myScheduler.start();
    msleep(100);
    ENSURE (!myWriter.hasClicks());
    myReader->setVolume (0.1);
    msleep(100);
    ENSURE (!myWriter.hasClicks());
    myReader->setVolume (1.0);
    msleep(100);
    /*
    int i;
    for (i=0; i<10; i++) {
        myReader->setVolume (1.0-double(i)/10.0);
        msleep (10);
    }
    */
    ENSURE (!myWriter.hasClicks());
    myScheduler.stop();
    ENSURE_MSG (true, "Volume test ran."); 
}

void 
TestWAVReader::testStartStop() {
    AudioScheduler & myScheduler = AudioScheduler::get();
    {
        SoundCard * myCard = SoundCardManager::get().getCard(0);
        myCard->setParams(0.1, 44100, 16, 2);
        SoundOut myWriter(myCard, 2, 48000);

        WAVReader * myReader1 = new WAVReader 
                ("../../testfiles/A440-48000-16-2.wav", 48000, 1, 0, false);
        myScheduler.connect (myReader1->getOutput (Left), myWriter.getInput(0));
        myScheduler.connect (myReader1->getOutput (Right), myWriter.getInput(1));
        myScheduler.start();
        msleep(1000);
        myScheduler.stop();
    }
    {
        SoundCard * myCard = SoundCardManager::get().getCard(0);
        myCard->setParams(0.1, 44100, 16, 2);
        SoundOut myWriter(myCard, 2, 48000);

        WAVReader * myReader1 = new WAVReader 
                ("../../testfiles/pitchbend.wav", 48000, 1, 0, false);
        myScheduler.connect (myReader1->getOutput (Left), myWriter.getInput(0));
        myScheduler.connect (myReader1->getOutput (Right), myWriter.getInput(1));
        myScheduler.start();
        msleep(400);
        myReader1->stop();
        msleep (200);
        myScheduler.stop();
        
        ENSURE_MSG (true, "Stop test ran."); 
    }
    {
        SoundOut myWriter(SoundCardManager::get().getCard(0), 2, 48000);

        WAVReader * myReader2 = new WAVReader 
                ("../../testfiles/pitchbend.wav", 48000, 1, 0.5, false);
        myScheduler.connect (myReader2->getOutput (Left), myWriter.getInput(0));
        myScheduler.connect (myReader2->getOutput (Right), myWriter.getInput(1));
        myScheduler.start();
        msleep (200);
        myReader2->stop();
        msleep (200);
        myScheduler.stop();
        ENSURE_MSG (true, "Start time test ran."); 
    }
/*
TODO: The scheduler currently crashes if there are no inputs. Fix that.    
    {
        SoundOut myWriter(SoundCardManager::get().getCard(0), 2, 48000);

        WAVReader * myReader3 = new WAVReader 
                ("../../testfiles/pitchbend.wav", 48000, 1, 5, false);
        myScheduler.connect (myReader3->getOutput (Left), myWriter.getInput(0));
        myScheduler.connect (myReader3->getOutput (Right), myWriter.getInput(1));
        myScheduler.start();
        msleep(100);
        myScheduler.stop();
        ENSURE_MSG (true, "Start time > eof test ran."); 
    }
*/    
}

void 
TestWAVReader::run() {
    try {
cerr << "1" << endl;        
        SoundCard * myCard = SoundCardManager::get().getCard(0);
cerr << "2" << endl;        
        myCard->setParams(0.1, 48000, 16, 2);
cerr << "3" << endl;        
        
        //testFile ("../../testfiles/A440-48000-16-2.wav", true, 0.5);
        testFile ("../../testfiles/A440-48000-16-1.wav", false, 0.5);
cerr << "4" << endl;        
        testFile ("../../testfiles/A440-48000-16-1a.wav", false, 0.5);        
        testFile ("../../testfiles/stereotest480.wav", true, 2.1);
        testFile ("../../testfiles/A440-48000-24-1.wav", false, 0.5);
        testFile ("../../testfiles/A440-48000-24-2.wav", true, 0.5);

        testException ("filenotfound.wav", "File not found exception test");
        testException ("../../testfiles/A440-48000-8-1.wav", 
                       "Unsupported file format exception test");
        testException ("TestWAVReader.o", "Illegal file format exception test");

        testVolume ();
        testLoop ();
        testStartStop ();
    } catch (SoundException & e) {
        ENSURE_MSG (false, asl::as_string (e).c_str());
    } 
}


