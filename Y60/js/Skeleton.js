//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Skeleton.js,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 10:00:26 $
//
//=============================================================================
// example windows start code
// y6ß.exe -I "$PRO/src/Y60/js;$PRO/src/Y60/shader/" Skeleton.js shaderlibrary.xml rehearsal

use("SceneViewer.js");

function Skeleton(theArguments) {
    this.Constructor(this, theArguments);
}

Skeleton.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];

    var _myPrivateMember = null;

    //////////////////////////////////////////////////////////////////////
    //
    // Callbacks
    //
    //////////////////////////////////////////////////////////////////////

    // Will be called once, initialize window, scene, mover, etc.
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theFullscreen, theTitle) {
        Base.setup(theWidth, theHeight, theFullscreen, theTitle);
    }

    //  Will be called first in renderloop, has the time since application start
    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
    }

    // Will be called before rendering the frame
    Base.onPreRender = self.onPreRender;
    self.onPreRender = function() {
        Base.onPreRender();
    }

    // Will be called after rendering the frame, but before swap buffer
    Base.onPostRender = self.onPostRender;
    self.onPostRender = function() {
        Base.onPostRender();
    }

    // Will be called on a mouse move
    Base.onMouseMotion = self.onMouseMotion;
    self.onMouseMotion = function(theX, theY) {
        Base.onMouseMotion(theX, theY);
    }

    // Will be called on a mouse button
    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        Base.onMouseButton(theButton, theState, theX, theY);
    }

    // Will be called on a keyboard event
    Base.onKey = self.onKey;
    self.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag)
    {
        if (theControlFlag) {
            Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
        } else if (theKeyState) {
            if (theShiftFlag) {
                theKey = theKey.toUpperCase();
            }

            switch (theKey) {
                case "h":
                    printHelp();
                    break;
            }
        }
    }

    // Will be called on a mouse wheel event
    Base.onMouseWheel = self.onMouseWheel;
    self.onMouseWheel = function(theDeltaX, theDeltaY) {
        Base.onMouseWheel(theDeltaX, theDeltaY);
    }

    // Will be called on a joystick axis event
    Base.onAxis = self.onAxis;
    self.onAxis = function(device, axis, value) {
        Base.onAxis(device, axis, value);
    }

    // Will be called on a joystick button event
    Base.onButton = self.onButton;
    self.onButton = function(theDevice, theButton, theState) {
        Base.onButton(theDevice, theButton, theState);
    }

    // Will be called on a window reshape event
    Base.onResize = self.onResize;
    self.onResize = function(theNewWidth, theNewHeight) {
        Base.onResize(theNewWidth, theNewHeight);
    }

    // Will be called before exit
    Base.onExit = self.onExit;
    self.onExit = function() {
        Base.onExit();
    }

    //////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    //////////////////////////////////////////////////////////////////////
    function printHelp() {
        print("Skeleton help:");
    }
}

//
// main
//
try {
    var ourShow = new Skeleton(arguments);
    var myWindowWidth  = 800;
    var myWindowHeight = 640;
    ourShow.setup(myWindowWidth, myWindowHeight, ourShow.getReleaseMode(), "Skeleton");

    window.backgroundColor = [0, 0, 0];
    window.showMouseCursor = !ourShow.getReleaseMode();
    ourShow.go();
} catch (ex) {
    reportException(ex);
    exit(1);
}
