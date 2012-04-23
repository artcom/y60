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

        _myMaterial = Modelling.createColorMaterial(window.scene, [1,1,1,1]);
        _myShape    = Modelling.createQuad(window.scene, _myMaterial.id, [-1,-1,-5], [1,1,-5]);
        _myBody     = Modelling.createBody(window.scene.world, _myShape.id);
        _myBody.visible =  false;

        _my10secMaterial = Modelling.createColorMaterial(window.scene, [0,1,0,1]);
        _my10secShape    = Modelling.createQuad(window.scene, _my10secMaterial.id, [-1,-1,-5], [1,1,-5]);
        _my10secBody     = Modelling.createBody(window.scene.world, _my10secShape.id);
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
        window.renderText( myPos, myTimeString, 
                           new Node("<style textColor='[1,0,0,1]'/>").childNode(0), "Screen15");

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


