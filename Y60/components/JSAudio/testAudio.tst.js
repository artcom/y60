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
//
//    $RCSfile: testAudio.tst.js,v $
//
//   $Revision: 1.18 $
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

use("UnitTest.js");

plug("jsaudio");
var ourAudioController = null;

var myAudioConfigString =
'<AudioConfig latency="0.1" sampleRate="48000" priorityClass="SCHED_OTHER" priority="MAX" maxSounds="32">' +
'        <SoundOut id="SoundOut" numChannels="2" ' +
'                    timeSource="true" volume="1.0"/>' +
'        <StereoMixer id="Mixer">' +
'            <Output channel="0" dest="SoundOut.0"/>' +
'            <Output channel="1" dest="SoundOut.1"/>' +
'        </StereoMixer>' +
'</AudioConfig>';

function AudioControllerUnitTest() {
    this.Constructor(this, "AudioControllerUnitTest");
}

AudioControllerUnitTest.prototype.Constructor = function(obj, theName) {
    UnitTest.prototype.Constructor(obj, theName);

    obj.runSimple = function() {
        ENSURE(true);

        // Make sure no exceptions is thrown
        print("Testing init() ...");
        ourAudioController.init(48000, 0.1);
// TODO: This throws as it should, but it leaves the sound card open.
//        print("Testing init() with 41000 Hz  ...");
//        ENSURE_EXCEPTION('ourAudioController.init(41000, 0.1)');
        //print("Testing initFromNode() ...");
        //ourAudioController.initFromFile("../../testfiles/audioconfig.xml");

        //obj.exceptionThrown = false;
        //ENSURE_EXCEPTION('ourAudioController.initFromFile("nix.da")');

        print("Testing play() ...");
        obj.mySoundId = ourAudioController.play("../../testfiles/test.wav", "Mixer", 1, 0, false);
        ENSURE('mySoundId != ""');

        print("Sound duration: " + ourAudioController.getDuration(obj.mySoundId));
        //ENSURE('Math.abs(ourAudioController.getDuration(mySoundId) - 7.59979) < 0.001');
        msleep(1000);

        //ENSURE_EXCEPTION('ourAudioController.play("no_such_file", "Mixer", 1.0, 0, false)');;
        /*
        try {
            ourAudioController.play("no_such_file", "Mixer", 1.0, 0, false)
        } catch (ex) {
            print("Excepiton! " + ex);
            print(ex);
        }
        */

        print("Testing setVolume() ...");
        ourAudioController.setVolume(obj.mySoundId, 0.5);
		msleep(500);
        ENSURE('ourAudioController.getVolume(mySoundId) == 0.5');

/*
        print("Testing setVolume() for mixer...");
        ourAudioController.setVolume("Mixer", 0.1);
        msleep(1000);
        ENSURE('ourAudioController.getVolume("Mixer") == 0.1');

        ENSURE_EXCEPTION('ourAudioController.setVolume("0", 0.5);');
        ENSURE_EXCEPTION('ourAudioController.getVolume("0")');;
        ENSURE_EXCEPTION('ourAudioController.setVolume(undefined, 0.5)');
*/
        print("Testing isPlaying() ...");
        ENSURE('ourAudioController.isPlaying(mySoundId)');
        msleep(8000);
        ENSURE('!ourAudioController.isPlaying(mySoundId)');
        print("Testing stop() ...");
        obj.mySoundId = ourAudioController.play("../../testfiles/test.wav", "Mixer", 0.3, 0, false);
        msleep(1000);
        ourAudioController.stop(obj.mySoundId);
//        ENSURE_EXCEPTION('ourAudioController.getVolume(mySoundId) == 0.5');

		print("Testing isPlaying() ...");
		ENSURE('!ourAudioController.isPlaying("0")');
		ENSURE('!ourAudioController.isPlaying(mySoundId)');
    }

    obj.run = function() {
        try {
            ourAudioController = new AudioController();
            this.runSimple();
        } catch (ex) {
            print("An exception occured: " + ex + "\n");
        }
    }
}

function main() {
    var myTestName = "testAudioController.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new AudioControllerUnitTest());
    mySuite.run();

    print(">> Finished test suite '" + myTestName + "', return status = " +
          mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(5);
};


