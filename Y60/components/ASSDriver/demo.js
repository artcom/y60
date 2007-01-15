//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
if (__main__ == undefined) var __main__ = "ASSDriverTest";

use("SceneViewer.js");


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
    var _swappedRaster = false;

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
        _myDriver = plug("ASSDriver");
        window.addExtension(_myDriver);
        //self.registerSettingsListener( _myDriver, "ASSDriver" );

        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, "testbild00.rgb");
        var myQuad = Modelling.createQuad(window.scene, _myMaterial.id, [-200,-100,0], [200,100,0]);
        var myBody = Modelling.createBody(window.scene.world, myQuad.id );
        myBody.position.z = -400;
        print("setup done");
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

        // hack!
        var myRaster = window.scene.dom.getElementById("ASSRawRaster");
        if (myRaster && ! _swappedRaster) {
            _myMaterial.childNode("textures", 0).childNode(0).image = myRaster.id;
            var myXScale = myRaster.width / nextPowerOfTwo( myRaster.width );
            var myYScale = myRaster.height / nextPowerOfTwo( myRaster.height );
            myRaster.matrix.makeScaling( new Vector3f( myXScale, myYScale, 1));
            _swappedRaster = true;
            myRaster.min_filter = TextureSampleFilter.nearest;
            myRaster.mag_filter = TextureSampleFilter.nearest;
        };
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
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function( theButton, theState, theX, theY ) {
        Base.onMouseButton( theButton, theState, theX, theY);
    }

    var _myDriver = null;
}

if (__main__ == "ASSDriverTest") {
    print("ASSDriverTest");
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

