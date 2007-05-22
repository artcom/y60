//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

if (__main__ == undefined) var __main__ = "ASSDriverTest";

use("SceneViewer.js");
use("ASSManager.js");
use("Button.js");

const DISPLAY_SCALE = 20;
const X_MIRROR = false;
const Y_MIRROR = true;
const ORIENTATION = 0.5 * Math.PI;

var ourButtons = new Array();

window = new RenderWindow();

function ASSDriverTestApp(theArguments) {
    this.Constructor(this, theArguments);
}

ASSDriverTestApp.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];
    var _myLastEventTime = 0;
    var _myASSManager = null;
    var _myDummyAppContainer = null;

    var _myWorldCross = null;
    
    var _mySmallButton = null;
    var _myMediumButton = null;
    var _myLargeButton = null;
    var _myPicking     = null;
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
        _myPicking = new Picking(window);
        window.resize(theWidth, theHeight);

        _myDummyAppContainer = Modelling.createTransform( window.scene.world );
        _myDummyAppContainer.name = "DummyAppContainer";
        _myDummyAppContainer.scale = [DISPLAY_SCALE, DISPLAY_SCALE, 1];

        _myASSManager = new ASSManager( self, _myDummyAppContainer );

        //self.getConfigurator().saveSettings();

        window.camera.frustum.type = ProjectionType.orthonormal;
        window.camera.frustum.width = 400;
        window.camera.position.z = 40;
        
        _mySmallButton = buildButton(_myDummyAppContainer, 
                                     "TEX/playbutton.png", "TEX/playbutton_selected.png", 
                                        "SmallButton",
                                        new Vector3f(0,0,0), 
                                        self.pressedSmall, 
                                        "8smallID", TOGGLE_BUTTON);
//        _myMediumButton= buildButton(_myDummyAppContainer, 
//                                     "TEX/playbutton.png", "TEX/playbutton.png", 
//                                        "MediumButton",
//                                        new Vector3f(30,0,0), 
//                                        self.pressedMedium, 
//                                        "8mediumID");
//        _myLargeButton= buildButton(_myDummyAppContainer, 
//                                     "TEX/playbutton.png", "TEX/playbutton.png", 
//                                     "LargeButton",
//                                      new Vector3f(50,0,0), 
//                                        self.pressedLarge, 
//                                        "8largeID");
        
        _mySmallButton.body.position = new Vector3f(7,0,0);        
        ourButtons.push(_mySmallButton);
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

        _myASSManager.onFrame( theTime );
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function( theButton, theState, theX, theY ) {
        Base.onMouseButton( theButton, theState, theX, theY);
        
    }

    self.onASSEvent = function( theNode ) {

        _myASSManager.onASSEvent( theNode );

        if (theNode.type == "configure" ) {
            _myASSManager.valueColor = new Vector4f(0,0,1,1);
            //print("event " + theNode.type + " grid size: " + theNode.grid_size);
        } else if ( theNode.type == "lost_communication" || theNode.type == "lost_sync" ) {
            //print("event " + theNode.type );
        } else if ( theNode.type == "touch") {
            var myBody = _myPicking.pickBodyByWorldPos(theNode.position3D);
            if(myBody) {
                _mySmallButton.press();
            }
        } else {
            if ( ! _myWorldCross ) {
                var myShape = window.scene.world.getElementById("XXX");
                if (myShape) {
                    _myWorldCross = Modelling.createBody( window.scene.world, myShape.id );
                    _myWorldCross.position.z = 5; // XXX
                }
            }
            if ( _myWorldCross ) {
                _myWorldCross.position = new Vector3f( theNode.position3D );
            }
            //print("event " + theNode.type + " at position: " + theNode.position3D +
            //        " dt: " + (theNode.when - _myLastEventTime));
        }
        _myLastEventTime = theNode.when;
    }
    
    self.pressedSmall = function() {
        
        //print("pressed small");   
    }
    self.pressedMedium = function() {
        print("pressed medium");   
    }
    self.pressedLarge = function() {
        print("pressed large");   
    }
    
    ///////////////////////////////////////////////////////
    // private funtions 
    ///////////////////////////////////////////////////////
    
    function buildButton(theGroupNode, theFileName, thePressedFileName, 
                     theName, thePosition, theFunctionPtr, theTestID, theType, theSize) 
    {
        //print("Utils::buildButton: theType = " + theType);
        var myButton = new Button(theFileName, thePressedFileName, theName, theTestID, theType, theSize);
        myButton.body.position = new Vector3f(thePosition.x, thePosition.y, thePosition.z);
        myButton.onClick = theFunctionPtr;
        theGroupNode.appendChild(myButton.body);
        return myButton;
    }
    
    
    
}

if (__main__ == "ASSDriverTest") {
    try {
        var ourASSDriverTestApp = new ASSDriverTestApp(
                [expandEnvironment("${PRO}") + "/src/Y60/shader/shaderlibrary_nocg.xml"]);
        //ourASSDriverTestApp.setup(600, 600, "ASSDriverTest");
        ourASSDriverTestApp.setup(1400, 1050, "ASSDriverTest");
        ourASSDriverTestApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}

