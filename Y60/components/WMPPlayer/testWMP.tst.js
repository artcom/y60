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
//    $RCSfile: testTask.tst.js,v $
//
//   $Revision: 1.7 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
if (__main__ == undefined) var __main__ = "WMPlayer";

use("SceneViewer.js");
plug("y60WMPPlayer");

function WMPlayerTest(theArguments) {
    this.Constructor(this, theArguments);
}

WMPlayerTest.prototype.Constructor = function(self, theArguments) {


    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];
    var _myWMPPlayer = new WMPPlayer();

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        window = new RenderWindow();
        window.decorations = false;
        window.position = new Vector2i(10, 10);
        Base.setup(theWidth, theHeight, false, theTitle);

        _myWMPPlayer.setup(); //parent window must be constructed
        print("Loading: " + theArguments[1]);
        _myWMPPlayer.load(theArguments[1]);
        _myWMPPlayer.play();
        _myWMPPlayer.canvasposition = [100, 100];
        _myWMPPlayer.canvassize = [100, 100];
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theKey == 'l' && theState) {
            _myWMPPlayer.load("http://bacon/media/movies/emil.wmv");
        } else if (theKey == 's' && theState) {
            print("width: " + _myWMPPlayer.width + " height: " + _myWMPPlayer.height);
        } else if (theKey == 'p' && theState) {
            print("play/pause now " + _myWMPPlayer.state);
            if (_myWMPPlayer.state == "playing") {
                _myWMPPlayer.pause();
            } else {
                _myWMPPlayer.play();
            }
        } else if (theKey == 'v' && theState) {
            print("volume " + _myWMPPlayer.volume);
            _myWMPPlayer.volume = theShiftFlag ? _myWMPPlayer.volume - 0.1 : _myWMPPlayer.volume + 0.1;
        }

        Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
    }
}

//
// main
//

if (__main__ == "WMPlayer") {
    print("WMPlayer");
    try {
        var ourWMPlayerTest = new WMPlayerTest(arguments);

        ourWMPlayerTest.setup(800, 600, "WMPlayer");
        ourWMPlayerTest.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}
