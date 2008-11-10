plug( "EventLoop" );
plug( "ASSEventSource" );
plug( "SynergyServer" );


var mySynergyServer = new SynergyServer( "localhost", 24800 );
var myASSEventSource = new ASSEventSource();

var mySettings = new Node();
mySettings.parseFile( "settings.xml" );
myASSEventSource.onUpdateSettings( mySettings );

// wait until we have a connection with the synergy client
while (!mySynergyServer.isConnected()) {
    ;
}

//use( "ClickOnPointHandler.js" );
//var myEventHandler = new ClickOnPointHandler( mySynergyServer, mySettings );

//use( "ClickWithSecondHand.js" );
//var myEventHandler = new ClickWidthSecondHand( mySynergyServer, mySettings );

use( "RelativeMouseHandler.js" );
var myEventHandler = new RelativeMouseHandler( mySynergyServer, mySettings );

function onFrame( theTime ) {
    myEventHandler.onFrame( theTime );
}

function onASSEvent( theEvent ) {
    myEventHandler.onASSEvent( theEvent ); 
}


var myEventLoop = new EventLoop();
myEventLoop.go(this);


