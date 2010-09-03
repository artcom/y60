/*jslint nomen:false plusplus:false*/
/*globals spark, Vector3f, sum*/

spark.Slider = spark.ComponentClass("Slider");

spark.Slider.Constructor = function (Protected) {
    var Base   = {};
    var Public = this;
    Public.Inherit(spark.Transform);

    /////////////////////
    // Private Members //
    /////////////////////

    var _mySliderBackground = null;
    var _myActiveCursor     = null;
    var _myIdleCursor       = null;
                            
    var _horizontalLock     = null;
    var _verticalLock       = null;
    var _centered           = null;
    var _sticky             = null;
                            
    var _myEventTarget      = null;
                            
    var _myCursorOrigin     = null;
    var _myPosHistory       = {};
    
    var _stopTime           = 0;

    var DAMPENING_HISTORY = 5;
    
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function dampPosition(theEvent) {
        var i;
        _myPosHistory[theEvent.cursor.id].push(new Vector3f(theEvent.stageX, theEvent.stageY, 0.0));
        while (_myPosHistory[theEvent.cursor.id].length > DAMPENING_HISTORY) {
            _myPosHistory[theEvent.cursor.id].shift();
        }
        var myDampenedPos = new Vector3f(0.0, 0.0, 0.0);
        for (i = 0; i < _myPosHistory[theEvent.cursor.id].length; i++) {
            myDampenedPos = sum(myDampenedPos, _myPosHistory[theEvent.cursor.id][i]);
        }
        for (i = 0; i < myDampenedPos.length; i++) {
            myDampenedPos[i] /= _myPosHistory[theEvent.cursor.id].length;
        }
        theEvent.dampenedPos = myDampenedPos;
    }

    function getRelativeY() {
        if (_mySliderBackground.height === _myActiveCursor.height) {
            return 0;
        } else {
            return (_myActiveCursor.y - _mySliderBackground.y) /
                   (_mySliderBackground.height - _myActiveCursor.height);
        }
    }

    function getRelativeX() {
        if (_mySliderBackground.width === _myActiveCursor.width) {
            return 0;
        } else {
            return (_myActiveCursor.x - _mySliderBackground.x) /
                   (_mySliderBackground.width - _myActiveCursor.width);
        }
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////

    Public.__defineSetter__("eventTarget", function (theTarget) {
        _myEventTarget = theTarget;
    });
    
    Public.__defineSetter__("centered", function (theFlag) {
        _centered = theFlag;
    });
    
    Public.__defineGetter__("centered", function () {
        return _centered;
    });
    
    Public.__defineSetter__("sticky", function (theFlag) {
        _sticky = theFlag;
    });
    
    Public.__defineGetter__("sticky", function () {
        return _sticky;
    });
    
    Public.__defineGetter__("activeCursor", function () {
        return _myActiveCursor;
    });
    
    Public.__defineGetter__("idleCursor", function () {
        return _myIdleCursor;
    });
    
    Public.__defineGetter__("horizontalLock", function () {
        return _horizontalLock;
    });
    
    Public.__defineSetter__("horizontalLock", function (theHorizontalLock) {
        _horizontalLock = theHorizontalLock;
    });
    
    Public.__defineGetter__("verticalLock", function () {
        return _verticalLock;
    });
    
    Public.__defineSetter__("verticalLock", function (theVerticalLock) {
        _verticalLock = theVerticalLock;
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        _mySliderBackground = Public.getChildByName("background");
        _myActiveCursor     = _mySliderBackground.getChildByName("active-cursor");
        _myIdleCursor       = _mySliderBackground.getChildByName("idle-cursor");
        _myCursorOrigin     = _myIdleCursor.position;
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        Base.postRealize();
        //map mouse events on cursor event, so that cursor.grab can be used
        _verticalLock   = Protected.getBoolean("vertical-lock", false);
        _horizontalLock = Protected.getBoolean("horizontal-lock", false);
        _centered       = Protected.getBoolean("centered", true);
        _sticky         = Protected.getBoolean("sticky", true);
        Public.addEventListener(spark.GenericCursorEvent.APPEAR_TOUCHENTER, Public.onSlideStart, true);
        Public.addEventListener(spark.GenericCursorEvent.MOVE, Public.onSlide, true);
        Public.addEventListener(spark.GenericCursorEvent.VANISH_TOUCHLEAVE, Public.onSlideStop, true);

        if (_centered) {
            Public.centerCursor();
        }
    };

    Public.onSlideStart = function (theEvent) {
        var currentTime = millisec();
        if(Math.abs(currentTime - _stopTime) < 3) {
            return;
        }
        _myPosHistory[theEvent.cursor.id] = [];
        _myIdleCursor.visible   = false;
        _myActiveCursor.visible = true;
        theEvent.cursor.grab(theEvent.target);
        Public.onSlide(theEvent);
        if (_myEventTarget) {
            var mySliderStart = new spark.SliderEvent(spark.SliderEvent.START,
                                                      Public.name,
                                                      getRelativeX(),
                                                      getRelativeY());
            _myEventTarget.dispatchEvent(mySliderStart);
        }
    };

    Public.onSlideStop = function (theEvent) {
        _stopTime = millisec();
        if(theEvent.cursor.id in _myPosHistory) {
            Public.onSlide(theEvent);
            delete _myPosHistory[theEvent.cursor.id];
            _myIdleCursor.position = _myActiveCursor.position;
            _myIdleCursor.visible   = true;
            _myActiveCursor.visible = false;
            theEvent.cursor.ungrab(theEvent.target);
            if (!_sticky) {
                if (_centered) {
                    Public.centerCursor();
                } else {
                    _myIdleCursor.position   = _myCursorOrigin;
                    _myActiveCursor.position = _myCursorOrigin;
                }
            }
            if (_myEventTarget) {
                var mySliderStop = new spark.SliderEvent(spark.SliderEvent.STOP,
                                                         Public.name,
                                                         getRelativeX(),
                                                         getRelativeY());
                _myEventTarget.dispatchEvent(mySliderStop);
            }
        }
    };

    Public.onSlide = function (theEvent) {
        if (theEvent.cursor.id in _myPosHistory) {
            dampPosition(theEvent);
            if (!_horizontalLock) {
                var myNewX = theEvent.dampenedPos.x -
                             Public.x -
                             _mySliderBackground.x -
                             _myActiveCursor.width / 2;
                var myMinX = _mySliderBackground.x;
                if (myNewX < myMinX) {
                    _myActiveCursor.x = myMinX;
                } else {
                    var myMaxX = _mySliderBackground.x +
                                 _mySliderBackground.width -
                                 _myActiveCursor.width;
                    if (myNewX > myMaxX) {
                        _myActiveCursor.x = myMaxX;
                    } else {
                        _myActiveCursor.x = myNewX;
                    }
                }
            }
            if (!_verticalLock) {
                var myNewY = -theEvent.dampenedPos.y +
                             Public.stage.height -
                             Public.y -
                             _mySliderBackground.y -
                             _myActiveCursor.height / 2;
                var myMinY = _mySliderBackground.y;
                if (myNewY < myMinY) {
                    _myActiveCursor.y = myMinY;
                } else {
                    var myMaxY = _mySliderBackground.height - _myActiveCursor.height;
                    if (myNewY > myMaxY) {
                        _myActiveCursor.y = myMaxY;
                    } else {
                        _myActiveCursor.y = myNewY;
                    }
                }

            }
            if (_myEventTarget) {
                var mySliderMove = new spark.SliderEvent(spark.SliderEvent.MOVE,
                                                         Public.name,
                                                         getRelativeX(),
                                                         getRelativeY());
                _myEventTarget.dispatchEvent(mySliderMove);
            }
        }
    };
    
    Public.centerCursor = function () {
        _myIdleCursor.x   = _mySliderBackground.width / 2  - _myIdleCursor.width / 2;
        _myIdleCursor.y   = _mySliderBackground.height / 2 - _myIdleCursor.height / 2;
        _myActiveCursor.x = _myIdleCursor.x;
        _myActiveCursor.y = _myIdleCursor.y;
    };
};

spark.SliderEvent = spark.Class("SliderEvent");

spark.SliderEvent.MOVE    = "slider-move";
spark.SliderEvent.START   = "slider-start";
spark.SliderEvent.STOP    = "slider-stop";

spark.SliderEvent.Constructor = function (Protected, theType, theId, theX, theY) {
    var Public = this;
    Public.Inherit(spark.Event, theType);

    Public.__defineGetter__("id", function () {
        return theId;
    });

    Public.__defineGetter__("sliderX", function () {
        return theX;
    });

    Public.__defineGetter__("sliderY", function () {
        return theY;
    });
};