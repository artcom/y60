
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
        //"testfiles/testmovie.m2v",
        //"c:/TEST_MOVIES/AVI/enterprise.avi",
        //"c:/TEST_MOVIES/MPEG/MPEG_4_1280_1024.wmv",
        //"c:/TEST_MOVIES/MPEG/MPEG_4_LORES.wmv",
        //"c:/TEST_MOVIES/MPEG/bmw_aichi_windkanal_1280.mpg",
        //"c:/TEST_MOVIES/MPEG/bmw_aichi_windkanal_pal.m2v",
        "c:/TEST_MOVIES/MPEG/counter_768.mpg",
        //"c:/TEST_MOVIES/MPEG/mpeg2_1024_768_15000bps.m2v",
        //"c:/TEST_MOVIES/MPEG/mpeg2_1280_960_20000.m2v",
        //"c:/TEST_MOVIES/MPEG/aichi.m2v",
        //"c:/TEST_MOVIES/MPEG/linetest.m2v",
    ];

    var _myMovies       = [];
    var _myFrameCounter = 0;
    var _myTests        = [];
    var _myPos          = 10;

    Base.setup = Public.setup;
    Public.setup = function() {
        Public.setSplashScreen(false);
        Base.setup(840, 500);
        window.fixedFrameTime = 0.04;
        setupMovies();
    }

/*
    Base.onIdle = _myShow.onIdle;
    _myShow.onIdle = function(theTime) {
        Base.onIdle(theTime);

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
                window.renderText([10 / window.width, myPos / window.height], myMovie.movie.src, "Screen15");
                window.renderText([800 / window.width, myPos / window.height], myMovie.movie.currentframe, "Screen15");
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
            var myMovie = new MovieOverlay(Public.getOverlayManager(), _myMovieSources[i], new Vector2f(300, _myPos));
            myMovie.movie.playspeed = thePlaySpeed;
            myMovie.movie.playmode  = thePlayMode;
            myMovie.width  = 400;
            myMovie.height = 300;
            //myMovie.movie.startime  = theStartTime;
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
