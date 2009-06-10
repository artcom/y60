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


use("UnitTest.js");

plug("Sound");
plug("WMADecoder");

plug("y60ProcFunctions");

function WMADecoder2UnitTest() {
    this.Constructor(this, "WMADecoder2UnitTest");
}

const SOUNDFILE = searchFile("../../../src/y60/components/WMADecoder/testfiles/music_cut_wm9.wma");

WMADecoder2UnitTest.prototype.Constructor = function(obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    obj.playSound = function(myFileName) {
        obj.mySound = new Sound(myFileName);
        obj.mySound.play();
        msleep(2000);
        obj.mySound.stop();
    }

    obj.runWMADecoder2Test = function() {
        obj.mySoundManager.preloadSound(SOUNDFILE);
        obj.mySound = new Sound(SOUNDFILE);

        DTITLE("Playing sound...");
        obj.mySound.play();
        ENSURE("obj.mySound.playing");
        msleep(1000);
        obj.mySound.seek(11);
        ENSURE("obj.mySound.playing");
        msleep(2000);
        // Play to end.
        ENSURE("!obj.mySound.playing");

        obj.mySound.play();
        msleep(1000);
        obj.mySound.stop();
        msleep(1000);
        obj.mySound.play();
        msleep(1000);
        obj.mySound.pause();

        msleep(1000);
        DTITLE("Pausing sound...");
        obj.mySound.pause();
        ENSURE("!obj.mySound.playing");
        msleep(200);
        DTITLE("Playing sound...");
        obj.mySound.play();
        ENSURE("obj.mySound.playing");
        msleep(1000);
        ENSURE("obj.mySound.time > 0");
        DTITLE("Seek to second 1");
        obj.mySound.seek(1);
        ENSURE("Math.abs(obj.mySound.time - 1) < 0.1");
        msleep(1000);

        DTITLE("Seek relative minus 0.5 seconds");
        obj.mySound.seekRelative(-0.5);
        ENSURE("Math.abs(obj.mySound.time - 1.5) < 0.2");
        DPRINT(obj.mySound.time);
        msleep(1000);

        DTITLE("Stopping sound...");
        obj.mySound.stop();

        obj.playSound("http://himmel/testfiles/Leben.wma");
        obj.playSound("http://himmel/testfiles/track2.mp3");
        obj.playSound("http://himmel/testfiles/helsing.wma");

        ENSURE("!obj.mySound.playing");

        delete obj.mySound;
        gc();
        msleep(200);
        ENSURE("obj.mySoundManager.soundcount == 0");

        ENSURE_EXCEPTION("new Sound(\"../../testWMADecoder2.tst.js\")",
                "*");

        // Stress test - runs for hours :-)
        // Starts 5 sounds per second.
/*
        for (var i=0; i<5*60*60*8; ++i) {
            var mySound = new Sound("../../testfiles/music_cut_wm9.wma");
            mySound.play();
            msleep(200);
            gc();
        }
*/
    }

    obj.runLeakTest = function() {

        print("### INITIAL USAGE:" + getProcessMemoryUsage());

        obj.mySoundManager.volume == 1.0;

        const mySoundFile = "../../testfiles/music_cut_wm9.wma"
        var mySound = null;

        for (var i = 0; i < 20; ++i) {
            mySound = new Sound(mySoundFile, false, false);
            mySound.play();
            while (mySound.playing) {
                msleep(100);
            }
            mySound = null;

            var myUsage = getProcessMemoryUsage();
            if (i == 0) {
                obj.myStartUsage = myUsage;
                print("### START USAGE:" + obj.myStartUsage);
            }
            obj.myDiff = myUsage - obj.myStartUsage;
            print("### TEST " + i + " USAGE:" + myUsage + " DIFF:" + obj.myDiff + " SOUNDCOUNT:" + obj.mySoundManager.soundcount);
        }
        mySound = null;

        var myFiniUsage = getProcessMemoryUsage();
        obj.myDiff = myFiniUsage - obj.myStartUsage;
        print("### FINI USAGE:" + myFiniUsage + " DIFF:" + obj.myDiff + " SOUNDCOUNT:" + obj.mySoundManager.soundcount);

        msleep(1000);
        gc();
        msleep(1000);
        ENSURE("obj.mySoundManager.soundcount == 0");

        var myGCUsage = getProcessMemoryUsage();
        obj.myDiff = myGCUsage - obj.myStartUsage;
        print("### GC USAGE: " + myGCUsage + " DIFF:" + obj.myDiff);
        ENSURE("obj.myDiff < (obj.myStartUsage * 0.2)");
    }

    obj.run = function() {
        obj.mySoundManager = new SoundManager();
        this.runWMADecoder2Test();
//        this.runLeakTest();
    }
}

function main() {
    var myTestName = "testAudio.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new WMADecoder2UnitTest());
    mySuite.run();

    print(">> Finished test suite '" + myTestName + "', return status = " +
          mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

rc = main();
if (rc != 0) {
    exit(5);
} else {
    exit(0);
}


