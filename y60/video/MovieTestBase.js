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

use("SceneViewer.js");

plug("Sound");

function MovieTestBase(Public, theArguments, theDecoderName) {
    var Base   = {};

    SceneViewer.prototype.Constructor(Public, theArguments);

    var _myMovie = null;
    var _myCurMovieIndex = -1;
    var _myCurTestIndex = -1;
    var _myTestFrameCount = 0;
    var _mySuccessCount = 0;
    var _myFailCount = 0;
    var _myDecoderName = theDecoderName;

    var _myMovies;
    var _myTests;

    Base.setup = Public.setup;
    Public.setup = function() {
        Public.setSplashScreen(false);
        Base.setup(320, 200);
        try {
            window.eventListener = this;
            var myNoisyString = expandEnvironment("${Y60_NOISY_SOUND_TESTS}");
            if (myNoisyString == "") {
                var mySoundManager = new SoundManager();
                mySoundManager.volume = 0.0;
            }
            window.swapInterval = 0;
            window.fixedFrameTime = 1.0/25;
            nextMovie();
        } catch (e) {
            print(e);
            this.assure_msg(false, "Exception caught.");
            Public.nextTest();
        }
    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
        _myTestFrameCount++;
        var myMovieName = _myMovies[_myCurMovieIndex];
        try {
//            print();
//            print("----- Test frame: "+_myTestFrameCount+", Movie frame: "+_myMovie.currentframe+" -----");
            _myTests[_myCurTestIndex](Public, _myTestFrameCount, myMovieName, _myMovie);
        } catch (e) {
            print(e);
            this.assure_msg(false, "Exception caught.");
            Public.nextTest();
        }
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


    function initMovie(theMovieName) {
        if (_myMovie) {
            _myMovie.removeFromScene();
            delete _myMovie;
            _myMovie = 0;
        }
        _myMovie = new MovieOverlay(window.scene,
                "../../../video/testmovies/"+theMovieName,
                new Vector2f(80, 60), null, false, null, _myDecoderName, "play");

        _myMovie.playspeed = 1;
        _myMovie.loopcount = 1;
        _myMovie.avdelay = 0;
    }

    Public.nextTest = function() {
        _myCurTestIndex++;
        if (_myCurTestIndex < _myTests.length) {
            _myTestFrameCount = -1;
        } else {
            nextMovie();
        }
        initMovie(_myMovies[_myCurMovieIndex]);
    }

    function nextMovie() {
        _myCurMovieIndex++;
        if (_myCurMovieIndex < _myMovies.length) {
            print("Testing movie "+_myMovies[_myCurMovieIndex]+"...");
        } else {
            if (_myMovie) {
                _myMovie.removeFromScene();
                delete _myMovie;
                _myMovie = 0;
            }
            print ("Tests succeeded: "+_mySuccessCount);
            print ("Tests failed: "+_myFailCount);
            if (ENDLESS_TEST) {
                if (_myFailCount > 0) {
                    exit(_myFailCount);
                } else {
                    _myCurMovieIndex = 0;
                }
            } else {
                exit(_myFailCount);
            }
        }
        _myCurTestIndex = -1;
        Public.nextTest();
    }

    Public.initTests = function(theMovies, theTests) {
        _myMovies = theMovies;
        _myTests = theTests;
    }

    Public.assure_msg = function(theCondition, theMsg) {
        if (!theCondition) {
            print("    FAILED : "+theMsg);
            _myFailCount++;
        } else {
            print("    SUCCESS: "+theMsg);
            _mySuccessCount++;
        }
    }

}
