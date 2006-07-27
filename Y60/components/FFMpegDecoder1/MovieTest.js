//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("../../video/MovieTestBase.js");

const ENDLESS_TEST = false;
const RANDOM_SEEK_ITERATIONS = 50;

function FFMpegTest(theArguments) {
    var Public = this;
    var Base   = {};

    MovieTestBase(Public, theArguments, "y60FFMpegDecoder1");

    var _myTestMovies = [
        "huffyuv_160x120_25_1_audio.avi",
        "mjpeg_160x120_25_1_audio.avi",
        "mpeg2intra_160x120_25_1_audio.mpg",
//        "xvid_160x120_25_1_audio.mov"
    ];

    var _myTests = [
        testPlayToEnd,
        testStop,
        testPause,
        testLoop,
        testPauseStop,
        testStopPause,
        testSeek,
        testRandomSeek
    ];

    this.initTests(_myTestMovies, _myTests);
    
    function testPlayToEnd(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Play to end...");
                This.assure_msg(theMovie.framecount == 26 || theMovie.framecount == -1, 
                        "Movie framecount is correct.");
                break;
            case 1:
                This.assure_msg(theMovie.playmode == "play", "Movie is playing.");
                break;
            case 3:
                This.assure_msg(theMovie.currentframe == 3, "Current frame is correct.");
                break;
            case 25:
                This.assure_msg(theMovie.playmode == "play", "Movie is playing last frame.");
                This.assure_msg(theMovie.currentframe == 25, "Current frame is last frame.");
                break;
            case 26:
                This.assure_msg(theMovie.playmode == "stop", "Movie has stopped.");
                break;
            case 27:
                This.nextTest();
                break;
        }
    }

    function testStop(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Play, stop, play again...");
                break;
            case 2:
                theMovie.playmode = "stop";
                break;
            case 3:
                This.assure_msg(theMovie.playmode == "stop", "Movie has stopped.");
                break;
            case 5:
                theMovie.playmode = "play";
                break;
            case 9:
                This.assure_msg(theMovie.currentframe == 3, "Movie has played 3 frames.");
                theMovie.playmode = "stop";
                This.nextTest();
                break;
        }
    }
    
    function testPause(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Play, pause, play again...");
                break;
            case 2:
                theMovie.playmode = "pause";
                break;
            case 3:
                This.assure_msg(theMovie.playmode == "pause", "Movie has paused.");
                break;
            case 5:
                theMovie.playmode = "play";
                break;
            case 8:
                This.assure_msg(theMovie.currentframe == 4, "Movie has played 4 frames.");
                theMovie.playmode = "stop";
                This.nextTest();
                break;
        }
    }
    
    function testLoop(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Loop...");
                theMovie.loopcount = 0;
                break;
            case 25:
                This.assure_msg(theMovie.playmode == "play", "Movie is playing last frame.");
                This.assure_msg(theMovie.currentframe == 25, "Current frame is last frame.");
                break;
            case 26:
                This.assure_msg(theMovie.playmode == "play", "Movie is playing.");
                This.assure_msg(theMovie.currentframe == 0, "Current frame is first frame.");
                break;
            case 29:
                This.assure_msg(theMovie.currentframe == 3, "Current frame is frame 3.");
                theMovie.playmode = "stop";
                This.nextTest();
                break;
        }
    }  
  
    function testPauseStop(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Play, pause, stop again...");
                break;
            case 3:
                theMovie.playmode = "pause";
                break;
            case 4:
                This.assure_msg(theMovie.playmode == "pause", "Movie has paused.");
                break;
            case 6:
                theMovie.playmode = "stop";
                break;
            case 7:
                This.assure_msg(theMovie.playmode == "stop", "Movie has stopped.");
                This.assure_msg(theMovie.currentframe == 0, "Current frame is 0");
                break;
            case 9:
                theMovie.playmode = "play";
                break;
            case 11:
                This.assure_msg(theMovie.playmode == "play", "Movie is playing again.");
                This.assure_msg(theMovie.currentframe == 1, "Current frame is 1");
                break;
            case 12:
                This.assure_msg(theMovie.currentframe == 2, "Movie has played 3 frames.");
                theMovie.playmode = "stop";
                This.nextTest();
                break;
        }
    }
    
    function testStopPause(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Play, stop, pause, play again...");
                break;
            case 3:
                theMovie.playmode = "stop";
                break;
            case 4:
                This.assure_msg(theMovie.playmode == "stop", "Movie has stopped.");
                break;
            case 5:
                theMovie.playmode = "pause";
                break;
            case 6:
                This.assure_msg(theMovie.playmode == "pause", "Movie has paused.");
                This.assure_msg(theMovie.currentframe == 0, "Current frame is 0");
                break;
            case 7:
                theMovie.playmode = "play";
                break;
            case 9:
                This.assure_msg(theMovie.playmode == "play", "Movie is playing again.");
                This.assure_msg(theMovie.currentframe == 1, "Current frame is 1");
                break;
            case 10:
                This.assure_msg(theMovie.currentframe == 2, "Movie has played 3 frames.");
                theMovie.playmode = "stop";
                This.nextTest();
                break;
        }
    }

    function testSeek(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Seek...");
                if (theMovieName == "mpeg1_160x120_25_1_audio.mpg" ||
                    theMovieName == "mpeg2_160x120_25_1_audio.mpg" ||
                    theMovieName == "mpeg2intra_160x120_25_1_audio.mpg") 
                {
                    This.nextTest();
                }
                break;
            case 3:
                theMovie.playmode = "pause";
                theMovie.currentframe = 15;
                window.scene.loadMovieFrame(theMovie.movie);
                theMovie.playmode = "play";
                break;
            case 4:
                This.assure_msg(theMovie.currentframe == 15, "Seek forward ok.");
                break;
            case 8:
                This.assure_msg(theMovie.currentframe == 19, "Playback after seek forward ok.");
                theMovie.playmode = "pause";
                theMovie.currentframe = 4;
                window.scene.loadMovieFrame(theMovie.movie);
                theMovie.playmode = "play";
                break;
            case 9:
                This.assure_msg(theMovie.currentframe == 4, "Seek backward ok.");
                break;
            case 12:
                This.assure_msg(theMovie.currentframe == 7, "Playback after seek backward ok.");
                This.nextTest();
                break;
        }
    }
    
    function testRandomSeek(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Random seek...");
                break;
            case 53:
                This.assure_msg(theMovie.playmode == "play", "Playback after random seeks.");
                This.nextTest();
                break;
        }
        if (theTestFrame > 0 && theTestFrame < RANDOM_SEEK_ITERATIONS) {
            theMovie.playmode = "pause";
            var mySeekDest = Math.floor( Math.random()*26);
            theMovie.currentframe = mySeekDest;
            window.scene.loadMovieFrame(theMovie.movie);
            theMovie.playmode = "play";
        }
    }
}

var ourShow = new FFMpegTest(arguments);
ourShow.setup();
ourShow.go();

