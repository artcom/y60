/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

/*jslint nomen:false, plusplus:false, bitwise:false*/
/*globals use, Logger, BaseViewer, AutoTimer, resetCursor, setCursor,
          PerfMeter, OnScreenDisplay, clamp, Renderer, print, hostname,
          fileExists, Scene, reuse, padStringFront, VideoRecorder,
          getFocalLength, WalkMover, FlyMover, TrackballMover,
          ClassicTrackballMover, CenteredTrackballMover, RenderWindow,
          ImageOverlay, Vector2f, Vector4f, Configurator, Shutter,
          ImageManager, AnimationManager, DebugVisual, MemoryMeter,
          TextureAtlasManager*/

// use this idiom in each level of inheritance and
// you'll know if you are the outermost .js file.
// (See ImageViewer and SliceViewer for an example of how to use this)
var __main__ = __main__ || "SceneViewer";

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
use("TextureAtlasManager.js");
use("Timer.js");
use("shutter.js");
use("OnScreenDisplay.js");
use("MemoryMeter.js");
use("VideoRecorder.js");
use("PerfMeter.js");
use("Ruler.js");

// Global window object (similar to html window)
var window = window || null;
var renderer = renderer || null;

function SceneViewer(theArguments) {
    this.Constructor(this, theArguments);
}

SceneViewer.prototype.Constructor = function (self, theArguments) {
    BaseViewer.prototype.Constructor(self, theArguments);
    self.BaseViewer = [];

    /////////////////////
    // Private Members //
    /////////////////////

    var _myFullscreen             = false;
    var _myConfigurator           = null;
    var _myDebugVisual            = null;
    var _myAnimationManager       = null;
    var _myImageManager           = null;
    var _myScreenshotCount        = 0;
    var _myCurrentTime            = 0;
    //var _myTimer                  = new AutoTimer("SceneViewer Timer");
    var _mySplashScreen           = null;
    var _mySplashScreenFlag       = false;
    var _myShutter                = null;
    var _myOnScreenDisplay        = null;
    var _mySetDefaultRenderingCap = true;
    var _myMemoryMeter            = null;
    var _myPerfMeter              = null;
    var _myOnScreenStatistics     = 0;
    var _myVideoRecorder          = null;
    var _myTextStyle              = new Node("<style textColor='[1,1,1,1]'/>").childNode(0);
    var _myStatisticPositionX     = null;
    var _myStatisticPositionY     = null;
    var _mySinceLastVersion       = "1";
    var _myRuler                  = null;

    ////////////////////
    // Public Members //
    ////////////////////

    self.configuratorClass = null;

    /////////////////////
    // Private Methods //
    /////////////////////

    function fadeSplashScreen(theTime) {
        var START_FADE = 1;
        var STOP_FADE  = 3;
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
            myText.push("Vertices:     " + myStatistics.renderedVertices);
            myText.push("Primitives:   " + myStatistics.renderedPrimitives);
            myText.push("Bodies:       " + myStatistics.bodies);
            myText.push("ActiveWorlds: " + myStatistics.activeWorlds);
            myText.push("ActiveLights: " + myStatistics.activeLights);
            myText.push("Overlays:     " + myStatistics.overlays);
            myText.push("Materials:    " + window.scene.materials.childNodesLength());
            myText.push("MaterialChange: " + myStatistics.materialChange);
            myText.push("gc:  " + myStatistics.gc.toFixed(3));
        }

        var myViewport = self.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel
        for (var i = 0; i < myText.length; ++i) {
            window.renderText([_myStatisticPositionX ? _myStatisticPositionX : (myViewport.size[0] * window.width) - 201, _myStatisticPositionY ? _myStatisticPositionY + (i * 15) : 19 + (i * 15)], myText[i], new Node("<style textColor='[0,0,0,1]'/>").childNode(0), "Screen13", myViewport);
            window.renderText([_myStatisticPositionX ? _myStatisticPositionX : (myViewport.size[0] * window.width) - 200.8, _myStatisticPositionY ? _myStatisticPositionY + (i * 15) : 19.2 + (i * 15)], myText[i], _myTextStyle, "Screen13", myViewport);
        }
    }

    function cycleBoundingVolumeMode() {
        if (renderer.boundingVolumeMode === Renderer.BV_NONE) {
            renderer.boundingVolumeMode = Renderer.BV_BODY;
            print("Bounding volume mode: per body");
        } else if (renderer.boundingVolumeMode === Renderer.BV_BODY) {
            renderer.boundingVolumeMode = Renderer.BV_SHAPE;
            print("Bounding volume mode: per shape");
        } else if (renderer.boundingVolumeMode === Renderer.BV_SHAPE) {
            renderer.boundingVolumeMode = Renderer.BV_HIERARCHY;
            print("Bounding volume mode: hierarchy");
        } else if (renderer.boundingVolumeMode === Renderer.BV_HIERARCHY) {
            renderer.boundingVolumeMode = Renderer.BV_NONE;
            print("Bounding volume mode: off");
        }
    }

    function printHelp() {
        print("Scene Viewer Keys:");
        print("  print or W   write screenshot as png image to the current directory");
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
        print("    O          save scene to d60 file");
        print("    q          quits the application");
        print("    X          dumps scene on console");
        print("    P          prints a list of all nodes that have changed since last print");
        print("    U          flush currently unused nodes in lazy loading mode");
        print("    u          toggle used memory display");
        print("    T          toggle timing display");
        print("    e          re-evaluate all include-files");
        print("    i          Cycles window swap interval");
        print("    +          increase zoom factor");
        print("    -          decrease zoom factor");
        print("    ,          switch to previous camera");
        print("    .          switch to next camera");
        print(" NumPad-/      draw camera frustums");
        print("   0-9         switch to camera number 0-9");
        print("    N          start Nagios plugin");
        print("    A          free dom value caches");
        print("    h          print this help");
    }

    function screenShot(theBasename) {
        theBasename = theBasename || "screenshot";

        var myDate = new Date();
        myDate = String(myDate.getFullYear()) + padStringFront(myDate.getMonth() + 1, "0", 2) + padStringFront(myDate.getDate(), "0", 2);

        var myFilename = "";
        while (1) {
            myFilename = theBasename + "-";
            myFilename += myDate;
            myFilename += "-" + _myScreenshotCount + ".png";
            if (!fileExists(myFilename)) {
                break;
            }
            ++_myScreenshotCount;
        }
        print("Saving screenshot as '" + myFilename + "'");
        window.saveBuffer(myFilename);
    }

    ////////////////////
    // Public Methods //
    ////////////////////

    self.textureAtlasManager = new TextureAtlasManager();

    self.setCursor = function (theImageString, theSize) {
        setCursor(theImageString, theSize);
    };

    self.resetCursor = function () {
        resetCursor();
    };

    self.getSettings = function () {
        if (_myConfigurator) {
            return _myConfigurator.getSettings();
        } else {
            Logger.warning("no settings.xml used -> no Configurator");
            return null;
        }
    };

    self.getConfigurator = function () {
        if (_myConfigurator) {
            return _myConfigurator;
        } else {
            Logger.warning("no settings.xml used -> no Configurator");
            return null;
        }
    };

    self.registerSettingsListener = function (theListener, theSection) {
        if (_myConfigurator) {
            _myConfigurator.addListener(theListener, theSection);
        } else {
            Logger.warning("no settings.xml used -> no Configurator");
        }
    };
    
    self.deregisterSettingsListener = function (theListener) {
        if (_myConfigurator) {
            _myConfigurator.removeListener(theListener);
        } else {
            Logger.warning("no settings.xml used -> no Configurator");
        }
    };

    self.getCurrentTime = function () {
        return _myCurrentTime;
    };
    
    self.__defineSetter__("statisticColor", function (theColor) {
        _myTextStyle.textColor = theColor;
    });

    self.__defineSetter__("statisticPositionX", function (thePosition) {
        _myStatisticPositionX = thePosition;
    });
    self.__defineSetter__("statisticPositionY", function (thePosition) {
        _myStatisticPositionY = thePosition;
    });
    
    self.__defineGetter__("current_time", function () {
        return _myCurrentTime;
    });

    self.initializeVideoRecorder = function(theFrameRate) {
        if (!_myVideoRecorder) {
            _myVideoRecorder = new VideoRecorder(theFrameRate?theFrameRate:30);
        }
    };

    self.getAnimationManager = function () {
        return _myAnimationManager;
    };

    self.getImageManager = function () {
        Logger.warning("ImageManager is deprecated.");
        return _myImageManager;
    };
    
    self.getPerfMeter = function () {
        if (!_myPerfMeter) {
            _myPerfMeter = new PerfMeter(self);
        }
        return _myPerfMeter;
    };

    self.setSplashScreen = function (theFlag) {
        _mySplashScreenFlag = theFlag;
    };

    self.setMessage = function (theMessage, theLine) {
        if (!_myOnScreenDisplay) {
            _myOnScreenDisplay = new OnScreenDisplay(self);
        }
        _myOnScreenDisplay.setMessage(theMessage, theLine);
    };
    
    self.getOSD = function () {
        return _myOnScreenDisplay;
    };

    self.toggleVideoRecording = function () {
        if (!_myVideoRecorder) {
            _myVideoRecorder = new VideoRecorder(30);
        }
        _myVideoRecorder.enabled = !_myVideoRecorder.enabled;
    };

    self.BaseViewer.onFrame = self.onFrame;
    self.onFrame = function (theTime) {
        self.BaseViewer.onFrame(theTime);
        _myCurrentTime = theTime;
        if (_myShutter) {
            _myShutter.onFrame(theTime);
        }
        if (_myVideoRecorder) {
            _myVideoRecorder.onFrame();
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
        if (_myConfigurator) {
            _myConfigurator.onFrame(theTime);
        }
        if (_myPerfMeter) {
            _myPerfMeter.onFrame(theTime);
        }
    };

    self.BaseViewer.onPostRender = self.onPostRender;
    self.onPostRender = function () {
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
        if (_myPerfMeter) {
            _myPerfMeter.onPostRender();
        }
        if (_myOnScreenStatistics > 0) {
            showStatistics();
        }
        if (_myRuler && _myRuler.enabled) {
            _myRuler.onPostRender();
        }
    };

    self.onKey = function (theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag, theTimeStamp, theScancode) {
        var myCamera, myHfov;
        var myMover = self.getMover(self.getActiveViewport());
        
        if (myMover) {
            myMover.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
        if (_myRuler && _myRuler.enabled) {
            _myRuler.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
        if (!theAltFlag) {
            self.getLightManager().onKey(theKey, theKeyState, theShiftFlag, theCtrlFlag, theAltFlag);
            if (_myConfigurator) {
                _myConfigurator.onKey(theKey, theKeyState, theShiftFlag, theCtrlFlag, theAltFlag);
            }
            if (theCtrlFlag) {
                _myDebugVisual.onKey(theKey, theKeyState, theShiftFlag);
                _myAnimationManager.onKey(theKey, theKeyState, theShiftFlag);
            }
        }
        if (_myShutter) {
            if (_myShutter.onKey(theKey, theKeyState, theShiftFlag, theCtrlFlag, theAltFlag)) {
                return;
            }
        }
        
        // theKeyState is true, if the key is pressed
        if (!theKeyState || !theCtrlFlag) {
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
                var myNextMover = self.nextMover();
                print("Mover Set to: " + myNextMover.constructor.name);
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
            case 'O':
                self.getScene().saveWithCatalog("saved_scene.d60", "saved_scene.d60", true);
                print("Saved scene and catalog to: saved_scene.d60");
                break;
            case 'p':
                window.pause = !window.pause;
                print("Pause: " + (window.pause ? "on" : "off"));
                break;
            case 'P':
                print("Node changed since version " + _mySinceLastVersion + ":");
                window.scene.dom.printChangedNodes(_mySinceLastVersion);
                _mySinceLastVersion = window.scene.dom.nodeVersion;
                break;
            case 'U':
                window.scene.dom.flushUnusedChildren();
                break;
            case 'h':
                printHelp();
                break;
            case 's':
                _myOnScreenStatistics++;
                if (_myOnScreenStatistics > 2) {
                    _myOnScreenStatistics = 0;
                }
                window.printStatistics();
                print("  Scene size     " + window.scene.getWorldSize(self.getActiveCamera()).toFixed(1) + "m");
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
            case 'A':
                print("Freeing dom caches...");
                window.scene.dom.freeCaches();
                print("Freeing dom caches done.");
                break;
            case 'X':
                print(window.scene.world);
                break;
            case 'N':
                var myPlugin = self.enableNagios(2349);
                myPlugin.timeout = 5000;
                print("Nagios plugin listening on port " + myPlugin.port + ", frame timeout is " + myPlugin.timeout + " ms");
                break;
            case "W":
            case "sys req":
            case "print screen":
                screenShot();
                break;
            case "PRINT SCREEN":
            case "SYS REQ":
                if (!_myVideoRecorder) {
                    _myVideoRecorder = new VideoRecorder(30);
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
                myCamera = self.getActiveCamera();
                if (myCamera.frustum.hfov) { // persp camera
                    myHfov = myCamera.frustum.hfov;
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
                myCamera = self.getActiveCamera();
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
            case 'T':
                self.getPerfMeter().toggleEnableFlag();
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
                // Cycle swap interval
                var mySwapInterval = window.swapInterval;
                mySwapInterval++;
                if (mySwapInterval > 4) {
                    mySwapInterval = 0;
                }
                window.swapInterval = mySwapInterval;
                print("Swap interval=" + window.swapInterval);
                break;
            //XXX: disabled onKey scene optimizing, because it borks on a lot of scenes
            //case 'o':
            //    print("Optimizing scene...");
            //    window.scene.optimize();
            //    break;
            case '[/]':
                self.drawFrustums = ! self.drawFrustums;
                print("Draw frustums: " + (self.drawFrustums ? "On" : "Off"));
                break;
            case 'r':
                if (_myRuler.enabled) {
                    _myRuler.enabled = false;
                    print("ruler deactivated");
                } else {
                    _myRuler.enabled = true;
                    print("ruler activated");
                }                
                break;
            default:
                break;
            }
        }
    };

    self.BaseViewer.onMouseMotion = self.onMouseMotion;
    self.onMouseMotion = function (theX, theY) {
        self.BaseViewer.onMouseMotion(theX, theY);
        if (_myShutter && _myShutter.enabled()) {
            if (_myShutter.onMouseMotion(theX, theY)) {
                return;
            }
        }
        if (_myRuler && _myRuler.enabled) {
            _myRuler.onMouseMotion(theX, theY);
        }
    };
    
    self.BaseViewer.onMouseButton = self.onMouseButton;
    self.onMouseButton = function (theButton, theState, theX, theY) {
        self.BaseViewer.onMouseButton(theButton, theState, theX, theY);
        if (_myShutter && _myShutter.enabled()) {
            if (_myShutter.onMouseButton(theButton, theState, theX, theY)) {
                return;
            }
        }
        if (_myRuler && _myRuler.enabled) {
            _myRuler.onMouseButton(theButton, theState, theX, theY); 
        }
    };

    self.BaseViewer.onAxis = self.onAxis;
    self.onAxis = function (theDevice, theAxis, theValue) {
        self.BaseViewer.onAxis(theDevice, theAxis, theValue);
    };

    self.BaseViewer.onButton = self.onButton;
    self.onButton = function (theDevice, theButton, theState) {
        self.BaseViewer.onButton(theDevice, theButton, theState);
    };

    self.onResize = function (theNewWidth, theNewHeight) {
        if (_myShutter) {
            _myShutter.onResize();
        }
    };

    self.BaseViewer.onExit = self.onExit;
    self.onExit = function () {
        self.BaseViewer.onExit();
    };

    // Must be called before setup
    // Use the following constants:
    //     Renderer.TEXTURECOMPRESSION_SUPPORT [default]
    //     Renderer.MULTITEXTURE_SUPPORT       [default]
    //     Renderer.CUBEMAP_SUPPORT            [default]
    //     Renderer.CUBEMAP_SUPPORT            [default]
    //     Renderer.POINT_PARAMETER_SUPPORT
    //     Renderer.TEXTURE_3D_SUPPORT
    self.addRenderingCapability = function (theCapability) {
        _mySetDefaultRenderingCap = false;
        window.renderingCaps = window.renderingCaps | theCapability;
    };
    
    self.removeRenderingCapability = function (theCapability) {
        _mySetDefaultRenderingCap = false;
        window.renderingCaps = window.renderingCaps & (~theCapability);
    };

    self.setup = function (theWindowWidth, theWindowHeight, theFullscreen, theWindowTitle, theScene, theSwitchNodeFlag) {
        //print(theWindowWidth + " x " + theWindowHeight + " fullscreen: " + theFullscreen);
        window = window || new RenderWindow();
        if (theWindowWidth && theWindowHeight) {
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
                myScene = new Scene(myModelname, true);
            } else {
                myScene = new Scene();
            }
            self.preprocessScene(myScene);
            myScene.setup();
        }
        var myCanvas = myScene.dom.find("/scene/canvases/canvas");

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
        _myRuler            = new Ruler(self);

        // [CH]: Deprecated, should be removed someday
        _myImageManager = new ImageManager(self);

        var myOptionsString = ("settings" in self['arguments']) ? self['arguments'].settings : undefined;
        
        if (!self.configuratorClass) {
            use("LegacyConfigurator.js");
            self.configuratorClass = LegacyConfigurator;
        }
        _myConfigurator = self.configuratorClass.create(self, myOptionsString);

        //exit()

        if (theWindowTitle !== undefined) {
            window.title = theWindowTitle;
        }
        if (_mySplashScreenFlag) {
            _mySplashScreen = new ImageOverlay(window.scene, "shadertex/ac_logo.png", [0, 0]);
            _mySplashScreen.name = "splashscreen";
            _mySplashScreen.position = new Vector2f((window.width - _mySplashScreen.width) / 2,
                                                    (window.height - _mySplashScreen.height) / 2);
        }
        _mySinceLastVersion = "1";
    };

    self.createShutter = function () {
        if (!_myShutter) {
            print("Creating shutter");
            _myShutter = new Shutter(self, new Vector4f(0, 0, 0, 0));
            self.registerSettingsListener(_myShutter, "Shutter");
        }
    };

    self.go = function () {
        window.go();
    };
};
