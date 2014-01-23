//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SceneTester.js");

var ourMovies = [];

var ourMovieSources = [
    "testmovie_i60_RLE.m60",
    "testmovie_i60_DRLE.m60",
    "testmovie_png_RLE.m60",
    "testmovie_png_DRLE.m60",

    // ffmpeg decoder is currently in a none-too-deterministic state...
    //"testmovie.m2v",

    // Windows Media Player does not allow exact frame seeks, so the results on will be different
    // on different machines, due to different timing.
    //"testmovie.wmv",
];

function showMovie(theFilename, thePosition, theStartFrame, thePlaySpeed, thePlayMode) {
    const MOVFILE = searchFile("movies/" + theFilename);

    var myMovie = new MovieOverlay(window.scene, MOVFILE);
    myMovie.position = thePosition;
    myMovie.src          = "movies/" + theFilename;
    myMovie.playspeed    = thePlaySpeed;
    myMovie.loopcount    = 0;
    myMovie.currentframe = theStartFrame;
    myMovie.playmode     = thePlayMode;
    myMovie.startframe   = theStartFrame;
    if (myMovie.startframe > myMovie.framecount) {
        myMovie.startframe %= myMovie.framecount;
    }

    window.scene.save("movieTest.x60");

    return ourMovies.push(myMovie);
}

try {
    var Base = [];
    var ourShow = new SceneTester(arguments);

    var _myFrameCounter = 0;

    Base.onFrame = ourShow.onFrame;
    ourShow.onFrame = function(theTime) {
        Base.onFrame(theTime);
        for (var i = 0; i < ourMovies.length; ++i) {
            var myMovie = ourMovies[i];

            // We run the paused movie by hand
            if (myMovie.playmode == "pause") {
                var myFrame = myMovie.currentframe;
                myFrame = (Math.round(theTime * myMovie.playspeed * myMovie.fps) + myMovie.framecount + myMovie.startframe) % myMovie.framecount;
                myMovie.currentframe = myFrame;
                window.scene.loadMovieFrame(myMovie.movie);
            }

            // Time-based frame number
            var myExpectedFrame = Math.round(theTime * myMovie.fps * myMovie.playspeed);
            while (myExpectedFrame < 0) {
                myExpectedFrame += myMovie.framecount;
            }
            myExpectedFrame += myMovie.startframe;
            myExpectedFrame %= myMovie.framecount;

            if (myMovie.currentframe != myExpectedFrame) {
                Logger.error("got=" + myMovie.currentframe + " expected=" + myExpectedFrame + " movie=" + myMovie.src + " fps=" + myMovie.fps + " playspeed=" + myMovie.playspeed + " startframe=" +myMovie.startframe+ " time=" + theTime);
                exit(1);
            }
            // Some MPEG framecounts are valid only after the end of file has been reached once
            if (_myFrameCounter > 30 && myMovie.framecount != 30 && myMovie.playspeed > 0) {
                Logger.error("Movie=" + myMovie.src + ", got framecount=" + myMovie.framecount + ", expected=30");
                exit(1);
            }
        }
        _myFrameCounter++;
    }
    
    Base.onPreRender = ourShow.onPreRender;
    ourShow.onPreRender = function() { 
        // printScreenText(); differences in font rendering make this flaky
        Base.onPreRender();
    }

    function printScreenText() {
        var myPos = 42;
        for (var i = 0; i < _myTests.length; ++i) {
            var myRenderer = ourShow.getRenderWindow(); 
            var myTextColor = new Vector4f(0,1,1,1);            
            myRenderer.renderText([10, myPos], _myTests[i], new Node("<style textColor='" + myTextColor + "'/>").childNode(0),
                                  "Screen15");
            myTextColor = new Vector4f(1,1,1,1);            
            myPos += 20;
            for (var j = 0; j < ourMovieSources.length; ++j) {
                var myMovie = ourMovies[i * ourMovieSources.length + j];
                myRenderer.renderText([10, myPos], myMovie.src, new Node("<style textColor='" + myTextColor + "'/>").childNode(0),
                                      "Screen15");
                myRenderer.renderText([800, myPos], myMovie.currentframe, new Node("<style textColor='" + myTextColor + "'/>").childNode(0),
                                      "Screen15");
                myPos += 20;
            }
            myPos += 10;
        }
    }

    var _myTests = [];
    var _myPos = 10;

    function addTest(theName, theStartFrame, thePlaySpeed, thePlayMode) {
        _myTests.push(theName);
        for (var i = 0; i < ourMovieSources.length; ++i) {
            showMovie(ourMovieSources[i], new Vector2f(300, _myPos), theStartFrame, thePlaySpeed, thePlayMode);
            _myPos += 20;
        }
        _myPos += 30;
    }

    function setupTests() {
        window.fixedFrameTime = 0.04;

        var myOverlay = new ImageOverlay(ourShow.getScene(), "movies/reference.png");
        myOverlay.position = new Vector2i(300, _myPos);
        _myPos += 40;

        addTest("Start Position 0",   0,  1, "play");
        addTest("Start Position 10", 10,  1, "play");
        addTest("Start Position 40", 40,  1, "play");
        addTest("Playspeed 3",        0,  3, "play");
        addTest("Playspeed -1",       0, -1, "play");
        addTest("Paused",            10,  2, "pause");
    }

    ourShow.setSplashScreen(false);
    ourShow.setup(820, 700);

    ourShow.setTestDurationInFrames(70);
    setupTests();
    ourShow.go();
} catch (ex) {
    print("-------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-------------------------------------------------------------------------------");
    exit(-1);
}
