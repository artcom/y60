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

use("../../../video/MovieTestBase.js");

const ENDLESS_TEST = false;

function SeekTest(theArguments) {
    var Public = this;
    var Base   = {};

    MovieTestBase(Public, theArguments, "FFMpegDecoder3");

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

