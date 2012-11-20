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
*/

use("../../../video/MovieTestBase.js");

const ENDLESS_TEST = false;
const RANDOM_SEEK_ITERATIONS = 50;

function FFMpegTest(theArguments) {
    var Public = this;
    var Base   = {};

    MovieTestBase(Public, theArguments, "FFMpegDecoder3");

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
        "xvid_160x120_25_1_audio.mov",
        "xvid_160x120_25_1_noaudio.avi",
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
                This.assure_msg(theMovie.currentframe == 4, "Movie has played 4 frames.");
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
                This.assure_msg(theMovie.currentframe == 5, "Movie has played 5 frames.");
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
                This.assure_msg(theMovie.currentframe == 2, "Current frame is 2");
                break;
            case 12:
                This.assure_msg(theMovie.currentframe == 3, "Movie has played 3 frames.");
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
                This.assure_msg(theMovie.currentframe == 2, "Current frame is 2");
                break;
            case 10:
                This.assure_msg(theMovie.currentframe == 3, "Movie has played 3 frames.");
                theMovie.playmode = "stop";
                This.nextTest();
                break;
        }
    }

    function testSeek(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Seek...");
                break;
            case 3:
                theMovie.playmode = "pause";
                theMovie.currentframe = 15;
                window.scene.loadMovieFrame(theMovie.movie);
//                theMovie.currentframe = 16;
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
/*
                if (theMovieName == "mpeg1_160x120_25_1_audio.mpg" ||
                    theMovieName == "mpeg2_160x120_25_1_audio.mpg" ||
                    theMovieName == "mpeg2intra_160x120_25_1_audio.mpg")
                {
                    This.nextTest();
                }
*/
                break;
            case RANDOM_SEEK_ITERATIONS+3:
                This.assure_msg(theMovie.playmode == "play", "Playback after random seeks.");
                This.nextTest();
                break;
        }
        if (theTestFrame > 0 && theTestFrame < RANDOM_SEEK_ITERATIONS) {
            theMovie.playmode = "pause";
            var mySeekDest = Math.floor(Math.random()*20);
            theMovie.currentframe = mySeekDest;
            window.scene.loadMovieFrame(theMovie.movie);
            theMovie.playmode = "play";
        }
    }
}

var ourShow = new FFMpegTest(arguments);
ourShow.setup();
ourShow.go();


