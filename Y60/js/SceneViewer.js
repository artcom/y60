//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

// use this idiom in each level of inheritance and
// you'll know if you are the outermost .js file.
// (See ImageViewer and SliceViewer for an example of how to use this)
if (__main__ == undefined) var __main__ = "SceneViewer";

use("Y60JSSL.js");
use("Exception.js");
use("BaseViewer.js");

use("ClassicTrackballMover.js");
use("TrackballMover.js");
use("WalkMover.js");
use("FlyMover.js");
use("KeyMover.js");
use("Cursor.js");
use("LightManager.js");
use("DebugVisual.js");
use("AnimationManager.js");
use("ImageManager.js");
use("Timer.js");
use("Configurator.js");
use("shutter.js");
use("OnScreenDisplay.js");
use("MemoryMeter.js");
use("VideoRecorder.js");

//if (operatingSystem() == "WIN32") {
//    plug("y60QuicktimeDecoder"); // turn quicktime decoder on for windows and better mov decoder support
//}
//plug("y60FFMpegDecoder1");

// Global window object (similar to html window)
if (window == undefined) var window   = null;
if (renderer == undefined) var renderer = null;

var SETTINGS_FILE_NAMES = ["settings.xml", "CONFIG/settings.xml" ];

function SceneViewer(theArguments) {
    this.Constructor(this, theArguments);
}

SceneViewer.prototype.Constructor = function(self, theArguments) {
    BaseViewer.prototype.Constructor(self, theArguments);
    self.BaseViewer = [];

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    self.setCursor = function(theImageString, theSize) {
        setCursor(theImageString, theSize);
    }

    self.resetCursor = function() {
        resetCursor();
    }

    self.getSettings = function() {
        return _myConfigurator.getSettings();
    }

    self.getConfigurator = function() {
        return _myConfigurator;
    }

    self.registerSettingsListener = function(theListener, theSection) {
        _myConfigurator.addListener(theListener, theSection);
    }
    self.deregisterSettingsListener = function(theListener) {
        _myConfigurator.removeListener(theListener);
    }

    self.setPBufferSize = function(theWidth, theHeight) {
        _myPBufferHeight = theHeight;
        _myPBufferWidth = theWidth;
    }

    self.getCurrentTime = function(){
        return _myCurrentTime;
    }
    self.statisticColor setter = function(theColor) {
        _myStatisticColor = theColor;
    }
    self.current_time getter = function() {
        return _myCurrentTime;
    }

    self.getAnimationManager = function() {
        return _myAnimationManager;
    }

    self.getImageManager = function() {
        Logger.warning("ImageManager is deprecated.");
        return _myImageManager;
    }

    self.setSplashScreen = function(theFlag) {
        _mySplashScreenFlag = theFlag;
    }

    self.setMessage = function(theMessage, theLine) {
        if (!_myOnScreenDisplay) {
            _myOnScreenDisplay = new OnScreenDisplay(self);
        }
        _myOnScreenDisplay.setMessage(theMessage, theLine);
    }
    self.getOSD = function() {
        return _myOnScreenDisplay;
    }

    self.BaseViewer.onFrame = self.onFrame;
    self.onFrame = function(theTime) {        
        self.BaseViewer.onFrame(theTime);
        _myCurrentTime = theTime;
        if (_myShutter) {
            _myShutter.onFrame(theTime);
        }
        if (_myVideoRecorder) {
            _myVideoRecorder.onFrame(theTime);
        }
        var myCanvas = self.getActiveViewport().parentNode; //self.getRenderWindow().canvas;
        if (myCanvas) {
            for (var i=0; i < myCanvas.childNodesLength('viewport'); ++i) {
                var myMover = self.getMover(myCanvas.childNode('viewport'));
                if (myMover) {
                    myMover.onFrame(theTime);
                }
            }
        }
        if (_myAnimationManager) {
            _myAnimationManager.onFrame(theTime);
        }
        if (_myOnScreenDisplay) {
            _myOnScreenDisplay.onFrame(theTime);
        }
        if (_mySplashScreen) {
            fadeSplashScreen(theTime);
        }
        _myConfigurator.onFrame(theTime);
    }

    self.onPreRender = function() {
    }

    self.BaseViewer.onPostRender = self.onPostRender;
    self.onPostRender = function() {
        self.BaseViewer.onPostRender();
        if (_myAnimationManager) {
            _myAnimationManager.onPostRender();
        }
        if (_myOnScreenDisplay) {
            _myOnScreenDisplay.onPostRender();
        }
        if (_myMemoryMeter) {
            _myMemoryMeter.onPostRender();
        }
        if (_myOnScreenStatistics > 0) {
            showStatistics();
        }
    }

    self.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        var myMover = self.getMover(self.getActiveViewport());
    	if (myMover) {
            myMover.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
        if (!theAltFlag) {
            _myDebugVisual.onKey(theKey, theKeyState, theShiftFlag);
            self.getLightManager().onKey(theKey, theKeyState, theShiftFlag);
            _myAnimationManager.onKey(theKey, theKeyState, theShiftFlag);
            _myConfigurator.onKey(theKey, theKeyState, theShiftFlag);
        }
        if (_myShutter) {
            if (_myShutter.onKey(theKey, theKeyState, theShiftFlag, theCtrlFlag, theAltFlag)) {
                return;
            }
        }

        // theKeyState is true, if the key is pressed
        if (!theKeyState) {
            return;
        }

        if (theShiftFlag) {
            theKey = theKey.toUpperCase();
        }
        if (!theAltFlag) {
            switch (theKey) {
                case 'n':
                    self.getActiveViewport().drawnormals = !self.getActiveViewport().drawnormals;
                    print("Normals: " + (self.getActiveViewport().drawnormals ? "on" : "off"));
                    break;
                case 'm':
                    self.nextMover();
                    break;
                case 'F':
                    self.getActiveViewport().flatshading = !self.getActiveViewport().flatshading;
                    print("Flatshading: " + (self.getActiveViewport().flatshading ? "on" : "off"));
                    break;
                case 'w':
                    self.getActiveViewport().wireframe = !self.getActiveViewport().wireframe;
                    print("Wireframe: " + (self.getActiveViewport().wireframe ? "on" : "off"));
                    break;
                case 'b':
                    self.getActiveViewport().backfaceculling = !self.getActiveViewport().backfaceculling;
                    print("Backface culling: " + (self.getActiveViewport().backfaceculling ? "on" : "off"));
                    break;
                case 'c':
                    self.getActiveViewport().culling = !self.getActiveViewport().culling;
                    print("Culling: " + (self.getActiveViewport().culling ? "on" : "off"));
                    break;
                case 'C':
                    self.getActiveViewport().debugculling = !self.getActiveViewport().debugculling;
                    print("Debug culling: " + (self.getActiveViewport().debugculling ? "on" : "off"));
                    break;
                case 'V':
                    self.autoClicker.enabled = !self.autoClicker.enabled;
                    self.autoClicker.interval = 4;
                    break;
                case 'v':
                    cycleBoundingVolumeMode();
                    break;
                case 'B':
                    self.getScene().save("saved_scene.b60", true);
                    print("Saved scene to: saved_scene.b60");
                    break;
                case 'p':
                    window.pause = !window.pause;
                    print("Pause: " + (window.pause ? "on" : "off"));
                    break;
                case 'h':
                case 'H':
                    printHelp();
                    break;
                case 's':
                    _myOnScreenStatistics++;
                    if (_myOnScreenStatistics > 2) {
                        _myOnScreenStatistics = 0;
                    }
                    window.printStatistics();
                    var myCamera    = self.getActiveCamera(); //window.scene.dom.getElementById(window.canvas.childNode(0).camera);
                    print("  Scene size     " + window.scene.getWorldSize(myCamera).toFixed(1) + "m");
                    break;
                case 'S':
                    self.getScene().save("saved_scene.x60", false);
                    print("Saved scene to: saved_scene.x60");
                    break;
                case 'f':
                    _myFullscreen = !_myFullscreen;
                    window.resize(0, 0, _myFullscreen);
                    print("Fullscreen: " + (_myFullscreen ? "on" : "off"));
                    break;
                case 'X':
                    print(window.scene.world);
                    break;
                case "sys req":
                case "print screen":
                    var myDate = new Date();
                    var myFilename = "";
                    while (1) {
                        myFilename = "screenshot-";
                        myFilename += String(myDate.getFullYear()) + padStringFront(myDate.getMonth()+1, "0", 2) + padStringFront(myDate.getDate(), "0", 2);
                        myFilename += "-" + _myScreenShotCounter + ".png";
                        if (!fileExists(myFilename)) {
                            break;
                        }
                        _myScreenShotCounter++;
                    }
                    print("Saving screenshot as '" + myFilename + "'");
                    window.saveBuffer(myFilename);
                    break;
                case "PRINT SCREEN":
                case "SYS REQ":
                    if (!_myVideoRecorder) {
                        _myVideoRecorder = new VideoRecorder(25);
                    }
                    _myVideoRecorder.enabled = !_myVideoRecorder.enabled;
                    break;
                case 'M':
                    window.showMouseCursor = !window.showMouseCursor;
                    print("Mouse cursor: " + (window.showMouseCursor ? "on" : "off"));
                    break;
                case "g":
                    self.glow = !self.glow;
                    print("glow: " + (self.glow ? "on" : "off"));
                    break;
                case "t":
                    self.getActiveViewport().texturing = !self.getActiveViewport().texturing;
                    print("Texturing: " + (self.getActiveViewport().texturing ? "on" : "off"));
                    break;
                case '[-]':
                    var myCamera = self.getActiveCamera();
                    if (myCamera.frustum.hfov) { // persp camera
                        var myHfov = myCamera.frustum.hfov;
                        if (myHfov < 5) {
                            myHfov += 0.1;
                        } else if (myHfov < 175) {
                            myHfov += 5;
                        }
                        myCamera.frustum.hfov = myHfov;
                        print("Zoom to " + getFocalLength(myHfov).toFixed(1) + "mm (hfov: " + myHfov.toFixed(1) + ")");
                    } else {  // ortho camera
                        myCamera.width *= 1.1;
                        print("Zoom to width " + myCamera.width);
                    }
                    break;
                case '[+]':
                    var myCamera = self.getActiveCamera();
                    if (myCamera.frustum.hfov) { // persp camera
                        myHfov = myCamera.frustum.hfov;
                        if (myHfov > 5) {
                            myHfov -= 5;
                        } else if (myHfov > 0.1) {
                            myHfov -= 0.1;
                        }
                        myCamera.frustum.hfov = myHfov;
                        print("Zoom to " + getFocalLength(myHfov).toFixed(1) + "mm (hfov: " + myHfov.toFixed(1) + ")");
                    } else {  // ortho camera
                        myCamera.width /= 1.1;
                        print("Zoom to width " + myCamera.width);
                    }
                    break;
                case 'u':
                    if (!_myMemoryMeter) {
                        _myMemoryMeter = new MemoryMeter(self);
                    }
                    _myMemoryMeter.toggleEnableFlag();
                    break;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    //window.camera = self.setActiveCamera(Number(theKey));
                    self.setActiveCamera(Number(theKey));
                    print("Active camera: " + self.getActiveCamera().name);
                    break;
                case ".":
                    self.nextCamera();
                    print("Active camera: " + self.getActiveCamera().name);
                    break;
                case ",":
                    self.prevCamera();
                    print("Active camera: " + self.getActiveCamera().name);
                    break;
                case "e":
                    reuse();
                    print("All includes re-evaluated.");
                    break;
                case "i":
                    {
                        // Cycle swap interval
                        var mySwapInterval = window.swapInterval;
                        mySwapInterval++;
                        if (mySwapInterval > 4) {
                            mySwapInterval = 0;
                        }
                        window.swapInterval = mySwapInterval;
                        print("Swap interval=" + window.swapInterval);
                    }
                    break;
                case 'o':
                    print("Optimizing scene...");
                    window.scene.optimize();
                    break;
                case '#':
                    self.drawFrustums = ! self.drawFrustums;
                    print("Draw frustums: " + (self.drawFrustums ? "On" : "Off"));
                    break;
                default:
                    break;
            }
        }
    }

    self.BaseViewer.onMouseMotion = self.onMouseMotion;
    self.onMouseMotion = function(theX, theY) {
        self.BaseViewer.onMouseMotion(theX, theY);
        if (_myShutter && _myShutter.enabled()) {
            if (_myShutter.onMouseMotion(theX, theY)) {
                return;
            }
        }
    }
    self.BaseViewer.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        self.BaseViewer.onMouseButton(theButton, theState, theX, theY);
        if (_myShutter && _myShutter.enabled()) {
            if (_myShutter.onMouseButton(theButton, theState, theX, theY)) {
                return;
            }
        }
    }

    self.BaseViewer.onAxis = self.onAxis;
    self.onAxis = function(theDevice, theAxis, theValue) {
        self.BaseViewer.onAxis( theDevice, theAxis, theValue);
    }

    self.BaseViewer.onButton = self.onButton;
    self.onButton = function(theDevice, theButton, theState) {
        self.BaseViewer.onButton( theDevice, theButton, theState);
    }

    self.onResize = function(theNewWidth, theNewHeight) {
        if (_myShutter) {
            _myShutter.onResize();
        }
    }

    self.BaseViewer.onExit = self.onExit;
    self.onExit = function() {
        self.BaseViewer.onExit();
    }

    // Must be called before setup
    // Use the following constants:
    //     Renderer.TEXTURECOMPRESSION_SUPPORT [default]
    //     Renderer.MULTITEXTURE_SUPPORT       [default]
    //     Renderer.CUBEMAP_SUPPORT            [default]
    //     Renderer.CUBEMAP_SUPPORT            [default]
    //     Renderer.POINT_PARAMETER_SUPPORT
    //     Renderer.TEXTURE_3D_SUPPORT
    self.addRenderingCapability = function(theCapability) {
        _mySetDefaultRenderingCap = false;
        window.renderingCaps = window.renderingCaps | theCapability;
    }
    self.removeRenderingCapability = function(theCapability) {
        _mySetDefaultRenderingCap = false;
        window.renderingCaps = window.renderingCaps & (~theCapability);
    }

    self.setup = function(theWindowWidth, theWindowHeight, theFullscreen, theWindowTitle, theScene, theSwitchNodeFlag) {
        //print(theWindowWidth + " x " + theWindowHeight + " fullscreen: " + theFullscreen);
        if (window == null) {
            window = new RenderWindow();
        }

        if (theWindowWidth != null && theWindowHeight != null) {
            window.resize(theWindowWidth, theWindowHeight, theFullscreen);
        }

        // set the window behavior while sleeping
        window.renderWhileSleep = false;

        self.setupWindow(window, _mySetDefaultRenderingCap);
        var myScene;
        if (theScene) {
            myScene = theScene;
        } else {
            var myModelname = self.getModelName();
            if (myModelname) {
                myScene = new Scene(myModelname);
            } else {
                myScene = new Scene();
            }
            self.preprocessScene(myScene);
            myScene.setup();
        }
        var myCanvas = getDescendantByTagName(myScene.dom, 'canvas', true);
        self.setScene(myScene, myCanvas, theSwitchNodeFlag);
        renderer = window.getRenderer();

        // Turn on sync to V-Blank
        window.swapInterval = 1;

        // standard movers
        var myViewport = self.getRenderWindow().canvas.childNode("viewport");
        self.registerMover(CenteredTrackballMover);
        self.registerMover(TrackballMover);
        self.registerMover(ClassicTrackballMover);
        self.registerMover(FlyMover);
        self.registerMover(WalkMover);
        self.setMover(ClassicTrackballMover, myViewport);
        self.setActiveCamera(window.scene.dom.getElementById(myViewport.camera), myViewport);

        _myDebugVisual = new DebugVisual(myScene.world, self);

        _myAnimationManager = new AnimationManager(self);

        // [CH]: Deprecated, should be removed someday
        _myImageManager = new ImageManager(self);

        var mySettingsFile = "";
        for (var i = 0; i < SETTINGS_FILE_NAMES.length; ++i) {
            if ( fileExists( SETTINGS_FILE_NAMES[i] )) {
                mySettingsFile = SETTINGS_FILE_NAMES[i];
                break;
            }
        }
        _myConfigurator = new Configurator(self, mySettingsFile);

        if (theWindowTitle != null) {
            window.title = theWindowTitle;
        }

        if (_mySplashScreenFlag) {
            _mySplashScreen = new ImageOverlay(window.scene, "shadertex/ac_logo.png", [0, 0]);
            _mySplashScreen.name = "splashscreen";
            _mySplashScreen.position = new Vector2f((window.width - _mySplashScreen.width) / 2,
                                                    (window.height - _mySplashScreen.height) / 2);
        }
    }

    self.createShutter = function() {
        if (!_myShutter) {
            print("Creating shutter");
            _myShutter = new Shutter(self, new Vector4f(0,0,0,0));
            self.registerSettingsListener(_myShutter, "Shutter");
        }
    }

    self.go = function() {
        window.go();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    function fadeSplashScreen(theTime) {
        const START_FADE = 1;
        const STOP_FADE  = 3;
        _mySplashScreen.moveToTop();
        if (theTime > START_FADE && theTime < STOP_FADE) {
            _mySplashScreen.alpha = clamp((STOP_FADE - theTime) / (STOP_FADE - START_FADE), 0, 1);
        }
        if (theTime > STOP_FADE + 1) {
            _mySplashScreen.removeFromScene();
            _mySplashScreen = null;
        }
    }

    function showStatistics() {
        var myText = [];
        myText.push(window.fps.toFixed(1) + " swap=" + window.swapInterval);

        if (_myOnScreenStatistics > 1) {
            var myStatistics = window.scene.statistics;
            myText.push("Vertices:   " + myStatistics.renderedVertices + "/" + myStatistics.vertices);
            myText.push("Primitives: " + myStatistics.renderedPrimitives + "/" + myStatistics.primitives);
            myText.push("Worldnodes: " + myStatistics.worldNodes);
            myText.push("Bodies:     " + myStatistics.bodies);
            myText.push("Lights:     " + myStatistics.activeLights + "/" + myStatistics.lights);
            myText.push("Overlays:   " + myStatistics.overlays);
            myText.push("Materials:  " + myStatistics.materials);
        }

        for (var i = 0; i < myText.length; ++i) {
            window.setTextColor([0,0,0,1]);
            window.renderText([window.width - 201, 19 + (i * 15)], myText[i], "Screen13");
            window.setTextColor(_myStatisticColor);
            window.renderText([window.width - 200.8, 19.2 + (i * 15)], myText[i], "Screen13");
        }
    }

    function cycleBoundingVolumeMode() {
        if (renderer.boundingVolumeMode == Renderer.BV_NONE) {
            renderer.boundingVolumeMode = Renderer.BV_BODY;
            print("Bounding volume mode: per body");
        } else if (renderer.boundingVolumeMode == Renderer.BV_BODY) {
            renderer.boundingVolumeMode = Renderer.BV_SHAPE;
            print("Bounding volume mode: per shape");
        } else if (renderer.boundingVolumeMode == Renderer.BV_SHAPE) {
            renderer.boundingVolumeMode = Renderer.BV_HIERARCHY;
            print("Bounding volume mode: hierarchy");
        } else if (renderer.boundingVolumeMode == Renderer.BV_HIERARCHY) {
            renderer.boundingVolumeMode = Renderer.BV_NONE;
            print("Bounding volume mode: off");
        }
    }

    function printHelp() {
         print("Scene Viewer Keys:");
         print("  print        create screenshot named screenshot + date + counter, as png image");
         print("  shift-print  enables/disables video recorder");
         print("    w          toggle wireframe");
         print("    F          toggle flatshading");
         print("    t          toggle texturing");
         print("    b          toggle backface culling");
         print("    m          switch to next mover");
         print("    M          toggles mouse cursor visibility");
         print("    f          toggles fullscreen");
         print("    V          toggles autoClicker");
         print("    v          toggles bounding box visualization");
         print("    c          toggles culling");
         print("    C          toggles culling debug mode");
         print("    p          pause continous rendering");
         print("    s          print statistic");
         print("    S          save scene to file");
         print("    B          save scene to binary file");
         print("    q          quits the application");
         print("    X          dumps scene on console");
         print("    u          toggle used memory display");
         print("    e          re-evaluate all include-files");
         print("    i          Cycles window swap interval");
         print("    +          increase zoom factor");
         print("    -          decrease zoom factor");
         print("    ,          switch to previous camera");
         print("    .          switch to next camera");
         print("    #          draw camera frustums");
         print("   0-9         switch to camera number 0-9");
         print("    h          print this help");
    }

    var _myFullscreen            = false;

    var _myConfigurator          = null;
    var _myDebugVisual           = null;
    var _myAnimationManager      = null;
    var _myImageManager          = null;
    var _myScreenShotCounter     = 0;
    var _myPBufferHeight         = 0;
    var _myPBufferWidth          = 0;
    var _myCurrentTime           = 0;
    var _myTimer                 = new AutoTimer("SceneViewer Timer");
    var _mySplashScreen          = null;
    var _mySplashScreenFlag      = false;
    var _myShutter               = null;
    var _myOnScreenDisplay       = null;
    var _mySetDefaultRenderingCap= true;
    var _myMemoryMeter           = null;
    var _myOnScreenStatistics    = 0;
    var _myVideoRecorder         = null;
    var _myStatisticColor        = [1,1,1,1];
}
