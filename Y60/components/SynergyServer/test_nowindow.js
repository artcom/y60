plug( "EventLoop" );
plug( "ASSEventSource" );
plug( "SynergyServer" );

var _mySynergyServer = new SynergyServer( "localhost", 24800 );

var myASSEventSource = new ASSEventSource();
var mySettings = new Node();
mySettings.parseFile( "settings.xml" );
myASSEventSource.onUpdateSettings( mySettings );

const MOVE_VELOCITY = 100; 
const RESIZE_VELOCITY = 20; 
const DAMPING_FACTOR = 1.3; 

var _myMouseMoveId = null;
var _myButtonPressedId = null;
var _myTargetPosition = null;
var _myVelocity = new Vector2f(0,0);
var _myMousePosition = new Vector2f( 0, 0 );


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
        _mySynergyServer.onMouseMotion( _myMousePosition.x, 
                                        _myMousePosition.y );

    }

}


function onASSEvent ( theEvent ) {

    if ( theEvent.type == "add") {
        if (_myMouseMoveId == null) {
            _myMouseMoveId = theEvent.id;
            _myTargetPosition = null;
            var myCurrentMousePosition = new Vector2f( theEvent.position3D.x,
                                                       theEvent.position3D.y );
            if (distance( _myMousePosition, myCurrentMousePosition ) < 100) {
                _myMousePosition = myCurrentMousePosition;
            }
            _myVelocity = new Vector2f( 0, 0 );
        } else if (_myButtonPressedId == null) {
            _myButtonPressedId = theEvent.id;
            _mySynergyServer.onMouseButton( 1, true );
        }
    } else if ( theEvent.type == "remove") {
        if (_myMouseMoveId == theEvent.id) {
            _myMouseMoveId = null;
        } else if (_myButtonPressedId == theEvent.id) {
            _myButtonPressedId = null;
            _mySynergyServer.onMouseButton( 1, false );
        }
    } else if ( theEvent.type == "move" ) {
        if (_myMouseMoveId == theEvent.id) {
            _myTargetPosition = new Vector2f( theEvent.position3D.x, 
                                              theEvent.position3D.y );
        }
    }

}


var myEventLoop = new EventLoop();
myEventLoop.go(this);

