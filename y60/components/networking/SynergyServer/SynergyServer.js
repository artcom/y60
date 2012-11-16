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

plug("EventLoop");
plug("SynergyServer");

var myASSEventSource = plug("ASSEventSource");

myASSEventSource.disconnect();

const DEFAULT_MODE = 0;
const CLICKONPOINT_MODE = 1;
const RELATIVE_MODE = 2;

var myHost = "localhost";
var myPort = 24800;
var myXOffset = 0;
var myYOffset = 0;
var myWidthOffset = 0;
var myHeightOffset = 0;

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
    } else if (myArgumentStrings[0] == "xoffset") {
        myXOffset = Number( myArgumentStrings[1] );
        print( "XOffset:", myXOffset );
    } else if (myArgumentStrings[0] == "yoffset") {
        myYOffset = Number( myArgumentStrings[1] );
        print( "YOffset:", myYOffset );
    } else if (myArgumentStrings[0] == "widthoffset") {
        myWidthOffset = Number( myArgumentStrings[1] );
        print( "WidthOffset:", myWidthOffset );
    } else if (myArgumentStrings[0] == "heightoffset") {
        myHeightOffset = Number( myArgumentStrings[1] );
        print( "HeightOffset:", myHeightOffset );
    }

}


var mySynergyServer = new SynergyServer( myHost, myPort );

var mySettings = new Node();
mySettings.parseFile( "settings.xml" );
myASSEventSource.onUpdateSettings( mySettings );

// wait until we have a connection with the synergy client
while (!mySynergyServer.isConnected()) {
    msleep(250);
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

myEventHandler.positionOffset = new Vector2f( myXOffset, myYOffset );
myEventHandler.sizeOffset = new Vector2f( myWidthOffset, myHeightOffset );

function onFrame( theTime ) {
    myEventHandler.onFrame( theTime );
}

function onASSEvent( theEvent ) {
    myEventHandler.onASSEvent( theEvent );
}


var myEventLoop = new y60EventLoop();
myASSEventSource.connect();
myEventLoop.go(this);


