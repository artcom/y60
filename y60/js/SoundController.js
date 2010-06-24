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

const DUMMY_AUDIO_CONTROLLER = false;

var ourSoundController = null;

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
        plug("Sound");
        ourSoundController = new SoundManager();
    } catch (ex) {
        print("### Warning: Could not construct SoundController. Sound will not work.");
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

function playSound(theFilename, theVolume, theLoopFlag, theSeekOffset) {
    if (!ourSoundController) {
        initSoundController();
    }

    if (theVolume == undefined) {
        theVolume = 1.0;
    }

    if (theLoopFlag == undefined) {
        theLoopFlag = false;
    }

    if (theSeekOffset == undefined) {
        theSeekOffset = 0.0;
    }

    try {
        var myFileName = searchFile(theFilename);
        var mySound = new Sound(myFilename, theLoopFlag);
        mySound.volume = theVolume;
        mySound.seek(theSeekOffset * mySound.duration);
        mySound.play();
        return mySound;
    } catch (ex) {
        print ("### Error playing " + theFilename);
    }

    return null;
}

function preloadSound(theFilename) {
    if (!ourSoundController) {
        initSoundController();
    }

    var myFileName = searchFile(theFilename);
    ourSoundController.preloadSound(myFilename);
}
