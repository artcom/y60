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
//
//   $RCSfile: testMovies.js,v $
//   $Author: danielk $
//   $Revision: 1.11 $
//   $Date: 2005/03/17 20:31:50 $
//
//=============================================================================

use("SceneViewer.js");
plug("y60JSSound");

function FFMpegTest(theArguments) {
    var Public = this;
    var Base   = {};

    SceneViewer.prototype.Constructor(Public, theArguments);

    var _myMovie = null;
    var _myTestName = "";
    var _myCurTestIndex = -1;

    Base.setup = Public.setup;
    Public.setup = function() {
        Public.setSplashScreen(false);
        Base.setup(840, 500);
        window.eventListener = Public;
        var myNoisyString = expandEnvironment("${Y60_NOISY_SOUND_TESTS}");
        if (myNoisyString == "") {
            var mySoundManager = new SoundManager();
            mySoundManager.volume = 0.0;
        }
        Public.nextTest();
    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }

    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
        var myPos = 42;
        window.setTextColor([1,1,1,1]);
        window.renderText([10, myPos], _myTestName, "Screen15");
        window.setTextColor([1,1,1,1]);
        if (_myMovie) {
            myPos += 20;
            var myText  = getFilenamePart(_myMovie.src) + " " + _myMovie.playmode + " " +
                    _myMovie.currentframe + "/" + _myMovie.framecount;
            window.renderText([10, myPos], myText, "Screen15");
            myPos += 10;
        }
    }

    function setupTest(theName, theFile, theUseSound) {
        _myTestName = theName;
        if (_myMovie) {
            _myMovie.removeFromScene();
            delete _myMovie;
            _myMovie = 0;
        }
        print ("Starting test: "+theName);
        var myMovie = new MovieOverlay(window.scene, theFile,
                new Vector2f(300, 70), null, theUseSound, null, "y60FFMpegDecoder");
        myMovie.playspeed = 1;
        myMovie.loopcount = 1;
        myMovie.avdelay   = 0;

        if (myMovie.width > 480) {
            var myAspect = myMovie.width / myMovie.height;
            myMovie.width  = 480;
            myMovie.height = myMovie.width / myAspect;
        }
        _myMovie = myMovie;
    }

    var _myTests = [
        "setupPlayTest(true)",
        "setupStopTest(true)",
        "setupPlayTest(false)",
        "setupStopTest(false)",
        "setupLoopTest(false)",
        "setupPauseTest(true)",
        "setupPauseTest(false)",
        "setupSeekBackTest(false)",
        "setupSeekFwdTest(false)",
        "setupPauseStopTest(false)",
        "setupPauseStopTest(true)",
        "setupStopPauseTest(false)",
        "setupStopPauseTest(true)"

            //                "setupLongTest(true)"
    ];

    Public.nextTest = function() {
        if (_myTestName) {
            print ("Test finished: "+_myTestName);
        }
        _myCurTestIndex++;
        if (_myCurTestIndex < _myTests.length) {
            var myTestFunc = _myTests[_myCurTestIndex];
            print(myTestFunc);
            eval(myTestFunc);
        } else {
            if (_myMovie) {
                _myMovie.removeFromScene();
                delete _myMovie;
                _myMovie = 0;
            }
            exit(0);
        }
    }

    Public.testPlaying = function() {
        assure_msg(_myMovie.playmode == "play", "Movie is still playing.");
    }

    Public.testStopped = function() {
        assure_msg(_myMovie.playmode == "stop", "Movie has stopped.");
    }

    Public.play = function() {
        print("Starting playback.");
        _myMovie.playmode = "play";
    }

    Public.stop = function() {
        print("Stopping playback.");
        _myMovie.playmode = "stop";
    }

    Public.pause = function() {
        print("Pausing playback.");
        _myMovie.playmode = "pause";
    }

    var _mySeekDest;

    Public.seek = function() {
        print ("Seek to second " + _mySeekDest);
        _myMovie.playmode = "pause";
        _myMovie.currentframe = _mySeekDest*25;
        window.scene.loadMovieFrame(_myMovie.movie);
        _myMovie.playmode = "play";
    }

    function setupPlayTest(theUseSound) {
        setupTest("Play to End", "testfiles/counter_short.mpg", theUseSound);
        window.setTimeout("testPlaying", 1000);
        window.setTimeout("testStopped", 10000);
        window.setTimeout("nextTest", 10100);
    }

    function setupStopTest(theUseSound) {
        setupTest("Play, Stop, Play again", "testfiles/counter_short.mpg", theUseSound);
        window.setTimeout("stop", 1000);
        window.setTimeout("play", 2000);
        window.setTimeout("stop", 3000);
        window.setTimeout("nextTest", 4000);
    }

    function setupPauseTest(theUseSound) {
        setupTest("Play, Pause, Play again", "testfiles/counter_short.mpg", theUseSound);
        window.setTimeout("pause", 1000);
        window.setTimeout("play", 2000);
        window.setTimeout("stop", 3000);
        window.setTimeout("nextTest", 4000);
    }

    function setupLoopTest(theUseSound) {
        setupTest("Loop", "testfiles/counter_short.mpg", theUseSound);
        _myMovie.loopcount = 0;
        window.setTimeout("testPlaying", 1000);
        window.setTimeout("testPlaying", 10000);
        window.setTimeout("stop", 10050);
        window.setTimeout("nextTest", 10100);
    }

    function setupSeekBackTest(theUseSound) {
        setupTest("SeekBack", "testfiles/counter_short.mpg", theUseSound);
        _myMovie.loopcount = 1;
        _mySeekDest = 1;
        window.setTimeout("seek", 2000);
        window.setTimeout("seek", 6000);
        window.setTimeout("nextTest", 8000);
    }

    function setupSeekFwdTest(theUseSound) {
        setupTest("SeekFwd", "testfiles/counter_short.mpg", theUseSound);
        _myMovie.loopcount = 1;
        _mySeekDest = 3;
        window.setTimeout("seek", 1000);
        window.setTimeout("nextTest", 2000);
    }

    function setupPauseStopTest(theUseSound) {
        setupTest("PauseStop", "testfiles/counter_short.mpg", theUseSound);
        _myMovie.loopcount = 1;
        window.setTimeout("pause", 1000);
        window.setTimeout("stop", 1200);
        window.setTimeout("play", 1400);
        window.setTimeout("stop", 1600);
        window.setTimeout("nextTest", 2000);
    }

    function setupStopPauseTest(theUseSound) {
        setupTest("StopPause", "testfiles/counter_short.mpg", theUseSound);
        _myMovie.loopcount = 1;
        window.setTimeout("stop", 1000);
        window.setTimeout("pause", 1200);
        window.setTimeout("play", 1400);
        window.setTimeout("stop", 1600);
        window.setTimeout("nextTest", 2000);
    }

    function setupLongTest(theUseSound) {
        setupTest("Almost Endless test", "/tmp/FantFour.mpg");
        _myMovie.loopcount = 0;
    }

    function assure_msg(theCondition, theMsg) {
        if (!theCondition) {
            print("FAILED : "+theMsg);
            exit(5);
        } else {
            print("SUCCESS: "+theMsg);
        }
    }
}

var ourShow = new FFMpegTest(arguments);
ourShow.setup();
ourShow.go();


