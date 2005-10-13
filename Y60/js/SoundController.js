//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: AudioController.js,v $
//   $Author: christian $
//   $Revision: 1.30 $
//   $Date: 2005/04/08 18:41:05 $
//
//
//=============================================================================

const DUMMY_AUDIO_CONTROLLER = false;

var ourSoundController = null;
//initSoundController();


if (DUMMY_AUDIO_CONTROLLER) {
    ourSoundController = [];
    ourSoundController.isRunning = function() {}
    ourSoundController.play = function() {}
    ourSoundController.pause = function() {}
    ourSoundController.setSeekOffset = function() {};
    ourSoundController.stop = function() {};
    ourSoundController.stopAllSounds = function() {};
    ourSoundController.setVolume = function() {};
    ourSoundController.getVolume = function() {};
    ourSoundController.getDuration = function() {};

}

function initSoundController() {
    try {
        plug("y60JSSound");
        ourSoundController = new SoundManager();
    } catch (ex) {
        print("### Warning: Could not construct AudioController. Sound will not work.");
        print("### Error was " + ex);
    }
}

function setMasterVolume(theVolume) {
    if (theVolume == undefined) {
        theVolume = 1.0;
    }
    if (!ourSoundController) {
        return;
    }
    try {
        print("Audio Volume set to:"+theVolume);
        ourSoundController.volume = theVolume;
        return ourSoundController.volume;
    } catch (ex) {
        print ("### Error setting volume to  " + theVolume);
    }
}

function isPlaying(theSound) {
    if (theSound==undefined || !ourSoundController) {
        return false;
    }
    try {
        return theSound.playing;
    } catch (ex) {
        print ("### Error retrieving sound state  " + theSoundId);
    }
}

function stopSound(theSound) {
    if (theSound==undefined || !ourSoundController) {
        return;
    }
    if (!theSound.playing) {
        return;
    }
    try {
        theSound.stop();
    } catch (ex) {
        print ("### Error stopping sound:  " + theSoundId);
    }
}

function pauseSound(theSound) {
    if (theSound==undefined || !ourSoundController) {
        return;
    }
    if (!theSound.playing) {
        return;
    }
    try {
        theSound.pause();
    } catch (ex) {
        print ("### Error pausing sound:  " + theSoundId);
    }
}

function setSeekOffset(theSound, theSeekOffset) {
    if (theSound==undefined || !ourSoundController) {
        return;
    }
    if (!theSound.playing) {
        return;
    }
    try {
        theSound.seekRelative(theSeekOffset);
    } catch (ex) {
        print ("### Error setting seek offset:  " + theSoundId);
    }
}

function playSound(theFilename, theVolume, theLoopFlag) {
    if (!ourSoundController) {
        initSoundController();
    }

    if (theVolume == undefined) {
        theVolume = 1.0;
    }

    if (theLoopFlag == undefined) {
        theLoopFlag = false;
    }

    try {
        var mySound = ourSoundController.createSound(theFilename, theLoopFlag);
        mySound.volume = theVolume;
        mySound.play();
        return mySound;
    } catch (ex) {
        print ("### Error playing " + theFilename);
    }

    return null;
}
