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

const DISPLAY_SCALE = 20;
const X_MIRROR = false;
const Y_MIRROR = true;
const ORIENTATION = 0.5 * Math.PI;

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
    var _myMaterial = null;
    var _myMarkers = [];
    var _myRegions = [];
    var _myGotDataFlag = false;
    var _myDriver = null;
    var _myCrosshairShape = null;
    var _myGroup = null;
    var _myDisplaySize3D = null;
    var _myBoxShape = null;

    var _myLastEventTime = 0;

    var _myLastFrameTime = 0.0;

    var _myASSManager = null;
    var _myDummyAppContainer = null;

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, false, theTitle);
        window.resize(theWidth, theHeight);

        _myDummyAppContainer = Modelling.createTransform( window.scene.world );
        _myDummyAppContainer.name = "DummyAppContainer";
        _myDummyAppContainer.scale = [DISPLAY_SCALE, DISPLAY_SCALE, 1];

        _myASSManager = new ASSManager( self, _myDummyAppContainer );
        _myDriver = ASSManager.driver;

        self.getConfigurator().saveSettings();

        window.camera.position.z = 400;
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
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
        } else if ( theNode.type == "lost_communication" || theNode.type == "lost_sync" )
        {
            //print("event " + theNode.type );
        } else {
            //print("event " + theNode.type + " at position: " + theNode.position3D +
            //        " dt: " + (theNode.when - _myLastEventTime));
        }
        _myLastEventTime = theNode.when;
    }
}

if (__main__ == "ASSDriverTest") {
    try {
        var ourASSDriverTestApp = new ASSDriverTestApp(
                [expandEnvironment("${PRO}") + "/src/Y60/shader/shaderlibrary_nocg.xml"]);
        ourASSDriverTestApp.setup(600, 600, "ASSDriverTest");
        ourASSDriverTestApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}

