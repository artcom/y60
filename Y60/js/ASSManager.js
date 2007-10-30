//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function ASSManager(theViewer) {
    this.Constructor(this, {}, theViewer, false);
}

ASSManager.driver = null;

ASSManager.prototype.Constructor = function(Public, Protected, theViewer, theInheritedFlag) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _myViewer = theViewer;
    var _myWindow = theViewer.getRenderWindow();
    var _myScene = theViewer.getScene();
    var _myDriver = null;
    var _mySettings = null;

    var _myGridSize =  new Vector2f(1, 1);
    var _myInitialSettingsLoaded = false;

    var _myValueOverlay = null;

    var _myEventQueues = {};

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.driver getter = function() {
        return _myDriver;
    }

    Public.valueColor getter = function() {
        var myMaterial = _myScene.world.getElementById( _myDriver.rasterNames[0] + "Material");
        if (myMaterial) {
            return myMaterial.properties.surfacecolor;
        } else {
            Logger.warning("Can not get valueColor. Materials have not been created yet.");
            return Vector4f(1, 1, 1, 1);
        }
    }

    Public.valueColor setter = function(theColor) {
        var myRasterNames = _myDriver.rasterNames;
        for (var i = 0; i < myRasterNames.length; ++i) {
            var myId = _myDriver.rasterNames[0] + "Material";
            var myMaterial = _myScene.world.getElementById( myId );
            if (myMaterial) {
                myMaterial.properties.surfacecolor = theColor;
            } else {
                Logger.warning("Can not set valueColor. Material with id '" + myId + "' has not been created yet.");
            }
        }
    }

    Public.gridColor getter = function() {
        return _myDriver.gridColor;
    }

    Public.gridColor setter = function(theColor) {
        _myDriver.gridColor = theColor;
    }

    Public.cursorColor getter = function() {
        return _myDriver.cursorColor;
    }

    Public.cursorColor setter = function(theColor) {
        _myDriver.cursorColor = theColor;
    }

    Public.touchColor getter = function() {
        return _myDriver.touchColor;
    }

    Public.touchColor setter = function(theColor) {
        _myDriver.touchColor = theColor;
    }

    Public.textColor getter = function() {
        return _myDriver.textColor;
    }

    Public.textColor setter = function(theColor) {
        _myDriver.textColor = theColor;
    }

    Public.probeColor getter = function() {
        return _myDriver.probeColor;
    }

    Public.probeColor setter = function(theColor) {
        _myDriver.probeColor = theColor;
    }

    Public.getPrecedingEvent = function(theEvent, theNumber) {
        if (theEvent.id in _myEventQueues) {
            var thisQueue = _myEventQueues[theEvent.id];
            if (thisQueue.length > theNumber) {
                return thisQueue[thisQueue.length - theNumber - 1];
            }
            return thisQueue[0];
        } else {
            Logger.warning("Could not retrieve preceding event " + theNumber + " for event " + theEvent.id);
        }
    }

    Public.onUpdateSettings = function( theSettings ) {
        
        _mySettings = theSettings;

        if (_myValueOverlay) {

            // XXX: shearing hack
            var myShearX = _mySettings.childNode("ShearX");
            if(myShearX) {
                _myDriver.shearX = Number(myShearX.childNode("#text"));
            }
            var myShearY = _mySettings.childNode("ShearY");
            if(myShearY) {
                _myDriver.shearY = Number(myShearY.childNode("#text"));
            }

            var myMaterialId = getMaterialIdForValueDisplay();
            if (myMaterialId) {
                _myValueOverlay.material = _myScene.world.getElementById( myMaterialId );
            }

            _myValueOverlay.visible = new Number( _mySettings.childNode("ShowASSData").childNode("#text") );
            _myValueOverlay.position = _mySettings.childNode("SensorPosition").childNode("#text");
            _myValueOverlay.rotation = new Number( _mySettings.childNode("SensorOrientation").childNode("#text") );
            var myOldScale = new Vector2f( _myValueOverlay.scale );
            _myValueOverlay.scale =  _mySettings.childNode("SensorScale").childNode("#text"); 
            var myMirrorXFlag = new Number( _mySettings.childNode("MirrorX").childNode("#text"));
            var mySize = product( myOldScale, _myGridSize);
            if (myMirrorXFlag != 0) {
                _myValueOverlay.scale.x *= -1;
            }
            var myMirrorYFlag = new Number(_mySettings.childNode("MirrorY").childNode("#text"));
            if (myMirrorYFlag != 0) {
                _myValueOverlay.scale.y *= -1;
            }

            if ( _myInitialSettingsLoaded) {
                if ((myOldScale.x > 0 && _myValueOverlay.scale.x < 0) ||
                        (myOldScale.x < 0 && _myValueOverlay.scale.x > 0))
                {
                    _myValueOverlay.position.x += mySize.x;
                    _mySettings.childNode("SensorPosition").childNode("#text").nodeValue =
                            _myValueOverlay.position;
                }

                if ((myOldScale.y > 0 && _myValueOverlay.scale.y < 0) ||
                        (myOldScale.y < 0 && _myValueOverlay.scale.y > 0))
                {
                    _myValueOverlay.position.y += mySize.y;
                    _mySettings.childNode("SensorPosition").childNode("#text").nodeValue =
                            _myValueOverlay.position;
                }
            }
            _myInitialSettingsLoaded = true;
        }
    }

    Public.onASSEvent = function(theEventNode) {
        if (!(theEventNode.id in _myEventQueues)) {
            _myEventQueues[theEventNode.id] = new Array();
        }

        _myEventQueues[theEventNode.id].push(theEventNode);

        if (_myEventQueues[theEventNode.id].length > 1000) {
            _myEventQueues[theEventNode.id].shift();
        }

        if(theEventNode.type == "configure") {
            onConfigure(theEventNode);
        }

        
    }

    ////////////////////////////////////////
    // Protected
    ////////////////////////////////////////

    Protected.setup = function() {
        if ( ! ASSManager.driver ) {
            ASSManager.driver = plug("ASSEventSource");
        }

        _myDriver = ASSManager.driver;

        _myWindow.addExtension( ASSManager.driver );
        _myViewer.registerSettingsListener( _myDriver, "ASSDriver" );
        _myViewer.registerSettingsListener( Public, "ASSDriver" );
    }

    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////

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

            var myXScale = _myGridSize.x / myRaster.width;
            var myYScale = _myGridSize.y / myRaster.height;
            myRaster.matrix.makeScaling( new Vector3f( myXScale, myYScale, 1));
            var myColorScale = 255 / 254;
            myRaster.color_scale = new Vector4f(myColorScale, myColorScale, myColorScale, 1);
            //myRaster.color_bias = [1, 1, 1, 1];
            myRaster.min_filter = TextureSampleFilter.nearest;
            myRaster.mag_filter = TextureSampleFilter.nearest;
            myRaster.texturepixelformat = "INTENSITY";

        }
    }

    function onConfigure(theEvent) {
        _myGridSize = new Vector2f( theEvent.grid_size );

        setupValueMaterials();

        var myMaterial = _myScene.world.getElementById( getMaterialIdForValueDisplay() );
        if ( ! _myValueOverlay) {
            _myValueOverlay = new ImageOverlay( _myScene, 
                    _myScene.world.getElementById( myMaterial.childNode("textures").firstChild.image ));
        }
        _myDriver.overlay = _myValueOverlay.node;

        if (_mySettings) {
            Public.onUpdateSettings( _mySettings );
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

    ////////////////////////////////////////
    // Setup
    ////////////////////////////////////////

    if(!theInheritedFlag) {
        Protected.setup();
    }
}
