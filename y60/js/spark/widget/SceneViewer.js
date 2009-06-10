/**
 * Simple wrapper for the Y60 scene viewer.
 */
use("SceneViewer.js"); // XXX: bad place for this

spark.SceneViewer = spark.ComponentClass("SceneViewer");

spark.SceneViewer.Constructor = function(Protected) {
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
    
    //////////////////////////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////////////////////////

    //  Will be called first in renderloop, has the time since application start
    Base.onFrame = Public.onFrame;
    Public.onFrame = function(theTime) {
        Base.onFrame(theTime);
    };

    // Will be called before rendering the frame
    Base.onPreRender = Public.onPreRender;
    Public.onPreRender = function() {
        Base.onPreRender();
    };

    // Will be called after rendering the frame, but before swap buffer
    Base.onPostRender = Public.onPostRender;
    Public.onPostRender = function() {
        Base.onPostRender();
    };

    // Will be called on a mouse move
    var _myMouseFocusedWidget = null;
    Base.onMouseMotion = Public.onMouseMotion;
    Public.onMouseMotion = function(theX, theY) {
        Base.onMouseMotion(theX, theY);
        
        var myWidget = Public.pickWidget(theX, theY);
        if(myWidget) {
            if(myWidget != _myMouseFocusedWidget) {
                Logger.debug("Mouse focuses " + myWidget.name
                             + (_myMouseFocusedWidget ? ", leaving " + _myMouseFocusedWidget.name : ""));
                
                if(_myMouseFocusedWidget) {
                    var myLeaveEvent = new spark.MouseEvent(spark.MouseEvent.LEAVE, theX, theY);
                    _myMouseFocusedWidget.dispatchEvent(myLeaveEvent);
                }
                
                _myMouseFocusedWidget = myWidget;
                
                var myEnterEvent = new spark.MouseEvent(spark.MouseEvent.ENTER, theX, theY);
                myWidget.dispatchEvent(myEnterEvent);
            }
        } else {
            if(_myMouseFocusedWidget) {
                Logger.debug("Mouse leaves " + _myMouseFocusedWidget.name);
                var myLeaveEvent = new spark.MouseEvent(spark.MouseEvent.LEAVE, theX, theY);
                _myMouseFocusedWidget.dispatchEvent(myLeaveEvent);
            }
        }
        _myMouseFocusedWidget = myWidget;
    };

    // Will be called on a mouse button
    Base.onMouseButton = Public.onMouseButton;
    Public.onMouseButton = function(theButton, theState, theX, theY) {
        Base.onMouseButton(theButton, theState, theX, theY);
        
        if(theState) {
            var myWidget = Public.pickWidget(theX, theY);
            if(myWidget) {
                Logger.info("Mouse clicks " + myWidget.name);
                var myEvent = new spark.MouseEvent(spark.MouseEvent.CLICK, theX, theY);
                myWidget.dispatchEvent(myEvent);
            }
        }
    };

    // Will be called on a keyboard event
    var _myKeyboardFocusedWidget = null;
    Base.onKey = Public.onKey;
    Public.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag) {
        Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
        
        if(_myKeyboardFocusedWidget) {
            var myModifiers =
                (theShiftFlag ? spark.Keyboard.SHIFT : 0)
                | (theControlFlag ? spark.Keyboard.CTRL : 0)
                | (theAltFlag ? spark.Keyboard.ALT : 0);
            var myType = theKeyState ? spark.KeyboardEvent.KEY_DOWN : spark.KeyboardEvent.KEY_UP;
            var myEvent = new spark.KeyboardEvent(myType, theKey, myModifiers);
            _myKeyboardFocusedWidget.dispatchEvent(myEvent);
        }
    };

    // Will be called on a mouse wheel event
    Base.onMouseWheel = Public.onMouseWheel;
    Public.onMouseWheel = function(theDeltaX, theDeltaY) {
        Base.onMouseWheel(theDeltaX, theDeltaY);
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
