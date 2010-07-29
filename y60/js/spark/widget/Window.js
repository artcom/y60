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
    const MAX_CURSOR_POSITIONS_IN_HISTORY = 10;
    
    var _myCamera = null;
    var _myWorld = null;
    var _myPickRadius = PICK_RADIUS;
    var _myPickCounter = 0;
    var _myPickList = {};
    var _myCursorPositionHistory = {};
    var _myMultitouchCursors = {};
    

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
        _myPickCounter++;
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
        _myPickCounter = 0;
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

    // Will be called on a gesture event
    Public.onGesture = function(theGesture) {
        var myPositionScale = new Vector2f(1,1);
        // ass event come in screen coordinates
        // tuio events are normalized from 0 to 1, so scale'em here
        // actually they must be scaled to screen-resolution
        // and not window-resolution, it now only works for fullscreen apps        
        if (theGesture.baseeventtype == TUIO_BASE_EVENT) {
            myPositionScale = new Vector2f(window.width, window.height);
        } 
        var myPos = new Vector2f(theGesture.position3D.x * myPositionScale.x, theGesture.position3D.y * myPositionScale.y);
        if (theGesture.baseeventtype == ASS_BASE_EVENT) {
            myPos = getAveragedAssPosition(theGesture.cursorid, myPos);
        }
        
        var mySparkConformedCursorId = getSparkConformedCursorId(theGesture.baseeventtype, theGesture.cursorid);
        
        // picking with considering cursor grabbing
        var myWidget;
        var myCursor = null;
        if (mySparkConformedCursorId in _myMultitouchCursors) {
            myCursor = _myMultitouchCursors[mySparkConformedCursorId];
        }
        
        if(myCursor && myCursor.grabbed) {
            myWidget = myCursor.grabHolder;
        } else {
            if (theGesture.cursorid in _myPickList) {
                myWidget = _myPickList[theGesture.cursorid];
            } else {
                myWidget = Public.pickWidget(myPos.x, myPos.y);
                _myPickList[theGesture.cursorid] = myWidget;
            }
        }
        if(!myWidget) {
            myWidget = Public;
        }
        if (myCursor) {
            myCursor.update(myWidget, myPos);
        }
        var myPartnerCursor = null;
        var myCenterPoint = null;
        // Do some multicursor gesture specific things (zoom/rotate)
        if (theGesture.type == "cursor_pair_start" || theGesture.type == "zoom" || theGesture.type == "cursor_pair_finish" || theGesture.type == "rotate") {
            if (theGesture.type != "cursor_pair_finish") {
                myCenterPoint = new Vector3f(theGesture.centerpoint.x * myPositionScale.x, theGesture.centerpoint.y * myPositionScale.y, 0);
            }
            var mySparkconformedpartnerCursorId = getSparkConformedCursorId(theGesture.baseeventtype, theGesture.cursorpartnerid);
            // Get gesture partner cursor in case this is a multicursor event (cursor_pair_start, zoom/rotate, cursor_pair_finish)
            if (mySparkconformedpartnerCursorId in _myMultitouchCursors) {
                myPartnerCursor = _myMultitouchCursors[mySparkconformedpartnerCursorId];
            }            
            // zoom, rotate and cursor_pair_start need both cursors
            if ((theGesture.type=="cursor_pair_start" || theGesture.type=="zoom" || theGesture.type=="rotate")&& (!myCursor || !myPartnerCursor)) {
                return;
            }
        }
        
        switch(theGesture.type) {    
            case "wipe":            
                var myDir = new Vector3f(theGesture.direction.x * myPositionScale.x, theGesture.direction.y * myPositionScale.y, 0);
                var myWipeEvent = new spark.WipeGestureEvent(spark.GestureEvent.WIPE, theGesture.baseeventtype, myDir, myCursor);
                myWidget.dispatchEvent(myWipeEvent);
                break;                
            case "cursor_pair_start":
                var myZoomEvent = new spark.ZoomGestureEvent(spark.GestureEvent.CURSOR_PAIR_START, theGesture.baseeventtype, myCursor, myPartnerCursor, null, null, myCenterPoint);
                myWidget.dispatchEvent(myZoomEvent);
                break;                
            case "zoom":
                var myFirstDistance = theGesture.initialdistance;
                var myDistance = theGesture.distance;
                var myLastDistance = theGesture.lastdistance;
                var myZoomEvent = new spark.ZoomGestureEvent(spark.GestureEvent.ZOOM, theGesture.baseeventtype, myCursor, myPartnerCursor, myFirstDistance, myDistance, myCenterPoint, myLastDistance);
                myWidget.dispatchEvent(myZoomEvent);
                break;                
            case "cursor_pair_finish":                
                var myZoomEvent = new spark.ZoomGestureEvent(spark.GestureEvent.CURSOR_PAIR_FINISH, theGesture.baseeventtype, myCursor, myPartnerCursor);
                myWidget.dispatchEvent(myZoomEvent);
                break;                
            case "rotate":
                var myRotateEvent = new spark.RotateGestureEvent(spark.GestureEvent.ROTATE, theGesture.baseeventtype, myCursor, myPartnerCursor, theGesture.angle, myCenterPoint);
                myWidget.dispatchEvent(myRotateEvent);
                break;
            default:
                Logger.info("Unknown gesture : " + theGesture);            
        }
    }

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
    
    function getSparkConformedCursorId(theBaseEventType, theId) {
        var mySparkConformedCursorId = "";
        if (theBaseEventType == ASS_BASE_EVENT) {
            mySparkConformedCursorId  = "pmtx" + theId;
        } else {
            mySparkConformedCursorId  = "tuio" + theId;
        }
        return mySparkConformedCursorId;
    }


    function getMultitouchCursorId(theEvent) {
        switch(theEvent.callback) {
        case "onASSEvent":
            return "pmtx" + theEvent.id;
        case "onTuioEvent":
            return "tuio" + theEvent.id;
        default:
            Logger.fatal("Unknown multitouch event type");
            return null;
        }
    };
    
    
    function getMultitouchCursorPosition(theEvent) {
        switch(theEvent.callback) {
        case "onASSEvent":
            var myPosition = new Point2f(theEvent.position3D.x, theEvent.position3D.y);
            return getAveragedAssPosition(theEvent.id, myPosition);
        case "onTuioEvent":
            return new Point2f(theEvent.position.x * Public.width,
                               theEvent.position.y * Public.height);
        default:
            Logger.fatal("Unknown multitouch event type");
            return null;
        }
    };
    
    
    function initCursorPosHistory(theCursorId) {
        if (!(theCursorId in _myCursorPositionHistory)) {
            _myCursorPositionHistory[theCursorId] = [];
        }
    }
    
    function removeCursorPosHistory(theCursorId) {
        if (theCursorId in _myCursorPositionHistory) {
            delete _myCursorPositionHistory[theCursorId];
        }
    }
    
    function getAveragedAssPosition(theCursorId, thePosition) {
        
        if (!(theCursorId in _myCursorPositionHistory)) {
            return thePosition;
        }
        
        if (_myCursorPositionHistory[theCursorId].length >= MAX_CURSOR_POSITIONS_IN_HISTORY) {
            _myCursorPositionHistory[theCursorId].shift();
        }
        _myCursorPositionHistory[theCursorId].push(thePosition);
        var myCounter = 0;
        
        var myAveragedPos = new Vector2f(0,0);
        for (var i = 0; i < _myCursorPositionHistory[theCursorId].length; i++) {
            myCounter += i;
            myAveragedPos[0] += _myCursorPositionHistory[theCursorId][i][0]*i;
            myAveragedPos[1] += _myCursorPositionHistory[theCursorId][i][1]*i;
        }
        myAveragedPos[0] /= myCounter;
        myAveragedPos[1] /= myCounter;
        return myAveragedPos;
    }

    function handleMultitouchEvent(theEvent) {
        if(theEvent.callback == "onASSEvent") {
            spark.proximatrix.onASSEvent(theEvent);
        }

        var myId = getMultitouchCursorId(theEvent);
        switch(theEvent.type) {
        case "configure":
            Logger.info("proximatrix got configured");
            break;

        case "add":
            initCursorPosHistory(theEvent.id);
        case "move": // proximatrix
        case "update": // tuio        
            var myCursor;
            if(myId in _myMultitouchCursors) {
                myCursor = _myMultitouchCursors[myId];
            } else {
                Logger.debug("Cursor " + myId + " added");
                myCursor = new spark.Cursor(myId);
                _myMultitouchCursors[myId] = myCursor;
            }
            if(theEvent.type == "add") {
                myCursor.activate();
            }
            var myPosition = getMultitouchCursorPosition(theEvent);
            
            var myFocused = myCursor.focused;

            var myPick = null;
            if(myCursor.grabbed) {
                myPick = myCursor.grabHolder;
            } else {
                if (theEvent.id in _myPickList) {
                    myPick = _myPickList[theEvent.id];
                } else {
                    myPick = Public.pickWidget(myPosition.x, myPosition.y);
                    _myPickList[theEvent.id] = myPick;
                }
            }
            if(!myPick) {
                myPick = Public;
            }
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
                Logger.debug("Cursor " + myId + " moves to " + myPosition + " over " + myPick);
                var myMove = new spark.CursorEvent(spark.CursorEvent.MOVE, myCursor);
                myPick.dispatchEvent(myMove);
            }

            break;

        case "remove":
            if(myId in _myMultitouchCursors) {
                Logger.debug("Cursor " + myId + " removed");
                var myCursor   = _myMultitouchCursors[myId];
                var myPosition = getMultitouchCursorPosition(theEvent);
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
            removeCursorPosHistory(theEvent.id);
            break;
        }
    };

    Public.onASSEvent = handleMultitouchEvent;
    Public.onTuioEvent = handleMultitouchEvent;

};
