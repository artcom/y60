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
                _mySynergyServer.onMouseMotion( myMousePosition.x + POSITION_OFFSET[0], 
                                                myMousePosition.y + POSITION_OFFSET[1]);
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
