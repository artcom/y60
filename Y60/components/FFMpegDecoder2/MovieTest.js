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
    var _myCurMovieIndex = -1;
    var _myCurTestIndex = -1;
    var _myTestFrameCount = 0;
    var _mySuccessCount = 0;
    var _myFailCount = 0;

    var _myTestMovies = [
        "huffyuv_160x120_25_1_audio.avi",
        "mjpeg_160x120_25_1_audio.avi",
        "mpeg1_160x120_25_1_audio.mpg",
        "mpeg2_160x120_25_1_audio.mpg",
        "mpeg2intra_160x120_25_1_audio.mpg",
        "mpeg4_160x120_25_1_audio.avi",
        "mpeg4_160x120_25_1_audio.mov",
        "mpeg4_160x120_25_1_audio.mp4",
        "msmpeg4_160x120_25_1_audio.avi",
        "msmpeg4_160x120_25_1_audio.avi",
        "msmpeg4v2_160x120_25_1_audio.avi",
//        "realvideo_160x120_25_1_audio.rm",
        "xvid_160x120_25_1_audio.avi",
        "xvid_160x120_25_1_noaudio.avi",
    ];

    var _myTests = [
        testPlayToEnd,
        testStop,
        testPause,
        testLoop
    ];
    
    Base.setup = Public.setup;
    Public.setup = function() {
        Public.setSplashScreen(false);
        Base.setup(320, 200);
        window.eventListener = this;
        var myNoisyString = expandEnvironment("${Y60_NOISY_SOUND_TESTS}");
        if (myNoisyString == "") {
            var mySoundManager = new SoundManager();
            mySoundManager.volume = 0.0;
        }
        window.swapInterval = 0;
        window.fixedFrameTime = 1.0/25;
        nextMovie();
    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
        _myTestFrameCount++;
        var myMovieName = _myTestMovies[_myCurMovieIndex];
        _myTests[_myCurTestIndex](_myTestFrameCount, myMovieName);
    }

    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
        
        var myPos = 10;
        window.setTextColor([1,1,1,1]);
        if (_myMovie) {
            myPos += 20;
            var myText  = getFilenamePart(_myMovie.src) + " " + _myMovie.playmode
            window.renderText([10, myPos], myText, "Screen15");
            myPos += 20;
            myText  = " Frame " + _myMovie.currentframe + " of " + _myMovie.framecount;
            window.renderText([10, myPos], myText, "Screen15");
        }
    }
    
    
    function nextTest() {
        _myCurTestIndex++;
        if (_myCurTestIndex < _myTests.length) {
            _myTestFrameCount = -1;
        } else {
            nextMovie();
        }
    }
    
    function nextMovie() {
        _myCurMovieIndex++;
        if (_myCurMovieIndex < _myTestMovies.length) {
            print("Testing movie "+_myTestMovies[_myCurMovieIndex]+"...");
        } else {
            if (_myMovie) {
                _myMovie.removeFromScene();
                delete _myMovie;
                _myMovie = 0;
                print ("Tests succeeded: "+_mySuccessCount);
                print ("Tests failed: "+_myFailCount);
                exit(_myFailCount);
            }
        }
        _myCurTestIndex = -1;
        nextTest();
    }

    function testLoop(theTestFrame, theMovieName) {
        if (theTestFrame > 0) {
//            print ("TestFrame: "+theTestFrame+", Movie.currentframe: "+_myMovie.currentframe);
        }
        switch(theTestFrame) {
            case 0:
                print("  Loop...");
                initMovie(theMovieName);
                _myMovie.loopcount = 0;
                _myMovie.playmode = "play";
                break;
            case 26:
                assure_msg(_myMovie.playmode == "play", "Movie is playing last frame.");
                assure_msg(_myMovie.currentframe == 25, "Current frame is last frame.");
                break;
            case 27:
                assure_msg(_myMovie.playmode == "play", "Movie is playing.");
                assure_msg(_myMovie.currentframe == 0, "Current frame is first frame.");
                break;
            case 30:
                assure_msg(_myMovie.currentframe == 3, "Current frame is frame 3.");
                _myMovie.playmode = "stop";
                nextTest();
                break;
        }
    }
    
    function testPause(theTestFrame, theMovieName) {
        switch(theTestFrame) {
            case 0:
                print("  Play, pause, play again...");
                initMovie(theMovieName);
                _myMovie.playmode = "play";
                break;
            case 3:
                _myMovie.playmode = "pause";
                break;
            case 4:
                assure_msg(_myMovie.playmode == "pause", "Movie has paused.");
                break;
            case 6:
                _myMovie.playmode = "play";
                break;
            case 9:
                assure_msg(_myMovie.currentframe == 4, "Movie has played 4 frames.");
                _myMovie.playmode = "stop";
                nextTest();
                break;
        }
//        print ("TestFrame: "+theTestFrame+", Movie.currentframe: "+_myMovie.currentframe);
    }
    
    function testStop(theTestFrame, theMovieName) {
        switch(theTestFrame) {
            case 0:
                print("  Play, stop, play again...");
                initMovie(theMovieName);
                _myMovie.playmode = "play";
                break;
            case 3:
                _myMovie.playmode = "stop";
                break;
            case 4:
                assure_msg(_myMovie.playmode == "stop", "Movie has stopped.");
                break;
            case 6:
                _myMovie.playmode = "play";
                break;
            case 10:
                assure_msg(_myMovie.currentframe == 3, "Movie has played 3 frames.");
                _myMovie.playmode = "stop";
                nextTest();
                break;
        }
//        print ("TestFrame: "+theTestFrame+", Movie.currentframe: "+_myMovie.currentframe);
    }
    
    function testPlayToEnd(theTestFrame, theMovieName) {
        switch(theTestFrame) {
            case 0:
                print("  Play to end...");
                initMovie(theMovieName);
                _myMovie.playmode = "play";
                assure_msg(_myMovie.framecount == 26 || _myMovie.framecount == -1, 
                        "Movie framecount is correct.");
                break;
            case 1:
                assure_msg(_myMovie.playmode == "play", "Movie is playing.");
                break;
            case 4:
                assure_msg(_myMovie.currentframe == 3, "Current frame is correct.");
                break;
            case 26:
                assure_msg(_myMovie.playmode == "play", "Movie is playing last frame.");
                assure_msg(_myMovie.currentframe == 25, "Current frame is last frame.");
                break;
            case 27:
                assure_msg(_myMovie.playmode == "stop", "Movie has stopped.");
                break;
            case 28:
                nextTest();
                break;
        }
//        print (_myTestFrameCount, _myMovie.currentframe);
    }

    function initMovie(theMovieName) {
        if (_myMovie) {
            _myMovie.removeFromScene();
            delete _myMovie;
            _myMovie = 0;
        }
        _myMovie = new MovieOverlay(Public.getOverlayManager(), 
                "../../video/testmovies/"+theMovieName,
                new Vector2f(80, 60), null, false, null, "y60FFMpegDecoder2");
        _myMovie.playspeed = 1;
        _myMovie.loopcount = 1;
        _myMovie.avdelay = 0;
    }
    
    function assure_msg(theCondition, theMsg) {
        if (!theCondition) {
            print("    FAILED : "+theMsg);
            _myFailCount++;
        } else {
            print("    SUCCESS: "+theMsg);
            _mySuccessCount++;
        }
    }
}

var ourShow = new FFMpegTest(arguments);
ourShow.setup();
ourShow.go();


