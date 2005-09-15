/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");
plug("y60JSSound");
plug("y60WMADecoder2");

function WMADecoder2UnitTest() {
    this.Constructor(this, "WMADecoder2UnitTest");
}

WMADecoder2UnitTest.prototype.Constructor = function(obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    obj.runWMADecoder2Test = function() {
        obj.myMedia = new SoundManager();
        
//        obj.mySound = obj.myMedia.createSound("../../testWMADecoder2.tst.js");
        obj.mySound = obj.myMedia.createSound("../../testfiles/music_cut_wm9.wma");

        DTITLE("Playing sound...");
        obj.mySound.play();
        ENSURE("obj.mySound.playing");
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
        
        DTITLE("Seek relative minus 0.5 seconds");
        obj.mySound.seekRelative(-0.5);
        ENSURE("Math.abs(obj.mySound.time - 0.5) < 0.1");
      
        DTITLE("Stopping sound...");
        obj.mySound.stop();
        
        ENSURE("!obj.mySound.playing");

        delete obj.mySound;
        gc();
        msleep(100);
        ENSURE("obj.myMedia.soundcount == 0");
    }

    obj.run = function() {
        this.runWMADecoder2Test();
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
};


