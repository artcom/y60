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

//const POSITION_OFFSET = [0,140];
//const SCREENSIZE_OFFSET = [0, -280];
const EVENT_QUEUE_SIZE = 10;
const DIRECTION_MINIMUM = 100;

const DIRECTION_UP = -1;
const DIRECTION_DOWN = 1;
const DIRECTION_NONE = 0;

const MOVE_VELOCITY = 40;

function SynergyEventHandler( theSynergyServer, theSettings ) {
    Protected = [];
    this.Constructor( this, Protected, theSynergyServer, theSettings );
}

SynergyEventHandler.prototype.Constructor = function( Public, Protected, theSynergyServer,
                                                      theSettings )
{

    var _myFirstEvents = [];
    var _mySecondEvents = [];

    var _mySettings = theSettings;
    var _mySynergyServer = theSynergyServer;

    var _myScreenSize = _mySynergyServer.getScreenSize();
    var _myGridSize = new Vector2f( 1, 1 );
    var _myMirror = new Vector2i(0,0);

    var _myPositionOffset = new Vector2f( 0, 0 );
    var _mySizeOffset = new Vector2f( 0, 0 );

    var myMirrorXNode = _mySettings.find( "//MirrorX" );
    if (myMirrorXNode) {
        _myMirror.x = myMirrorXNode.childNode("#text").nodeValue;
    }

    var myMirrorYNode = _mySettings.find( "//MirrorY" );
    if (myMirrorYNode) {
        _myMirror.y = myMirrorYNode.childNode("#text").nodeValue;
    }

    var _myMousePosition = new Vector2f(0,0);
    var _myTargetPosition = null;


    // TODO: implement onAddFirst, onMoveFirst, onAddSecond, onMoveSecond ...
    Public.onRemoveFirst = function( theEvent ) {}
    Public.onRemoveSecond = function( theEvent ) {}

    Public.onFrame = function( theTime ) {

        if (_myTargetPosition) {

            var myPosition = product( sum( product( _myMousePosition,
                                                    MOVE_VELOCITY),
                                           _myTargetPosition),
                                      1 / (MOVE_VELOCITY + 1));
            _myMousePosition = myPosition;
            if (distance( _myMousePosition, _myTargetPosition ) < 0.1) {
                _myTargetPosition = null;
            }
            _mySynergyServer.onMouseMotion( _myMousePosition.x + _myPositionOffset[0],
                                            _myMousePosition.y + _myPositionOffset[1] );

        }

    }


    Public.onASSEvent = function( theEvent ) {

        if (theEvent.type == "add") {
            if (_myFirstEvents.length == 0) {
                _myFirstEvents.push(theEvent);
            }
            else if (_mySecondEvents.length == 0) {
                _mySecondEvents.push(theEvent);
            }
        }
        else if (theEvent.type == "remove") {
            if (_myFirstEvents.length > 0
                && _myFirstEvents[0].id == theEvent.id)
            {
                _myFirstEvents = [];

                Public.onRemoveFirst( theEvent );
                if (_mySecondEvents.length > 0) {
                    _myFirstEvents = _mySecondEvents;
                    _mySecondEvents = [];
                }
            }
            else if (_mySecondEvents.length > 0
                     && _mySecondEvents[0].id == theEvent.id)
            {
                _mySecondEvents = [];
                Public.onRemoveSecond( theEvent );
            }

        }
        else if (theEvent.type == "move") {
            if (_myFirstEvents.length > 0
                && _myFirstEvents[0].id == theEvent.id)
            {
                Protected.pushEvent( _myFirstEvents, theEvent );
            } else if (_mySecondEvents.length > 0
                       && _mySecondEvents[0].id == theEvent.id)
            {
                Protected.pushEvent( _mySecondEvents, theEvent );
            }
        }
        else if ( theEvent.type == "configure" ) {
            _myGridSize = theEvent.grid_size;
        }
    }

    Protected.targetPosition getter = function() {
        return _myTargetPosition;
    }

    Protected.targetPosition setter = function( theTargetPosition ) {
        _myTargetPosition = theTargetPosition;
    }

    Protected.mousePosition getter = function() {
        return _myMousePosition;
    }

    Protected.mousePosition setter = function( theMousePosition ) {
        _myMousePosition = theMousePosition;
    }

    Protected.firstEvents getter = function() {
        return _myFirstEvents;
    }

    Protected.firstEvents setter = function( theEventQueue ) {
        _myFirstEvents = theEventQueue;
    }

    Protected.secondEvents getter = function() {
        return _mySecondEvents;
    }

    Protected.secondEvents setter = function( theEventQueue ) {
        _mySecondEvents = theEventQueue;
    }

    Protected.screenSize getter = function() {
        return _myScreenSize;
    }

    Protected.positionOffset setter = function( thePositionOffset ) {
        _myPositionOffset = thePositionOffset;
    }

    Protected.positionOffset getter = function() {
        return _myPositionOffset;
    }

    Protected.sizeOffset setter = function( theSizeOffset ) {
        _mySizeOffset = theSizeOffset;
    }

    Protected.getMousePos = function( theRawPosition ) {

        //print( "SynergyEventHandler.getMousePos(",theRawPosition,")");

        var myRawX = theRawPosition.x;
        if (_myMirror.x > 0) {
            myRawX = _myGridSize.x - theRawPosition.x;
        }
        var myX = myRawX / _myGridSize.x
                  * (_myScreenSize.x + _mySizeOffset[0]);

        var myRawY = theRawPosition.y;
        if (_myMirror.y > 0) {
            myRawY = _myGridSize.y - theRawPosition.y;
        }

        var myY = myRawY / _myGridSize.y
                  * (_myScreenSize.y + _mySizeOffset[1]);
        var myMousePosition = new Vector2f( myX, myY );

        //print( "SynergyEventHandler.getMousePos: myMousePosition =", myMousePosition );

        return myMousePosition;
    }

    Protected.outOfBounds = function( thePosition ) {
        return ( thePosition.x < 0 || thePosition.x > _myScreenSize.x ||
                 thePosition.y < 0 || thePosition.y > _myScreenSize.y );
    }

    Protected.singleCursor = function( theEvent ) {
        return ( _mySecondEvents.length == 0 && _myFirstEvents.length > 0
                 && _myFirstEvents[0].id == theEvent.id );
    }

    Protected.dualCursor = function( theEvent ) {
        return (_myFirstEvents.length > 0 && _mySecondEvents.length > 0
                && (_myFirstEvents[0].id == theEvent.id
                    || _mySecondEvents[0].id == theEvent.id));
    }

    Protected.getDirectionVector = function( theEventQueue ) {

        if (theEventQueue.length < 2) {
            return new Vector2f( 0, 0 );
        }

        var myLastEvent = theEventQueue[theEventQueue.length-1];
        var myFirstEvent = theEventQueue[theEventQueue.length-2];

        var myFirstPos = Protected.getMousePos( myLastEvent.raw_position );
        var mySecondPos = Protected.getMousePos( myFirstEvent.raw_position );
        myDifference = difference( myFirstPos, mySecondPos );

        return myDifference;

    }

    Protected.getAvgDirectionVector = function( theEventQueue ) {

        var myAvgDirection = new Vector2f( 0, 0 );
        for (var i = 0; i < theEventQueue.length-1; i++) {
            var myFirstPos = Protected.getMousePos( theEventQueue[i].raw_position );
            var mySecondPos = Protected.getMousePos( theEventQueue[i+1].raw_position );
            myAvgDirection = sum( myAvgDirection, difference( mySecondPos, myFirstPos ) );
        }
        return product( myAvgDirection, 1/theEventQueue.length );

    }

    Protected.getMoveDistance = function( theEventQueue ) {

        var myDistance = 0;
        var myFirstPos = Protected.getMousePos( theEventQueue[0].raw_position );

        for (var i = 1; i < theEventQueue.length; i++) {
            myDistance += distance( Protected.getMousePos( theEventQueue[i].raw_position ),
                                    myFirstPos );
        }
        myDistance /= theEventQueue.length - 1;
        return myDistance;

    }

    Protected.pushEvent = function( theEventQueue, theEvent ) {
        theEventQueue.push( theEvent );
        if (theEventQueue.length > EVENT_QUEUE_SIZE) {
            theEventQueue.shift();
        }
    }

    Protected.getDirection = function( theEventQueue ) {

        if (theEventQueue.length == 0) {
            return DIRECTION_NONE;
        }

        var myLastEvent = theEventQueue[theEventQueue.length-1];
        var myFirstEvent = theEventQueue[0];

        var myDifference = difference( myLastEvent.raw_position, myFirstEvent.raw_position );

        var myDirection = dot( myDifference, new Vector2f( 0, 1 ) );

        if (myDirection != 0) {
            return (myDirection / Math.abs( myDirection));
        } else {
            return DIRECTION_NONE;
        }

    }

    Protected.tryMouseWheel = function( theEvent ) {
        if (Protected.dualCursor( theEvent )) {
            Protected.targetPosition = null;
            var myDirection = Protected.getDirection( Protected.firstEvents );
            var mySecondDirection = Protected.getDirection( Protected.secondEvents );
            if (myDirection != 0 && myDirection == mySecondDirection) {
                _mySynergyServer.onMouseWheel( 0, myDirection );
            }

        }
    }
}

