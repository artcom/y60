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
