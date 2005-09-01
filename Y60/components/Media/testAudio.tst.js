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

plug("y60Media");

function SoundUnitTest() {
    this.Constructor(this, "SoundUnitTest");
}

SoundUnitTest.prototype.Constructor = function(obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);


    obj.runNewAudioLibTest = function() {
        obj.myMedia = new MediaController();
        obj.myMedia.volume == 1.0;
        msleep(250);
        ENSURE("obj.myMedia.running == true");
        ENSURE("obj.myMedia.soundcount == 0");
        ENSURE("obj.myMedia.volume == 1.0");
        obj.myMedia.volume = 0.5;        
        msleep(250);
        ENSURE("obj.myMedia.volume == 0.5");
        obj.myMedia.fadeToVolume(1.0, 0.1);        
        msleep(150);
        ENSURE("obj.myMedia.volume == 1.0");


        obj.mySound = obj.myMedia.createSound("../../testfiles/aussentuer.mp3");
        //obj.mySound = new Sound("../../testfiles/aussentuer.mp3");

        ENSURE("obj.myMedia.soundcount == 1");
        ENSURE("obj.mySound.time == 0");

        print("### FIXME: Volume setting does not work!");
/*        obj.mySound.volume = 1.0;
        msleep(250);
        DPRINT("obj.mySound.volume");
        ENSURE("obj.mySound.volume == 1.0");        
*/        
        ENSURE("!obj.mySound.playing");        
        ENSURE("obj.mySound.src == '../../testfiles/aussentuer.mp3'");        
        ENSURE("obj.mySound.time == 0.0");
        ENSURE("obj.mySound.looping == false");
        ENSURE("obj.mySound.duration == 1.619499");
                
        print("### FIXME: Volume fading does not work!");
/*        obj.mySound.fadeToVolume(0.5, 0.1);
        msleep(250);
        DPRINT("obj.mySound.volume");
        ENSURE("obj.mySound.volume == 0.5");
*/      
        
        print("### TODO: Change looping state.");
/*        obj.mySound.looping = true;
        ENSURE("obj.mySound.looping == true");
*/
        
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
        ENSURE("obj.mySound.time == 1");
        DTITLE("Seek relative minus 0.5 seconds");
       
        print("### FIXME: Relative seeking does not work!");
/*        obj.mySound.seekRelative(-0.5);
        DPRINT("obj.mySound.time");
        ENSURE("obj.mySound.time == 0.5");
*/      
        DTITLE("Stopping sound...");
        obj.mySound.stop();
        
        ENSURE("!obj.mySound.playing");

        print("### FIXME: How do I ged rid of sounds ???");
/*        delete obj.mySound;
        gc();
        ENSURE("obj.myMedia.soundcount == 0");

        obj.mySound = obj.myMedia.createSound("../../testfiles/aussentuer.mp3");
        ENSURE("obj.myMedia.soundcount == 1");
        obj.myMedia.stopAll();
        ENSURE("obj.myMedia.soundcount == 0");
*/            
        print("### FIXME: Strange error messages and warnings at the end!");
    }

    obj.run = function() {
        try {
            this.runNewAudioLibTest();
        } catch (ex) {
            print("An exception occured: " + ex + "\n");
        }
    }
}

function main() {
    var myTestName = "testAudio.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new SoundUnitTest());
    mySuite.run();

    print(">> Finished test suite '" + myTestName + "', return status = " +
          mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(5);
};


