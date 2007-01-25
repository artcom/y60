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
    var _myMarkers = [];
    var _swappedRaster = false;
    var _myDriver = null;
    var _myCrosshairShape = null;

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

        _myDriver.threshold = 80;
        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, "testbild00.rgb");
        var myQuad = Modelling.createQuad(window.scene, _myMaterial.id, [-200,-100,0], [200,100,0]);
        var myBody = Modelling.createBody(window.scene.world, myQuad.id );
        myBody.position.z = -400;
        myBody.orientation.assignFromEuler( new Vector3f(0.0, 0.0, 0.5 * Math.PI));
        myBody.scale.y = -1;

        var myFrameMaterial = Modelling.createUnlitTexturedMaterial(window.scene, "",
                            "Blue", true, false, 1, [0, 0, 1, 1]);        
        myFrameMaterial.properties.surfacecolor = [0, 0, 1, 1];
        var myFrameShape = Modelling.createQuad(window.scene, myFrameMaterial.id, [-200,-100,0], [200,100,0]);
        myFrameShape.childNode("primitives", 0).childNode(0).type = "lineloop";
        myBody = Modelling.createBody(window.scene.world, myFrameShape.id );
        myBody.position.z = -399;
        myBody.orientation.assignFromEuler( new Vector3f(0.0, 0.0, 0.5 * Math.PI));
        myBody.scale.y = -1;
        
        _myCrosshairShape = Modelling.createCrosshair(window.scene, myFrameMaterial.id, 
                                                 1, 2, "Crosshair"); 
        print("setup done");
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

        // hack!
        if ( ! _swappedRaster ) {
            var myRaster = window.scene.dom.getElementById("ASSRawRaster");
            //var myRaster = window.scene.dom.getElementById("ASSBinaryRaster");
            if (myRaster) {
                var myOldId = _myMaterial.childNode("textures", 0).childNode(0).image;
                var myOldImage = window.scene.dom.getElementById(myOldId);
                myOldImage.parentNode.removeChild( myOldImage );
                _myMaterial.childNode("textures", 0).childNode(0).image = myRaster.id;
                var myXScale = myRaster.width / nextPowerOfTwo( myRaster.width );
                var myYScale = myRaster.height / nextPowerOfTwo( myRaster.height );
                myRaster.matrix.makeScaling( new Vector3f( myXScale, myYScale, 1));
                _swappedRaster = true;
                var myColorScale = 255 / _myDriver.maxOccuringValue;
                myRaster.color_scale = new Vector4f(myColorScale, myColorScale, myColorScale, 1);
                myRaster.min_filter = TextureSampleFilter.nearest;
                myRaster.mag_filter = TextureSampleFilter.nearest;
            }
        }

        var myPositions = _myDriver.positions;
        const myPixelCenterOffset = 10;

        while (_myMarkers.length < myPositions.length) {
            var myNewMarker = Modelling.createBody(window.scene.world, _myCrosshairShape.id );
            myNewMarker.position.z = -399;
            myNewMarker.scale = new Vector3f(3, 3, 3);
            _myMarkers.push( myNewMarker );
            print("spawn marker");
        }
        while (myPositions.length < _myMarkers.length) {
            var myOldMarker = _myMarkers.pop();
            window.scene.world.removeChild( myOldMarker );
            print("remove marker");
        }
        for (var i = 0; i < myPositions.length; ++i) {
            var myPos = myPositions[i];
            // XXX swapped
            _myMarkers[i].position.x = -1 * (20 * myPos.y - 100 + myPixelCenterOffset);
            _myMarkers[i].position.y = 20 * myPos.x - 200 + myPixelCenterOffset;
        }
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

