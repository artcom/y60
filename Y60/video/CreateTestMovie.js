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

const MOVIE_WIDTH  = Number(arguments[0]);
const MOVIE_HEIGHT = Number(arguments[1]);
const FRAMERATE    = Number(arguments[2]);
const MOVIE_LENGTH = Number(arguments[3]); // seconds

const SWAP_INTERVAL = 0;
const FIXED_FRAME_TIME = 1/FRAMERATE;


function CreateTestMovie(theArguments) {
    var Public = this;
    var Base   = {};

    SceneViewer.prototype.Constructor(Public, theArguments);

    var _myMaterial = null;
    var _myShape    = null;
    var _myBody     = null;
    
    var _my10secMaterial = null;
    var _my10secShape    = null;
    var _my10secBody     = null;

    var _myFrameCount = 0;
    var _myDirectory = "testmovies/tmp";

    Base.setup = Public.setup;
    Public.setup = function() {
        Public.setSplashScreen(false);
        Base.setup(MOVIE_WIDTH, MOVIE_HEIGHT);
        window.eventListener = Public;
        
        print("creating images");
        
        _myMaterial = window.scene.createColorMaterial([1,1,1,1]);
        _myShape    = window.scene.createQuadShape(_myMaterial, [-1,-1,-5], [1,1,-5]);
        _myBody     = window.scene.createBody(_myShape);
        _myBody.visible =  false;

        _my10secMaterial = window.scene.createColorMaterial([0,1,0,1]);
        _my10secShape    = window.scene.createQuadShape(_my10secMaterial, [-1,-1,-5], [1,1,-5]);
        _my10secBody     = window.scene.createBody(_my10secShape);
        _my10secBody.visible =  false;
        
        window.swapInterval = SWAP_INTERVAL;
        window.fixedFrameTime = FIXED_FRAME_TIME;

    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
        
        var myMinutes = Math.floor(_myFrameCount/(60*FRAMERATE));
        var mySeconds = Math.floor(_myFrameCount/FRAMERATE) % 60;
        var myFrames  = _myFrameCount % FRAMERATE;

        var myMinutesString = padStringFront(myMinutes, "0", 2);
        var mySecondsString = padStringFront(mySeconds, "0", 2);
        var myFramesString  = padStringFront(myFrames, "0", 2);

        var myTimeString = myMinutesString + ":" + mySecondsString + "." + myFramesString;

        var myPos = [MOVIE_WIDTH/2 - 30, MOVIE_HEIGHT/2];
        window.setTextColor([1,0,0,1]);
        window.renderText(myPos, myTimeString, "Screen15");
        
        if(myFrames < 0.1) {
            if (mySeconds % 10 == 0){
                _my10secBody.visible = true;
            } else {
                _myBody.visible = true;
            }
        } else {
            _myBody.visible = false;
            _my10secBody.visible = false;
        }
        
    }

    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
        
        var myFileName = _myDirectory + "/" + MOVIE_WIDTH + "x" + MOVIE_HEIGHT + "_frame";
        myFileName += padStringFront(_myFrameCount, "0", 7);
        myFileName += ".png";
        window.saveBuffer(myFileName);
        
        if (_myFrameCount >= MOVIE_LENGTH*FRAMERATE) {
            print("images created");
            exit(0);    
        }
        _myFrameCount++;
    }


}

var ourShow = new CreateTestMovie(arguments);
ourShow.setup();
ourShow.go();


