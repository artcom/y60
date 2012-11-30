//=============================================================================
// Copyright (C) 2009, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/*jslint forin: true*/
//howto use
//hold ctrl, click target, move with mouse or arrows or shift-arrows
//+ moves target to parent
//. moves target to next intersected widget
//
//a toggles additional features of target
//f change size of widget
//d change z-rotation of widget
//"down" moves down in z direction
//"up" moves up in z direction
//o toggles visibility of layoutImage, which can be an commandLineArgument (layoutimage=<src>, layoutimagesize=WxH)

//classes using Layouter can implement hooks
// * layouterUpdatePosition - for additional updates in the widget
// * layouterSetPosition - force specific position update in widget
// * writebackPosition
// * layouterToggleWidget - can be used to change widget style e.g.
//
//use bind to register callbacks for 
//CATCH: if a widget is clicked
//RELEASE: if a widget was released
//INTERSECTION_FILTER: use this to filter intersected widgets, return value of callback should be
//  'true' if widget is valid
//  'false' if widget should be filtered
//

spark.Layouter = spark.ComponentClass("Layouter");
spark.Layouter.BINDING_SLOT = {
    CATCH  : "CATCH",
    RELEASE : "RELEASE",
    INTERSECTION_FILTER : "INTERSECTION_FILTER"
};
spark.Layouter.Constructor = function(Protected) {

    var Base = {};
    var Public = this;
    this.Inherit(spark.Component);
    
    var _bindings = {};
    (function () {
        for (var slot in spark.Layouter.BINDING_SLOT) {
            _bindings[spark.Layouter.BINDING_SLOT[slot]] = {};
        }
    }());

    var IDLE = 0;
    var ACTIVE = 1;
    var LISTENING = 2;

    var _myState       = IDLE;
    var _myNewFrameFlag = true;
    var _myOldPos      = null;
    var _myWidget      = null;
    var _myShiftFlag   = false;
    var _myIsCtrlPressed = false;
    var _mySizeManipulation = false;   //change size 
    var _myZManipulation = false;   //change z position
    var _myRotationZManipulation = false;   //change z rotation
    var _myStage       = null;
    var _myListeners = [];
    var _myIntersections = []; //all intersected & filtered widgets
    var _myCurrentSparkNode = null;
    var _myCurrentSparkFile = null;
    var _mySparkNodes = [];
    var _mySparkFiles = [];
    var _myVisualMode = 2; // box with mesurement
    var _myLayoutImage = null;
    var _myBackup = {originalZ:null, originalWidth:null, originalHeight:null, originalRotationZ:null};
    var DEBUG_COLOR = new Vector4f(1,0,0,1);
    
    
    function _unbind(theHandle) {
        delete _bindings[theHandle.bind_info.slot][theHandle.bind_info.id];
        return true;
    }
    
    function Handle(theBindInfo, theCb) {
        this.bind_info = theBindInfo; //back-pointer
        this.cb = theCb;
    }
    
    Handle.prototype.unbind = function () {
        _unbind(this);
    };
    
    Public.bind = function (theBindingSlot, cb) {
        var bind_info = {id   : createUniqueId(),
                         slot : theBindingSlot};
        var my_handle = new Handle(bind_info, cb);
        _bindings[theBindingSlot][bind_info.id] = my_handle;
        return my_handle;
    };
    
    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        _myStage = findStage();
        _myStage.focusKeyboard(_myStage);
        
        if ("layoutimage" in _myStage.arguments) {
            _myLayoutImage = _myStage.arguments.layoutimage;
            Logger.warning("found layoutimage: " + _myLayoutImage);
            //XXX: TODO change to overlay
            var myNode = new Node("<Image name='layoutimage' z='50' src='" + _myLayoutImage + "' visible='false' alpha='0.4' sensible='false'/>");
            _myLayoutImage = spark.loadDocument(myNode, _myStage);
            _myLayoutImage.sensible = false;
        }
        if ("layoutimagesize" in _myStage.arguments && _myLayoutImage) {
            var mySizeString = _myStage.arguments.layoutimagesize;
            Logger.warning("found layoutimagesize: " + mySizeString);
            _myLayoutImage.width = mySizeString.split("x")[0];
            _myLayoutImage.height = mySizeString.split("x")[1];
        }
        Base.onKey = _myStage.onKey;
        _myStage.onKey = function(theKey, theKeyState, theX, theY,
                             theShiftFlag, theControlFlag, theAltFlag) {
            Logger.info("onKey "+ theKey+" "+_myZManipulation+" "+_mySizeManipulation+" "+_myShiftFlag);
            var myKeyEaten = true;
            if(theKeyState) {
                if(theKey.search(/ctrl/) != -1) {
                    _myIsCtrlPressed = true;
                    activate();
                } else if (Public.active && theKey.search(/shift/) != -1) {
                    _myShiftFlag = true;
                } else if (Public.active && theKey === "f") {
                    _mySizeManipulation = true;
                } else if (Public.active && (theKey === "y" || theKey === "z")) {
                    _myZManipulation = true;
                } else if (Public.active && theKey === "d") {
                    _myRotationZManipulation = true;
                } else if ((theKey === "left" || 
                            theKey === "right" || 
                            theKey === "up" || 
                            theKey === "down") && Public.active) {
                    correctPositionAndSize(theKey);
                } else if (((theKey === "+")||(theKey === "]")) && Public.active && _myWidget && _myWidget.parent) { 
                    Public.target = _myWidget.parent;
                    print("moved targed to parent: ", Public.target);
                } else if ((theKey === ".") && Public.active && _myIntersections.length > 1) { 
                    var myIndex = js.array.indexOf(_myIntersections, _myWidget);
                    Public.target = _myIntersections[(myIndex + 1) % _myIntersections.length];
                    print("moved targed to intersected widget: ", Public.target);
                } else if ((theKey === "a") && Public.active && _myWidget && "layouterToggleWidget" in _myWidget) {
                    _myWidget.layouterToggleWidget();
                } else if (Public.active && theKey === "x") {
                    _myVisualMode = (_myVisualMode + 1) % 3;
                } else if (Public.listening && theKey === "o" && _myLayoutImage) {
                    _myLayoutImage.visible = !_myLayoutImage.visible;
                    print("layoutimage "+ _myLayoutImage.visible);
                } else {
                    myKeyEaten = false;
                }
            } else {
                if(theKey.search(/ctrl/) != -1) {
                    _myIsCtrlPressed = false;
                    deactivate();
                    stop();   
                } else if (Public.active && theKey.search(/shift/) != -1) {
                    _myShiftFlag = false;
                } else if (Public.active && theKey === "f") {
                    _mySizeManipulation = false;
                } else if (Public.active && (theKey === "y" || theKey === "z")) {
                    _myZManipulation = false;
                } else if (Public.active && theKey === "d") {
                    _myRotationZManipulation = false;
                } else {
                    myKeyEaten = false;
                }
            }
            if (!myKeyEaten) {
                Base.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag);
            }
        };
        _myStage.addEventListener(spark.StageEvent.POST_RENDER, onPostRender);
        _myStage.addEventListener(spark.StageEvent.FRAME, function () {
            _myNewFrameFlag = true;
        });
    };
    
    function onPostRender() {
        if (_myIsCtrlPressed && Public.active && _myWidget && _myVisualMode > 0) {
            var myMatrix = new Matrix4f();
            var myBox = _myWidget.sceneNode.boundingbox;
            window.getRenderer().draw(myBox, DEBUG_COLOR, myMatrix, 2);
            if (_myVisualMode > 1) {
                window.getRenderer().draw(new LineSegment(new Point3f(0,myBox.min.y,window.camera.position.z-1),new Point3f(window.width,myBox.min.y,window.camera.position.z-1) ), DEBUG_COLOR, myMatrix, 1);
                window.getRenderer().draw(new LineSegment(new Point3f(myBox.min.x, 0, window.camera.position.z-1),new Point3f(myBox.min.x,window.height,window.camera.position.z-1) ), DEBUG_COLOR, myMatrix, 1);
                var myTextColor = new Vector4f(1,0,0,1);                                        
                var myViewport = _myStage.getViewportAtWindowCoordinates(0, 0);
                var myDifference = difference(myBox.min, new Vector3f(0,0,myBox.min.z));
                var myDifferenceTopLeft = difference(myBox.min, new Vector3f(0,window.height,myBox.min.z));
                window.renderText([clamp(myBox.min.x-80,0,window.width), clamp(window.height - (myBox.min.y-20), 0, window.height)], "[" + myDifference.x  + "," + myDifference.y + "]", new Node("<style textColor='" + myTextColor + "'/>"), "Screen13", myViewport );
                window.renderText([clamp(myBox.min.x-80, 0,window.width), clamp(window.height - (myBox.min.y+20), 0,window.height)], "[" + myDifferenceTopLeft.x  + "," + Math.abs(myDifferenceTopLeft.y) + "]", new Node("<style textColor='" + myTextColor + "'/>"), "Screen13", myViewport );
            }
        }
        
    }
    // poor man's Public.stage getter
    function findStage() {
        for (var object in spark.ourComponentsByNameMap) {
            if("Stage" in spark.ourComponentsByNameMap[object]._classes_) {
                return spark.ourComponentsByNameMap[object];
            }    
        }
    }

    Public.__defineGetter__("active", function () {
        return (_myState === ACTIVE);
    });

    Public.__defineGetter__("listening", function () {
        return (_myState === LISTENING);
    });

    Public.__defineGetter__("layoutimage", function () {
        return _myLayoutImage;
    });

    Public.__defineGetter__("target", function () {
        return _myWidget;
    });

    Public.__defineSetter__("target", function (theWidget) {
        print("target widget "+ theWidget);
        _myWidget = theWidget;
        for (var handleId in _bindings[spark.Layouter.BINDING_SLOT.CATCH]) {
            var myHandle = _bindings[spark.Layouter.BINDING_SLOT.CATCH][handleId];
            myHandle.cb(_myWidget);
        }
        if (_myOldPos) {
            _myOldPos.z = _myWidget.z;
        }
        _myBackup.originalZ = _myWidget.z;
        _myBackup.originalRotationZ = ("rotationZ" in _myWidget) ? _myWidget.rotationZ : null;
        _myBackup.originalWidth = ("width" in _myWidget) ? _myWidget.width : null;
        _myBackup.originalHeight = ("height" in _myWidget) ? _myWidget.height : null;
        _myState = ACTIVE;
        var myFile = "";
        if (!(_myWidget.name in _mySparkFiles)) {
            myFile = findSparkFile(_myWidget);
            _mySparkFiles[_myWidget.name] = myFile;
        }
        if (myFile) {
            if (!(myFile in _mySparkNodes)) {
                var myFileWithPath = searchFile(myFile);
                var myNode = new Node();
                myNode.parseFile(myFileWithPath);
                _myCurrentSparkNode = myNode;
                _mySparkNodes[myFile] = myNode;
            } else {
                _myCurrentSparkNode = _mySparkNodes[myFile];
            }
            _myCurrentSparkFile = myFile;
        }
    });

    function findSparkFile(theWidget) {
        if (!theWidget) {
            return "";
        }
        if (theWidget._sparkFile_) {
            return theWidget._sparkFile_;
        }
        return findSparkFile(theWidget.parent);
    }

    function updatePosition(theX, theY, theZ) {
        if (!_myNewFrameFlag) {
            return;
        }
        _myNewFrameFlag = false;
        if (!_myOldPos) {
            _myOldPos = new Vector3f(theX, window.height - theY, theZ);
            return;
        }
        var myRotation = _myWidget.sceneNode.globalmatrix.getRotation();
        var myWidgetRotation = new Vector3f(radFromDeg(_myWidget.rotation.x), radFromDeg(_myWidget.rotation.y), radFromDeg(_myWidget.rotation.z));
        myRotation = difference(myRotation, myWidgetRotation);
        var myXOffset = theX - _myOldPos.x;
        var myYOffset = (window.height - theY) - _myOldPos.y;
        var myZOffset = theZ - _myOldPos.z;
        var myMatrix = new Matrix4f();
        myMatrix.makeTranslating(new Vector3f(myXOffset, myYOffset, myZOffset));
        myMatrix.rotateZ(myRotation.z);
        var myTranslation = myMatrix.getTranslation();
        if (!("layouterSetPosition" in _myWidget)) {
            _myWidget.x += myTranslation.x;
            _myWidget.y += myTranslation.y;
            _myWidget.z += myTranslation.z;
            _myWidget.x = Math.round(_myWidget.x);
            _myWidget.y = Math.round(_myWidget.y);
            Logger.info(_myWidget.name + " moved to x: " + _myWidget.x + " y: " + _myWidget.y + " z: " +_myWidget.z);
        } else {
            _myWidget.layouterSetPosition(_myOldPos, new Vector3f(theX, theY, theZ), myTranslation);
        }
        if ("layouterUpdatePosition" in _myWidget) {
            _myWidget.layouterUpdatePosition(_myOldPos, new Vector3f(theX, theY, theZ), myTranslation);
        }
        _myOldPos = new Vector3f(theX, window.height - theY, theZ);
    }
    
    function activate () {
        print("activate");
        _myState = LISTENING;
        _myStage.addEventListenerInFront(spark.CursorEvent.APPEAR, onMouseDown , true);
        _myStage.addEventListenerInFront(spark.CursorEvent.MOVE, onMouseMove, true);
        _myStage.addEventListenerInFront(spark.MouseEvent.BUTTON_DOWN, onMouseDown , true);
        _myStage.addEventListenerInFront(spark.MouseEvent.MOVE, onMouseMove, true);
    }
    
    function deactivate() {
        print("deactivate");
        _myStage.removeEventListener(spark.CursorEvent.APPEAR, onMouseDown, true);
        _myStage.removeEventListener(spark.CursorEvent.MOVE, onMouseMove, true);
        _myStage.removeEventListener(spark.MouseEvent.BUTTON_DOWN, onMouseDown , true);
        _myStage.removeEventListener(spark.MouseEvent.MOVE, onMouseMove, true);
        if (_myWidget) {
            for (var i = 0; i < _myListeners.length; ++i) {
                _myWidget.addEventListener(spark.CursorEvent.APPEAR, _myListeners[i].listener, _myListeners[i].useCapture);
            }
        }
        _myIntersections = [];
    }

    function collectIntersections (theX, theY, theCanvas) {
        var myIntersectionInformation = window.scene.getPickedBodiesInformation(theX, theY, theCanvas);
        if (myIntersectionInformation) {
            for (var i = myIntersectionInformation.length-1; i >= 0; --i) {
                var myWidget = null;
                var myBodyId = myIntersectionInformation[i].body.id;
                if (myBodyId in spark.sceneNodeMap) {
                    myWidget = spark.sceneNodeMap[myBodyId];
                    if ("Canvas" in myWidget._classes_) {
                        var myCanvasPosition = myWidget.convertToCanvasCoordinates(theX, theY);
                        collectIntersections(myCanvasPosition.x, myCanvasPosition.y, myWidget.canvas);
                    }
                }
                var isValid = true;
                for (var handleId in _bindings[spark.Layouter.BINDING_SLOT.INTERSECTION_FILTER]) {
                    var myHandle = _bindings[spark.Layouter.BINDING_SLOT.INTERSECTION_FILTER][handleId];
                    isValid &= myHandle.cb(myIntersectionInformation[i], myWidget);
                }
                if (isValid && myWidget) {
                    _myIntersections.push(myWidget);
                }
            }
        }
    }

    function onMouseDown(theEvent) {
        if (!_myNewFrameFlag) {
            return;
        }
        if (theEvent.target === _myStage) {
            return;
        }
        collectIntersections(theEvent.stageX, theEvent.stageY);
        _myIntersections.sort(function (a, b) {
            return (b.sceneNode.globalmatrix.getTranslation().z - a.sceneNode.globalmatrix.getTranslation().z);
        });
        if (!_myWidget && _myIntersections.length > 0) {
            Public.target = _myIntersections[0];
            _myListeners = clone(theEvent.target.getEventListeners(spark.CursorEvent.APPEAR));
        }
        for (var i = 0; i < _myListeners.length; ++i) {
            theEvent.target.removeEventListener(spark.CursorEvent.APPEAR, _myListeners[i].listener, _myListeners[i].useCapture);
        }
        if (!theEvent.cancelable) {
            theEvent.cancelable = true;
        }
        theEvent.cancelDispatch();
        if (_myWidget) {
            updatePosition(theEvent.stageX, theEvent.stageY, _myWidget.z);
        }
    }

    function onMouseMove(theEvent) {
        if (!_myNewFrameFlag) {
            return;
        }
        if (_myIsCtrlPressed && Public.active) {
            updatePosition(theEvent.stageX, theEvent.stageY, _myWidget.z);
        }
    }

    function correctPositionAndSize(theKey) {
        if (!_myOldPos) {
            return;
        }
        var myNewPos = new Vector3f(0, 0, 0);
        var myDist = 1;
        if (_myShiftFlag) {
            myDist = 5;
        }
        if (_myZManipulation) {  // z-pos manipulation
            switch(theKey) {
                case "up":
                    myNewPos = new Vector3f(_myOldPos.x, _myOldPos.y, _myOldPos.z + myDist);
                    break;  
                case "down":
                    myNewPos = new Vector3f(_myOldPos.x, _myOldPos.y, _myOldPos.z - myDist);
                    break;              
                default:
                    break;
            }
            updatePosition(myNewPos.x, window.height - myNewPos.y, myNewPos.z);
        } else if (_mySizeManipulation) { // size manipulation
            switch(theKey) {
                case "left":
                    if (_myWidget.width) {
                        _myWidget.width -= myDist;
                    } 
                    break;
                case "right":
                    if (_myWidget.width) {
                        _myWidget.width += myDist;
                    } 
                    break;
                case "up":
                    if (_myWidget.height) {
                        _myWidget.height += myDist;
                    } 
                    break;  
                case "down":
                    if (_myWidget.height) {
                        _myWidget.height -= myDist;
                    } 
                    break;              
                default:
                    break;
            }
        } else if (_myRotationZManipulation) { // rotationZ manipulation
            switch(theKey) {
                case "up":
                    if ("rotationZ" in _myWidget) {
                        _myWidget.rotationZ += myDist;
                    } 
                    break;
                case "down":
                    if ("rotationZ" in _myWidget) {
                        _myWidget.rotationZ -= myDist;
                    } 
                    break;
                default:
                    break;
            }
        } else { 
            switch(theKey) {
                case "left":
                    myNewPos = new Vector3f(_myOldPos.x-myDist, _myOldPos.y, _myOldPos.z);
                    break;
                case "right":
                    myNewPos = new Vector3f(_myOldPos.x+myDist, _myOldPos.y, _myOldPos.z);
                    break;
                case "up":
                    myNewPos = new Vector3f(_myOldPos.x, _myOldPos.y+myDist, _myOldPos.z);
                    break;  
                case "down":
                    myNewPos = new Vector3f(_myOldPos.x, _myOldPos.y-myDist, _myOldPos.z);
                    break;              
                default:
                    break;
            }
            updatePosition(myNewPos.x, window.height - myNewPos.y, myNewPos.z);
        }
    }

    function stop() {
        print("stop widget:"+_myWidget);
        _myState = IDLE;
        _myOldPos = null;
        if (_myWidget && _myCurrentSparkNode) {
            if ("writebackPosition" in _myWidget) {
                _myWidget.writebackPosition(_myCurrentSparkNode, _myCurrentSparkFile);
            } else {
                var myNode = findWritebackNode(_myWidget);
                if (myNode) {
                    myNode.x = Math.round(_myWidget.x);
                    myNode.y = Math.round(_myWidget.y);
                    if (_myWidget.z !== _myBackup.originalZ) {
                        myNode.z = Math.round(_myWidget.z);
                    }
                    if (_myWidget.rotationZ !== _myBackup.originalRotationZ) {
                        myNode.rotationZ = Math.round(_myWidget.rotationZ);
                    }
                    if ("width" in _myWidget && _myWidget.width && _myWidget.width !== _myBackup.originalWidth) {
                        myNode.width = Math.round(_myWidget.width);
                    }
                    if ("height" in _myWidget && _myWidget.height && _myWidget.height !== _myBackup.originalHeight) {
                        myNode.height = Math.round(_myWidget.height);
                    }
                    _myCurrentSparkNode.saveFile(_myCurrentSparkFile);
                    print("write to file ", _myCurrentSparkFile);
                }
            }
            for (var handleId in _bindings[spark.Layouter.BINDING_SLOT.RELEASE]) {
                var myHandle = _bindings[spark.Layouter.BINDING_SLOT.RELEASE][handleId];
                myHandle.cb(_myWidget);
            }
        }
        _myWidget = null;
    }

    function findWritebackNode(theWidget) {
        var myNodes = _myCurrentSparkNode.findAll(".//*[@name='" + _myWidget.name + "']");
        for (var i = 0, l = myNodes.length; i < l; ++i) {
            var myWidget = theWidget;
            var myNode = myNodes[i];
            while(myWidget && myWidget.parent && myNode.parentNode && !("Stage" in myWidget._classes_)) {
                if (myWidget.parent.name && "name" in myNode.parentNode && myWidget.parent.name !== myNode.parentNode.name) 
                {
                    myWidget = null;
                    break;
                }
                myWidget = myWidget.parent;
                myNode = myNode.parentNode;
            }
            if (myWidget) {
                return myNodes[i];
            }
        }
        return null;
    }
};

