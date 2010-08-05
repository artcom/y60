/*jslint nomen: false, white: false*/
/*global spark, use, SceneViewer, window*/

/**
 * Simple wrapper for the Y60 scene viewer.
 */

use("SceneViewer.js"); // XXX: bad place for this

spark.Window = spark.ComponentClass("Window");

spark.Window.Constructor = function(Protected) {
    var Public = this;
    var Base = {};
    
    this.Inherit(spark.Stage);
    
    const PICK_RADIUS = 1;
    const MAX_CURSOR_POSITIONS_FOR_AVERAGE = 10;
    const MOVE_DISTANCE_THRESHOLD = 0;
    
    var _myCamera = null;
    var _myWorld = null;
    var _myPickRadius = PICK_RADIUS;
    var _myPickList = {};
    var _myCursorPositionHistory = {};
    var _myMultitouchCursors = {};
    var _myMaxCursorPositionsForAverage = MAX_CURSOR_POSITIONS_FOR_AVERAGE;
    

    SceneViewer.prototype.Constructor(this, []);

    Public.title getter = function()  {
        return window.title;
    };

    Public.multitouchCursors getter = function()  {
        return _myMultitouchCursors;
    };

    Public.title setter = function(theTitle) {
        window.title = theTitle;
    };
    
    Public.maxCursorPositionsForAverage setter = function(theValue) {
        _myMaxCursorPositionsForAverage = theValue;
    }
    
    function seperateWorld() {
        // This is the first world - it contains the loaded x60 model
        var mySparkWorld = window.scene.dom.firstChild.firstChild;
        // Create a new world for the 3d elements
        _myWorld = Node.createElement("world");
        _myWorld.name = "3d-world";
        window.scene.dom.firstChild.appendChild(_myWorld);
        
        // move all over to the new world (at this moment the first world
        // does not yet contain spark quads)
        while (mySparkWorld.childNodesLength()) {
            var myNode = mySparkWorld.removeChild(mySparkWorld.firstChild);
            _myWorld.appendChild(myNode);
        }
        
        // Get the camera from the 3d world and place a copy of it in the original first world
        // The copy must have a unique id.
        _myCamera = _myWorld.find(".//camera[@name='perspShape']");
        var mySparkCam = _myCamera.cloneNode();
        mySparkCam.id = createUniqueId();
        mySparkWorld.appendChild(mySparkCam);
        
        // remove old Headlight with wrong id due to realloctaion of nodes
        var myHeadlight = _myCamera.find(".//light");
        _myCamera.removeChild(myHeadlight);
        
        // hook the new camera to the viewport (before it pointed to the 3d world's camera)
        var myViewport = window.scene.dom.find("//viewport");
        myViewport.camera = mySparkCam.id;
        
        // set new Lighting for 2d-viewport
        var myLightManager = Public.getLightManager();
        var myCanvas = window.scene.dom.find("//viewport/..");
        myLightManager.setupDefaultLighting(myCanvas);
        
        Public.worlds['2d'] = window.scene.dom.firstChild.firstChild;
        Public.worlds['3d'] = window.scene.dom.find(".//world[@name='3d-world']");
    }

    Base.realize = Public.realize;
    Public.realize = function() {
        Public.worlds = {}; // 'spark' and potentially '3d' is possible
        window = new RenderWindow();

        window.position = [
            Protected.getNumber("positionX", 0),
            Protected.getNumber("positionY", 0)
        ];

        window.decorations = Protected.getBoolean("decorations", true);
        window.multisamples = Protected.getNumber("multisamples", 0);

        var mySceneFile = Protected.getString("sceneFile", "");
        if (mySceneFile.length > 0) {
            Public.setModelName(mySceneFile);
        }
        Public.setup(Protected.getNumber("width", 640),
                     Protected.getNumber("height", 480),
                     Protected.getBoolean("fullscreen", false),
                     Protected.getString("title", "SPARK Application"));
        
        var worldSeperation = Protected.getBoolean("worldSeperation", false);
        if (mySceneFile.length > 0) {
            if(worldSeperation) {
                seperateWorld();
            } else {
                Public.worlds['2d'] = window.scene.dom.firstChild.firstChild;
            }
            if (_mySceneLoadedCallback) {
                _mySceneLoadedCallback(window.scene.dom);
            }
        } else {
            Public.worlds['2d'] = window.scene.dom.firstChild.firstChild;
        }

        Public.setMover(null);

        window.showMouseCursor = Protected.getBoolean("mouseCursor", true);
        window.swapInterval = Protected.getNumber("swapInterval", 1);
        var myFixedDeltaT = Number(eval(Protected.getString("fixedDeltaT", "0"))); // so we can say something like "1/30"
        if (myFixedDeltaT) {
            window.fixedFrameTime = myFixedDeltaT;
        }

        // mixed 2D/3D applications might want to keep the original frustum and manage
        // the screenspace transformation themself.
        var mySetupFrustumFlag = Protected.getBoolean("setupFrustum", true);
        if (mySetupFrustumFlag) {
            spark.setupCameraOrtho(window.scene.dom.find(".//camera"), window.width, window.height);
        }

        Protected.updateMouseButtonState(spark.Mouse.PRIMARY,   false);
        Protected.updateMouseButtonState(spark.Mouse.SECONDARY, false);
        Protected.updateMouseButtonState(spark.Mouse.TERTIARY,  false);

        Base.realize(window.scene.world);
        
        if(worldSeperation) {
            // if 3d world existent, then render it to another canvas in the window
            var my3dCanvas = new spark.OffscreenCanvas();
            my3dCanvas.name   = "3D-canvas";
            Public.addChild(my3dCanvas);
            my3dCanvas.camera = _myCamera;
            my3dCanvas.realize();
            my3dCanvas.width  = Public.width;
            my3dCanvas.height = Public.height;
            my3dCanvas.position = new Vector3f(0,0,0);
            my3dCanvas.postRealize();
        }
    };

    // XXX: Override size, width and height properties inherited via Stage->Widget
    // They do return a boundingbox size which, from class Windows standpoint, is just
    // crap.
    Public.size getter = function() {
        return new Vector2i(window.width, window.height);
    }
    Public.width getter = function() {
        return window.width;
    }
    Public.height getter = function() {
        return window.height;
    }

    Public.pickWidget = function(theX, theY) {
        var myBody = Public.picking.pickBodyBySweepingSphereFromBodies(theX, theY, _myPickRadius, Public.sceneNode);
        if(myBody) {
            var myBodyId = myBody.id;
            if(myBodyId in spark.sceneNodeMap) {
                var myWidget = spark.sceneNodeMap[myBodyId];
                return myWidget;
            }
        }
        return null;
    };


    var _myMousePosition = new Vector2f();

    Public.mousePosition getter = function() {
        return _myMousePosition;
    };

    Protected.updateMousePosition = function(theX, theY) {
        _myMousePosition.x = theX;
        _myMousePosition.y = theY;
    };

    var _myMouseButtonStates = {};

    Public.mouseButtonStates getter = function() {
        return _myMouseButtonStates;
    };

    Protected.updateMouseButtonState = function(theButton, theState) {
        _myMouseButtonStates[theButton] = theState;
    };


    var _myMouseFocused = null;

    Public.mouseFocused getter = function() {
        return _myMouseFocused;
    };


    var _myKeyboardFocused = null;

    Public.keyboardFocused getter = function() {
        return _myKeyboardFocused;
    };

    Public.focusKeyboard = function(theWidget) {
        _myKeyboardFocused = theWidget;
    };

    // XXX: a somewhat hackish callback to get a hand on the scene
    //      after the model has been loaded but before any spark components
    //      are added.
    var _mySceneLoadedCallback;

    Public.onSceneLoaded getter = function() {
        return _mySceneLoadedCallback;
    }

    Public.onSceneLoaded setter = function(f) {
        _mySceneLoadedCallback = f;
    }
    
    Public.pickRadius getter = function() {
        return _myPickRadius;
    }

    Public.pickRadius setter = function(theRadius) {
        _myPickRadius = theRadius;
    }

    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    //  Will be called before onFrame, has the time since application start
    Public.onProtoFrame = function(theTime) {
        var myEvent = new spark.StageEvent(spark.StageEvent.PROTO_FRAME, Public, theTime);
        Public.dispatchEvent(myEvent);
    };

    //  Will be called first in renderloop, has the time since application start
    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime, theDeltaT) {
        Base.onFrame(theTime, theDeltaT);
        if(Public.hasEventListener(spark.StageEvent.FRAME)) {
            var myEvent = new spark.StageEvent(spark.StageEvent.FRAME, Public, theTime, theDeltaT);
            Public.dispatchEvent(myEvent);
        }
        _myPickList = {};
    };

    // Will be called before rendering the frame
    Base.onPreRender = Public.onPreRender;
    Public.onPreRender = function() {
        Base.onPreRender();
        var myEvent = new spark.StageEvent(spark.StageEvent.PRE_RENDER, Public);
        Public.dispatchEvent(myEvent);
    };

    // Will be called after rendering the frame, but before swap buffer
    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
        var myEvent = new spark.StageEvent(spark.StageEvent.POST_RENDER, Public);
        Public.dispatchEvent(myEvent);
    };

    // Will be called on a mouse move
    Base.onMouseMotion = Public.onMouseMotion;
    Public.onMouseMotion = function(theX, theY) {
        Base.onMouseMotion(theX, theY);

        var myButtonStates = clone(_myMouseButtonStates);
        Protected.updateMousePosition(theX, theY);

        var myWidget = Public.pickWidget(theX, theY);

        if(!myWidget) {
            myWidget = Public;
        }

        if(myWidget != _myMouseFocused) {
            Logger.debug("Mouse focuses " + myWidget
                         + (_myMouseFocused ? ", leaving " + _myMouseFocused : ""));

            if(_myMouseFocused) {
                var myLeaveEvent = new spark.MouseEvent(spark.MouseEvent.LEAVE, theX, theY, 0, 0, null, myButtonStates);
                _myMouseFocused.dispatchEvent(myLeaveEvent);
            }

            _myMouseFocused = myWidget;

            var myEnterEvent = new spark.MouseEvent(spark.MouseEvent.ENTER, theX, theY);
            myWidget.dispatchEvent(myEnterEvent);
        }

        Logger.debug("Mouse moves to [" + theX + "," + theY + "] over " + myWidget);
        var myMoveEvent = new spark.MouseEvent(spark.MouseEvent.MOVE, theX, theY);
        myWidget.dispatchEvent(myMoveEvent);

        _myMouseFocused = myWidget;
    };

    // Will be called on a mouse button
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        Base.onMouseButton(theButton, theState, theX, theY);

        var myButton = spark.Mouse.buttonFromId(theButton);
        Protected.updateMouseButtonState(myButton, theState);

        var myButtonStates = clone(_myMouseButtonStates);

        Protected.updateMousePosition(theX, theY);

        var myWidget = Public.pickWidget(theX, theY);

        if(myWidget) {
            if(theState) {
                // XXX: click should be more well-defined and button-up-based.
                Logger.debug("Mouse clicks " + myWidget + " with button " + myButton);
                var myClickEvent = new spark.MouseEvent(spark.MouseEvent.CLICK, theX, theY, 0, 0, myButton, myButtonStates);
                myWidget.dispatchEvent(myClickEvent);

                Logger.debug("Mouse " + myButton + " button down on " + myWidget);
                var myDownEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_DOWN, theX, theY, 0, 0, myButton, myButtonStates);
                myWidget.dispatchEvent(myDownEvent);
            } else {
                Logger.debug("Mouse " + myButton + " button up on " + myWidget);
                var myUpEvent = new spark.MouseEvent(spark.MouseEvent.BUTTON_UP, theX, theY, 0, 0, myButton, myButtonStates);
                myWidget.dispatchEvent(myUpEvent);
            }
        }
    };

    // Will be called on a keyboard event
    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag) {
        Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);

        if(_myKeyboardFocused) {
            var myModifiers =
                (theShiftFlag ? spark.Keyboard.SHIFT : 0) | (theControlFlag ? spark.Keyboard.CTRL : 0) | (theAltFlag ? spark.Keyboard.ALT : 0);
            var myType = theKeyState ? spark.KeyboardEvent.KEY_DOWN : spark.KeyboardEvent.KEY_UP;
            var myEvent = new spark.KeyboardEvent(myType, theKey, myModifiers);
            Logger.debug("Key " + myEvent.keyString + " " + (theKeyState ? "down" : "up") + " on " + _myKeyboardFocused);
            _myKeyboardFocused.dispatchEvent(myEvent);
        }
    };

    // Will be called on a mouse wheel event
    Base.onMouseWheel = Public.onMouseWheel;
    Public.onMouseWheel = function(theDeltaX, theDeltaY) {
        Base.onMouseWheel(theDeltaX, theDeltaY);

        var myButtonStates = clone(_myMouseButtonStates);

        if(_myMouseFocused) {
            Logger.debug("Mouse scrolls " + _myMouseFocused + " by [" + theDeltaX + "," + theDeltaY + "]");
            var myEvent = new spark.MouseEvent(
                spark.MouseEvent.SCROLL,
                _myMousePosition.x, _myMousePosition.y,
                theDeltaX, theDeltaY, null, myButtonStates
            );
            _myMouseFocused.dispatchEvent(myEvent);
        }
    };

    // Will be called on a joystick axis event
    Base.onAxis = Public.onAxis;
    Public.onAxis = function(device, axis, value) {
        Base.onAxis(device, axis, value);
    };

    // Will be called on a joystick button event
    Base.onButton = Public.onButton;
    Public.onButton = function(theDevice, theButton, theState) {
        Base.onButton(theDevice, theButton, theState);
    };

    // Will be called on a window reshape event
    Base.onResize = Public.onResize;
    Public.onResize = function(theNewWidth, theNewHeight) {
        Base.onResize(theNewWidth, theNewHeight);
    };

    // Will be called before exit
    Base.onExit = Public.onExit;
    Public.onExit = function() {
        Base.onExit();
    };


    // Handle DSA events
    Public.onTouch = function(theEventName, theId, theBitMask, theGridSizeX, theGridSizeY, theCount) {
        Logger.trace("onTouch " + theEventName +
                     ", " + theId + ", " + theBitMask + ", " + theGridSizeX +
                     ", " + theGridSizeY + ", " + theCount);
        var myEvent = new spark.DSAEvent(spark.DSAEvent.TOUCH, theEventName, theId, theBitMask,
                                         new Vector2f(theGridSizeX, theGridSizeY), theCount);
        Public.dispatchEvent(myEvent);
    };
    
    function handleMultitouchEvent(theEvent) {
        if(theEvent.callback == "onASSEvent") {
            spark.proximatrix.onASSEvent(theEvent);
        }

        var myId = getSparkConformedCursorId(theEvent, theEvent.id);
        switch(theEvent.type) {
        case "configure":
            Logger.info("proximatrix got configured");
            break;

        case "add":
            initCursorPositionHistory(myId);
        case "move": // proximatrix
        case "update": // tuio        
            var myCursor = getMultitouchCursor(myId);
            if(theEvent.type == "add") {
                myCursor.activate();
            }
            var myFocused = myCursor.focused;
            
            var myLastPosition = getAveragedPosition(myId);
            var myPosition = getMultitouchCursorPosition(theEvent);
            myPosition = calculateAveragePosition(myId, myPosition);
            
            var myPick = getWidgetForMultitouchCursor(myCursor, myPosition);
            myCursor.update(myPick, myPosition);

            if(theEvent.type == "add") {
                Logger.debug("Cursor " + myId + " appears in " + myFocused);
                var myAppear = new spark.CursorEvent(spark.CursorEvent.APPEAR, myCursor);
                myPick.dispatchEvent(myAppear);
            }
            if(myPick != myFocused) {
                Logger.debug("Cursor " + myId + " focuses " + myPick
                             + (myFocused ? ", leaving " + myFocused : ""));

                if(myFocused) {
                    var myLeave = new spark.CursorEvent(spark.CursorEvent.LEAVE, myCursor);
                    myFocused.dispatchEvent(myLeave);
                }

                var myEnter = new spark.CursorEvent(spark.CursorEvent.ENTER, myCursor);
                myPick.dispatchEvent(myEnter);
            }

            if(theEvent.type == "move" || theEvent.type == "update") {
                
                var myMoveDistance = distance(myPosition, myLastPosition);
                if (myMoveDistance >= MOVE_DISTANCE_THRESHOLD) {
                    Logger.debug("Cursor " + myId + " moves to " + myPosition + " over " + myPick);
                    var myMove = new spark.CursorEvent(spark.CursorEvent.MOVE, myCursor);
                    myPick.dispatchEvent(myMove);
                }
            }

            break;

        case "remove":
            if(myId in _myMultitouchCursors) {
                Logger.debug("Cursor " + myId + " removed");
                var myCursor   = _myMultitouchCursors[myId];
                var myPosition = getMultitouchCursorPosition(theEvent);
                myPosition = calculateAveragePosition(myId, myPosition);
                var myFocused  = myCursor.focused;

                myCursor.update(myFocused, myPosition);

                if(myFocused) {
                    Logger.debug("Cursor " + myId + " leaves " + myFocused);
                    var myLeave = new spark.CursorEvent(spark.CursorEvent.LEAVE, myCursor);
                    myFocused.dispatchEvent(myLeave);

                    Logger.debug("Cursor " + myId + " vanishes in " + myFocused);
                    var myVanish = new spark.CursorEvent(spark.CursorEvent.VANISH, myCursor);
                    myFocused.dispatchEvent(myVanish);
                }

                myCursor.deactivate();
                delete _myMultitouchCursors[myId];
            }
            removeCursorPositionHistory(myId);
            break;
        }
    };
    
    Public.onASSEvent = handleMultitouchEvent;
    Public.onTuioEvent = handleMultitouchEvent;
    
    
    // Will be called on a gesture event
    Public.onGesture = function(theGesture) {
        
        var mySparkConformedCursorId = getSparkConformedCursorId(theGesture, theGesture.cursorid);
        var myPosition = getAveragedPosition(mySparkConformedCursorId);
        if (!myPosition) {
            myPosition = getMultitouchCursorPosition(theGesture);
        }
                         
        
        // picking with considering cursor grabbing
        var myCursor = getMultitouchCursor(mySparkConformedCursorId);
        var myWidget = getWidgetForMultitouchCursor(myCursor, myPosition);
        
        var myScale = new Vector3f(1, 1, 1);
        if (theGesture.baseeventtype == TUIO_BASE_EVENT) {
            myScale = new Vector3f(Public.width, Public.height, 1);
        }
        var myCursorPartner = null;
        var myCenterPoint = null;
        // Do some multicursor gesture specific things (cursor_pair_start, zoom, rotate, cursor_pair_finish)
        if (theGesture.type == "cursor_pair_start" || theGesture.type == "zoom" || theGesture.type == "cursor_pair_finish" || theGesture.type == "rotate") {
            if (theGesture.type != "cursor_pair_finish") {
                myCenterPoint = new Vector3f(theGesture.centerpoint.x, theGesture.centerpoint.y, 0);
                myCenterPoint.mult(myScale);
            }
            var mySparkConformedCursorPartnerId = getSparkConformedCursorId(theGesture, theGesture.cursorpartnerid);
            var myCursorPartner = getMultitouchCursor(mySparkConformedCursorPartnerId);
        }
        
        switch(theGesture.type) {    
            case "wipe":            
                var myDir = new Vector3f(theGesture.direction.x, theGesture.direction.y, 0);
                myDir.mult(myScale);
                var myWipeEvent = new spark.WipeGestureEvent(spark.GestureEvent.WIPE, theGesture.baseeventtype, myDir, myCursor);
                myWidget.dispatchEvent(myWipeEvent);
                break;                
            case "cursor_pair_start":
                var myZoomEvent = new spark.ZoomGestureEvent(spark.GestureEvent.CURSOR_PAIR_START, theGesture.baseeventtype, myCursor, myCursorPartner, null, null, myCenterPoint);
                myWidget.dispatchEvent(myZoomEvent);
                break;                
            case "zoom":
                var myFirstDistance = theGesture.initialdistance;
                var myDistance = theGesture.distance;
                var myLastDistance = theGesture.lastdistance;
                var myZoomEvent = new spark.ZoomGestureEvent(spark.GestureEvent.ZOOM, theGesture.baseeventtype, myCursor, myCursorPartner, myFirstDistance, myDistance, myCenterPoint, myLastDistance);
                myWidget.dispatchEvent(myZoomEvent);
                break;   
            case "rotate":
                var myRotateEvent = new spark.RotateGestureEvent(spark.GestureEvent.ROTATE, theGesture.baseeventtype, myCursor, myCursorPartner, theGesture.angle, myCenterPoint);
                myWidget.dispatchEvent(myRotateEvent);
                break;             
            case "cursor_pair_finish":                
                var myZoomEvent = new spark.ZoomGestureEvent(spark.GestureEvent.CURSOR_PAIR_FINISH, theGesture.baseeventtype, myCursor, myCursorPartner);
                myWidget.dispatchEvent(myZoomEvent);
                break;                
            default:
                Logger.info("Unknown gesture : " + theGesture);            
        }
    }
    
    function getWidgetForMultitouchCursor(theCursor, thePosition) {
        var myWidget;
        if (theCursor.grabbed) {
            myWidget = theCursor.grabHolder;
        } else {
            if (theCursor.id in _myPickList) {
                myWidget = _myPickList[theCursor.id];
            } else {
                myWidget = Public.pickWidget(thePosition.x, thePosition.y);
                _myPickList[theCursor.id] = myWidget;
            }
        }
        if (!myWidget) {
            myWidget = Public;
        }
        return myWidget;
    }
    
    function getMultitouchCursor(theId) {
        var myCursor;
        if (theId in _myMultitouchCursors) {
            myCursor = _myMultitouchCursors[theId];
        } else {
            Logger.debug("Cursor " + theId + " added");
            myCursor = new spark.Cursor(theId);
            _myMultitouchCursors[theId] = myCursor;
        }
        return myCursor;
    }
    
    function getSparkConformedCursorId(theEvent, theId) {
        switch(theEvent.callback) {
        case "onASSEvent":
            return "pmtx" + theId;
        case "onTuioEvent":
            return "tuio" + theId;
        case "onGesture":
            if (theEvent.baseeventtype == ASS_BASE_EVENT) {
                return "pmtx" + theId;
            } else {
                return "tuio" + theId;
            }
        default:
            Logger.fatal("Unknown multitouch event type");
            return null;
        }
    };
    
    
    function getMultitouchCursorPosition(theEvent) {
        var myPosition;
        switch(theEvent.callback) {
        case "onASSEvent":
            myPosition = new Point2f(theEvent.position3D.x, theEvent.position3D.y);
            break;
        case "onTuioEvent":
            myPosition = new Point2f(theEvent.position.x * Public.width,
                                     theEvent.position.y * Public.height);
            break;
        case "onGesture":
            if (theEvent.baseeventtype == ASS_BASE_EVENT) {
                myPosition = new Point2f(theEvent.position3D.x, theEvent.position3D.y);
            } else {
                myPosition = new Point2f(theEvent.position3D.x * Public.width,
                                         theEvent.position3D.y * Public.height);
            }
            break;
        default:
            Logger.fatal("Unknown multitouch event type");
            return null;
        }
        return myPosition;
    };
    
    
    function initCursorPositionHistory(theCursorId) {
        _myCursorPositionHistory[theCursorId] = [];
    }
    
    function removeCursorPositionHistory(theCursorId) {
        if (theCursorId in _myCursorPositionHistory) {
            delete _myCursorPositionHistory[theCursorId];
        }
    }
    
    function calculateAveragePosition(theCursorId, thePosition) {
        
        if (!(theCursorId in _myCursorPositionHistory)) {
            return thePosition;
        }
        
        if (_myCursorPositionHistory[theCursorId].length >= _myMaxCursorPositionsForAverage) {
            _myCursorPositionHistory[theCursorId].shift();
        }
        _myCursorPositionHistory[theCursorId].push(thePosition);
        
        return getAveragedPosition(theCursorId);
    }
    
    function getAveragedPosition(theCursorId) {
        var myAveragedPos = new Vector2f(0,0);
        var myCounter = 0;
        
        if (!(theCursorId in _myCursorPositionHistory)) {
            return null;
        }
        for (var i = 0; i < _myCursorPositionHistory[theCursorId].length; i++) {
            myCounter+=(i+1);
            myAveragedPos[0] += _myCursorPositionHistory[theCursorId][i][0] * (i+1);
            myAveragedPos[1] += _myCursorPositionHistory[theCursorId][i][1] * (i+1);
        }
        if (myCounter > 0) {
            myAveragedPos.div(myCounter);
        }
        return myAveragedPos;
    }

};
