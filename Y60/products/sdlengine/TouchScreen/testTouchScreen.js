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

        var myBackground = obj.getOverlayManager().create("TestBackground", myBackgroundImage.id, true);
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

        window.setTextColor([0.4,0.4,1,1], [1,1,1,1]);
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

