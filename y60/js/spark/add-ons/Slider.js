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
    var _snappy             = null;
    var _precision          = 10;
                            
    var _myEventTarget      = null;
                            
    var _myCursorOrigin     = null;
    var _myPosHistory       = {};
    
    var _stopTime           = 0;
    var _myCursorOffset     = null;

    var DAMPENING_HISTORY   = 5;

    var _rasterValues             = [];
    var _rasterIndex              = null;
    var _relativeRasterPositions  = [];
    var _initValue                = null;
    var _initIndex                = null;
    
    
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

    function ensurePrecision() {
        if (!_verticalLock && _mySliderBackground.height !== _myActiveCursor.height) {
            _myActiveCursor.y = getRelativeY() * (_mySliderBackground.height - _myActiveCursor.height);
        }
        if (!_horizontalLock && _mySliderBackground.width !== _myActiveCursor.width) {
            _myActiveCursor.x = getRelativeX() * (_mySliderBackground.width - _myActiveCursor.width);
        }
    }
    
    function getRelativeY() {
        if (_verticalLock) {
            return 0;
        }
        if (_mySliderBackground.height === _myActiveCursor.height) {
            return 0;
        } else if (_relativeRasterPositions.length > 0) {
            var myRelativeY = _myActiveCursor.y / (_mySliderBackground.height - _myActiveCursor.height);
            setRasterIndex(myRelativeY);
            return _relativeRasterPositions[_rasterIndex];
        } else {
            var myRelativeY = _myActiveCursor.y / (_mySliderBackground.height - _myActiveCursor.height);
            return (_precision === 0) ? myRelativeY : myRelativeY.toFixed(_precision);
        }
    }

    function getRelativeX() {
        if (_horizontalLock) {
            return 0;
        }
        if (_mySliderBackground.width === _myActiveCursor.width) {
            return 0;
        } else if (_relativeRasterPositions.length > 0) {
            var myRelativeX = _myActiveCursor.x / (_mySliderBackground.width - _myActiveCursor.width);
            setRasterIndex(myRelativeX);
            return _relativeRasterPositions[_rasterIndex];
        } else {
            var myRelativeX = _myActiveCursor.x / (_mySliderBackground.width - _myActiveCursor.width);
            return (_precision === 0) ? myRelativeX : myRelativeX.toFixed(_precision);
        }
    }

    function setRasterIndex(thePos){
        
        var myDistance = 0;
        var mySnapPos = 1;
        for (var i = 0; i < _relativeRasterPositions.length; i++) {
            myDistance = Math.abs(_relativeRasterPositions[i] - thePos);
            if (myDistance < mySnapPos) {
                mySnapPos = myDistance;
                _rasterIndex = i;
            }                    
        };
    }

    var centerCursor = function () {
        _myCursorOrigin.x = _mySliderBackground.width / 2  - _myIdleCursor.width / 2;
        _myCursorOrigin.y = _mySliderBackground.height / 2  - _myIdleCursor.height / 2;
        Public.resetCursor();
    };
    
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
    
    Public.__defineSetter__("snappy", function (theFlag) {
        _snappy = theFlag;
    });
    
    Public.__defineGetter__("snappy", function () {
        return _snappy;
    });
    
    Public.__defineSetter__("precision", function (thePrecision) {
        _precision = thePrecision;
    });
    
    Public.__defineGetter__("precision", function () {
        return _precision;
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

    Public.__defineGetter__("value", function () { 
        if(_verticalLock) {
            return getRelativeX();
        } else {
            return getRelativeY();
        }
        
    });

    Public.__defineGetter__("rasterValue", function () { 
        if (_rasterValues.length > 0) {
            return _rasterValues[_rasterIndex];
        } 
        return null;
    });

    Public.__defineGetter__("initIndex", function () {
        return _initIndex;
    });

    Public.__defineGetter__("relativeRasterPositions", function () {
        return _relativeRasterPositions;
    });

    Base.realize = Public.realize;
    Public.realize = function () {
        Base.realize();
        _mySliderBackground = Public.getChildByName("background");
        _myActiveCursor     = _mySliderBackground.getChildByName("active-cursor");
        _myIdleCursor       = _mySliderBackground.getChildByName("idle-cursor");
        _myIdleCursor.visible = true;
        _myActiveCursor.visible = false;
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
        _snappy         = Protected.getBoolean("snappy", true);
        _precision      = Protected.getNumber("precision", _precision);
        Public.addEventListener(spark.CursorEvent.APPEAR_ENTER, Public.onSlideStart, true);
        Public.addEventListener(spark.CursorEvent.MOVE, Public.onSlide, true);
        Public.addEventListener(spark.CursorEvent.VANISH_LEAVE, Public.onSlideStop, true);

        if (_centered) {
            centerCursor();
        }
        _rasterValues = Protected.getArray("rasterValues", []);
        if (_rasterValues.length > 0) {
            var myDistance = 1/(_rasterValues.length-1);
            for (var i= 0; i<_rasterValues.length; i++) {
                _relativeRasterPositions.push(i*myDistance);
            }
        }
        _initValue = Protected.getNumber("initValue", 0);
        _rasterIndex = 0;
       
        if (_rasterValues.length > 0) {
            for (i= 0; i<_rasterValues.length; i++) {
                if (_initValue == _rasterValues[i]) {
                    _rasterIndex = i;
                    break;
                }
            }
        }
        _initIndex = _rasterIndex;
        
        if (_rasterValues.length > 0) {
            Public.setRelativeCursorPosition(_relativeRasterPositions[_rasterIndex]);
        } else {
            Public.setRelativeCursorPosition(_initValue);
        }
        
        
    };

    Public.reset = function() {
        _rasterIndex = 0;
        
        if (_rasterValues.length > 1) {
            for (var i= 0; i<_rasterValues.length; i++) {
                if (_initValue == _rasterValues[i]) {
                    _rasterIndex = i;
                    break;
                }
            }
        }
        
        if (_rasterValues.length > 0) {
            Public.setRelativeCursorPosition(_relativeRasterPositions[_rasterIndex]);
        } else {
            Public.setRelativeCursorPosition(_initValue);
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
        _myCursorOffset = new Vector2f(theEvent.stageX - _myActiveCursor.worldPosition.x, 
                                        Public.stage.height - theEvent.stageY - _myActiveCursor.worldPosition.y);
        if ((_myCursorOffset.x > _myActiveCursor.width || _myCursorOffset.x < 0) && _snappy) {
            _myCursorOffset.x = _myActiveCursor.width/2;
        }
         if ((_myCursorOffset.y > _myActiveCursor.height || _myCursorOffset.y < 0) && _snappy) {
            _myCursorOffset.y = _myActiveCursor.height/2;
        }
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
                Public.resetCursor();
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
                             Public.worldPosition.x -
                             _mySliderBackground.x -
                             _myCursorOffset.x;
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
                             Public.worldPosition.y -
                             _mySliderBackground.y -
                             _myCursorOffset.y;
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
            ensurePrecision();
            if (_myEventTarget) {
                var mySliderMove = new spark.SliderEvent(spark.SliderEvent.MOVE,
                                                         Public.name,
                                                         getRelativeX(),
                                                         getRelativeY());
                _myEventTarget.dispatchEvent(mySliderMove);
            }
        }
    };
    
    Public.resetCursor = function() {
        _myIdleCursor.x   = _myCursorOrigin.x;
        _myIdleCursor.y   = _myCursorOrigin.y;
        _myActiveCursor.x = _myIdleCursor.x;
        _myActiveCursor.y = _myIdleCursor.y;
        // to update rasterIndex
        getRelativeX();
        getRelativeY();
    };

    Public.setCursorPosition = function (theX, theY) {
        var myX = (theX !== undefined) ? theX : _myIdleCursor.x;
        var myY = (theY !== undefined) ? theY : _myIdleCursor.y;
        _myIdleCursor.x   = myX;
        _myIdleCursor.y   = myY;
        _myActiveCursor.x = myX;
        _myActiveCursor.y = myY;
        _myCursorOrigin.x = myX;
        _myCursorOrigin.y = myY;
        // to update rasterIndex
        getRelativeX();
        getRelativeY();
    };

    Public.setRelativeCursorPosition = function (theX, theY) {
        var myX = (theX !== undefined) ? theX * (_mySliderBackground.width - _myIdleCursor.width) : _myIdleCursor.x;
        var myY = (theY !== undefined) ? theY * (_mySliderBackground.height - _myIdleCursor.height) : _myIdleCursor.y;
        _myIdleCursor.x   = myX;
        _myIdleCursor.y   = myY;
        _myActiveCursor.x = myX;
        _myActiveCursor.y = myY;
        _myCursorOrigin.x = myX;
        _myCursorOrigin.y = myY;
        // to update rasterIndex
        getRelativeX();
        getRelativeY();
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
