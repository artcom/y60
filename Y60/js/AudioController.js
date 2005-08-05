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

var ourAudioController = null;
//initAudioController();

if (DUMMY_AUDIO_CONTROLLER) {
    ourAudioController = [];
    ourAudioController.isRunning = function() {}
    ourAudioController.play = function() {}
    ourAudioController.pause = function() {}
    ourAudioController.setSeekOffset = function() {};
    ourAudioController.stop = function() {};
    ourAudioController.stopAllSounds = function() {};
    ourAudioController.setVolume = function() {};
    ourAudioController.getVolume = function() {};
    ourAudioController.getDuration = function() {};

}

function initAudioController() {
    try {
        plug("jsaudio");        
        ourAudioController = new AudioController();
        if (ourAudioController.isRunning() == false) {
            ourAudioController.init(44100, 0.1);
        } else {
            print("### WARNING: AudioController already running");
        }
    } catch (ex) {
        print("### Warning: Could not construct AudioController. Sound will not work.");
        print("### Error was " + ex);
    }
}

function setMasterVolume(theVolume) {
    if (theVolume == undefined) {
        theVolume = 1.0;
    }
    if (!ourAudioController) {
        return;
    }
    try {
        print("Audio Volume set to:"+theVolume);
        return ourAudioController.setVolume("Mixer", theVolume);
    } catch (ex) {
        print ("### Error setting volume to  " + theVolume);
    }
}

function isPlaying(theSoundId) {
    if (theSoundId==undefined || !ourAudioController) {
        return;
    }
    try {
        return ourAudioController.isPlaying(theSoundId);
    } catch (ex) {
        print ("### Error retrieving sound state  " + theSoundId);
    }
}

function stopSound(theSoundId) {
    if (theSoundId==undefined || !ourAudioController) {
        return;
    }
    if (!isPlaying(theSoundId)) {
        return;
    }
    try {
        return ourAudioController.stop(theSoundId);
    } catch (ex) {
        print ("### Error stopping sound:  " + theSoundId);
    }
}

function pauseSound(theSoundId) {
    if (theSoundId==undefined || !ourAudioController) {
        return;
    }
    if (!isPlaying(theSoundId)) {
        return;
    }
    try {
        return ourAudioController.pause(theSoundId);
    } catch (ex) {
        print ("### Error pausing sound:  " + theSoundId);
    }
}

function setSeekOffset(theSoundId, theSeekOffset) {
    if (theSoundId==undefined || !ourAudioController) {
        return;
    }
    if (!isPlaying(theSoundId)) {
        return;
    }
    try {
        return ourAudioController.setSeekOffset(theSoundId, theSeekOffset);
    } catch (ex) {
        print ("### Error setting seek offset:  " + theSoundId);
    }
}

function playSound(theFilename, theVolume, theLoopFlag) {
    if (!ourAudioController) {
        initAudioController();
    }

    if (theVolume == undefined) {
        theVolume = 1.0;
    }

    if (theLoopFlag == undefined) {
        theLoopFlag = false;
    }

    try {
        return ourAudioController.play(theFilename, "Mixer", theVolume, 0, theLoopFlag);
    } catch (ex) {
        print ("### Error playing " + theFilename);
    }

    return null;
}

// Only one exclusive sound per channel will play at a time
var ourExclusiveSoundIds = [];
function playExclusiveSound(theFilename, theVolume, theLoopFlag, theChannel) {
    if (!ourAudioController) {
        initAudioController();
    }

    if (theChannel in ourExclusiveSoundIds &&
        ourExclusiveSoundIds[theChannel])
    {
        if (ourAudioController.isPlaying(ourExclusiveSoundIds[theChannel])) {
            try {
                ourAudioController.stop(ourExclusiveSoundIds[theChannel]);
            } catch (ex) {
            }
        }
        ourExclusiveSoundIds[theChannel] = null;
    }

    ourExclusiveSoundIds[theChannel] = playSound(theFilename, theVolume, theLoopFlag);
    return ourExclusiveSoundIds[theChannel];
}

function stopExclusiveSound(theChannel) {
    if (ourAudioController) {
        if (theChannel == undefined) {
            for (var myChannel in ourExclusiveSoundIds) {
                stopExclusiveSound(myChannel);
            }
        } else {
            if (ourExclusiveSoundIds[theChannel]) {
                if (ourAudioController.isPlaying(ourExclusiveSoundIds[theChannel])) {
                    try {
                        ourAudioController.stop(ourExclusiveSoundIds[theChannel]);
                    } catch (ex) {
                    }
                }
                ourExclusiveSoundIds[theChannel] = null;
            }
        }
    }
}
