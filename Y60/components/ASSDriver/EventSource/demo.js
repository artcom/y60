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
use("ASSOSD.js");
use("BuildUtils.js");
use("SoundController.js");

const LATENCY_TEST=false;

const DISPLAY_SCALE = 20;
const X_MIRROR = false;
const Y_MIRROR = true;
const ORIENTATION = 0.5 * Math.PI;

const PLOT_DATAFILE = "/tmp/ttt.plot"


var ourLatencyTestPort = null;
var ourLastFrameNo = 0;

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
    
    var _myPicking     = null;

    var _myDisplayImage = null;
    var _myDisplayMaterial = null;
    var _myDisplayBody = null;
    
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
        //_myASSManager.driver.performTara(); // XXX
        //_myASSManager.driver.callibrateTransmissionLevels(); // XXX

        //self.getConfigurator().saveSettings();

        window.camera.frustum.type = ProjectionType.orthonormal;
        window.camera.frustum.width = 400;
        window.camera.position.z = 40;

        //window.canvas.backgroundcolor = [1,1,1,1];

        

        //_myASSManager.textColor = [0,0,0,1];
        

        if (LATENCY_TEST) {
            ourLatencyTestPort = new SerialDevice(0);
            ourLatencyTestPort.open();
        }
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
        _myASSManager.valueColor = [0, 0, 1, 1];
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

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {

        switch( theKey ) {
            case '1':
                if ( theState ) {
                    print("Callibrate transmission levels.");
                    _myASSManager.driver.callibrateTransmissionLevels();
                }
                break;
            case '2':
                if ( theState ) {
                    print("Perform tara.");
                    _myASSManager.driver.performTara();
                }
                break;
            case '3':
                if ( theState ) {
                    print("Query config mode");
                    _myASSManager.driver.queryConfigMode();
                }
                break;
            default:
                Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
    }

    self.onASSEvent = function( theNode ) {

        if (LATENCY_TEST) {
            if (ourLastFrameNo != theNode.frameno) {
                if (theNode.frameno % 16 == 0) {
                    ourLatencyTestPort.status = ourLatencyTestPort.status ? 0 : 0x10;
                } else {
            //        ourLatencyTestPort.status = 0;
                }
                ourLastFrameNo = theNode.frameno;
            }
        }

        _myASSManager.onASSEvent( theNode );


        //print( theNode );

        if (theNode.type == "configure" ) {
            _myASSManager.valueColor = new Vector4f(0,0,1,1);
            //print("event " + theNode.type + " grid size: " + theNode.grid_size);
        } else if ( theNode.type == "lost_communication" || theNode.type == "lost_sync" ) {
            //print("event " + theNode.type );
        } else if ( theNode.type == "touch") {
            var myBody = _myPicking.pickBody(theNode.position3D.x, theNode.position3D.y);
            if(myBody) {                
                for(var i=0; i<ourButtons.length; ++i ) {
                    if (myBody.id == ourButtons[i].body.id) {
                        ourButtons[i].press();
                    }
                }
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
            //print("event " + theNode.type + " at position: " + theNode.position3D );
        }
        _myLastEventTime = theNode.when;
    }
    

    ///////////////////////////////////////////////////////
    // private funtions 
    ///////////////////////////////////////////////////////
   

}

if (__main__ == "ASSDriverTest") {
    try {
        var ourASSDriverTestApp = new ASSDriverTestApp(arguments);
        //ourASSDriverTestApp.setup(600, 600, "ASSDriverTest");
        ourASSDriverTestApp.setup(1400, 1050, "ASSDriverTest");
        //ourASSDriverTestApp.setup(1920, 1280, "ASSDriverTest");
        ourASSDriverTestApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}

