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
        window.resize(256,256);//600, 600);
        _myWater = new SimWater();
        window.addExtension(_myWater);
        _myWater.addFloormap( "data/whiskeyglas2.png" );
        print( "active floormap: " + _myWater.activeFloormap + " of " + _myWater.floormapCount );
        _myWater.addFloormap( "data/greypattern.png" );
        print( "active floormap: " + _myWater.activeFloormap  + " of " + _myWater.floormapCount );
        _myWater.activeFloormap = 1;

        _myWater.addCubemap( "data/cube_top.png", "data/cube_left.png", "data/cube_bottom.png", 
                             "data/cube_right.png", "data/cube_roof.png", "data/cube_ground.png" ); 

        print( "active cubemap: " + _myWater.activeCubemap  + " of " + _myWater.cubemapCount );

        _myWater.addCubemap( "data/forest_2_front.jpg", "data/forest_2_left.jpg", "data/forest_2_back.jpg", 
                             "data/forest_2_right.jpg", "data/forest_2_top.jpg", "data/forest_2_bottom.jpg" ); 

        print( "active cubemap: " + _myWater.activeCubemap  + " of " + _myWater.cubemapCount );
        _myWater.activeCubemap = 0;

        self.registerSettingsListener( _myWater, "Water" );
        print("setup done");
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
         if (theCtrlFlag) {
            Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
             return;
         }
         if (!theKeyState) {
            return;
        }
        switch (theKey) {
            case 'space':
                var myIndex = _myWater.activeCubemap + 1;
                if (myIndex == _myWater.cubemapCount) {
                    myIndex = 0;
                }
                _myWater.activeCubemap = myIndex;
                print( "active cubemap: " + _myWater.activeCubemap  + " of " + _myWater.cubemapCount );
                break;
        }
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function( theButton, theState, theX, theY ) {
        Base.onMouseButton( theButton, theState, theX, theY);
        _myWater.splash(new Vector2i(theX, theY), 7, 10);
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

