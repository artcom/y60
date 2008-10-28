plug( "EventLoop" );
plug( "ASSEventSource" );
plug( "SynergyServer" );

var _mySynergyServer = new SynergyServer( "localhost", 24800 );

var myASSEventSource = new ASSEventSource();
var mySettings = new Node();
mySettings.parseFile( "settings.xml" );
myASSEventSource.onUpdateSettings( mySettings );

// wait until we have a connection with the synergy client
while (!_mySynergyServer.isConnected()) {
    ;
}


const CLICK_THRESHOLD = 20;
var _myMouseButtonPressed = false;

const DIRECTION_UP = -1;
const DIRECTION_DOWN = 1;
const DIRECTION_NONE = 0;

const MOVE_VELOCITY = 40; 

const POSITION_OFFSET = [0,140];
const SCREENSIZE_OFFSET = [0, -280];
//const POSITION_OFFSET = [0,0];
//const SCREENSIZE_OFFSET = [0, 0];
var _myScreenSize = _mySynergyServer.getScreenSize();

var _myFirstEvents = [];
var _mySecondEvents = [];

var _myTargetPosition = null;
var _myVelocity = new Vector2f(0,0);
var _myMousePosition = new Vector2f( 0, 0 );
var _myGridSize = new Vector2f( 1, 1 );
var _myMirror = new Vector2i(0,0);

var myMirrorXNode = mySettings.find( "//MirrorX" );
if (myMirrorXNode) {
    _myMirror.x = myMirrorXNode.childNode("#text").nodeValue;
}

var myMirrorYNode = mySettings.find( "//MirrorY" );
if (myMirrorYNode) {
    _myMirror.y = myMirrorYNode.childNode("#text").nodeValue;
}


function onFrame ( theTime ) {

    if (_myTargetPosition) {

        var myPosition = product( sum( product( _myMousePosition, 
                                                MOVE_VELOCITY), 
                                       _myTargetPosition), 
                                  1 / (MOVE_VELOCITY + 1)); 
        _myMousePosition = myPosition; 
        if (distance( _myMousePosition, _myTargetPosition ) < 0.1) {
            _myTargetPosition = null;
        }
        _mySynergyServer.onMouseMotion( _myMousePosition.x + POSITION_OFFSET[0], 
                                        _myMousePosition.y + POSITION_OFFSET[1] );

    }

}

function getMousePos( theRawPosition ) {
    var myRawX = theRawPosition.x;
    if (_myMirror.x > 0) {
        myRawX = _myGridSize.x - theRawPosition.x;
    }
    var myX = myRawX / _myGridSize.x 
              * (_myScreenSize.x + SCREENSIZE_OFFSET[0]);
              
    var myRawY = theRawPosition.y;
    if (_myMirror.y > 0) {
        myRawY = _myGridSize.y - theRawPosition.y;
    }
              
    var myY = myRawY / _myGridSize.y 
              * (_myScreenSize.y + SCREENSIZE_OFFSET[1]);

    return new Vector2f( myX, myY );
}


function onASSEvent ( theEvent ) {

    if ( theEvent.type == "add") {
        if (_myFirstEvents.length == 0) {
            _myFirstEvents.push(theEvent);
            _myTargetPosition = null;

            var myCurrentPos = getMousePos( theEvent.raw_position );
            if (distance( myCurrentPos, _myMousePosition ) < CLICK_THRESHOLD) {
                _mySynergyServer.onMouseButton( 1, true );
                _myMouseButtonPressed = true;
            } else {
                _myMousePosition = myCurrentPos; 
                _mySynergyServer.onMouseMotion( _myMousePosition.x 
                                                + POSITION_OFFSET[0], 
                                                _myMousePosition.y 
                                                + POSITION_OFFSET[1] );
            }
        }
        else if (_mySecondEvents.length == 0) {
            _mySecondEvents.push(theEvent);
        }
    } else if ( theEvent.type == "remove") {
        // ending our mouse movement
        if (_myFirstEvents.length > 0 && _myFirstEvents[0].id == theEvent.id) {
            _myFirstEvents = [];
            if (_myMouseButtonPressed) {
                _mySynergyServer.onMouseButton( 1, false );
                _myMouseButtonPressed = false;
            }
            if (_mySecondEvents.length > 0) {
                _myFirstEvents = _mySecondEvents;
                _mySecondEvents = [];
            }
        }
        else if (_mySecondEvents.length > 0 && _mySecondEvents[0].id == theEvent.id) {
            _mySecondEvents = [];
        }
    } else if ( theEvent.type == "move" ) {

        if (_mySecondEvents.length == 0 && _myFirstEvents.length > 0 
            && _myFirstEvents[0].id == theEvent.id) 
        {
            _myTargetPosition = getMousePos( theEvent.raw_position );
            return;
        }

        if (_myFirstEvents.length > 0 && _mySecondEvents.length > 0) {

            if (_myFirstEvents[0].id == theEvent.id) {
                pushEvent( _myFirstEvents, theEvent );
            } else if (_mySecondEvents[0].id == theEvent.id) {
                pushEvent( _mySecondEvents, theEvent );
            }

            if (_myFirstEvents.length == 5 && _mySecondEvents.length == 5) {
                var myDirection = getDirection( _myFirstEvents );
                if (myDirection == getDirection( _mySecondEvents )) {
                    _mySynergyServer.onMouseWheel( 0, myDirection );
                }
            }
        }

    } else if ( theEvent.type == "configure" ) {
        _myGridSize = theEvent.grid_size;
    }

}

function pushEvent( theEventQueue, theEvent ) {
    theEventQueue.push( theEvent );
    if (theEventQueue.length > 5) {
        theEventQueue.shift();
    }
}

function getDirection( theEventQueue ) {

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


var myEventLoop = new EventLoop();
myEventLoop.go(this);

