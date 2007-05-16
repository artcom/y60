//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function ASSManager(theViewer, theParentTransform) {
    this.Constructor(this, theViewer, theParentTransform );
}

ASSManager.driver = null;

const ASS_CROSSHAIR_SCALE = 0.15;
const DECORATION_Z_OFFSET = 0.1;
const TOUCH_LIFE_TIME = 1.0;

ASSManager.prototype.Constructor = function(self, theViewer, theParentTransform, theDecorationZOffset)
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
            var mySize = product( myOldScale, difference( _myGridSize3d, new Vector3f(1,1,1)));
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

    self.onFrame = function( theTime ) {
        var myDeltaT = theTime - _myLastFrameTime;

        if ( _myGroup.visible ) {
            if ( _myTouchList.length > 0) {
                for (var i = _myTouchList.length - 1; i >= 0; --i ) {
                    _myTouchList[i].timer -= myDeltaT;
                    if ( _myTouchList[i].timer <= 0 ) {
                        _myGroup.removeChild( _myGroup.getElementById( _myTouchList[i].id ) );
                        _myTouchList.splice( i, 1);
                    }
                }
            }
        }

        _myLastFrameTime = theTime;
    }

    self.onASSEvent = function( theEventNode ) {
        if (_myGroup.visible) {
            if (theEventNode.type == "configure") {
                onConfigure( theEventNode );
            } else if (theEventNode.type == "add") {
                //print("add");
                var myNewMarker = Modelling.createBody(_myGroup, _myCursorShape.id );
                myNewMarker.scale = new Vector3f(ASS_CROSSHAIR_SCALE, ASS_CROSSHAIR_SCALE,
                        ASS_CROSSHAIR_SCALE);
                myNewMarker.name = "ASSCursor" + theEventNode.id;
                myNewMarker.id = "ASSCursor" + theEventNode.id;
                myNewMarker.position.z = DECORATION_Z_OFFSET; // XXX
                myNewMarker.visible = true; // XXX

                var myROIBox = Modelling.createBody( _myGroup, _myROIBoxShape.id );
                myROIBox.name = "ASSROI" + theEventNode.id;
                myROIBox.id = "ASSROI" + theEventNode.id;
                myROIBox.position.z = DECORATION_Z_OFFSET; // XXX
                myROIBox.visible = true; // XXX

                moveMarkerAndBox( myNewMarker, myROIBox, theEventNode);
            } else if (theEventNode.type == "move") {
                //print("move");
                var myMarker = _myGroup.getElementById("ASSCursor" + theEventNode.id);
                var myROIBox = _myGroup.getElementById("ASSROI" + theEventNode.id);
                //print ("position: " + theEventNode.position3D + " raw: " + theEventNode.raw_position);
                if (myMarker && myROIBox) {
                    moveMarkerAndBox( myMarker, myROIBox, theEventNode);
                }
            } else if ( theEventNode.type == "touch") {
                //print("touch");

                var myTouchMarker = Modelling.createBody( _myGroup, _myTouchMarkerShape.id );
                myTouchMarker.position.xy = theEventNode.raw_position;
                myTouchMarker.position.z = DECORATION_Z_OFFSET;

                myTouchMarker.scale = new Vector3f(ASS_CROSSHAIR_SCALE, ASS_CROSSHAIR_SCALE,
                        ASS_CROSSHAIR_SCALE);
                _myTouchList.push( {id: myTouchMarker.id, timer: TOUCH_LIFE_TIME } );
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
            Logger.warning("Can not get valueColor. Materials have not been created.");
        } else {
            var myMaterial = _myScene.world.getElementById( myRasterNames[0] + "Material" );
            return myMaterial.properties.surfacecolor;
        }
    }

    self.transform getter = function() {
        return _myGroup;
    }
    self.driver getter = function() {
        return _myDriver;
    }

    function moveMarkerAndBox( theMarker, theROIBox, theEvent) {
        const myPixelCenterOffset = 0.5;
        var myRawPosition = theEvent.raw_position;
        theMarker.position.x = myRawPosition.x;
        theMarker.position.y = myRawPosition.y;

        var myROI = theEvent.roi;
        theROIBox.position.x = myROI.center.x/* - 0.5*/; // integer coordinate correction
        theROIBox.position.y = myROI.center.y/* - 0.5*/;
        theROIBox.scale.x = myROI.size.x + 1;
        theROIBox.scale.y = myROI.size.y + 1;
    }

    function setup( theParent) {
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

        setupDisplay();
    }

    function setupDisplay() {

        _myFrameMaterial = Modelling.createUnlitTexturedMaterial(_myScene, "",
                            "ASSFrame", true, false, 1, [1, 1, 1, 1]);        
        _myFrameMaterial.properties.surfacecolor = [1, 1, 1, 1];
        _myFrameMaterial.transparent = true;

        _myOriginMaterial = Modelling.createUnlitTexturedMaterial(_myScene, "",
                            "ASSOrigin", true, false, 1, [1, 1, 1, 1]);        
        _myOriginMaterial.properties.surfacecolor = [1, 0.56, 0, 1];
        _myOriginMaterial.transparent = true;

        _myOriginShape = Modelling.createCrosshair(_myScene, _myOriginMaterial.id, 
                                                 1, 2, "ASSOrigin"); 
        _myOriginShape.renderstyle.polygon_offset = true;
        _myOriginShape.renderstyle.ignore_depth = true;

        _myCursorMaterial = Modelling.createUnlitTexturedMaterial(_myScene, "",
                            "ASSCursorMaterial", true, false, 1, [1, 1, 1, 1]);        

        _myCursorMaterial.properties.surfacecolor = [1, 1, 0, 1];

        _myCursorShape = Modelling.createCrosshair(_myScene, _myCursorMaterial.id, 
                                                 1, 2, "ASSCursor"); 
        _myCursorShape.renderstyle.polygon_offset = true;
        _myCursorShape.renderstyle.ignore_depth = true;
    
        _myROIBoxShape = Modelling.createQuad(_myScene, _myCursorMaterial.id, [-0.5, -0.5, 0],
                    [0.5, 0.5, 0.0]);
        _myROIBoxShape.renderstyle.polygon_offset = true;
        _myROIBoxShape.renderstyle.ignore_depth = true;
        _myROIBoxShape.childNode("primitives", 0).childNode(0).type = "lineloop";
        _myROIBoxShape.name = "ASSROIBox";

        _myTouchMarkerMaterial = Modelling.createUnlitTexturedMaterial(_myScene, "",
                            "ASSTouchMarkerMaterial", true, false, 1, [1, 1, 1, 1]);        

        _myTouchMarkerMaterial.properties.surfacecolor = [1, 1, 1, 1];


        _myTouchMarkerShape = Modelling.createCrosshair(_myScene, _myTouchMarkerMaterial.id, 
                                                 2, 2, "ASSTouchMarker"); 
        _myTouchMarkerShape.renderstyle.polygon_offset = true;
        _myTouchMarkerShape.renderstyle.ignore_depth = true;

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

            var myXScale = _myGridSize.x / myRaster.width;
            var myYScale = _myGridSize.y / myRaster.height;
            myRaster.matrix.makeScaling( new Vector3f( myXScale, myYScale, 1));
            var myColorScale = 255 / _myDriver.maxOccuringValue;
            myRaster.color_scale = new Vector4f(myColorScale, myColorScale, myColorScale, 1);
            myRaster.min_filter = TextureSampleFilter.nearest;
            myRaster.mag_filter = TextureSampleFilter.nearest;
            myRaster.texturepixelformat = "INTENSITY";

        }
    }

    function onConfigure( theEvent ) {

        _myGridSize = new Vector2f( theEvent.grid_size );
        _myGridSize3d = new Vector3f( theEvent.grid_size.x, theEvent.grid_size.y, 0);

        setupValueMaterials();
        createWireGrid();

        var myPixelCenterOffset = new Vector3f(-0.5, -0.5, 0);

        var myTopRight = sum( _myGridSize3d, myPixelCenterOffset);

        if ( _myFrameShape ) {
            _myFrameShape.parentNode.removeChild( _myFrameShape );
        }
        _myFrameShape = Modelling.createQuad(_myScene, _myFrameMaterial.id, myPixelCenterOffset,
                                             myTopRight);
        _myFrameShape.childNode("primitives", 0).childNode(0).type = "lineloop";
        _myFrameShape.renderstyle.polygon_offset = true;
        _myFrameShape.renderstyle.ignore_depth = true;
        _myFrameShape.name = "ASSFrame";

        if ( _mySensorFrame ) {
            _mySensorFrame.shape = _myFrameShape.id;
        } else {
            _mySensorFrame =  Modelling.createBody( _myGroup, _myFrameShape.id );
            _mySensorFrame.name = "ASSSensorFrame";
            _mySensorFrame.position.z = DECORATION_Z_OFFSET; // XXX
        }

        if ( _mySensorValueShape ) {
            _mySensorValueShape.parentNode.removeChild( _mySensorValueShape );   
        }

        var myValueMaterialId = getMaterialIdForValueDisplay();
        _mySensorValueShape = Modelling.createQuad(_myScene, myValueMaterialId, myPixelCenterOffset,
                                                   myTopRight);
        _mySensorValueShape.name = "ASSSensorValues";

        if ( _mySensorValueDisplay ) {
            _mySensorValueDisplay.shape = _mySensorValueShape.id;
        } else {
            _mySensorValueDisplay =  Modelling.createBody( _myGroup, _mySensorValueShape.id );
            _mySensorValueDisplay.position.y = _myGridSize3d.y - 1; //myTopRight.y;
            _mySensorValueDisplay.scale.y *= -1;
            _mySensorValueDisplay.name = "ASSSensorValueDisplay";
        }

        if ( ! _myOriginMarker ) {
            _myOriginMarker = Modelling.createBody( _myGroup, _myOriginShape.id );
            _myOriginMarker.scale = new Vector3f( ASS_CROSSHAIR_SCALE,
                    ASS_CROSSHAIR_SCALE, ASS_CROSSHAIR_SCALE );
            _myOriginMarker.name = "ASSOrigin";
            _myOriginMarker.position.z = DECORATION_Z_OFFSET; // XXX
        }

        if (_mySettings) {
            self.onUpdateSettings( _mySettings );
        }
    }

    function createWireGrid() {

        if (_myWireGridShape) {
            _myWireGridShape.parentNode.removeChild( _myWireGridShape );
        }
        
        var _myWireGridShape = Node.createElement("shape");
        _myScene.shapes.appendChild( _myWireGridShape );

        _myWireGridShape.name = "ASSWireGrid";

        var myVertexDataNode = Node.createElement("vertexdata");
        _myWireGridShape.appendChild( myVertexDataNode );

        var myPositions = Node.createElement("vectorofvector3f");
        myVertexDataNode.appendChild( myPositions );
        myPositions.name = "position";
        
        var myVertices = new Array();
        var i;
        for (i = 0; i < _myGridSize3d.x; ++i) {
            myVertices.push( new Vector3f( i, -0.5, 0) );
            myVertices.push( new Vector3f( i, _myGridSize3d.y - 0.5, 0) );
        }
        for (i = 0; i < _myGridSize3d.y; ++i) {
            myVertices.push( new Vector3f( -0.5, i, 0) );
            myVertices.push( new Vector3f( _myGridSize3d.x - 0.5, i, 0) );
        }
        myPositions.appendChild( Node.createTextNode( "[" + myVertices + "]"));
        
        var myPrimitivesNode = Node.createElement("primitives");
        _myWireGridShape.appendChild( myPrimitivesNode );

        var myElements = Node.createElement("elements");
        myPrimitivesNode.appendChild( myElements );
        myElements.type = "lines";
        myElements.material = _myOriginMaterial.id; // XXX

        var myIndicesNode = Node.createElement("indices");
        myElements.appendChild( myIndicesNode );
        myIndicesNode.vertexdata = "position";
        myIndicesNode.role = "position";

        var myIndices = new Array();
        for (i = 0; i < myVertices.length; ++i) {
            myIndices.push( i );
        }
        myIndicesNode.appendChild( Node.createTextNode( "[" + myIndices + "]"));

        if (_myWireGrid) {
            _myWireGrid.shape = _myWireGridShape.id;
        } else {
            _myWireGrid = Modelling.createBody( _myGroup, _myWireGridShape.id );
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

    var _myTouchList = new Array();
    var _myTouchMarkerMaterial = null;
    var _myTouchMarkerShape = null;

    var _myGridSize3d = new Vector3f(1, 1, 1);
    var _myGridSize =  new Vector3f(1, 1, 1);
    var _myFrameShape = null;
    var _mySensorFrame = null;
    var _mySensorValueShape = null;
    var _mySensorValueDisplay = null;
    var _myWireGridShape = null;
    var _myWireGrid = null;

    var _myInitialSettingsLoaded = false;
    var _myLastFrameTime = 0;

    setup(theParentTransform);
}


