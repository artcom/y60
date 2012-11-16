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


function ClickOnPointHandler( theSynergyServer, theSettings ) {
    this.Constructor( this, theSynergyServer, theSettings );
}

ClickOnPointHandler.prototype.Constructor
    = function( Public, theSynergyServer, theSettings )
{

    var Protected = [];
    SynergyEventHandler.prototype.Constructor( Public, Protected, theSynergyServer,
                                               theSettings );
    var Base = [];

    var _mySynergyServer = theSynergyServer;

    var _myMouseButtonPressed = false;
    var _myLastRemoveEvent = null;

    const MOVE_VELOCITY = 40;

    var _myVelocity = new Vector2f(0,0);


    // gets called when the last cursor has been removed
    Base.onRemoveFirst = Public.onRemoveFirst;
    Public.onRemoveFirst = function( theEvent ) {
        if (_myMouseButtonPressed) {
            print( "mouse button released" );
            _mySynergyServer.onMouseButton( 1, false );
            _myMouseButtonPressed = false;
        }
    }

    Base.onASSEvent = Public.onASSEvent;
    Public.onASSEvent = function( theEvent ) {
        Base.onASSEvent( theEvent );

        if ( theEvent.type == "add") {
            if (Protected.firstEvents.length == 1) {
                Protected.mousePosition = Protected.getMousePos( theEvent.raw_position );
                _mySynergyServer.onMouseMotion( Protected.mousePosition.x,
                                                Protected.mousePosition.y );
                _mySynergyServer.onMouseButton( 1, true );
                print( "mouse button pressed" );
                Protected.targetPosition = null;
                _myMouseButtonPressed = true;
            }
        }
        else if ( theEvent.type == "move" ) {

            if (Protected.singleCursor( theEvent )) {

                Protected.targetPosition = Protected.getMousePos( theEvent.raw_position );

            }

            Protected.tryMouseWheel( theEvent );

        }
    }

}
