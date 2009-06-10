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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

const MOVIE_DEMO = false;

if (__main__ == undefined) var __main__ = "WiimoteTest";

use("SceneViewer.js");
plug("FFMpegDecoder1");

window = new RenderWindow();

function WiiRemote(theId, theApp, theMasterWii) {
    this.Constructor(this, theId, theApp, theMasterWii);
}



WiiRemote.prototype.Constructor = function(self, theId, theApp, theMasterWii) {
    var _myBody = null;
    var _myCursor = null;
    var _myLowpassedUpVector = new Vector3f(0, 1, 0);
    var _myPickedBody = null;
    var _myRumbleWii = null;

    var _myApp = theApp;
    var _myDriver = theApp.getWiimoteDriver();
    setup();

    function setup() {
        _myBody = theMasterWii.cloneNode();
        _myBody.id = theId;
        window.scene.world.appendChild( _myBody );
        _myBody.visible = true;
        _myBody.insensible = false;

        _myCursor = new ImageOverlay(window.scene, "tex/pointer.png", [0,0], window.scene.overlays);
        _myCursor.visible = false;
    }
    self.finalize = function() {
        window.scene.world.removeChild( _myBody );
        _myCursor.removeFromScene();
    }
    self.handleEvent = function( theEvent ) {
        switch (theEvent.type) {
            case "motiondata":
                var myUpVector = normalized( new Vector3f( theEvent.motiondata.x, theEvent.motiondata.y,
                        theEvent.motiondata.z ));
                _myLowpassedUpVector = normalized( sum( product( _myLowpassedUpVector, 0.9),
                                          product( myUpVector, 0.1) ) );
                _myBody.orientation = new Quaternionf( _myLowpassedUpVector, new Vector3f(0, 1, 0) );
                break;
            case "found_ir_cursor":
                _myCursor.visible = true;
                break;
            case "lost_ir_cursor":
                _myCursor.visible = false;
                break;
            case "infrareddata":
                var myPosition = new Vector2f(window.width - ((theEvent.screenposition.x *
                        window.width/2.0) + window.width/2.0),
                        (theEvent.screenposition.y * window.height/2.0) + window.height/2.0 );
                _myCursor.position = myPosition;
                if ( _myPickedBody ) {
                    _myPickedBody.position = _myApp.pickPointOnPlane( myPosition );
                }
                break;
            case "button":
                handleButton( theEvent );
                break;
        }
    }

    self.__defineSetter__('position', function( thePos ) {
        _myBody.position = thePos;
    });
    self.__defineSetter__('rumble', function( theFlag ) {
        _myDriver.setRumble(_myBody.id, theFlag);
    });
    self.__defineGetter__('id', function() {
        return _myBody.id;
    });

    function handleButton( theEvent ) {
        print("button " + theEvent.buttonname + (theEvent.pressed ? " pressed " : " released ")
                + " on Wii " + theEvent.id );
        if (theEvent.buttonname == "A") {
            if (_myCursor.visible && theEvent.pressed) {
                var myBody = _myApp.pickBody( _myCursor.position );
                if (myBody) {
                    _myRumbleWii = _myApp.getWiiById( myBody.id );
                    if (_myRumbleWii) {
                        _myRumbleWii.rumble = true;
                    }
                }
            } else if ( ! theEvent.pressed ) {
                if (_myRumbleWii) {
                    _myRumbleWii.rumble = false;
                    _myRumbleWii = null;
                }
            }
        }
        if (theEvent.buttonname == "B") {
            if (_myCursor.visible && theEvent.pressed) {
                _myPickedBody = _myApp.pickBody( _myCursor.position );
            } else if ( ! theEvent.pressed ) {
                _myPickedBody = null;
            }
        }
    }

}

function WiimoteTestApp(theArguments) {
    this.Constructor(this, theArguments);
}



WiimoteTestApp.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];

    const MOVIE_WIDTH = 1;
    const MOVIE_HEIGHT = 0.66;
    
    var _myMasterWii = null;
    var _myWiiCount = 0;
    var _myLowPassedDownVector = new Vector3f( 0, 1, 0 );
    var _myOrientationVector = null;
    var _myAngleVector = null;
    var _myCrosshair = null;

    var _myPicking = null;
    var _myLastPosition = null;;
    var _myPickedBody = null;
    var _myPlane = null;

    var _myMovieMaterial = new Array(2);
    var _myMovieImage = new Array(2);
    var _myMovieQuad = new Array(2);
    var _myMovieBody = new Array(2);
    var _myMovieControlFlag = false;
    var _myCurrentMovieImage = null;
    
    var _myCurrentCursorOwner = null;

    var _myWiimoteDriver = null;
    var _myWiimotes = {};

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////
   
    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, false, theTitle);
        //window.position = [0, 0];
        window.decorations = false;
        window.resize(theWidth, theHeight);
        _myWiimoteDriver = plug("y60Wiimote");


        _myMasterWii = getDescendantByAttribute(window.scene.world, "name", "wii_controller");
        _myMasterWii.scale = new Vector3f(5, 5, 5);
        _myMasterWii.visible = false;
        _myMasterWii.insensible = true;


        /*
        var myClone = _myBody.cloneNode(true);
        createNewIds(myClone);
        myClone.position.x += 2;
        myClone.name = "clone";
        window.scene.world.appendChild(myClone);
        
        _myOrientationVector = Node.createElement("vector");
        window.scene.world.appendChild( _myOrientationVector );
        _myOrientationVector.color = new Vector4f(1,1,1,1);
        _myOrientationVector.scale = [1,1,1];
        */
        
        _myPlane = new Planef(new Vector3f(0,0,1), 0);
        
        //_myCrosshair = new ImageOverlay(window.scene, "tex/pointer.png", [0,0], window.scene.overlays);
        //_myCrosshair.position = [window.width/2.0, window.height/2.0];

        _myPicking = new Picking(window);

        if (MOVIE_DEMO) {
            for( var i=0; i<2; i++) {
                _myMovieImage[i] = self.getImageManager().createMovie("../../../../testmodels/movies/testshot.mpg", "y60FFMpegDecoder1");
                _myMovieImage[i].name = "MovieTexture" + i;
                _myMovieImage[i].src = "testshot.mpg";

                _myMovieImage[i].playmode = "play";
                _myMovieImage[i].audio = false;
                _myMovieImage[i].loopcount = 0;

                _myMovieMaterial[i] = Modelling.createUnlitTexturedMaterial(window.scene, _myMovieImage[i]);  
                _myMovieQuad[i] = Modelling.createQuad(window.scene, _myMovieMaterial[i].id, [-MOVIE_WIDTH/2,-MOVIE_HEIGHT/2,0], 
                        [MOVIE_WIDTH/2,MOVIE_HEIGHT/2,0]);
                _myMovieBody[i] = Modelling.createBody(window.scene.world, _myMovieQuad[i].id);
                _myMovieBody[i].name = "movie"+i;
                _myMovieBody[i].position.y = 1;
                _myMovieBody[i].position.x = -.7 + i*1.2;
            }
        }

        
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

        //_myMasterWii.orientation.assignFromEuler( new Vector3f( 0, 0.1 * theTime, 0));
    }

    Base.onPostRender = self.onPostRender;
    self.onPostRender = function() {
        Base.onPostRender();
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function( theButton, theState, theX, theY ) {
        Base.onMouseButton( theButton, theState, theX, theY);
        print( "mouse pos: " + theX + " " + theY);
    }

    self.getWiimoteDriver = function() {
        return _myWiimoteDriver;
    }


    function updateTargetPositions() {
        if ( _myWiiCount > 0) {
            var mySpacePerWii = 1;
            var myStartPos = mySpacePerWii * (_myWiiCount - 1) * -0.5;
            var i = 0;
            for (var myWii in _myWiimotes) {
                _myWiimotes[myWii].position = new Vector3f( myStartPos + i * mySpacePerWii, 0, 0 );
                i++;
            }
        }
    }

    self.onWiiEvent = function( theEvent ) {
        //print(theEvent);

        if (theEvent.type == "button" && theEvent.buttonname == "Home" && theEvent.pressed == 0) {
            print("Got quit from controller " + theEvent.id);
            print("Going home ... good bye!");
            exit( 0 );
        }

        switch (theEvent.type) {
            case "found_wii":
                _myWiiCount += 1;
                _myWiimotes[ theEvent.id ] = new WiiRemote( theEvent.id, self, _myMasterWii );
                updateTargetPositions();
                break;
            case "lost_connection":
                _myWiiCount -= 1;
                _myWiimotes[ theEvent.id ].finalize();
                delete _myWiimotes[ theEvent.id ];
                updateTargetPositions();
                break;
            default:
                if ( theEvent.id in _myWiimotes ) {
                    _myWiimotes[ theEvent.id ].handleEvent( theEvent );
                } else {
                    Logger.error("Received event for unknown Wii remote.");
                }
        }

        return;

/*
        if('screenposition' in theEvent && _myPickedBody) {
            _myPickedBody.position = _myPicking.pickPointOnPlane(_myLastPosition.x, _myLastPosition.y, _myPlane);
        }
        
        if (theEvent.type == "button" && theEvent.buttonname == "B" && theEvent.pressed == 1
            && theEvent.id == _myCurrentCursorOwner)
        {
            if(!_myPickedBody) {
                _myPickedBody = _myPicking.pickBody(_myLastPosition.x, _myLastPosition.y);
                if(_myPickedBody) {
                    for(var i=0; i<2; i++) {
                        if(_myPickedBody.name == ("movie"+i)) {
                            _myCurrentMovieImage = _myMovieImage[i];
                        }
                    }
                }
            } 
            print("Picked body: " + _myPickedBody);
        }
        if (theEvent.type == "button" && theEvent.buttonname == "B" && theEvent.pressed == 0) {
            _myPickedBody = null;
        }
        
        if (theEvent.type == "button" && theEvent.buttonname == "A") {
            _myWiimoteDriver.setRumble( theEvent.id, theEvent.pressed );
        }

        if (theEvent.type == "button" && theEvent.buttonname == "+" && theEvent.pressed == 0) {
            _myWiimoteDriver.requestStatusReport( theEvent.id );
        }
        
        if (theEvent.type == "motiondata") {
            //print(theEvent);
            
            var myDownVector = new Vector3f( theEvent.motiondata.x, theEvent.motiondata.y,
                                             theEvent.motiondata.z );


            myDownVector = normalized( myDownVector );


            _myLowPassedDownVector = normalized( sum( product( _myLowPassedDownVector, 0.9),
                                          product( myDownVector, 0.1) ) );


            //_myOrientationVector.value = _myLowPassedDownVector;

            //print("down: " + _myLowPassedDownVector + " magnitude: " + magnitude( _myLowPassedDownVector ));
            //_myMasterWii.orientation = new Quaternionf( _myLowPassedDownVector, new Vector3f(0, 1, 0) );

            if(MOVIE_DEMO && _myMovieControlFlag && _myCurrentMovieImage) {
                if( _myLowPassedDownVector.z > 0 ) {
                    _myCurrentMovieImage.playspeed = 1 + _myLowPassedDownVector.z;
                } else {
                    _myCurrentMovieImage.playspeed = -1 + _myLowPassedDownVector.z;
                }
                //print("playspeed" + _myMovieImage.playspeed);
            }

        }

        if (theEvent.type == "lost_connection") {
            print("Wii controller with id " + theEvent.id + " is gone.");
        }

        if( theEvent.type == "status") {
            print("============ STATUS =============");
            print("Battery level      : " + theEvent.battery_level);
            print("Extension connected: " + theEvent.extension);
            print("Speaker enabled    : " + theEvent.speaker_enabled);
            print("Continous reporting: " + theEvent.continous_reports);
            print("LED 1              : " + theEvent.led0);
            print("LED 2              : " + theEvent.led1);
            print("LED 3              : " + theEvent.led2);
            print("LED 4              : " + theEvent.led3);
        }

        if (theEvent.type == "infrareddata" && theEvent.id == _myCurrentCursorOwner) {
            
            var myPosition = new Vector2f(window.width - ((theEvent.screenposition.x * window.width/2.0) + window.width/2.0),
                                          (theEvent.screenposition.y * window.height/2.0) + window.height/2.0 );
            _myCrosshair.position = myPosition;
            _myLastPosition = myPosition;
        }

        if (theEvent.type == "lost_ir_cursor") {
            _myCrosshair.visible = false;
            _myCurrentCursorOwner = null;
        }
        if (theEvent.type == "found_ir_cursor") {
            if (_myCurrentCursorOwner == null) {
                _myCurrentCursorOwner = theEvent.id;
                _myCrosshair.visible = true;
            }
        }

        if( theNode.type == "found_wii") {
        }
        
        // media system control :)
        if (MOVIE_DEMO && theEvent.type == "button" && theEvent.buttonname == "1" && theEvent.pressed == 1) {
            if( _myCurrentMovieImage.playmode == "pause" ){
                _myCurrentMovieImage.playmode = "play";
            } else {
                _myCurrentMovieImage.playmode = "pause";
            }
        }
        if (MOVIE_DEMO && theEvent.type == "button" && theEvent.buttonname == "2" && theEvent.pressed == 1) {
            _myMovieControlFlag = !_myMovieControlFlag;
            if(_myCurrentMovieImage) {
                _myCurrentMovieImage.playspeed = 1.0;
            }
        }
        
        */
    }
    
    self.getWiimoteDriver = function() {
        return _myWiimoteDriver;
    }

    self.pickBody = function ( thePosition ) {
        return _myPicking.pickBody( thePosition.x, thePosition.y);
    }
    self.pickPointOnPlane = function ( thePosition ) {
        return _myPicking.pickPointOnPlane(thePosition.x, thePosition.y, _myPlane);
    }
    self.getWiiById = function ( theId ) {
        return _myWiimotes[ theId ];
    }
    ///////////////////////////////////////////////////////
    // private funtions 
    ///////////////////////////////////////////////////////

    function createNewIds(theNode) {
        theNode.id = createUniqueId();
        for (var i = 0; i < theNode.childNodesLength(); ++i) {
            createNewIds(theNode.childNode(i));
        }
    }
}


if (__main__ == "WiimoteTest") {
    try {
        var ourWiimoteTestApp = new WiimoteTestApp(arguments);
        //var ourWiimoteTestApp = new WiimoteTestApp(
        //    [expandEnvironment("${PRO}") + "/src/y60/shader/shaderlibrary_nocg.xml"]);
        //ourWiimoteTestApp.setup(600, 600, "WiimoteTest");
        ourWiimoteTestApp.setup(1400, 1050, "Wiimote");
        ourWiimoteTestApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}




