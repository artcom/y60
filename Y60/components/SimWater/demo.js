//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

if (__main__ == undefined) var __main__ = "SimWaterTest";

use("SceneViewer.js");

plug("SimWater");

window = new RenderWindow();

function SimWaterTestApp(theArguments) {
    this.Constructor(this, theArguments);
}

SimWaterTestApp.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, false, theTitle);
        print("setup");

        print("width: " + window.width + " height: " + window.height);
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

        if ( ! _myWater ) {
            _myWater = new SimWater();
            window.addExtension(_myWater);
            self.registerSettingsListener( _myWater, "Water" );
        }

    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function( theButton, theState, theX, theY ) {
        Base.onMouseButton( theButton, theState, theX, theY);
        _myWater.splash(new Vector2i(50, 50), 7, 10);
    }

    var _myWater = null;
}

if (__main__ == "SimWaterTest") {
    print("SimWaterTest");
    try {
        var ourSimWaterTestApp = new SimWaterTestApp(
                [expandEnvironment("${PRO}") + "/src/Y60/shader/shaderlibrary_nocg.xml"]);
        ourSimWaterTestApp.setup(320, 200, "SimWaterTest");
        ourSimWaterTestApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}

