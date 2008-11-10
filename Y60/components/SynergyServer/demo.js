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

// ClickOnPointHandler: 
//      Hand positions the mouse cursor directly and on every touch ("add") event, 
//      a Left-Button-Click is performed until the hand is removed again.
//      Both hands moved up or down are interpreted as mousewheel.

//use( "ClickOnPointHandler.js" );
//var myEventHandler = new ClickOnPointHandler( mySynergyServer, mySettings );

// ClickWithSecondHand:
//      Hand positions the mouse cursor just as normal mouse motion. When you put your other
//      hand on the table you get a mouse click until you lift one or both hand off the 
//      table.

//use( "ClickWithSecondHand.js" );
//var myEventHandler = new ClickWidthSecondHand( mySynergyServer, mySettings );

// RelativeMouseHandler:
//      hand movement moves the mouse cursor only a certain amount in move-direction.
//      cursor clicks are done with a second touch on the same position where you lifted 
//      your hand before.

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


