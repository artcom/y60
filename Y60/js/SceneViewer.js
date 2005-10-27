//=============================================================================
// Copyright (C) 2003-2005, ART+COM AG Berlin
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
use("OverlayManager.js");
use("Timer.js");
use("Configurator.js");
use("shutter.js");
use("OnScreenDisplay.js");
use("MemoryMeter.js");

//plug("y60QuicktimeDecoder"); // turn on for windows and better mov decoder support
//plug("y60FFMpegDecoder"); // turn on for windows and better mov decoder support

// Global window object (similar to html window)
if (window == undefined) var window   = null;
if (renderer == undefined) var renderer = null;

var SETTINGS_FILE_NAME = "settings.xml";

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

    self.registerSettingsListener = function(theListener, theSection) {
        _myConfigurator.addListener(theListener, theSection);
    }

    self.setPBufferSize = function(theWidth, theHeight) {
        _myPBufferHeight = theHeight;
        _myPBufferWidth = theWidth;
    }

    self.getCurrentTime = function(){
        return _myCurrentTime;
    }

    self.getAnimationManager = function() {
        return _myAnimationManager;
    }

    self.getImageManager = function() {
        return _myImageManager;
    }

    self.getOverlayManager = function() {
        return _myOverlayManager;
    }

    self.setSplashScreen = function(theFlag) {
        _mySplashScreen = theFlag;
    }
    self.setMessage = function(theMessage, theLine) {
        _myOnScreenDisplay.setMessage(theMessage, theLine);
    }

    function fadeSplashScreen(theTime) {
        if (_mySplashScreen && _mySplashScreenOverlay) {
            const START_FADE = 1;
            const STOP_FADE  = 3;
            _mySplashScreenOverlay.moveToTop();
            if (theTime > START_FADE && theTime < STOP_FADE) {
                _mySplashScreenOverlay.alpha = clamp((STOP_FADE - theTime) / (STOP_FADE - START_FADE), 0, 1);
            }
            if (theTime > STOP_FADE + 1) {
                _mySplashScreenOverlay.removeFromScene();
                _mySplashScreen = false;
                _mySplashScreenOverlay = null;
            }
        }
    }

    self.BaseViewer.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        self.BaseViewer.onFrame(theTime);
        _myCurrentTime = theTime;
        if (_myShutter) {
            _myShutter.onFrame(theTime);
        }
        var myCanvas = self.getRenderWindow().canvas;
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

        _myOnScreenDisplay.onFrame(theTime);
        fadeSplashScreen(theTime);

        if (_myStatisticCycle != 0) {
            window.setTextColor(new Vector4f(1,0,0,1), new Vector4f(1,0,1,1));
             window.printStatistics(RenderWindow.RENDERED_STATISTIC, _myStatisticCycle);
        }
   }

    self.BaseViewer.onPreRender = self.onPreRender;
    self.onPreRender = function() {
        //var myImagesCount = self.getImages().childNodesLength();
        //if (myImagesCount > 0) {
            //var myImage = self.getImages().childNodes[0];
        //}
        self.BaseViewer.onPreRender();
    }

    self.BaseViewer.onPostRender = self.onPostRender;
    self.onPostRender = function() {
        self.BaseViewer.onPostRender();
        if (_myAnimationManager) {
            _myAnimationManager.onPostRender();
        }
        _myOnScreenDisplay.onPostRender();
        _myMemoryMeter.onPostRender();
    }

    self.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        var myMover = self.getMover(self.getActiveViewport());
    	if (myMover) {
            myMover.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
        if (!theAltFlag) {
            _myDebugVisual.onKey(theKey, theKeyState);
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
            return true;
        }

        if (theShiftFlag) {
            theKey = theKey.toUpperCase();
        }
        if (!theAltFlag) {
            switch (theKey) {
                /*
                case 'd':
                   print (self.getWorld().childNode("body").rootNode);
                    break;
                */
                case 'n':
                    window.drawnormals = !window.drawnormals;
                    print("Normals: " + (window.drawnormals ? "on" : "off"));
                    break;
                case 'm':
                    self.nextMover();
                    break;
                case 'F':
                    window.flatshading = !window.flatshading;
                    print("Flatshading: " + (window.flatshading ? "on" : "off"));
                    break;
                case 'w':
                    window.wireframe = !window.wireframe;
                    print("Wireframe: " + (window.wireframe ? "on" : "off"));
                    break;
                case 'b':
                    window.backfaceCulling = !window.backfaceCulling;
                    print("Backface culling: " + (window.backfaceCulling ? "on" : "off"));
                    break;
                case 'c':
                    window.culling = !window.culling;
                    print("Culling: " + (window.culling ? "on" : "off"));
                    break;
                case 'C':
                    window.debugCulling = !window.debugCulling;
                    print("Debug culling: " + (window.debugCulling ? "on" : "off"));
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
                    _myStatisticCycle++;
                    // check if statisticCycle triggers a valid statistic rendering
                    // print text statistic anyway
                    var myIsValidStatisticLevel = window.printStatistics(RenderWindow.RENDERED_STATISTIC | RenderWindow.TEXT_STATISTIC, _myStatisticCycle);
                    if (!myIsValidStatisticLevel) {
                        _myStatisticCycle = 0;
                    }
                    print("  Scene size     " + window.getWorldSize(window.camera).toFixed(1) + "m");
                    break;
                case 'S':
                    self.getScene().save("saved_scene.x60", false);
                    print("Saved scene to: saved_scene.x60");
                    break;
                case 'f':
                    _myFullscreen = !_myFullscreen;
                    window.setVideoMode (0, 0, _myFullscreen);
                    print("Fullscreen: " + (_myFullscreen ? "on" : "off"));
                    break;
                case 'X':
                    var mySceneNode = window.scene.dom;
                    print(mySceneNode.firstChild);
                    break;
                case "sys req":
                case "print screen":
                    var myDate = new Date();
                    print("save screenshot as: " + "screenshot_" + myDate.getDate() + "." +
                        (myDate.getMonth()+1) + "_" + _myScreenShotCounter + ".png");
                    window.saveBuffer("FRAMEBUFFER", "screenshot_" + myDate.getDate() + "." +
                                    (myDate.getMonth()+1) + "_" + _myScreenShotCounter + ".png");
                    _myScreenShotCounter++;
                    break;
                case 'M':
                    window.showMouseCursor = !window.showMouseCursor;
                    print("Mouse cursor: " + (window.showMouseCursor ? "on" : "off"));
                    break;
                case "t":
                    window.texturing = !window.texturing;
                    print("Texturing: " + (window.texturing ? "on" : "off"));
                    break;
                case '[-]':
                    var myHfov = window.camera.hfov;
                    if (myHfov < 5) {
                        myHfov += 0.1;
                    } else if (myHfov < 175) {
                        myHfov += 5;
                    }
                    window.camera.hfov = myHfov;
                    print("Zoom to " + getFocalLength(myHfov).toFixed(1) + "mm (hfov: " + myHfov.toFixed(1) + ")");
                    break;
                case '[+]':
                    myHfov = window.camera.hfov;
                    if (myHfov > 5) {
                        myHfov -= 5;
                    } else if (myHfov > 0.1) {
                        myHfov -= 0.1;
                    }
                    window.camera.hfov = myHfov;
                    print("Zoom to " + getFocalLength(myHfov).toFixed(1) + "mm (hfov: " + myHfov.toFixed(1) + ")");
                    break;
                case 'u':
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
                    window.camera = self.setActiveCamera(Number(theKey));
                    print("Active camera: " + window.camera.name);
                    break;
                case ".":
                    self.nextCamera();
                    print("Active camera: " + window.camera.name);
                    break;
                case ",":
                    self.prevCamera();
                    print("Active camera: " + window.camera.name);
                    break;
                case "e":
                    reuse();
                    print("All includes re-evaluated.");
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

    self.onAxis = function(device, axis, value) {
    }

    self.onButton = function(theDevice, theButton, theState) {
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
    self.setup = function(theWindowWidth, theWindowHeight, theFullscreen, theWindowTitle, theScene) {
        //print(theWindowWidth + " x " + theWindowHeight + " fullscreen: " + theFullscreen);
        if (window == null) {
            window = new RenderWindow();
        }

        if (theWindowWidth != null && theWindowHeight != null) {
            window.setVideoMode(theWindowWidth, theWindowHeight, theFullscreen);
        }

        // set the window behavior while sleeping
        window.renderWhileSleep = false;

        self.setupWindow(window, _mySetDefaultRenderingCap);

        var myScene;
        if (theScene) {
            myScene = theScene;
        } else {
            myScene = new Scene(self.getModelName());
        }
        var myCanvas = getDescendantByTagName(myScene.dom, 'canvas', true);
        self.setScene(myScene, myCanvas);
        renderer = window.getRenderer();

        // standard movers
        self.registerMover(TrackballMover);
        self.registerMover(ClassicTrackballMover);
        self.registerMover(FlyMover);
        self.registerMover(WalkMover);
        self.setMover(ClassicTrackballMover);
        self.setActiveCamera(window.camera);

        _myDebugVisual = new DebugVisual(self.getWorld(), self);

        _myAnimationManager = new AnimationManager(self);
        _myImageManager = new ImageManager(self.getImages());
        var myViewport = self.getRenderWindow().canvas.childNode("viewport");
        _myOverlayManager = new OverlayManager(self.getScene(), myViewport);

        if (_mySplashScreen) {
            showSplashScreen();
        }
        _myConfigurator = new Configurator(self, SETTINGS_FILE_NAME);

        if (theWindowTitle != null) {
            window.title = theWindowTitle;
        }
    }

    self.createShutter = function() {
        if (!_myShutter) {
            print("Creating shutter");
            _myShutter = new Shutter(self, new Vector4f(0,0,0,0));
            self.registerSettingsListener(_myShutter, "Global");
        }
    }

    self.go = function() {
        window.go();
    }

    function showSplashScreen() {
        _mySplashScreenOverlay = new ImageOverlay(_myOverlayManager, "shadertex/ac_logo.png", [0, 0]);
        _mySplashScreenOverlay.name = "splashscreen";
        //TODO viewport/canvas
        _mySplashScreenOverlay.position = new Vector2f((window.width - _mySplashScreenOverlay.width) / 2,
                                                       (window.height - _mySplashScreenOverlay.height) / 2);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // private members
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

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
         print("  print  create screenshot named screenshot + date + counter, as png image");
         print("    w    toggle wireframe");
         print("    F    toggle flatshading");
         print("    t    toggle texturing");
         print("    b    toggle backface culling");
         print("    m    switch to next mover");
         print("    M    toggles mouse cursor visibility");
         print("    f    toggles fullscreen");
         print("    v    toggles bounding box visualization");
         print("    c    toggles culling");
         print("    C    toggles culling debug mode");
         print("    p    pause continous rendering");
         print("    s    print statistic");
         print("    S    save scene to file");
         print("    B    save scene to binary file");
         print("    q    quits the application");
         print("    X    dumps scene on console");
         print("    u    toggle used memory display");
         print("    e    re-evaluate all include-files");
         print("    +    increase zoom factor");
         print("    -    decrease zoom factor");
         print("    ,    switch to previous camera");
         print("    .    switch to next camera");
         print("   0-9   switch to camera number 0-9");
         print("    h    print this help");
    }

    var _myFullscreen            = false;

    var _myConfigurator          = null;
    var _myDebugVisual           = null;
    var _myAnimationManager      = null;
    var _myImageManager          = null;
    var _myOverlayManager        = null;
    var _myScreenShotCounter     = 0;
    var _myEdgeBlending          = false;
    var _myEdgeBlendingAreaWidth = 50;
    var _myEdgeBlendingPoints    = 10;
    var _myEdgeBlendingGamma     = 1.0;
    var _myPBufferHeight         = 0;
    var _myPBufferWidth          = 0;
    var _myCurrentTime           = 0;
    var _myStatisticCycle        = 0;
    var _myTimer                 = new AutoTimer("SceneViewer Timer");
    var _mySplashScreen          = false; // self.getReleaseMode();
    var _mySplashScreenOverlay   = null;
    var _myShutter               = null;
    var _myOnScreenDisplay       = new OnScreenDisplay(self);
    var _mySetDefaultRenderingCap= true;
    var _myMemoryMeter           = new MemoryMeter(self);
}
