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
