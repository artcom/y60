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

use( "SynergyEventHandler.js" );


function ClickWidthSecondHand( theSynergyServer, theSettings ) {
    this.Constructor( this, theSynergyServer, theSettings );
}

ClickWidthSecondHand.prototype.Constructor = function( Public, theSynergyServer,
                                                       theSettings )
{

    var Protected = [];
    SynergyEventHandler.prototype.Constructor( Public, Protected, theSynergyServer,
                                               theSettings );
    var Base = [];

    var _mySynergyServer = theSynergyServer;

    const MOVE_VELOCITY = 40;

    var _myVelocity = new Vector2f(0,0);
    var _myMouseMoveId = null;
    var _myMouseButtonPressed = false;
    var _myButtonPressedId = null;

    function releaseMouse() {
        if (_myMouseButtonPressed) {
            print( "mouse button released" );
            _mySynergyServer.onMouseButton( 1, false );
            _myMouseButtonPressed = false;
        }
    }



    Base.onRemoveFirst = Public.onRemoveFirst;
    Public.onRemoveFirst = function( theEvent ) {
        releaseMouse();
    }

    Base.onRemoveSecond = Public.onRemoveSecond;
    Public.onRemoveSecond = function( theEvent ) {
        releaseMouse();
    }

    Base.onASSEvent = Public.onASSEvent;
    Public.onASSEvent = function( theEvent ) {

        Base.onASSEvent( theEvent );

        if ( theEvent.type == "add") {
            if (Protected.firstEvents.length == 1) {
                Protected.targetPosition = null;
                var myMousePosition = Protected.getMousePos( theEvent.raw_position );
                _mySynergyServer.onMouseMotion( myMousePosition.x
                                                + Protected.positionOffset[0],
                                                myMousePosition.y
                                                + Protected.positionOffset[1]);
//                if (_myButtonPressedId) {
//                    _mySynergyServer.onMouseButton( 1, true );
//                }
                _myVelocity = new Vector2f( 0, 0 );
            } else if (Protected.secondEvents.length == 1) {
                print( "mouse button pressed..." );
                _mySynergyServer.onMouseButton( 1, true );
                _myMouseButtonPressed = true;
            }
        }
        else if ( theEvent.type == "move" ) {
            if (Protected.singleCursor( theEvent )) {
                Protected.targetPosition = Protected.getMousePos( theEvent.raw_position );
            }
        }
    }
}
