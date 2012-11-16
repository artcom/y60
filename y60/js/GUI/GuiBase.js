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

function GuiBase(theArguments) {
    this.Constructor(this, theArguments);
}

GuiBase.prototype.Constructor = function(self, theArguments) {
    var Base = {};
    var Public = self;

    var _myZoomFactor = 1.0;

    // user interface
    var _myLayoutConfig = null;
    var _myLayerManager = null;

    // input handling
    var _myASS = null;
    var _myPicking = null;
    var _myPointerBodies = {};
    var _myPointerObjects = {};
    var _myMousePointer = false;

    var _myTestMode = false;

    var _myHeartBeatNode = new Node("<heartbeat/>").firstChild;
    var _myHeartBeatFile = "";
    var _myHeartBeatFrequency = 0;
    var _myLastHeartBeatTime = 0;

    var _myWidth  = 800;
    var _myHeight = 600;

    SceneViewer.prototype.Constructor(Public, theArguments);

    //////////////////////////////////////////////////////////////////
    // Public
    //////////////////////////////////////////////////////////////////

    Base.onExit = Public.onExit;
    Public.onExit = function() {
        window.showTaskbar  = true;
        window.showMouseCursor = true;
        Base.onExit();
    }

    Base.setup = Public.setup;
    Public.setup = function(theWidth, theHeight, theTitle, theLayoutConfig) {
        Logger.info("Setting up...");

        if (theWidth && theHeight) {
            _myWidth = theWidth;
            _myHeight = theHeight;
        }

        // configure window
        // (before Base.setup because of SDL window initialization)
        window              = new RenderWindow();
        window.position     = [0, 0];
        window.multisamples = 4;
        window.showTaskbar  = !Public.getReleaseMode();

        Base.setup(_myWidth, _myHeight, false, theTitle);

        window.showMouseCursor = !Public.getReleaseMode();
        window.swapInterval = 1;

        setupCamera();

        // input handling
        Logger.info("Initializing body picker...");
        _myPicking = new Picking(window);

        // TODO: make optional [sh]
        // Logger.info("Initializing ASS...");
        // _myASS = new ASSManager(Public, window.scene.world, false);

        // user interface
        Logger.info("Loading layout description...");
        var myLayoutConfig = "CONFIG/layers.xml";
        if (theLayoutConfig) {
            myLayoutConfig = theLayoutConfig;
        }

        _myLayoutConfig = new Node();
        _myLayoutConfig.parseFile(myLayoutConfig);
        _myLayoutConfig = _myLayoutConfig.getNodesByTagName("layers", true)[0];

        Logger.info("Initializing layer manager...");
        _myLayerManager = new LayerManager();
        _myLayerManager.setup(_myLayoutConfig);

        // nagios listener
        Logger.info("Initializing nagios plugin...");

        // TODO: implement somthing clever [sh]
        // if (NAGIOS_PORT == undefined) {
        //     Public.enableNagios(NAGIOS_PORT);
        // }

        // settings
        //  do this late so we can count on everything being
        //  initialized before the first settings event.
        Public.registerSettingsListener(Public, "GuiBase");

        //setup heartbeat
        // TODO: implement somthing clever [sh]
        if (false) {
            var myWatchdogXml = new Node();
            if (fileExists("watchdog.xml")) {
                myWatchdogXml.parseFile("watchdog.xml");
                var myHeartBeatNode = myWatchdogXml.childNode("WatchdogConfig").childNode("Application").childNode("Heartbeat");
                var myHeartBeatFile = myHeartBeatNode.childNode("Heartbeat_File").childNode("#text").nodeValue;
                var myHeartBeatFreq = myHeartBeatNode.childNode("Heartbeat_Frequency").childNode("#text").nodeValue;
                if (myHeartBeatFile) {
                    _myHeartBeatFile = myHeartBeatFile;
                    if (!fileExists(myHeartBeatFile)) {
                        _myHeartBeatNode.saveFile(myHeartBeatFile);
                        Logger.warning("Created heartbeat file '" + myHeartBeatFile + "'.");
                    }
                } else {
                    Logger.error("Heartbeat file entry missing in 'watchdog.xml'.");
                }

                if (myHeartBeatFreq && Number(myHeartBeatFreq) != NaN) {
                    _myHeartBeatFrequency = myHeartBeatFreq;
                } else {
                    Logger.error("Heartbeat frequency entry in 'watchdog.xml' missing or corrupt.");
                }
            } else {
                Logger.error("no watchdog file found.");
            }
        }
    }

    ////////////////////////////////////////////
    // getters and "getters"
    ////////////////////////////////////////////

    Public.__defineGetter__("layerManager", function() {
        return _myLayerManager;
    });

    ////////////////////////////////////////
    // frame callback dispatch
    ////////////////////////////////////////

    Public.getButtonByName = function(theName) {
        for (var i=0; i<ourButtons.length; ++i) {
            if (ourButtons[i].buttonname == theName) {
                return ourButtons[i];
            }
        }

        return null;
    }

    Public.clickButtonByName = function(theName) {
        var myButton = Public.getButtonByName(theName);
        if (!myButton) {
            Logger.warning("Button '"+theName+"' not found");
            return;
        }

        if (myButton.enabled) {
            myButton.onPointerClickWithSelf("bla", myButton);
        }
    }

    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);

        // TODO: implement somthing clever [sh]
        // if (USE_HEARTBEAT && _myHeartBeatFile != "" &&
        //     theTime - _myLastHeartBeatTime > _myHeartBeatFrequency)
        // {
        //     _myLastHeartBeatTime = theTime;
        //     _myHeartBeatNode.secondsSince1970 = Date.now() / 1000;
        //     _myHeartBeatNode.saveFile(_myHeartBeatFile);
        // }

        if(_myLayerManager) {
            for (var i = 0; i< _myLayerManager.layers.length ; i++) {
                _myLayerManager.layers[i].onFrame(theTime);
            }
        }

        if (_myTestMode) {
            if (Math.random() < 0.1) {
                var myCount = 0;
                for (var c=0; c<ourButtons.length; ++c) {
                    myCount++;
                }
                var myRand = Math.floor(Math.random() * myCount);
                for (var a=0; a<ourButtons.length; ++a) {
                    if (myRand==0 && ourButtons[a].body.visible) {
                        ourButtons[a].onPointerClickWithSelf("bla", ourButtons[a]);
                    }
                    myRand--;
                }
            }
        }
    }

    ////////////////////////////////////////
    // Pointer dispatch
    ////////////////////////////////////////

    function onPointerAdd(thePointer, theX, theY) {
        Logger.debug("Adding pointer " + thePointer + " at [" + theX + "," + theY + "]");

        var myClosest = _myPicking.pickBody(theX, theY);

        if(myClosest) {
            var myCoords = translateScreenToShape(myClosest, theX, theY);
            var myObject = getBodyOwner(myClosest);
            if(myObject) {
                _myPointerBodies[thePointer] = myClosest;
                _myPointerObjects[thePointer] = myObject;
                if("onPointerAddWithBody" in myObject) {
                    myObject.onPointerAddWithBody(thePointer, myClosest, myCoords[0], myCoords[1]);
                } else if ("onPointerAddWithSelf" in myObject) {
                    myObject.onPointerAddWithSelf(thePointer, myObject, myCoords[0], myCoords[1]);
                } else if("onPointerAdd" in myObject) {
                    myObject.onPointerAdd(thePointer, myCoords[0], myCoords[1]);
                } else {
                    Logger.debug("Widget has no handler.");
                }
            } else {
                Logger.debug("No widget associated with body.");
            }
        }
    }

    function onPointerMove(thePointer, theX, theY) {
        Logger.debug("Moving pointer " + thePointer + " to [" + theX + "," + theY + "]");

        if(thePointer in _myPointerObjects) {
            var myClosest = _myPointerBodies[thePointer];
            var myObject  = _myPointerObjects[thePointer];
            var myCoords  = translateScreenToShape(myClosest, theX, theY);

            if("onPointerMove" in myObject) {
                myObject.onPointerMove(thePointer, myCoords[0], myCoords[1]);
            } else {
                Logger.debug("Widget has no handler.");
            }
        }
    }

    function onPointerRemove(thePointer) {
        Logger.debug("Removing pointer " + thePointer);

        if(thePointer in _myPointerObjects) {
            var myObject = _myPointerObjects[thePointer];

            delete _myPointerBodies[thePointer];
            delete _myPointerObjects[thePointer];
            if("onPointerRemove" in myObject) {
                myObject.onPointerRemove(thePointer);
            } else {
                Logger.debug("Widget has no handler.");
            }
        }
    }

    function onPointerClick(thePointer, theX, theY) {
        Logger.debug("Clicking with " + thePointer + " at [" + theX + "," + theY + "]");

        var myClosest = _myPicking.pickBody(theX, theY);

        if(myClosest) {
            var myObject = getBodyOwner(myClosest);

            if(myObject) {
                if("onPointerClickWithBody" in myObject) {
                    myObject.onPointerClickWithBody(thePointer, myClosest);
                } else if ("onPointerClickWithSelf" in myObject) {
                    myObject.onPointerClickWithSelf(thePointer, myObject);
                } else if("onPointerClick" in myObject) {
                    myObject.onPointerClick(thePointer);
                } else {
                    Logger.debug("Widget has no handler.");
                }
            } else {
                Logger.debug("No widget associated with body.");
            }
        }
    }

    function translateScreenToShape(theBody, theX, theY) {
        var myWorldCoordinate = transformScreenToWorld(theX, theY, Public.getActiveViewport());
        var myInverseGlobal = new Matrix4f(theBody.globalmatrix);
        myInverseGlobal.invert();
        var myTranslation = new Matrix4f();
        myTranslation.makeTranslating(myWorldCoordinate);
        myTranslation.postMultiply(myInverseGlobal);
        return myTranslation.getTranslation();
    }

    ////////////////////////////////////////
    // Mouse handling
    ////////////////////////////////////////

    Base.onMouseMotion = Public.onMouseMotion;
    Public.onMouseMotion = function(theX, theY) {
        Base.onMouseMotion(theX, theY);
        if (_myMousePointer) {
            onPointerMove("MOUSE", theX, theY);
        }
    }

    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        if(theButton == LEFT_BUTTON) {
            _myMousePointer = theState;
            if(theState) {
                onPointerAdd("MOUSE", theX, theY);
                onPointerClick("MOUSE", theX, theY);
            } else {
                onPointerRemove("MOUSE");
            }
        } else {
            Base.onMouseButton(theButton, theState, theX, theY);
        }
    }

    Base.onMouseWheel = Public.onMouseWheel;
    Public.onMouseWheel = function(theDeltaX, theDeltaY) {
        _myZoomFactor += (-theDeltaY) * 0.05;
        if (_myZoomFactor < MINZOOMFACTOR) {
            _myZoomFactor = MINZOOMFACTOR;
        }
        zoomCamera();
    }

    ////////////////////////////////////////
    // Keyboard handling
    ////////////////////////////////////////

    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag) {
        if (theControlFlag) {
            Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
        } else {
            if(theKeyState) {
                Logger.info("Key pressed: " + theKey);

                var myLayer = null;

                switch(theKey) {
                case "t":
                    _myTestMode = !_myTestMode;
                    Logger.info("Testmode "+(_myTestMode?"enabled":"disabled"));
                    break;
                case "q":
                    exit(1);
                    break;
                default:
                    break;
                }
            }
        }
    }

    ////////////////////////////////////////
    // ASS handling
    ////////////////////////////////////////

    Public.onASSEvent = function(theNode) {
        _myASS.onASSEvent(theNode);

        if(theNode.type == "add") {
            onPointerAdd(theNode.id, theNode.position3D.x, theNode.position3D.y);
        } else if(theNode.type == "move") {
            onPointerMove(theNode.id, theNode.position3D.x, theNode.position3D.y);
        } else if(theNode.type == "remove") {
            onPointerRemove(theNode.id);
        } else if(theNode.type == "touch") {
            onPointerClick(theNode.id, theNode.position3D.x, theNode.position3D.y);
        }
    }

    ////////////////////////////////////////
    // Settings handler
    ////////////////////////////////////////

    Public.onUpdateSettings = function(theSettings) {
    }

    ////////////////////////////////////////
    // Camera manipulation
    ////////////////////////////////////////

    function setupCamera() {
        window.camera.frustum.width = _myWidth * _myZoomFactor;
        window.camera.frustum.type = ProjectionType.orthonormal;
        window.camera.frustum.nearplane = -2000.0;
        window.camera.frustum.farplane  = +2000.0;
        window.camera.position.x = _myWidth / 2.0;
        window.camera.position.y = _myHeight / 2.0;
        window.camera.position.z = 500.0;
    }

    function zoomCamera() {
        window.camera.frustum.width = _myWidth * _myZoomFactor;
    }
}
