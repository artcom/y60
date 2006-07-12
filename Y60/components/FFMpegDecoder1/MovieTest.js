//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
        var myMovie = new MovieOverlay(Public.getOverlayManager(), theFile,
                new Vector2f(300, 70), null, theUseSound, null, "y60FFMpegDecoder1");
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
    
    /*  
        needed movie:
        - only iframes
        
        needed tests:
        - intensive seek
        
    */

    var _myTests = [
        "setupIntensiveSeekTest()",
        "setupPlayTest()",
        "setupStopTest()",
        "setupLoopTest()",
        "setupPauseTest()",
        "setupSeekBackTest()",
        "setupSeekFwdTest()",
        "setupPauseStopTest()",
        "setupStopPauseTest()",

            // "setupLongTest(true)"
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

    function setupPlayTest() {
        setupTest("Play to End", "../FFMpegDecoder/testfiles/counter_short.mpg");
        window.setTimeout("testPlaying", 1000);
        window.setTimeout("testStopped", 10000);
        window.setTimeout("nextTest", 10100);
    }

    function setupStopTest() {
        setupTest("Play, Stop, Play again", "../FFMpegDecoder/testfiles/counter_short.mpg");
        window.setTimeout("stop", 1000);
        window.setTimeout("play", 2000);
        window.setTimeout("stop", 3000);
        window.setTimeout("nextTest", 4000);
    }

    function setupPauseTest() {
        setupTest("Play, Pause, Play again", "../FFMpegDecoder/testfiles/counter_short.mpg");
        window.setTimeout("pause", 1000);
        window.setTimeout("play", 2000);
        window.setTimeout("stop", 3000);
        window.setTimeout("nextTest", 4000);
    }

    function setupLoopTest() {
        setupTest("Loop", "../FFMpegDecoder/testfiles/counter_short.mpg");
        _myMovie.loopcount = 0;
        window.setTimeout("testPlaying", 1000);
        window.setTimeout("testPlaying", 10000);
        window.setTimeout("stop", 10050);
        window.setTimeout("nextTest", 10100);
    }

    function setupIntensiveSeekTest() {
        setupTest("IntensiveSeek", "../FFMpegDecoder/testfiles/counter_short.mpg");
        _myMovie.loopcount = 1;
        _mySeekDest = 1;
        window.setTimeout("seek", 500);
        window.setTimeout("seek", 1000);
        window.setTimeout("seek", 1500);
        window.setTimeout("seek", 2000);
        window.setTimeout("seek", 2500);
        window.setTimeout("seek", 3000);
        window.setTimeout("nextTest", 8000);
    }
    
    function setupSeekBackTest() {
        setupTest("SeekBack", "../FFMpegDecoder/testfiles/counter_short.mpg");
        _myMovie.loopcount = 1;
        _mySeekDest = 1;
        window.setTimeout("seek", 2000);
        window.setTimeout("seek", 6000);
        window.setTimeout("nextTest", 8000);
    }

    function setupSeekFwdTest() {
        setupTest("SeekFwd", "../FFMpegDecoder/testfiles/counter_short.mpg");
        _myMovie.loopcount = 1;
        _mySeekDest = 3;
        window.setTimeout("seek", 1000);
        window.setTimeout("nextTest", 2000);
    }

    function setupPauseStopTest() {
        setupTest("PauseStop", "../FFMpegDecoder/testfiles/counter_short.mpg");
        _myMovie.loopcount = 1;
        window.setTimeout("pause", 1000);
        window.setTimeout("stop", 1200);
        window.setTimeout("play", 1400);
        window.setTimeout("stop", 1600);
        window.setTimeout("nextTest", 2000);
    }

    function setupStopPauseTest() {
        setupTest("StopPause", "../FFMpegDecoder/testfiles/counter_short.mpg");
        _myMovie.loopcount = 1;
        window.setTimeout("stop", 1000);
        window.setTimeout("pause", 1200);
        window.setTimeout("play", 1400);
        window.setTimeout("stop", 1600);
        window.setTimeout("nextTest", 2000);
    }

    function setupLongTest() {
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


