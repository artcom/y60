//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
// example start code
// y60 Skeleton.js rehearsal
//=============================================================================

use("SceneViewer.js");

function Skeleton(theArguments, theWidth, theHeight, theTitle) {

    //////////////////////////////////////////////////////////////////////
    // Baseclass construction
    //////////////////////////////////////////////////////////////////////

    var Base   = {};
    var Public = this;
    SceneViewer.prototype.Constructor(Public, theArguments);

    Public.Constructor = function() {
        window = new RenderWindow();
        window.position = [0, 0];
        window.decorations = false;

        // Call base class setup
        Public.setSplashScreen(Public.getReleaseMode());
        Public.setup(theWidth, theHeight, false, theTitle);
        window.showTaskbar = !Public.getReleaseMode();
        window.backgroundColor = [0.8, 0.8, 0.8];
        window.showMouseCursor = !Public.getReleaseMode();
        window.swapInterval    = 1;

        // Add your own setup here

        // Some dummy code for fun
        _myCrossHair = new ImageOverlay(window.scene, "shadertex/crosshair.png", [100, 100]);
    }

    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    //  Will be called first in renderloop, has the time since application start
    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }

    // Will be called before rendering the frame
    Base.onPreRender = Public.onPreRender;
    Public.onPreRender = function() {
        Base.onPreRender();
    }

    // Will be called after rendering the frame, but before swap buffer
    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
    }

    // Will be called on a mouse move
    Base.onMouseMotion = Public.onMouseMotion;
    Public.onMouseMotion = function(theX, theY) {
        if (_myMoverEnabled) {
            Base.onMouseMotion(theX, theY);
        }

        // Some dummy code for fun
        _myCrossHair.position = [theX- _myCrossHair.width / 2, theY - _myCrossHair.height / 2];
    }

    // Will be called on a mouse button
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        if (_myMoverEnabled) {
            Base.onMouseButton(theButton, theState, theX, theY);
        }
    }

    // Will be called on a keyboard event
    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag)
    {
        if (theControlFlag) {
            Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
        } else if (theKeyState) {
            if (theShiftFlag) {
                theKey = theKey.toUpperCase();
            }

            switch (theKey) {
                case "m":
                    _myMoverEnabled = !_myMoverEnabled;
                    print("Mover " + (_myMoverEnabled ? "enabled" : "disabled"));
                    break;
                case "h":
                    printHelp();
                    break;
            }
        }
    }

    // Will be called on a mouse wheel event
    Base.onMouseWheel = Public.onMouseWheel;
    Public.onMouseWheel = function(theDeltaX, theDeltaY) {
        Base.onMouseWheel(theDeltaX, theDeltaY);
    }

    // Will be called on a joystick axis event
    Base.onAxis = Public.onAxis;
    Public.onAxis = function(device, axis, value) {
        Base.onAxis(device, axis, value);
    }

    // Will be called on a joystick button event
    Base.onButton = Public.onButton;
    Public.onButton = function(theDevice, theButton, theState) {
        Base.onButton(theDevice, theButton, theState);
    }

    // Will be called on a window reshape event
    Base.onResize = Public.onResize;
    Public.onResize = function(theNewWidth, theNewHeight) {
        Base.onResize(theNewWidth, theNewHeight);
    }

    // Will be called before exit
    Base.onExit = Public.onExit;
    Public.onExit = function() {
        Base.onExit();
    }

    //////////////////////////////////////////////////////////////////////
    // Private
    //////////////////////////////////////////////////////////////////////

    function printHelp() {
        print("Skeleton help:");
        print("   m    - Enable/Disable mover");
    }

    var _myMoverEnabled = false;
    var _myCrossHair    = null;

    Public.Constructor();
}

//
// main
//
try {
    var myWindowWidth  = 800;
    var myWindowHeight = 640;
    var ourShow = new Skeleton(arguments, myWindowWidth, myWindowHeight, "Skeleton");
    ourShow.go();
} catch (ex) {
    reportException(ex);
    exit(1);
}
