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

function RelativeMouseHandler( theSynergyServer, theSettings ) {
    this.Constructor( this, theSynergyServer, theSettings );
}

RelativeMouseHandler.prototype.Constructor = function( Public, theSynergyServer,
                                                       theSettings )
{

    var Protected = [];
    SynergyEventHandler.prototype.Constructor( Public, Protected, theSynergyServer,
                                               theSettings );
    var Base = [];

    var _mySynergyServer = theSynergyServer;

    var _myMouseButtonPressed = false;
    var _myLastRemoveEvent = null;

    const MOVE_VELOCITY = 40;
    const CLICK_THRESHOLD = 80;

    var _myVelocity = new Vector2f(0,0);

    Base.onRemoveFirst = Public.onRemoveFirst;
    Public.onRemoveFirst = function( theEvent ) {

        _myLastRemoveEvent = theEvent;

        if (_myMouseButtonPressed) {
            _mySynergyServer.onMouseButton( 1, false );
            _myMouseButtonPressed = false;
            _myLastRemoveEvent = null;
        }

    }


    Base.onASSEvent = Public.onASSEvent;
    Public.onASSEvent = function( theEvent ) {
        Base.onASSEvent( theEvent );

        if ( theEvent.type == "add") {

            if (_myLastRemoveEvent) {
                var myOldPos = Protected.getMousePos( _myLastRemoveEvent.raw_position );
                var myCurrentPos = Protected.getMousePos( theEvent.raw_position );

                if (distance( myCurrentPos, myOldPos ) < CLICK_THRESHOLD) {
                    print( "pressing mousebutton..." );
                    _mySynergyServer.onMouseButton( 1, true );
                    _myMouseButtonPressed = true;
                }
            }

        }
        else if ( theEvent.type == "remove") {
            // ending our mouse movement
        }
        else if ( theEvent.type == "move" ) {

            if (Protected.singleCursor( theEvent )) {

                var myAvgDirection =
                    Protected.getAvgDirectionVector( Protected.firstEvents );

                var myDistance = Protected.getMoveDistance( Protected.firstEvents );

                var myMoveIncrement = product( normalized( myAvgDirection ), myDistance );

                Protected.targetPosition = sum( Protected.mousePosition, myMoveIncrement );

                if (Protected.targetPosition.x < 0) {
                    Protected.targetPosition.x = 0;
                } else if (Protected.targetPosition.x > Protected.screenSize.x) {
                    Protected.targetPosition.x = Protected.screenSize.x;
                }
                if (Protected.targetPosition.y < 0) {
                    Protected.targetPosition.y = 0;
                } else if (Protected.targetPosition.y > Protected.screenSize.y) {
                    Protected.targetPosition.y = Protected.screenSize.y;
                }
                return;

            }

            Protected.tryMouseWheel( theEvent );

        }
    }
}
