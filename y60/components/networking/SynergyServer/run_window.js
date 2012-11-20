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
*/
// example start code
// y60 demo.js rehearsal
//=============================================================================

use("SceneViewer.js");
use("ASSManager.js");
plug("SynergyServer");

function SynergyServerSpike(theArguments, theWidth, theHeight, theTitle) {

    //////////////////////////////////////////////////////////////////////
    // Baseclass construction
    //////////////////////////////////////////////////////////////////////

    var _mySynergyServer = null;
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

        _mySynergyServer = new SynergyServer( _myHost, 24800);
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
            _mySynergyServer.onMouseMotion( _myMousePosition.x + WINDOW_POSITION[0],
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
            _mySynergyServer.onMouseMotion( theX + WINDOW_POSITION[0],
                                       theY + WINDOW_POSITION[1] );
        }
    }

    // Will be called on a mouse button
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        Base.onMouseButton(theButton, theState, theX, theY);
        if (_myHost != "localhost") {
            _mySynergyServer.onMouseButton( theButton, theState );
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
            _mySynergyServer.onMouseWheel( theDeltaX, theDeltaY );
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
                _mySynergyServer.onMouseButton( 1, true );
            }
        } else if ( theNode.type == "remove") {
            if (_myMouseMoveId == theNode.id) {
                _myMouseMoveId = null;
            } else if (_myButtonPressedId == theNode.id) {
                _myButtonPressedId = null;
                _mySynergyServer.onMouseButton( 1, false );
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
    var ourShow = new SynergyServerSpike(arguments, myWindowWidth, myWindowHeight, "SynergyServer");
    ourShow.go();
} catch (ex) {
    reportException(ex);
    exit(1);
}
