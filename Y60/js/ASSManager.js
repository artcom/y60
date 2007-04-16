//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function ASSManager(theViewer, theParentTransform, theDecorationZOffset) {
    this.Constructor(this, theViewer, theParentTransform, theDecorationZOffset);
}

ASSManager.driver = null;

const ASS_CROSSHAIR_SCALE = 0.15;

ASSManager.prototype.Constructor = function(self, theViewer, theParentTransform,
                                            theDecorationZOffset)
{
    self.onUpdateSettings = function( theSettings ) {

        _mySettings = theSettings;

        if ( _mySensorValueShape ) {
            var myMaterialId = getMaterialIdForValueDisplay();
            _mySensorValueShape.childNode("primitives").childNode("elements").material =
                    myMaterialId;
        }

        if (_myGroup) {
            _myGroup.visible = new Number( _mySettings.childNode("ShowASSData").childNode("#text") );
            _myGroup.position = _mySettings.childNode("SensorPosition").childNode("#text");
            var myOrientation = _mySettings.childNode("SensorOrientation").childNode("#text");
            _myGroup.orientation.assignFromEuler( new Vector3f(0, 0, myOrientation));
            var myOldScale = new Vector3f( _myGroup.scale );
            _myGroup.scale = _mySettings.childNode("SensorScale").childNode("#text");
            var myMirrorXFlag = new Number( _mySettings.childNode("MirrorX").childNode("#text"));
            var mySize = product( myOldScale, _myGridSize3d);
            if (myMirrorXFlag != 0) {
                _myGroup.scale.x *= -1;
            }
            var myMirrorYFlag = new Number(_mySettings.childNode("MirrorY").childNode("#text"));
            if (myMirrorYFlag != 0) {
                _myGroup.scale.y *= -1;
            }

            if ( _myInitialSettingsLoaded) {
                if ((myOldScale.x > 0 && _myGroup.scale.x < 0) ||
                        (myOldScale.x < 0 && _myGroup.scale.x > 0))
                {
                    _myGroup.position.x += mySize.x;
                    _mySettings.childNode("SensorPosition").childNode("#text").nodeValue =
                            _myGroup.position;
                }

                if ((myOldScale.y > 0 && _myGroup.scale.y < 0) ||
                        (myOldScale.y < 0 && _myGroup.scale.y > 0))
                {
                    _myGroup.position.y += mySize.y;
                    _mySettings.childNode("SensorPosition").childNode("#text").nodeValue =
                            _myGroup.position;
                }
            }
            _myInitialSettingsLoaded = true;
        }
    }

    self.onASSEvent = function( theEventNode ) {
        if (_myGroup.visible) {
            if (theEventNode.type == "configure") {
                onConfigure( theEventNode );
            } else if (theEventNode.type == "add") {
                //print("add");
                var myNewMarker = Modelling.createBody(_myGroup, _myCursorShape.id );
                myNewMarker.position.z = _myDecorationZOffset;
                myNewMarker.scale = new Vector3f(ASS_CROSSHAIR_SCALE, ASS_CROSSHAIR_SCALE,
                        ASS_CROSSHAIR_SCALE);
                myNewMarker.name = "ASSCursor" + theEventNode.id;
                myNewMarker.id = "ASSCursor" + theEventNode.id;

                var myROIBox = Modelling.createBody( _myGroup, _myROIBoxShape.id );
                myROIBox.position.z = _myDecorationZOffset;
                myROIBox.name = "ASSROI" + theEventNode.id;
                myROIBox.id = "ASSROI" + theEventNode.id;

                moveMarkerAndBox( myNewMarker, myROIBox, theEventNode);
            } else if (theEventNode.type == "move") {
                //print("move");
                var myMarker = _myGroup.getElementById("ASSCursor" + theEventNode.id);
                var myROIBox = _myGroup.getElementById("ASSROI" + theEventNode.id);
                if (myMarker && myROIBox) {
                    moveMarkerAndBox( myMarker, myROIBox, theEventNode);
                }
            }
        }
        if (theEventNode.type == "remove") {
            //print("remove");
            var myOldMarker = _myGroup.getElementById("ASSCursor" + theEventNode.id);
            if (myOldMarker) {
                _myGroup.removeChild( myOldMarker );
            }
            var myROIBox = _myGroup.getElementById("ASSROI" + theEventNode.id);
            if (myROIBox) {
                _myGroup.removeChild( myROIBox );
            }
        }
    }

    self.valueColor setter = function( theColor ) {
        var myRasterNames = _myDriver.rasterNames;
        if (myRasterNames.length == 0) {
            Logger.warning("Can not set valueColor. " + 
                    "Materials have not been created.");
        } else {
            for (var i = 0; i < myRasterNames.length; ++i) {

                var myMaterial = _myScene.world.getElementById( myRasterNames[i] + "Material" );
                myMaterial.properties.surfacecolor = theColor;
            }
        }
    }

    self.valueColor getter = function() {
        var myRasterNames = _myDriver.rasterNames;
        if (myRasterNames.length == 0) {
            Logger.warning("Can not get valueColor. " + 
                    "Materials have not been created.");
        } else {
            var myMaterial = _myScene.world.getElementById( myRasterNames[0] + "Material" );
            return myMaterial.properties.surfacecolor;
        }
    }

    function moveMarkerAndBox( theMarker, theROIBox, theEvent) {
        const myPixelCenterOffset = 0.5;
        var myRawPosition = theEvent.raw_position;
        theMarker.position.x = myRawPosition.x + myPixelCenterOffset;
        theMarker.position.y = _myGridSize3d.y - myRawPosition.y - myPixelCenterOffset;

        var myROI = theEvent.roi;
        theROIBox.position.x = myROI.center.x;
        theROIBox.position.y = _myGridSize3d.y - myROI.center.y;
        theROIBox.scale.x = myROI.size.x;
        theROIBox.scale.y = myROI.size.y;
    }

    function setup( theParent, theDecorationZ ) {
        if ( ! ASSManager.driver ) {
            ASSManager.driver = plug("ASSEventSource");
        }
        _myDriver = ASSManager.driver;

        _myWindow.addExtension( ASSManager.driver );
        _myViewer.registerSettingsListener( _myDriver, "ASSDriver" );
        _myViewer.registerSettingsListener( self, "ASSDriver" );

        var myParent = _myScene.world;
        if ( theParent != undefined ) {
            myParent = theParent;
        } else {
            Logger.warning("Using world node as parent.");
        }
        _myGroup = Modelling.createTransform( myParent );
        _myGroup.insensible = true;
        _myGroup.name = "ASSSensorDisplay";
        _myDriver.transform = _myGroup;

        if (theDecorationZ != undefined) {
            _myDecorationZOffset = theDecorationZ;
        }

        setupDisplay();
    }

    function setupDisplay() {

        _myFrameMaterial = Modelling.createUnlitTexturedMaterial(_myScene, "",
                            "ASSFrame", true, false, 1, [1, 1, 1, 1]);        
        _myFrameMaterial.properties.surfacecolor = [1, 1, 1, 1];

        _myOriginMaterial = Modelling.createUnlitTexturedMaterial(_myScene, "",
                            "ASSOrigin", true, false, 1, [1, 1, 1, 1]);        
        _myOriginMaterial.properties.surfacecolor = [1, 0.56, 0, 1];

        _myOriginShape = Modelling.createCrosshair(_myScene, _myOriginMaterial.id, 
                                                 1, 2, "ASSOrigin"); 

        _myCursorMaterial = Modelling.createUnlitTexturedMaterial(_myScene, "",
                            "ASSCursorMaterial", true, false, 1, [1, 1, 1, 1]);        

        _myCursorMaterial.properties.surfacecolor = [1, 1, 0, 1];

        _myCursorShape = Modelling.createCrosshair(_myScene, _myCursorMaterial.id, 
                                                 1, 2, "ASSCursor"); 
    
        _myROIBoxShape = Modelling.createQuad(_myScene, _myCursorMaterial.id, [-0.5, -0.5, 0],
                    [0.5, 0.5, 0.0]);
        _myROIBoxShape.childNode("primitives", 0).childNode(0).type = "lineloop";
        _myROIBoxShape.name = "ASSROIBox";
    }


    function setupValueMaterials() {
        var myRasterNames = _myDriver.rasterNames;
        for (var i = 0; i < myRasterNames.length; ++i) {

            var myMaterial = _myScene.world.getElementById( myRasterNames[i] + "Material" );
            var myRaster = _myScene.world.getElementById( myRasterNames[i] );

            if ( ! myMaterial ) {
                myMaterial = Modelling.createUnlitTexturedMaterial(_myScene, myRaster );
                myMaterial.name = myRasterNames[i] + "Material";
                myMaterial.id = myRasterNames[i] + "Material";
                myMaterial.transparent = true;
            }

            var myXScale = myRaster.width / nextPowerOfTwo( myRaster.width );
            var myYScale = myRaster.height / nextPowerOfTwo( myRaster.height );
            myRaster.matrix.makeScaling( new Vector3f( myXScale, myYScale, 1));
            var myColorScale = 255 / _myDriver.maxOccuringValue;
            myRaster.color_scale = new Vector4f(myColorScale, myColorScale, myColorScale, 1);
            myRaster.min_filter = TextureSampleFilter.nearest;
            myRaster.mag_filter = TextureSampleFilter.nearest;
            myRaster.texturepixelformat = "INTENSITY";

        }
    }

    function onConfigure( theEvent ) {
        setupValueMaterials();

        _myGridSize3d = new Vector3f( theEvent.grid_size.x, theEvent.grid_size.y, 0);

        if ( _myFrameShape ) {
            _myFrameShape.parentNode.removeChild( _myFrameShape );
        }
        _myFrameShape = Modelling.createQuad(_myScene, _myFrameMaterial.id, [0,0,0],
                                             _myGridSize3d);
        _myFrameShape.childNode("primitives", 0).childNode(0).type = "lineloop";
        _myFrameShape.name = "ASSFrame";

        if ( _mySensorFrame ) {
            _mySensorFrame.shape = _myFrameShape.id;
        } else {
            _mySensorFrame =  Modelling.createBody( _myGroup, _myFrameShape.id );
            _mySensorFrame.position.z = _myDecorationZOffset;
            _mySensorFrame.name = "ASSSensorFrame";
        }

        if ( _mySensorValueShape ) {
            _mySensorValueShape.parentNode.removeChild( _mySensorValueShape );   
        }

        var myValueMaterialId = getMaterialIdForValueDisplay();
        _mySensorValueShape = Modelling.createQuad(_myScene, myValueMaterialId, [0,0,0],
                                                   _myGridSize3d);
        _mySensorValueShape.name = "ASSSensorValues";

        if ( _mySensorValueDisplay ) {
            _mySensorValueDisplay.shape = _mySensorValueShape.id;
        } else {
            _mySensorValueDisplay =  Modelling.createBody( _myGroup, _mySensorValueShape.id );
            _mySensorValueDisplay.name = "ASSSensorValueDisplay";
        }

        if ( ! _myOriginMarker ) {
            _myOriginMarker = Modelling.createBody( _myGroup, _myOriginShape.id );
            _myOriginMarker.position.z = 2 * _myDecorationZOffset;
            _myOriginMarker.scale = new Vector3f( ASS_CROSSHAIR_SCALE,
                    ASS_CROSSHAIR_SCALE, ASS_CROSSHAIR_SCALE );
            _myOriginMarker.name = "ASSOrigin";
        }

        if (_mySettings) {
            self.onUpdateSettings( _mySettings );
        }
    }

    function getMaterialIdForValueDisplay() {
        var myRasterNames = _myDriver.rasterNames;
        if ( myRasterNames.length == 0) {
            Logger.error("No value rasters.");
        }
        var myIndex = 0;
        if ( _mySettings ) {
            myIndex = new Number( 
                    _mySettings.childNode("CurrentValueDisplay", 0).childNode("#text"));
        }
        if (myIndex < 0 || myIndex >= myRasterNames.length) {
            Logger.warning("CurrentValueDisplay is out of bounds. Using zero.");
            myIndex = 0;
        }
        return myRasterNames[ myIndex ] + "Material";
    }

    var _myViewer = theViewer;
    var _myWindow = theViewer.getRenderWindow();
    var _myScene = theViewer.getScene();
    var _myDriver = null;
    var _mySettings = null;

    var _myGroup = null;
    var _mySensorValueShape = null;
    var _mySensorValueBody = null;
    var _myFrameMaterial = null;
    var _myOriginMaterial = null;
    var _myOriginShape = null;
    var _myOriginMarker = null;
    var _myROIBoxShape = null;
    var _myCursorMaterial = null;
    var _myCursorShape = null;
    var _myDecorationZOffset = 0.15;

    var _myGridSize3d = new Vector3f(1, 1, 1);
    var _myFrameShape = null;
    var _mySensorFrame = null;
    var _mySensorValueShape = null;
    var _mySensorValueDisplay = null;

    var _myInitialSettingsLoaded = false;

    setup(theParentTransform, theDecorationZOffset);
}


