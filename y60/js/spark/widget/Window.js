/**
 * Simple wrapper for the Y60 scene viewer.
 */

use("SceneViewer.js"); // XXX: bad place for this

spark.Window = spark.ComponentClass("Window");

spark.Window.Constructor = function(Protected) {
    var Public = this;
    var Base = {};
    
    this.Inherit(spark.Widget);

    SceneViewer.prototype.Constructor(this, []);

    Public.title getter = function()  {
        return window.title;
    };
    
    Public.title setter = function(theTitle) {
        window.title = theTitle;
    };
    
    Base.realize = Public.realize;
    Public.realize = function() {
        window = new RenderWindow();
        
        window.position = [
            Protected.getNumber("positionX", 0),
            Protected.getNumber("positionY", 0)
        ];
        
        window.decorations = Protected.getBoolean("decorations", true);
        
        Public.setup(Protected.getNumber("width", 640),
                     Protected.getNumber("height", 480),
                     Protected.getBoolean("fullscreen", false),
                     Protected.getString("title", "SPARK Application"));

        Public.setMover(null);
        
        window.showMouseCursor = Protected.getBoolean("mouseCursor", false);        
        window.swapInterval = Protected.getNumber("swapInterval", 1);
        
        spark.setupCameraOrtho(window.scene.dom.find(".//camera"), window.width, window.height);
        
        Base.realize(window.scene.world);
    };
    
    const PICK_RADIUS = 1;
    
    Public.pickWidget = function(theX, theY) {
        var myBody = Public.picking.pickBodyBySweepingSphereFromBodies(theX, theY, PICK_RADIUS, Public.sceneNode);
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
    };
    
    Protected.updateMousePosition = function(theX, theY) {
        _myMousePosition.x = theX;
        _myMousePosition.y = theY;
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
    
    
    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    //  Will be called first in renderloop, has the time since application start
    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime, theDeltaT) {
        Base.onFrame(theTime, theDeltaT);
        var myEvent = new spark.StageEvent(spark.StageEvent.FRAME, Public, theTime, theDeltaT);
        Public.dispatchEvent(myEvent);
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
        
        Protected.updateMousePosition(theX, theY);
        
        var myWidget = Public.pickWidget(theX, theY);
        if(myWidget) {
            if(myWidget != _myMouseFocused) {
                Logger.debug("Mouse focuses " + myWidget
                             + (_myMouseFocused ? ", leaving " + _myMouseFocused : ""));
                
                if(_myMouseFocused) {
                    var myLeaveEvent = new spark.MouseEvent(spark.MouseEvent.LEAVE, theX, theY);
                    _myMouseFocused.dispatchEvent(myLeaveEvent);
                }
                
                _myMouseFocused = myWidget;
                
                var myEnterEvent = new spark.MouseEvent(spark.MouseEvent.ENTER, theX, theY);
                myWidget.dispatchEvent(myEnterEvent);
            }
        } else {
            if(_myMouseFocused) {
                Logger.debug("Mouse leaves " + _myMouseFocused);
                var myLeaveEvent = new spark.MouseEvent(spark.MouseEvent.LEAVE, theX, theY);
                _myMouseFocused.dispatchEvent(myLeaveEvent);
            }
        }
        _myMouseFocused = myWidget;
    };

    // Will be called on a mouse button
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        Base.onMouseButton(theButton, theState, theX, theY);
        
        Protected.updateMousePosition(theX, theY);
        
        if(theState) {
            var myWidget = Public.pickWidget(theX, theY);
            if(myWidget) {
                Logger.debug("Mouse clicks " + myWidget);
                var myEvent = new spark.MouseEvent(spark.MouseEvent.CLICK, theX, theY);
                myWidget.dispatchEvent(myEvent);
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
                (theShiftFlag ? spark.Keyboard.SHIFT : 0)
                | (theControlFlag ? spark.Keyboard.CTRL : 0)
                | (theAltFlag ? spark.Keyboard.ALT : 0);
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
        
        if(_myMouseFocused) {
            Logger.debug("Mouse scrolls " + _myMouseFocused + " by [" + theDeltaX + "," + theDeltaY + "]");
            var myEvent = new spark.MouseEvent(
                spark.MouseEvent.SCROLL,
                _myMousePosition.x, _myMousePosition.y,
                theDeltaX, theDeltaY
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
    
};
