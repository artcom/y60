//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: BaseViewer.js,v $
//   $Author: christian $
//   $Revision: 1.13 $
//   $Date: 2005/04/27 17:30:35 $
//
//
//=============================================================================

// use this idiom in each level of inheritance and
// you'll know if you are the outermost .js file.
if (__main__ == undefined) var __main__ = "SceneViewer";

use("Y60JSSL.js");
use("Exception.js");
use("LightManager.js");
use("picking_functions.js");
use("HeartbeatThrober.js");


function BaseViewer(theArguments) {
    this.Constructor(this, theArguments);
}

BaseViewer.prototype.Constructor = function(self, theArguments) {

    self.getReleaseMode = function() {
        return _myReleaseMode;
    }
    self.getShaderLibrary = function() {
        return _myShaderLibrary;
    }

    self.getModelName = function() {
        return _myModelName;
    }

    self.setModelName = function(theModelName) {
        _myModelName = theModelName;
    }

    // Use these functions for fast access to the scene graph
    self.getWorld = function() {
        return _myWorld;
    }
    self.getMaterials = function() {
        return _myMaterials;
    }
    self.getLightSources = function() {
        return _myLightSources;
    }
    self.getShapes = function() {
        return _myShapes;
    }
    self.getAnimations = function() {
        return _myAnimations;
    }
    self.getCharacters = function() {
        return _myCharacters;
    }
    self.getImages = function() {
        return _myImages;
    }

    self.getLightManager = function() {
        return _myLightManager;
    }

    self.getRenderWindow = function() {
        return _myRenderWindow;
    }

    self.getActiveViewport = function() {
        return _activeViewport;
    }

    self.attachWindow = function(theRenderWindow) {
        _myRenderWindow = theRenderWindow;
        // register our event listener
        _myRenderWindow.eventListener = self;
        self._myPicking = new Picking(_myRenderWindow);
    }

    self.setupWindow = function(theRenderWindow, theSetDefaultRenderingCap) {
        self.attachWindow(theRenderWindow);
        if (theSetDefaultRenderingCap == undefined || theSetDefaultRenderingCap) {
            _myRenderWindow.renderingCaps =
                Renderer.TEXTURE_3D_SUPPORT |
                Renderer.MULTITEXTURE_SUPPORT |
                Renderer.TEXTURECOMPRESSION_SUPPORT |
                Renderer.CUBEMAP_SUPPORT;
        }
   }

   self.getScene = function() {
       return _myRenderWindow.scene;
   }

   self.setScene = function(theScene, theCanvas) {
       var myStatus = _myRenderWindow.setScene(theScene);
       if (!myStatus) {
           throw new Exception("Could not load model", fileline());
       }
       if (!theScene) {
           Logger.debug("setScene(null)");
           self.prepareScene(null, null);
       } else {
           var myCanvas = theCanvas ? theCanvas : getDescendantByTagName(theScene.dom, 'canvas', true);
           self.prepareScene(theScene, myCanvas);
       }
   }

    self.setMover = function(theMoverFactory, theViewport) {
        var myNewMover = new theMoverFactory(theViewport);
        var myViewportId = getViewportIndex(theViewport);
        _myViewportMovers[myViewportId] = myNewMover;
        myNewMover.setMoverObject(myNewMover.getViewportCamera());
        return myNewMover;
    }

    self.getMover = function(theViewport) {
        var myIndex = getViewportIndex(theViewport);
        if (myIndex in _myViewportMovers) {
            return _myViewportMovers[myIndex];
        } else {
            return null;
        }
    }

    self.registerMover = function(theMoverFactory) {
        // should check if object type is already in list
        _myMoverFactories.push(theMoverFactory);
    }

    self.nextMover = function(theViewport) {
        var myViewportId = getViewportIndex(theViewport);

    	if (_myMoverFactories.length == 0) {
    	    return;
    	}

        // find next mover
        var myNextMoverIndex = 0;
        for (var i = 0; i < _myMoverFactories.length; ++i) {
            if (_myMoverFactories[i] == _myViewportMovers[myViewportId].constructor) {
                myNextMoverIndex = i+1;
                break;
            }
        }
        if (myNextMoverIndex >= _myMoverFactories.length) {
            myNextMoverIndex = 0;
        }

        // switch mover
        var myNewMover = self.setMover(_myMoverFactories[myNextMoverIndex]);
        print("Activated Mover: " + myNewMover.name);

    }

    self.setAutoNearFarPlane = function(theFlag) {
        _myAutoNearFarFlag = theFlag;
    }

    self.setCanvas = function(theCanvasNode) {
        if (theCanvasNode != _myRenderWindow.canvas) {
            _myRenderWindow.canvas = theCanvasNode;
        }
        _myLightManager.setupDefaultLighting(theCanvasNode);
        for (var i=0; i < theCanvasNode.childNodesLength('viewport'); ++i) {
            var myViewport = theCanvasNode.childNode('viewport');
            _myLightManager.setupHeadlight(myViewport);
        }
        _activeViewport = getDescendantByTagName(_myRenderWindow.canvas, 'viewport');
    }

    self.setCanvasByIndex = function(theIndex) {
        var myCanvasRoot = getDescendantByTagName(_myRenderWindow.scene.dom, "canvases", true);
        var myDefaultCamera = getDescendantByTagName(_myRenderWindow.scene.dom, "camera", true);
        if (myCanvasRoot.childNodes.length > theIndex) {
            self.setCanvas(myCanvasRoot.childNodes[theIndex]);
        } else {
            var myCanvas = Modelling.createCanvas(_myRenderWindow.scene, "AutoCanvas");
            var myViewport = myCanvas.childNode(0);
            myViewport.camera = myDefaultCamera.id;
            self.setCanvas(myCanvas);
        }
    }

    self.nextCamera = function(theViewport) {
        return self.setActiveCamera(_myActiveCameraIndex+1, theViewport);
    }
    self.prevCamera = function(theViewport) {
        return self.setActiveCamera(_myActiveCameraIndex-1, theViewport);
    }

    self.setActiveCamera = function(theCamera, theViewport) {
        if (!isNaN(theCamera)) {
            var myCameraIndex = theCamera;
            if (myCameraIndex < 0) {
                myCameraIndex = _myRenderWindow.scene.cameras.length-1;
            } else if (myCameraIndex > (_myRenderWindow.scene.cameras.length-1)) {
                myCameraIndex = 0;
            }
            try {
                theCamera = _myRenderWindow.scene.cameras[myCameraIndex];
                _myActiveCameraIndex = myCameraIndex;
            }
            catch (ex) {
                Logger.warning("No such camera index " + myCameraIndex);
                theCamera = _myRenderWindow.camera;
            }
        }
        if (!theViewport) {
            theViewport = getSingleViewport();
        }
        theViewport.camera = theCamera.id;

        if (self.getMover(theViewport)) {
            self.getMover(theViewport).setMoverObject(theCamera);
        }
        _myLightManager.setupHeadlight(theViewport);
        return theCamera;
    }

    self.addSkyBoxFromImage = function(theImageNode) {
        var myMaterialId = createUniqueId();
        var mySkyboxString =
            '<material name="background" id="' + myMaterialId + '">\n' +
            '    <properties>\n' +
            '        <vector4f name="diffuse">[1,1,1,1]</vector4f>\n' +
            '        <vector4f name="ambient">[0,0,0,1]</vector4f>\n' +
            '    </properties>\n' +
            '    <textures>\n' +
            '        <texture image="' + theImageNode.id + '" applymode="decal" wrapmode="repeat" />\n' +
            '    </textures>\n' +
            '    <requires>\n' +
            '    <feature class="lighting" values="[10[unlit]]"/>\n' +
            '        <feature class="textures" values="[100[skybox]]"/>\n' +
            '    </requires>\n' +
            '</material>';


        var mySkyboxDoc = new Node(mySkyboxString);
        _mySkyboxMaterial = mySkyboxDoc.firstChild;
        self.getMaterials().appendChild(_mySkyboxMaterial);
        self.getWorld().skyboxmaterial = myMaterialId;
        self.getScene().update(Scene.IMAGES);
    }

    self.addSkyBoxFromFile = function(theFileName, theTile) {
        if (theTile == undefined) {
            theTile = new Vector2i(1,6);
        }

        if (_mySkyboxMaterial) {
            var mySkyboxImage = _mySkyboxMaterial.getElementById(_mySkyboxMaterial.childNode("textures").firstChild.image);
            mySkyboxImage.src = theFileName;
            mySkyboxImage.tile = theTile;
            self.getWorld().skyboxmaterial = _mySkyboxMaterial.id;
        } else {
            var myImageId = createUniqueId();
            var mySkyboxImageString =
                '<image name="' + theFileName + '" id="' + myImageId + '" src="' + theFileName + '" type="cubemap" pixelformat="bgr" mipmap="0" tile="' + theTile + '"/>\n';
            var mySkyboxImageDoc = new Node(mySkyboxImageString);
            var mySkyboxImage = mySkyboxImageDoc.firstChild;
            self.getImages().appendChild(mySkyboxImage);
            self.addSkyBoxFromImage(mySkyboxImage);
        }
    }

    self.removeSkyBox = function() {
        self.getWorld().skyboxmaterial = "";
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    //  RenderWindow callback handlers
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    self.onPreRender = function() {
    }

    self.onPreViewport = function(theViewport) {
        var myCamera = theViewport.getElementById(theViewport.camera);
    	// calculate near/far planes from world size and distance camera-world
    	var myWorldSize = _myRenderWindow.getWorldSize(myCamera);
    	if (self.getMover(theViewport)) {
            self.getMover(theViewport).setWorldSize(myWorldSize);
        }
        if (_myAutoNearFarFlag) {
            var myNearPlane = 0.1;
            var myFarPlane = 1000.0;
            if (myWorldSize > 0.0) {
                myNearPlane = myWorldSize / 10000.0;
                myFarPlane = myWorldSize * 2.0;
            }
            myCamera.nearplane = myNearPlane;
            myCamera.farplane = myFarPlane;
        }
        _myLightManager.onPreViewport(theViewport);
    }

    self.onPostViewport = function(theViewport) {
        _myLightManager.onPostViewport(theViewport);
    }

    self.onPostRender = function() {
    }

    self.onResize = function() {
    }

    self.onIdle = function(theTime) {
        _myHeartbeatThrober.throb(theTime);
        _myLightManager.onIdle(theTime);
    }

    self.onMouseMotion = function(theX, theY) {
        var myViewportUnderMouse = self._myPicking.getViewportAt(theX, theY);
        if (myViewportUnderMouse && myViewportUnderMouse != _myClickedViewport && self.getMover(myViewportUnderMouse)) {
            self.getMover(myViewportUnderMouse).onMouseMotion(theX, theY);
        }
        if (_myClickedViewport && self.getMover(_myClickedViewport)) {
            self.getMover(_myClickedViewport).onMouseMotion(theX, theY);
        }
    }
    self.onMouseButton = function(theButton, theState, theX, theY) {
        var myMover = null;
        if (theState) {
            var myViewport = self._myPicking.getViewportAt(theX, theY);
            myMover = self.getMover(myViewport);
            _myClickedViewport = myViewport;
        } else {
            var myViewportUnderMouse = self._myPicking.getViewportAt(theX, theY);
            if (myViewportUnderMouse) {
                if (_myClickedViewport) {
                    if (myViewportUnderMouse.id != _myClickedViewport.id) {
                        self.getMover(myViewportUnderMouse).onMouseButton(theButton, theState, theX, theY);
                    }
                    myMover = self.getMover(_myClickedViewport);
                    _myClickedViewport = null;
                } else {
                    myMover = self.getMover(myViewportUnderMouse);
                }
            }
        }
        if (myMover) {
            myMover.onMouseButton(theButton, theState, theX, theY);
        }
    }
    self.onMouseWheel = function(theDeltaX, theDeltaY) {
        var myMover = self.getMover(self.getActiveViewport());
        if (myMover) {
            myMover.onMouseWheel(theDeltaX, theDeltaY);
        }
    }

    self.enableHeartbeat = function(theFrequency, theHeartbeatfile) {
        _myHeartbeatThrober.use(true, theFrequency, theHeartbeatfile);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    var _myRenderWindow          = null;
    var _myActiveCameraIndex     = 0;
    var _myModelName             = null;
    var _myShaderLibrary         = null;
    var _myReleaseMode           = true;
    var _myLightManager          = null;

    // Camera movers
    var _myMoverFactories        = [];  // Array of mover constructors

    var _myViewportMovers        = [];
    var _myClickedViewport       = null;
    var _myAutoNearFarFlag       = true;
    var _activeViewport          = null;

    // For fast scene access
    var _myWorld                 = null;
    var _myMaterials             = null;
    var _myLightSources          = null;
    var _myShapes                = null;
    var _myCharacters            = null;
    var _myAnimations            = null;
    var _myImages                = null;

    var _mySkyboxMaterial        = null;
    var _myHeartbeatThrober      = new HeartbeatThrober(false, 10, "${TEMP}/heartbeat.xml");
    self._myPicking               = null;

    function getSingleViewport() {
        if (_myRenderWindow.canvas.childNodesLength("viewport") == 1) {
            return _myRenderWindow.canvas.childNode("viewport");
        }
        Logger.warning("getSingleViewport called without a viewport in non-single viewport mode.");
        return null;
    }


    function getViewportIndex(theViewport) {
        if (theViewport) {
            return theViewport.id;
        }
        return getSingleViewport().id;
    }

    function parseArguments(theArguments) {
        var myArgumentMap = [];
        for (var i = 0; i < theArguments.length; ++i) {
            var myArgument = theArguments[i];
            if (!_myShaderLibrary && myArgument.search(/xml/) != -1) {
                // Take the first xml-file as shader library
                _myShaderLibrary = myArgument;
            } else if (myArgument.search(/\.[xb]60$/) != -1 ||
                       myArgument.search(/\.st.$/) != -1 ||
                       myArgument.search(/\.x3d$/) != -1)
            {
                _myModelName = myArgument;
            } else if (myArgument == "rehearsal") {
                _myReleaseMode = false;
            }

            myArgument = myArgument.split("=");
            if (myArgument.length > 1) {
                myArgumentMap[myArgument[0]] = myArgument[1];
            } else {
                myArgumentMap[myArgument[0]] = null;
            }
        }

        if (!_myShaderLibrary) {
            throw new Exception("Missing shaderlibrary argument", fileline());
        }

        return myArgumentMap;
    }

   self.prepareScene = function (theScene, theCanvas) {

        if (!theScene) {
            Logger.debug("No Scene found.");
        }

        if (theScene) {
            // Cache main scene nodes for fast access
            var myWorlds    = getDescendantByTagName(theScene.dom, "worlds", false);
            _myWorld        = getDescendantByTagName(myWorlds,     "world", false);
            _myMaterials    = getDescendantByTagName(theScene.dom, "materials", false);
            _myLightSources = getDescendantByTagName(theScene.dom, "lightsources", false);
            _myAnimations   = getDescendantByTagName(theScene.dom, "animations", false);
            _myCharacters   = getDescendantByTagName(theScene.dom, "characters", false);
            _myShapes       = getDescendantByTagName(theScene.dom, "shapes", false);
            _myImages       = getDescendantByTagName(theScene.dom, "images", false);

            if (!_myWorld || !_myMaterials || !_myLightSources || !_myCharacters || !_myAnimations || !_myShapes) {
                throw new Exception("Could not find world, materials, lightsources or shapes node", fileline());
            }
            _myLightManager = new LightManager(theScene, _myWorld);
            if (theCanvas) {
                self.setCanvas(theCanvas);
            } else {
                self.setCanvas(_myRenderWindow.canvas);
            }
        } else {
            Logger.trace("prepareScene has no scene");

            // No scene
            _myWorld = null;
            _myMaterials = null;
            _myLightSources = null;
            _myAnimations = null;
            _myCharacters = null;
            _myShapes = null;
            _myImages = null;
        }
    }



    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    if (theArguments != null) {
        self.arguments = parseArguments(theArguments);
    }
    var myShaderLibrary = self.getShaderLibrary();
    if (myShaderLibrary) {
        GLResourceManager.loadShaderLibrary(myShaderLibrary);
    } else {
        // Logger.warning("No Shaderlibrary found. Reload manually");
    }
}
