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
//   $RCSfile: testTouchScreen.js,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2004/11/07 04:50:21 $
//
//
//=============================================================================

const PORTRAIT_MODE = true;
const APP_WIDTH     = 1280;
const APP_HEIGHT    = 1024;
const TOUCHSCREEN_COM_PORT = 0;

use("SceneViewer.js");
use("TouchScreen.js");

function TouchScreenTest(theSceneFile, theShaderLib) {
    this.Constructor(this, theSceneFile, theShaderLib);
}

TouchScreenTest.prototype.Constructor = function(obj, theSceneFile, theShaderLib) {

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // Constructor code
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(obj, theSceneFile, theShaderLib);
    obj.SceneViewer = [];

    var _myTouchScreen     = null;

    // public methods
    obj.SceneViewer.setup = obj.setup;
    obj.setup = function(theWidth, theHeight, theTitle) {
        obj.SceneViewer.setup(theWidth, theHeight, theTitle);

        if (PORTRAIT_MODE) {
            ourApp.getScreens().firstChild.orientation = "portrait";
        }

        _myTouchScreen = new TouchScreen(obj, TOUCHSCREEN_COM_PORT, PORTRAIT_MODE);

        var myBackgroundImage = obj.getImageManager().getImageNode("TestBackground");
        myBackgroundImage.src = 'shadertex/ac_logo.png';
        myBackgroundImage.resize = "scale";

        var myBackground = new ImageOverlay(obj.SceneViewer.getScene(), myBackgroundImage);
        myBackground.position = new Vector2f(0, 0);
        myBackground.width  = renderer.width;
        myBackground.height = renderer.height;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // callbacks
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    obj.SceneViewer.onKey = obj.onKey;
    obj.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theCtrlFlag) {
            obj.SceneViewer.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
        if (!theKeyState) {
            return true;
        }

        if (theKey == "f1") {
            if (_myTouchScreen.isCalibrating()) {
                _myTouchScreen.stopCalibration();
            } else {
                _myTouchScreen.startCalibration();
            }
        }

        if (theKey == "f2") {
            _myTouchScreen.setFollowMode(!_myTouchScreen.getFollowMode());
        }

        return false;
    }

    obj.SceneViewer.onMouseButton = obj.onMouseButton;
    obj.onMouseButton = function(theMouseButton, theState, theX, theY) {
        print("onMouseButton: button: " + theMouseButton + " state: " + theState + " x: " + theX + " y: " + theY);
    }

    obj.SceneViewer.onMouseMotion = obj.onMouseMotion;
    obj.onMouseMotion = function(theX, theY) {
        print("onMouseMotion() x: " + theX + " y: " + theY);
    }

    obj.SceneViewer.onFrame = obj.onFrame;
    obj.onFrame = function (theTime) {
        obj.SceneViewer.onFrame(theTime);

        window.setTextColor([0.4,0.4,1,1]);
        window.renderText([20, 20], "---  F1 : Toggle Calibration Mode  |  F2 : Toggle Follow Mode  ---", "SyntaxBold18");

        _myTouchScreen.onFrame(theTime);
    }
}

try {
    var ourApp = new TouchScreenTest(arguments);
    ourApp.setup(APP_WIDTH, APP_HEIGHT, "TouchScreen Test");

    window.fullscreen = ourApp.getReleaseMode();
    renderer.backgroundcolor = [1,1,1];
    ourApp.go();
} catch (ex) {
    print("-----------------------------------------------------------------------------------------");
    print("### Error: " + ex);
    print("-----------------------------------------------------------------------------------------");
    exit(-1);
}

