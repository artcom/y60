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
var _myLastDistance = 0;

const MOVE_VELOCITY = 40; 

const POSITION_OFFSET = [0,140];
const SCREENSIZE_OFFSET = [0, -280];
//const POSITION_OFFSET = [0,0];
//const SCREENSIZE_OFFSET = [0, 0];
var _myScreenSize = _mySynergyServer.getScreenSize();

var _myFirstCursor = null;
var _mySecondCursor = null;
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
        if (_myFirstCursor == null) {
            _myFirstCursor = theEvent;
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
        else if (_mySecondCursor == null) {
            _mySecondCursor = theEvent;
        }
        _myLastDistance = 0;
    } else if ( theEvent.type == "remove") {
        // ending our mouse movement
        if (_myFirstCursor && _myFirstCursor.id == theEvent.id) {
            _myFirstCursor = null;
            if (_myMouseButtonPressed) {
                _mySynergyServer.onMouseButton( 1, false );
                _myMouseButtonPressed = false;
            }
        }
        else if (_mySecondCursor && _mySecondCursor.id == theEvent.id) {
            _mySecondCursor = null;
        }
        _myLastDistance = 0;
    } else if ( theEvent.type == "move" ) {
        if (_myFirstCursor && _myFirstCursor.id == theEvent.id) {
            _myFirstCursor = theEvent;
        } else if (_mySecondCursor && _mySecondCursor.id == theEvent.id) {
            _mySecondCursor = theEvent;
        }
        if (_myFirstCursor) {
            if (_myFirstCursor.id == theEvent.id && _mySecondCursor == null) {
                _myTargetPosition = getMousePos( theEvent.raw_position );
            } else if (_myFirstCursor && _mySecondCursor) 
            {
                myDistance = distance( _mySecondCursor.raw_position,
                                       _myFirstCursor.raw_position );
                var myMotion = myDistance - _myLastDistance;
                if (myMotion > 1) {
                    print( "wheel down" );
                    _mySynergyServer.onMouseWheel( 0, 1 ); 
                } else if (myMotion < -1) {
                    print( "wheel up" );
                    _mySynergyServer.onMouseWheel( 0, -1 ); 
                }
                _myLastDistance = myDistance;
            }
        }
    } else if ( theEvent.type == "configure" ) {
        _myGridSize = theEvent.grid_size;
    }

}


var myEventLoop = new EventLoop();
myEventLoop.go(this);

