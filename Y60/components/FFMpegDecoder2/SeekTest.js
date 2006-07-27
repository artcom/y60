//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("MovieTestBase.js");

const ENDLESS_TEST = false;

function SeekTest(theArguments) {
    var Public = this;
    var Base   = {};

    MovieTestBase(Public, theArguments);

    var _myTestMovies = [
        "mjpeg_160x120_25_10_audio.avi",
        "xvid_160x120_25_10_audio.avi",
        "xvid_160x120_25_10_audio.mov",
        "xvid_160x120_25_10_noaudio.mov",
        "mpeg1_160x120_25_10_audio.mpg",
        "mpeg2_160x120_25_10_audio.mpg",
        "mpeg2intra_160x120_25_10_audio.mpg"
    ];

    var _myTests = [
        testSeek
    ];

    function seek(theMovie, theFrame) {
        theMovie.playmode = "pause";
        theMovie.currentframe = theFrame;
        window.scene.loadMovieFrame(theMovie.movie);
        theMovie.playmode = "play";
    }
    
    function testSeek(This, theTestFrame, theMovieName, theMovie) {
        switch(theTestFrame) {
            case 0:
                print("  Seek test...");
                print ("Number of frames: "+theMovie.framecount);
                break;
            case 3:
                seek(theMovie, 150);
                break;
            case 4:
                This.assure_msg(theMovie.currentframe == 150, "Seek to frame 150.");
                break;
            case 5:
                This.assure_msg(theMovie.currentframe == 151, "Play after seek.");
                seek(theMovie, 100);
                break;
            case 6:
                This.assure_msg(theMovie.currentframe == 100, "Seek to frame 100.");
                break;
            case 7:
                This.assure_msg(theMovie.currentframe == 101, "Play after seek.");
                seek(theMovie, 249);
                break;
            case 8:
                This.assure_msg(theMovie.currentframe == 249, "Seek to frame 249.");
                break;
            case 9:
                This.nextTest();
                break;
        }
    }

    this.initTests(_myTestMovies, _myTests);
}

var ourShow = new SeekTest(arguments);
ourShow.setup();
ourShow.go();

