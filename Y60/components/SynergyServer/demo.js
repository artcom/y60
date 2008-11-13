plug( "EventLoop" );
plug( "ASSEventSource" );
plug( "SynergyServer" );

const DEFAULT_MODE = 0;
const CLICKONPOINT_MODE = 1;
const RELATIVE_MODE = 2;

var myHost = "localhost";
var myPort = 24800;

// parse arguments
var myMode = DEFAULT_MODE;
for (var i = 0; i < arguments.length; i++) {

    var myArgumentStrings = arguments[i].split('='); 
    if (myArgumentStrings[0] == "host") {
        myHost = myArgumentStrings[1];
        print( "Hostname:", myHost );
    } else if (myArgumentStrings[0] == "mode") {
        switch (myArgumentStrings[1]) {
            case "clickonpoint":
                myMode = CLICKONPOINT_MODE;
                break;
            case "relative":
                myMode = RELATIVE_MODE;
                break;
            default:
                myMode = DEFAULT_MODE;
                break;
        }
        print( "Mode:", myMode );
    } else if (myArgumentStrings[0] == "port") {
        myPort = Number( myArgumentStrings[1] );
        print( "Port:", myPort );
    }

}


var mySynergyServer = new SynergyServer( myHost, myPort );
var myASSEventSource = new ASSEventSource();

var mySettings = new Node();
mySettings.parseFile( "settings.xml" );
myASSEventSource.onUpdateSettings( mySettings );

// wait until we have a connection with the synergy client
while (!mySynergyServer.isConnected()) {
    ;
}



if (myMode == CLICKONPOINT_MODE) {

    // ClickOnPointHandler: 
    //      Hand positions the mouse cursor directly and on every touch ("add") event, 
    //      a Left-Button-Click is performed until the hand is removed again.
    //      Both hands moved up or down are interpreted as mousewheel.

    print( "Using click on point handler" );
    use( "ClickOnPointHandler.js" );
    var myEventHandler = new ClickOnPointHandler( mySynergyServer, mySettings );
} else if (myMode == RELATIVE_MODE) {

    // RelativeMouseHandler:
    //      hand movement moves the mouse cursor only a certain amount in move-direction.
    //      cursor clicks are done with a second touch on the same position where you lifted 
    //      your hand before.

    print( "Using relative mouse handler" );
    use( "RelativeMouseHandler.js" );
    var myEventHandler = new RelativeMouseHandler( mySynergyServer, mySettings );
} else {
    // ClickWithSecondHand:
    //      Hand positions the mouse cursor just as normal mouse motion. When you put your 
    //      other hand on the table you get a mouse click until you lift one or both hand 
    //      off the table.
    
    print( "Using standard event handler" );
    use( "ClickWithSecondHand.js" );
    var myEventHandler = new ClickWidthSecondHand( mySynergyServer, mySettings );
}

function onFrame( theTime ) {
    myEventHandler.onFrame( theTime );
}

function onASSEvent( theEvent ) {
    myEventHandler.onASSEvent( theEvent ); 
}


var myEventLoop = new EventLoop();
myEventLoop.go(this);


