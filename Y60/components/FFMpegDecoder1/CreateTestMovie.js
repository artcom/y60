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
use("VideoRecorder.js");
plug("y60JSSound");

var theRequest = [];
theRequest["MOVIE_WIDTH"]  = arguments[0];
theRequest["MOVIE_HEIGHT"] = arguments[1];
theRequest["FRAMERATE"]    = arguments[2];
theRequest["MOVIE_LENGTH"] = arguments[3];

const MOVIE_WIDTH  = theRequest["MOVIE_WIDTH"];
const MOVIE_HEIGHT = theRequest["MOVIE_HEIGHT"];
const FRAMERATE    = 25;
const MOVIE_LENGTH = 10; // seconds

const SWAP_INTERVAL = 0;
const FIXED_FRAME_TIME = 1/FRAMERATE;


function CreateTestMovie(theArguments) {
    var Public = this;
    var Base   = {};

    SceneViewer.prototype.Constructor(Public, theArguments);

    var _myMaterial = null;
    var _myShape = null;
    var _myBody = null;
    var _myTestName = "";
    var _myCurTestIndex = -1;
    var _myFrameCount = 0;
    var _myDirectory = "testmovies";

    Base.setup = Public.setup;
    Public.setup = function() {
        Public.setSplashScreen(false);
        Base.setup(MOVIE_WIDTH, MOVIE_HEIGHT);
        window.eventListener = Public;
        
        print("creating images");
        
        var myNoisyString = expandEnvironment("${Y60_NOISY_SOUND_TESTS}");
        if (myNoisyString == "") {
            var mySoundManager = new SoundManager();
            mySoundManager.volume = 0.0;
        }
        
        _myMaterial = window.scene.createColorMaterial([1,1,1,1]);
        _myShape    = window.scene.createQuadShape(_myMaterial, [-1,-1,-5], [1,1,-5]);
        _myBody     = window.scene.createBody(_myShape);
        _myBody.visible =  false;
        
        window.swapInterval = SWAP_INTERVAL;
        window.fixedFrameTime = FIXED_FRAME_TIME;

    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
        
        var myMinutes = Math.floor(theTime / 60);
        var mySeconds = Math.floor(theTime) % 60;
        var myFrames  = Math.floor(theTime* FRAMERATE) % FRAMERATE;

        var myMinutesString = padStringFront(myMinutes, "0", 2);
        var mySecondsString = padStringFront(mySeconds, "0", 2);
        var myFramesString  = padStringFront(myFrames, "0", 2);

        var myTimeString = myMinutesString + ":" + mySecondsString + "." + myFramesString;

        var myPos = [MOVIE_WIDTH/2 - 30, MOVIE_HEIGHT/2];
        window.setTextColor([1,0,0,1]);
        window.renderText(myPos, myTimeString, "Screen15");
        
        if(myFrames < 0.1) {
            _myBody.visible =  true;
        } else {
            _myBody.visible =  false;
        }
        
        if (mySeconds >= MOVIE_LENGTH){
            print("images created");
            exit(0);    
        }
        
    }

    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
        
        var myFileName = _myDirectory + "/" + MOVIE_WIDTH + "x" + MOVIE_HEIGHT + "_frame";
        myFileName += padStringFront(_myFrameCount, "0", 7);
        myFileName += ".bmp";
        window.saveBuffer(myFileName);
        _myFrameCount++;
        
    }


}

var ourShow = new CreateTestMovie(arguments);
ourShow.setup();
ourShow.go();


