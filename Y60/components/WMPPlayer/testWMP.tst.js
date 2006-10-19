/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testTask.tst.js,v $
//
//   $Revision: 1.7 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

plug("y60WMPPlayer");

var window = new RenderWindow();
//window.decorations = false;
window.position = new Vector2i(10, 10);

var ourWMPPlayer = new WMPPlayer();
var myFilename = expandEnvironment("${PRO}") + "/testmodels/movies/testmovie.wmv";

window.onStartMainLoop = function() {
    ourWMPPlayer.setup(window.title);
    if (myFilename) {
        print("playing " + myFilename);
        ourWMPPlayer.load(myFilename);
        ourWMPPlayer.play();
    }
}

window.onFrame = function() {
    if (ourWMPPlayer.state == "stopped") {
        exit(0);
    }
}

/*
window.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
    if (theState) {
        switch (theKey) {
            case "l":

                print("Playing: http://bacon/media/movies/emil.wmv");
                ourWMPPlayer.load("http://bacon/media/movies/emil.wmv");
                ourWMPPlayer.play();
                break;
            case "s":
                if (theShiftFlag) {
                    print("width: " + ourWMPPlayer.width + " height: " + ourWMPPlayer.height);
                } else {
                    ourWMPPlayer.stop();
                }
                break;
            case "p":
                if (ourWMPPlayer.state == "playing") {
                    print("Pause movie");
                    ourWMPPlayer.pause();
                } else {
                    print("Play movie");
                    ourWMPPlayer.play();
                }
                break
            case "v":
                ourWMPPlayer.volume = theShiftFlag ? ourWMPPlayer.volume + 0.1 : ourWMPPlayer.volume - 0.1;
                print("volume " + ourWMPPlayer.volume.toFixed(1));
                break;
            case "h":
                print("WMPlayer help:");
                print("  p   - play/pause");
                print("  s   - stop");
                print("  V   - increase volume");
                print("  v   - decrease volume");
                print("  S   - print player size");
        }
    }
}
*/

window.go();
