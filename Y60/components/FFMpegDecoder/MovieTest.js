
//=============================================================================
// Copyright (C) 2003-2005, ART+COM AG Berlin
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

plug("y60FFMpegDecoder");

function FFMpegTest(theArguments) {
    var Public = this;
    var Base   = {};

    SceneViewer.prototype.Constructor(Public, theArguments);

    const _myMovieSources = [
        //"testfiles/mpeg2_pal.m2v",
        //"c:/TEST_MOVIES/AVI/enterprise.avi",
        //"c:/TEST_MOVIES/MPEG/MPEG_4_1280_1024.wmv",
        //"c:/TEST_MOVIES/MPEG/MPEG_4_LORES.wmv",
        //"c:/TEST_MOVIES/MPEG/bmw_aichi_windkanal_1280.mpg",
        //"c:/TEST_MOVIES/MPEG/bmw_aichi_windkanal_pal.m2v",
        //"c:/TEST_MOVIES/MPEG/counter_768.mpg",
        //"c:/TEST_MOVIES/MPEG/counter.mpg",
        //"c:/TEST_MOVIES/MPEG/mpeg2_1024_768_15000bps.m2v",
        //"c:/TEST_MOVIES/MPEG/mpeg2_1280_960_20000.m2v",
        //"c:/TEST_MOVIES/MPEG/aichi.m2v",
        //"c:/TEST_MOVIES/MPEG/linetest.m2v",
        //"c:/TEST_MOVIES/MPEG/10.mpg",
        "c:/TEST_MOVIES/MPEG/ADTEC.MPG",
        //"c:/Dev/pro60/testmodels/movies/testmovie.m2v"
    ];

    var _myMovies       = [];
    var _myFrameCounter = 0;
    var _myTests        = [];
    var _myPos          = 10;

    Base.setup = Public.setup;
    Public.setup = function() {
        Public.setSplashScreen(false);
        Base.setup(840, 500);
        //window.fixedFrameTime = 0.04;
        setupMovies();
    }

    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        var myMovie = _myMovies[0];

        if (theCtrlFlag) {
            Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
            return;
        }

        if (theState) {
            switch (theKey) {
                case "p":
                    if (myMovie.playmode == "pause") {
                        myMovie.playmode = "play";
                    } else {
                        myMovie.playmode = "pause";
                    }
                    break;
                case "s":
                    myMovie.playmode = "stop";
                    break;
                case "left":
                    var myDelta = theShiftFlag ? myMovie.fps : 1;
                    myMovie.playmode = "pause";
                    if (myMovie.currentframe <= 0) {
                        myMovie.currentframe = myMovie.framecount - myDelta;
                    } else {
                        myMovie.currentframe -= myDelta;
                    }
                    break;
                case "right":
                    myDelta = theShiftFlag ? myMovie.fps : 1;
                    myMovie.playmode = "pause";
                    if (myMovie.currentframe >= myMovie.framecount) {
                        myMovie.currentframe = myDelta;
                    } else {
                        myMovie.currentframe += myDelta;
                    }
                    break;
                case "b":
                    myMovie.playmode = "pause";
                    myMovie.currentframe = myMovie.currentframe - 25;
                    break;
                case "f":
                    myMovie.playmode = "pause";
                    myMovie.currentframe = myMovie.currentframe + 25;
                    break;
                case "[0]":
                case "[1]":
                case "[2]":
                case "[3]":
                case "[4]":
                case "[5]":
                case "[6]":
                case "[7]":
                case "[8]":
                case "[9]":
                    myMovie.playmode = "pause";
                    myMovie.currentframe = Number(theKey.substring(1,2));
                    break;
                case "[+]":
                    myMovie.playspeed++;
                    break;
                case "[-]":
                    myMovie.playspeed--;
                    break;
            }
        }
    }


/*
    Base.onFrame = _myShow.onFrame;
    _myShow.onFrame = function(theTime) {
        Base.onFrame(theTime);

        for (var i = 0; i < _myMovies.length; ++i) {
            var myMovieNode = _myMovies[i].node;

            // We run the paused movie by hand
            if (myMovieNode.playmode == "pause") {
                var myFrame = myMovieNode.currentframe;
                myFrame = (Math.floor(theTime * myMovieNode.playspeed * myMovieNode.fps) + myMovieNode.framecount + _myMovies[i].startframe) % myMovieNode.framecount;
                myMovieNode.currentframe = myFrame;
            }

            window.loadMovieFrame(myMovieNode);
            if (_myMovies[i].startframe > myMovieNode.framecount) {
                _myMovies[i].startframe %= myMovieNode.framecount;
            }

            // Time-based frame number
            var myExpectedFrame = Math.floor(theTime * myMovieNode.fps * myMovieNode.playspeed);

            while (myExpectedFrame < 0) {
                myExpectedFrame += myMovieNode.framecount;
            }
            myExpectedFrame += _myMovies[i].startframe;
            myExpectedFrame %= myMovieNode.framecount;

            if (myMovieNode.currentframe != myExpectedFrame) {
                print("### ERROR: Movie=" + myMovieNode.src + " playmode=" + myMovieNode.playmode + ", got frame=" + myMovieNode.currentframe + ", expected=" + myExpectedFrame + ", playspeed=" + myMovieNode.playspeed);
                //exit(1);
            }
            // Some MPEG framecounts are valid only after the end of file has been reached once
            if (_myFrameCounter > 30 && myMovieNode.framecount != 30 && myMovieNode.playspeed > 0) {
                print("### ERROR: Movie=" + myMovieNode.src + ", got framecount=" + myMovieNode.framecount + ", expected=30");
                exit(1);
            }
        }
        _myFrameCounter++;

        printScreenText();
    }
    */

    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
        var myPos = 42;
        for (var i = 0; i < _myTests.length; ++i) {
            window.setTextColor([1,1,1,1], [1,1,1,1]);
            window.renderText([10 / window.width, myPos / window.height], _myTests[i], "Screen15");
            window.setTextColor([1,1,1,1], [1,1,1,1]);
            myPos += 20;
            for (var j = 0; j < _myMovieSources.length; ++j) {
                var myMovie = _myMovies[i * _myMovieSources.length + j];
                var myText  = basename(myMovie.src) + " " + myMovie.playmode + " " + myMovie.currentframe + "/" + myMovie.framecount;
                window.renderText([10 / window.width, myPos / window.height], myText, "Screen15");
                myPos += 20;
            }
            myPos += 10;
        }
    }

    function showMovie(theFilename, thePosition, theStartFrame, thePlaySpeed, thePlayMode) {
    }

    function addTest(theName, theStartFrame, thePlaySpeed, thePlayMode) {
        _myTests.push(theName);
        for (var i = 0; i < _myMovieSources.length; ++i) {
            var myMovie = new MovieOverlay(Public.getOverlayManager(), _myMovieSources[i], new Vector2f(300, _myPos), null, true);
            myMovie.playspeed = 1;
            myMovie.playmode  = thePlayMode;
            myMovie.loopcount = 0;
            myMovie.avdelay   = -0.1;
            //myMovie.startime  = theStartTime;

            if (myMovie.width > 480) {
                var myAspect = myMovie.width / myMovie.height;
                myMovie.width  = 480;
                myMovie.height = myMovie.width / myAspect;
            }
            _myMovies.push(myMovie);
            _myPos += 20;
        }
        _myPos += 30;
    }

    function setupMovies() {
        var myOverlay = new ImageOverlay(ourShow.getOverlayManager(), "testfiles/reference.png", [300, _myPos]);
        _myPos += 40;
        addTest("Simple Tests", 0,  1, "play");
    }
}

var ourShow = new FFMpegTest(arguments);
ourShow.setup();
ourShow.go();
