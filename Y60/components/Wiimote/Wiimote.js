
//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

const MOVIE_DEMO = false;

if (__main__ == undefined) var __main__ = "WiimoteTest";

use("SceneViewer.js");
plug("y60FFMpegDecoder1");

window = new RenderWindow();

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
    
    var _myBody = null;
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
    
    var _myWiimote = null;

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
        _myWiimote = plug("Wiimote");


        _myBody = getDescendantByAttribute(window.scene.world, "name", "wii_controller");
        _myBody.scale = new Vector3f(5, 5,5);

        _myOrientationVector = Node.createElement("vector");
        window.scene.world.appendChild( _myOrientationVector );
        _myOrientationVector.color = new Vector4f(1,1,1,1);
        _myOrientationVector.scale = [1,1,1];
        
        _myPlane = new Planef(new Vector3f(0,0,1), 0);
        
        _myCrosshair = new ImageOverlay(window.scene, "tex/pointer.png", [0,0], window.scene.overlays);
        _myCrosshair.position = [window.width/2.0, window.height/2.0];

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

        //_myBody.orientation.assignFromEuler( new Vector3f( 0, 0.1 * theTime, 0));
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

    self.onWiiEvent = function( theNode ) {
        //print(theNode);
        if('screenposition' in theNode && _myPickedBody) {
            _myPickedBody.position = _myPicking.pickPointOnPlane(_myLastPosition.x, _myLastPosition.y, _myPlane);
        }
        
        if (theNode.type == "button" && theNode.buttonname == "Home" && theNode.pressed == 0) {
            print("Got quit from controller " + theNode.id);
            print("Going home ... good bye!");
            exit( 0 );
        }
        if (theNode.type == "button" && theNode.buttonname == "B" && theNode.pressed == 1) {
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
        if (theNode.type == "button" && theNode.buttonname == "B" && theNode.pressed == 0) {
            _myPickedBody = null;
        }
        
        if (theNode.type == "button" && theNode.buttonname == "A") {
            _myWiimote.setRumble( theNode.id, theNode.pressed );
        }

        if (theNode.type == "button" && theNode.buttonname == "+" && theNode.pressed == 0) {
            _myWiimote.requestStatusReport( theNode.id );
        }
        
        if (theNode.type == "motiondata") {
            //print(theNode);
            
            var myDownVector = new Vector3f( theNode.motiondata.x, theNode.motiondata.y,
                                             theNode.motiondata.z );


            myDownVector = normalized( myDownVector );


            _myLowPassedDownVector = normalized( sum( product( _myLowPassedDownVector, 0.9),
                                          product( myDownVector, 0.1) ) );


            _myOrientationVector.value = _myLowPassedDownVector;

            //print("down: " + _myLowPassedDownVector + " magnitude: " + magnitude( _myLowPassedDownVector ));
            _myBody.orientation = new Quaternionf( _myLowPassedDownVector, new Vector3f(0, 1, 0) );

            if(MOVIE_DEMO && _myMovieControlFlag && _myCurrentMovieImage) {
                if( _myLowPassedDownVector.z > 0 ) {
                    _myCurrentMovieImage.playspeed = 1 + _myLowPassedDownVector.z;
                } else {
                    _myCurrentMovieImage.playspeed = -1 + _myLowPassedDownVector.z;
                }
                //print("playspeed" + _myMovieImage.playspeed);
            }
        }

        if (theNode.type == "infrareddata") {
            
            var myPosition = new Vector2f(window.width - ((theNode.screenposition.x * window.width/2.0) + window.width/2.0),
                                          (theNode.screenposition.y * window.height/2.0) + window.height/2.0 );
            _myCrosshair.position = myPosition;
            _myLastPosition = myPosition;
        }

        // media system control :)
        if (MOVIE_DEMO && theNode.type == "button" && theNode.buttonname == "1" && theNode.pressed == 1) {
            if( _myCurrentMovieImage.playmode == "pause" ){
                _myCurrentMovieImage.playmode = "play";
            } else {
                _myCurrentMovieImage.playmode = "pause";
            }
        }
        if (MOVIE_DEMO && theNode.type == "button" && theNode.buttonname == "2" && theNode.pressed == 1) {
            _myMovieControlFlag = !_myMovieControlFlag;
            if(_myCurrentMovieImage) {
                _myCurrentMovieImage.playspeed = 1.0;
            }
        }
        
    }
    

    ///////////////////////////////////////////////////////
    // private funtions 
    ///////////////////////////////////////////////////////


}


if (__main__ == "WiimoteTest") {
    try {
        var ourWiimoteTestApp = new WiimoteTestApp(arguments);
        //var ourWiimoteTestApp = new WiimoteTestApp(
        //    [expandEnvironment("${PRO}") + "/src/Y60/shader/shaderlibrary_nocg.xml"]);
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



