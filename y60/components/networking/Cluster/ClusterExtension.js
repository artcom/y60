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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: ClusterExtension.js,v $
//   $Author: pavel $
//   $Revision: 1.7 $
//   $Date: 2005/03/07 20:28:27 $
//
//
//=============================================================================

if (__main__ == undefined) var __main__ = "ClusterExtension";

var slave = false;
var master = false;

var ourFullScreenFlag = false;

for (var i = 0; i < arguments.length; ++i) {
    var myArgument = arguments[i];
    if (myArgument == "+slave") {
        slave = true;
    } if (myArgument == "+master") {
        master = true;
    }
}

if (!master && !slave) {
    print("missing +slave or +master parameter");
    exit(1);
}
if (master && slave) {
    print("must only specify +slave or +master");
    exit(1);
}

if (master) {
    use("SceneViewer.js");
    var window = new RenderWindow();
} else {
    var window = new RenderWindow();
    var renderer = null; // initialized at loadScene
}

plug("Cluster");

function ClusterExtensionApp(theArguments) {
    this.Constructor(this, theArguments);
}

ClusterExtensionApp.prototype.Constructor = function(self, theArguments) {
    var _myTextOverlay = null;
    var _myActiveCameraIndex = 0;
    var _myCamera = null;
    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    if (master) {
        SceneViewer.prototype.Constructor(self, theArguments);
    }
    var Base = [];
    var _myDemoRenderExtension = new rendexCluster();
    window.addExtension(_myDemoRenderExtension);
    _myDemoRenderExtension.master = master;
    _myDemoRenderExtension.slave = !master;

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {

        if (master) {
            Base.setup(theWidth, theHeight, theTitle);
            createTextOverlay();
            self.registerSettingsListener(_myDemoRenderExtension, "ClusterExtension");
        } else {
            // register our event listener
            window.eventListener = self;

            // set the window behavior while sleeping
            window.renderWhileSleep = false;

            if (theWidth != null && theHeight != null) {
                window.resize(theWidth, theHeight, ourFullScreenFlag);
            }

            // setting the pbuffer size must be done before loading (creating the rendering context)
            //window.setPBufferSize(_myPBufferWidth, _myPBufferHeight);
            //myTimer.add("setup first part");

            // load a scene
            print("Loading " + (_myModelName ? "scene '" + _myModelName + "'" : "empty scene") +
                " with '" + _myShaderLibrary.substr(_myShaderLibrary.lastIndexOf("/") + 1) + "'");

            var myStatus = window.loadScene(_myModelName, _myShaderLibrary);
            if (!myStatus) {
                throw new Exception("Could not load model", "SceneViewer::run()");
            }
            renderer = window.getRenderer();
            //myTimer.add("setup load scene");
            //window.scene.save("loaded_scene.x60", false);

            //window.setViewport(_myRenderArea[0], _myRenderArea[1], _myRenderArea[2], _myRenderArea[3]);
            self.setActiveCamera(0);
        }
    }

    Base.onKey = self.onKey;
    self.onKey = function(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (master) {
            Base.onKey(theKey, theState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
        if (slave) {
            // theKeyState is true, if the key is pressed
            if (!theState) {
                return true;
            }

            if (theShiftFlag) {
                theKey = theKey.toUpperCase();
            }

            switch (theKey) {
                case 'F':
                    renderer.flatshading = !renderer.flatshading;
                    print("Flatshading: " + (renderer.flatshading ? "on" : "off"));
                    break;
                case 'w':
                    renderer.wireframe = !renderer.wireframe;
                    print("Wireframe: " + (renderer.wireframe ? "on" : "off"));
                    break;
                case 'b':
                    renderer.backfaceCulling = !renderer.backfaceCulling;
                    print("Backface culling: " + (renderer.backfaceCulling ? "on" : "off"));
                    break;
                case 'c':
                    renderer.culling = !renderer.culling;
                    print("Culling: " + (renderer.culling ? "on" : "off"));
                    break;
                case 'C':
                    renderer.debugCulling = !renderer.debugCulling;
                    print("Debug culling: " + (renderer.debugCulling ? "on" : "off"));
                    break;
                case 'v':
                    cycleBoundingVolumeMode();
                    break;
                case 'B':
                    window.scene.save("saved_scene.b60", true);
                    break;
                case 'p':
                    window.pause = !window.pause;
                    print("Pause: " + (window.pause ? "on" : "off"));
                    break;
                case 'h':
                case 'H':
                    //printHelp();
                    break;
                case 's':
                    // check if statisticCycle triggers a valid statistic rendering
                    // print text statistic anyway
                    window.printStatistics();
                    print("  Scene size     " + window.scene.getWorldSize(_myCamera).toFixed(1) + "m");
                    _myTimer.print();
                    break;
                case 'S':
                    window.scene.save("saved_scene.x60", false);
                    break;
                case 'f':
                    window.fullscreen = !window.fullscreen;
                    print("Fullscreen: " + (window.fullscreen ? "on" : "off"));
                    break;
                case 'X':
                    var mySceneNode = window.scene;
                    print(mySceneNode.firstChild);
                    break;
                case '(':
                    _myEdgeBlendingAreaWidth = _myEdgeBlendingAreaWidth - 1;
                    if (_myEdgeBlendingAreaWidth < 0) {
                        _myEdgeBlendingAreaWidth = 0;
                    }
                        print("EdgeBlending area in pixels : " + _myEdgeBlendingAreaWidth);
                    window.activateEdgeBlender(_myEdgeBlending, _myEdgeBlendingAreaWidth,
                                            _myEdgeBlendingPoints, _myEdgeBlendingGamma);
                    break;
                case ')':
                    _myEdgeBlendingAreaWidth += 1;
                    if (_myEdgeBlendingAreaWidth > renderer.width / 2) {
                        _myEdgeBlendingAreaWidth = renderer.width / 2;
                    }
                    print("EdgeBlending area in pixels : " + _myEdgeBlendingAreaWidth);
                    window.activateEdgeBlender(_myEdgeBlending, _myEdgeBlendingAreaWidth,
                                            _myEdgeBlendingPoints, _myEdgeBlendingGamma);
                    break;
                case "e":
                    if ( _myEdgeBlending || _myPBufferHeight == 0 || _myPBufferWidth == 0) {
                        print("Sorry, toggling edgeblending is not available at the moment!");
                        break;
                    }
                    _myEdgeBlending = !_myEdgeBlending;
                    print("EdgeBlending: "+ (_myEdgeBlending ? "on" : "off"));
                    window.activateEdgeBlender(_myEdgeBlending, _myEdgeBlendingAreaWidth,
                                            _myEdgeBlendingPoints, _myEdgeBlendingGamma);
                    break;
                case "sys req":
                case "print screen":
                    var myDate = new Date();
                    print("save screenshot as: " + "screenshot_" + myDate.getDate() + "." +
                        (myDate.getMonth()+1) + "_" + _myScreenShotCounter + ".png");
                    window.saveBuffer("screenshot_" + myDate.getDate() + "." +
                                    (myDate.getMonth()+1) + "_" + _myScreenShotCounter + ".png");
                    _myScreenShotCounter++;
                    break;
                case 'M':
                    window.showMouseCursor = !window.showMouseCursor;
                    print("Mouse cursor: " + (window.showMouseCursor ? "on" : "off"));
                    break;
                case "n":
                    if (_myViewportManipulator > 0) {
                        _myRenderArea[_myViewportManipulator-1] = _myRenderArea[_myViewportManipulator-1] - 0.01;
                        if (_myRenderArea[_myViewportManipulator-1] < 0.0 ) {
                            print("Value cannot drop below 0.0 ");
                            _myRenderArea[_myViewportManipulator-1] = 0.0;
                        }
                    }
                    print("Viewport: [" + _myRenderArea[0] + "," + _myRenderArea[1] + "]-[" +
                                    _myRenderArea[2] + "," + _myRenderArea[3] + "]");
                    window.setViewport(_myRenderArea[0], _myRenderArea[1],
                                    _myRenderArea[2], _myRenderArea[3] );
                    break;
                    break;
                case 'u':
                    var mySceneUpdateStart = millisec();
                    var rc = window.updateScene(Renderer.ALL);
                    print("Scene update time: " + Number(millisec() - mySceneUpdateStart).toFixed(0) / 1000 + " sec.");
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
                    self.setActiveCamera(Number(theKey));
                    break;
                case ".":
                    self.setActiveCamera(_myActiveCameraIndex + 1);
                    break;
                case ",":
                    self.setActiveCamera(_myActiveCameraIndex - 1);
                    break;
            default:
                    break;
            }
            return true;
        }
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        if (master) {
            Base.onFrame(theTime);
        }
    }

    Base.onMouseMotion = self.onMouseMotion;
    self.onMouseMotion = function(theX, theY) {
        if (master) {
            if (Base.onMouseMotion) {
                Base.onMouseMotion(theX, theY);
            }
        }
    }

    Base.onMouseButton = self.onMouseButton;
    self.onMouseButton = function(theButton, theState, theX, theY) {
        if (master) {
             if (Base.onMouseButton) {
                Base.onMouseButton(theButton, theState, theX, theY);
             }
        }
    }

    Base.onMouseWheel = self.onMouseWheel;
    self.onMouseWheel = function(theDeltaX, theDeltaY) {
        if (master) {
            if (Base.onMouseWheel) {
                Base.onMouseWheel(theDeltaX, theDeltaY);
            }
        }
    }

    self.go = function() {
        window.go();
    }

    if (!master) {
        self.onRender = function() {
        }
        self.onPreRender = function() {
        }
        self.onPostRender = function() {
        }
        self.setActiveCamera = function(theCamera) {
            if (!isNaN(theCamera)) {
                var myCameraIndex = theCamera;
                if (myCameraIndex < 0) {
                    myCameraIndex = 0;
                }
                else if (myCameraIndex >= (renderer.cameras.length-1)) {
                    myCameraIndex = renderer.cameras.length-1;
                }
                try {
                    theCamera = renderer.cameras[myCameraIndex];
                    _myActiveCameraIndex = myCameraIndex;
                }
                catch (ex) {
                    print("*** No such camera index " + myCameraIndex);
                    theCamera = window.camera;
                }
            }
            window.camera = theCamera;
            _myCamera = theCamera;

            print("Switch to camera: " + _myCamera.name);
        }
    }
    ////////////// private members ////////////

    function createTextOverlay() {
        window.loadTTF("Arial", "${PRO}/src/y60/gl/text/fonts/arial.ttf", 18);

        var myTextImage = self.getImageManager().getImageNode("infobox");
        _myTextOverlay = new ImageOverlay(window.getScene(), myTextImage.id, [0,0]);
        _myTextOverlay.texture.applymode = "decal";
        _myTextOverlay.color = new Vector4f(0.8,0.8,1,0.6);

        var myTextSize = window.renderTextAsImage( myTextImage, "Press Ctrl-Q to quit.",
                                                   new Node("<style textColor='[0.2,0.2,1,1]'/>").childNode(0),  "Arial", 300, 0);
        _myTextOverlay.width  = myTextSize.x;
        _myTextOverlay.height = myTextSize.y;
        _myTextOverlay.srcsize.x = myTextSize.x / myTextImage.width;
        _myTextOverlay.srcsize.y = myTextSize.y / myTextImage.height;

        _myTextOverlay.position = new Vector2f(10, 30);
        _myTextOverlay.visible = true;
    }

    function parseArguments(theArguments) {
        for (var i = 0; i < theArguments.length; ++i) {
            var myArgument = theArguments[i];
            if (!_myShaderLibrary && myArgument.search(/xml/) != -1) {
                // Take the first xml-file as shader library
                _myShaderLibrary = myArgument;
            } else if (myArgument.search(/\.[xb]60/) != -1 ||
                       myArgument.search(/\.st./) != -1)
            {
                _myModelName = myArgument;
            } else if (myArgument == "rehearsal") {
                _myReleaseMode = false;
            }
        }

        if (!_myShaderLibrary) {
            throw new Exception("Missing shaderlibrary argument", "SceneViewer::parseArguments()");
        }
    }

    if (!master) {
        var _myModelName             = null;
        var _myShaderLibrary         = null;
        var _myReleaseMode           = true;
        parseArguments(theArguments);
    }

};

//
// main
//

if (__main__ == "ClusterExtension") {
    print("ClusterExtension");
    try {
        var ourClusterExtensionApp = new ClusterExtensionApp(arguments);

        ourClusterExtensionApp.setup(640, 480, "ClusterExtension");
        ourClusterExtensionApp.go();
    } catch (ex) {
        print("-------------------------------------------------------------------------------");
        print("### Error: " + ex);
        print("-------------------------------------------------------------------------------");
        exit(1);
    }
}


