//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
// example start code
// y60 ASSMouse.js rehearsal
//=============================================================================

use("SceneViewer.js");
use("ASSManager.js");
plug("ASSMouse");

function ASSMouseSpike(theArguments, theWidth, theHeight, theTitle) {

    //////////////////////////////////////////////////////////////////////
    // Baseclass construction
    //////////////////////////////////////////////////////////////////////
   
    var _myASSMouse = null;
    var _myASSManager = null;

    var _myMouseMoveId = null;
    var _myButtonPressedId = null;
    var _myTargetPosition = null;
    var _myMousePosition = new Vector2f( 0, 0 );
    var _myVelocity = new Vector2f( 0, 0 );
    var _myLastTime = 0;

    var _myHost = "localhost";

    const MOVE_VELOCITY = 50; 
    const RESIZE_VELOCITY = 20; 
    const DAMPING_FACTOR = 1.3; 

    const WINDOW_POSITION = [0, 0];

    var Base   = {};
    var Public = this;
    SceneViewer.prototype.Constructor(Public, theArguments);

    Public.Constructor = function() {
        window = new RenderWindow();
        window.position = [0, 0];
        window.decorations = false;

        // Call base class setup
        Public.setup(theWidth, theHeight, false, theTitle);
        window.swapInterval    = 0;

        if (theArguments.length == 3) { 
            _myHost = theArguments[2];
        }

        _myASSMouse = new ASSMouse( _myHost, 24800);
        _myASSManager = new ASSManager( Public );

        if (_myHost == "localhost" && operatingSystem() == "LINUX") {
//            exec( "killall synergyc" );
//            exec( "synergyc localhost" );
        }

        window.position = WINDOW_POSITION;
    }

    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    //  Will be called first in renderloop, has the time since application start
    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);

        if (_myTargetPosition) {

            var myPosition = product( sum( product( _myMousePosition, 
                                                    MOVE_VELOCITY), 
                                           _myTargetPosition), 
                                      1 / (MOVE_VELOCITY + 1)); 
            _myMousePosition = myPosition; 
            if (distance( _myMousePosition, _myTargetPosition ) < 0.01) {
                _myTargetPosition = null;
            }
            _myASSMouse.onMouseMotion( _myMousePosition.x + WINDOW_POSITION[0], 
                                       _myMousePosition.y + WINDOW_POSITION[1] );

        }

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
        Base.onMouseMotion(theX, theY);
        if (_myHost != "localhost") {
            _myASSMouse.onMouseMotion( theX + WINDOW_POSITION[0], 
                                       theY + WINDOW_POSITION[1] );
        }
    }

    // Will be called on a mouse button
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        Base.onMouseButton(theButton, theState, theX, theY);
        if (_myHost != "localhost") {
            _myASSMouse.onMouseButton( theButton, theState );
        }
    }

    // Will be called on a keyboard event
    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag)
    {
        Base.onKey( theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, 
                    theAltFlag);
    }

    // Will be called on a mouse wheel event
    Base.onMouseWheel = Public.onMouseWheel;
    Public.onMouseWheel = function(theDeltaX, theDeltaY) {
        Base.onMouseWheel(theDeltaX, theDeltaY);
        if (_myHost != "localhost") {
            _myASSMouse.onMouseWheel( theDeltaX, theDeltaY );
        }
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
        if (_myHost == "localhost" && operatingSystem() == "LINUX") {
//            exec("killall synergyc");
        }
    }

    Public.onASSEvent = function( theNode ) {

        _myASSManager.onASSEvent( theNode );

        if ( theNode.type == "add") {
            if (_myMouseMoveId == null) {
                _myMouseMoveId = theNode.id;
                _myTargetPosition = null;
                var myCurrentMousePosition = new Vector2f( theNode.position3D.x,
                                                           theNode.position3D.y );
                if (distance( _myMousePosition, myCurrentMousePosition ) < 100) {
                    _myMousePosition = myCurrentMousePosition;
                }
                _myVelocity = new Vector2f( 0, 0 );
            } else if (_myButtonPressedId == null) {
                _myButtonPressedId = theNode.id;
                _myASSMouse.onMouseButton( 1, true );
            }
        } else if ( theNode.type == "remove") {
            if (_myMouseMoveId == theNode.id) {
                _myMouseMoveId = null;
            } else if (_myButtonPressedId == theNode.id) {
                _myButtonPressedId = null;
                _myASSMouse.onMouseButton( 1, false );
            }
        } else if ( theNode.type == "move" ) {
            if (_myMouseMoveId == theNode.id) {
                _myTargetPosition = new Vector2f( theNode.position3D.x, 
                                                  theNode.position3D.y );
            }
        }

    }

    //////////////////////////////////////////////////////////////////////
    // Private
    //////////////////////////////////////////////////////////////////////

    Public.Constructor();
}

//
// main
//
try {
//    var mywindowwidth  = 1400;
//    var myWindowHeight = 770;
    var myWindowWidth  = 1920;
    var myWindowHeight = 1200;
    var ourShow = new ASSMouseSpike(arguments, myWindowWidth, myWindowHeight, "ASSMouse");
    ourShow.go();
} catch (ex) {
    reportException(ex);
    exit(1);
}
