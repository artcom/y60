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

const DISPLAY_SCALE = 20;
const X_MIRROR = false;
const Y_MIRROR = true;
const ORIENTATION = 0.5 * Math.PI;
const DISC_SIZE = 10;

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
    var _myMarkerGroup = null;
    var _myBox

    var _myLastFrameTime = 0.0;
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

        _myDriver.noiseThreshold = 15;
        _myDriver.componentThreshold = 5;
        _myDriver.gainPower = 2;

        print("setup done");
        window.camera.position.z = 400;
    }

    function createDisplay(theRaster) {

        var myGridSize = _myDriver.gridSize;
        _myDisplaySize3D = new Vector3f(myGridSize.x, myGridSize.y, 0);

        _myGroup = Modelling.createTransform( window.scene.world );
        _myGroup.name = "SensorDisplay";
        _myGroup.scale = [DISPLAY_SCALE, DISPLAY_SCALE, 1];

        if (X_MIRROR) {
            _myGroup.scale.x *= -1;
        }
        if (Y_MIRROR) {
            _myGroup.scale.y *= -1;
        }
        _myGroup.orientation.assignFromEuler( new Vector3f(0, 0, ORIENTATION));
        _myGroup.position = product( product( product(_myDisplaySize3D, _myGroup.scale),
                     _myGroup.orientation), -0.5);
        _myGroup.position.z = 0;

        _myDriver.transform = _myGroup;

        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, "testbild00.rgb");
        var myQuad = Modelling.createQuad(window.scene, _myMaterial.id, [0,0,0], _myDisplaySize3D);
        var myBody = Modelling.createBody(_myGroup, myQuad.id );

        var myWhiteMaterial = Modelling.createUnlitTexturedMaterial(window.scene, "",
                            "White", true, false, 1, [1, 1, 1, 1]);        
        myWhiteMaterial.properties.surfacecolor = [1, 1, 1, 1];
        var myFrameShape = Modelling.createQuad(window.scene, myWhiteMaterial.id, [0,0,0],
                    _myDisplaySize3D);
        myFrameShape.childNode("primitives", 0).childNode(0).type = "lineloop";
        myBody = Modelling.createBody(_myGroup, myFrameShape.id );
        myBody.position.z = 1;
        
        var myRedMaterial = Modelling.createUnlitTexturedMaterial(window.scene, "",
                            "Blue", true, false, 1, [1, 0, 0, 1]);        
        myRedMaterial.properties.surfacecolor = [1, 0, 0, 1];
        var myOriginMarkerShape = Modelling.createCrosshair(window.scene, myRedMaterial.id, 
                                                 1, 2, "Origin"); 
        var myOriginMarker = Modelling.createBody(_myGroup, myOriginMarkerShape.id );
        myOriginMarker.position.y = _myDisplaySize3D.y;
        myOriginMarker.position.z = 1;
        myOriginMarker.scale = new Vector3f(3 / DISPLAY_SCALE, 3 / DISPLAY_SCALE , 3 / DISPLAY_SCALE);

        _myCrosshairShape = Modelling.createCrosshair(window.scene, myWhiteMaterial.id, 
                                                 1, 2, "Crosshair"); 

        var myOldId = _myMaterial.childNode("textures", 0).childNode(0).image;
        var myOldImage = window.scene.dom.getElementById(myOldId);
        myOldImage.parentNode.removeChild( myOldImage );
        _myMaterial.childNode("textures", 0).childNode(0).image = theRaster.id;
        var myXScale = theRaster.width / nextPowerOfTwo( theRaster.width );
        var myYScale = theRaster.height / nextPowerOfTwo( theRaster.height );
        theRaster.matrix.makeScaling( new Vector3f( myXScale, myYScale, 1));
        var myColorScale = 255 / _myDriver.maxOccuringValue;
        theRaster.color_scale = new Vector4f(myColorScale, myColorScale, myColorScale, 1);
        theRaster.min_filter = TextureSampleFilter.nearest;
        theRaster.mag_filter = TextureSampleFilter.nearest;

        _myBoxShape = Modelling.createQuad(window.scene, myWhiteMaterial.id, [-0.5, -0.5, 0],
                    [0.5, 0.5, 0.0]);
        _myBoxShape.childNode("primitives", 0).childNode(0).type = "lineloop";
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);

        // hack!
        if ( ! _myGotDataFlag ) {
            var myRaster = window.scene.dom.getElementById("ASSRawRaster");
            //var myRaster = window.scene.dom.getElementById("ASSFilteredRaster");
            if (myRaster) {

                createDisplay(myRaster);

                _myGotDataFlag = true;
            }
        }

        var myPositions = _myDriver.positions;
        const myPixelCenterOffset =  0.5;
        const myBBoxFlag = true;

        while (_myMarkers.length < myPositions.length) {
            var myNewMarker = Modelling.createBody(_myGroup, _myCrosshairShape.id );
            myNewMarker.position.z = 1;
            myNewMarker.scale = new Vector3f(3 / DISPLAY_SCALE , 3 / DISPLAY_SCALE, 3 / DISPLAY_SCALE);
            _myMarkers.push( myNewMarker );

            if (myBBoxFlag) {
                var myNewBox = Modelling.createBody( _myGroup, _myBoxShape.id );
                myNewBox.position.z = 1;
                _myRegions.push( myNewBox );
            }
        }
        while (myPositions.length < _myMarkers.length) {
            var myOldMarker = _myMarkers.pop();
            _myGroup.removeChild( myOldMarker );

            if (myBBoxFlag) {
                var myOldBox = _myRegions.pop();
                _myGroup.removeChild( myOldBox );
            }
        }

        var myRegions = _myDriver.regions;
        for (var i = 0; i < myPositions.length; ++i) {
            var myPos = myPositions[i];
            _myMarkers[i].position.x = myPos.x + myPixelCenterOffset;
            _myMarkers[i].position.y = _myDisplaySize3D.y - myPos.y -
                        myPixelCenterOffset;
            if (myBBoxFlag) {
                _myRegions[i].position.x = myRegions[i].center.x;
                _myRegions[i].position.y = _myDisplaySize3D.y - myRegions[i].center.y;
                _myRegions[i].scale.x = myRegions[i].size.x;
                _myRegions[i].scale.y = myRegions[i].size.y;
            }
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
        switch (theKey) {
            case '+':
                _myDriver.componentThreshold +=  1;
                print("component threshold: " + _myDriver.componentThreshold);
                break;
            case '-':
                _myDriver.componentThreshold -= 1;
                print("component threshold: " + _myDriver.componentThreshold);
                break;
            case '.':
                _myDriver.noiseThreshold += 1;
                print("noise threshold: " + _myDriver.noiseThreshold);
                break;
            case ',':
                _myDriver.noiseThreshold -= 1;
                print("noise threshold: " + _myDriver.noiseThreshold);
                break;
            case 'm':
                _myDriver.gainPower += 1;
                print("gain power: " + _myDriver.gainPower);
                break;
            case 'n':
                _myDriver.gainPower -= 1;
                print("gain power: " + _myDriver.gainPower);
                break;
        }
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function( theButton, theState, theX, theY ) {
        Base.onMouseButton( theButton, theState, theX, theY);
    }
    self.onASSEvent = function( theNode ) {
        print("event: " + theNode );
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

