/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2010, ART+COM AG Berlin, Germany <www.artcom.de>
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
// Description: Widget base classes
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

/*jslint , nomen:false, plusplus:false*/
/*globals spark, Vector3f, Logger, Quaternionf, radFromDeg, GUI*/

/**
 * Map from scene node id to widget.
 * 
 * This is used in various places to retrieve
 * the widget responsible for a given scene node.
 * 
 * It is maintained by class Widget.
 */
spark.sceneNodeMap = {};


/**
 * Widgets: interactive graphical objects
 *
 * This is the mother of all GUI classes.
 * 
 * This is where positioning, orientation, scaling, visibility
 * and sensibility are handled.
 * 
 * In general, each widget is associated with a Y60 scene node,
 * which may be either a transform or a body, depending on the
 * concrete subclass.
 */
spark.Widget = spark.AbstractClass("Widget");

spark.Widget.Constructor = function (Protected) {
    var Base = {};
    var Public = this;

    // all widgets are potential component containers.
    // that does not necessitate that they can contain other widgets.
    this.Inherit(spark.Container);

    // all widgets participate in event dispatch.
    this.Inherit(spark.EventDispatcher);

    /////////////////////
    // Private Members //
    /////////////////////

    var _mySceneNode   = null;

    /**
     * Access the scene node for this widget.
     * 
     * XXX: try to get rid of this accessor
     */
    Public.__defineGetter__("sceneNode", function () {
        return _mySceneNode;
    });

    /**
     * Access the INNER scene node for this widget.
     * 
     * This is used exclusively by Canvas, returning
     * the root of the world inside the Canvas.
     * 
     * On other widgets, the inner node equals
     * the outer node.
     * 
     * XXX: try to get rid of this accessor
     */
    Public.__defineGetter__("innerSceneNode", function () {
        return _mySceneNode;
    });

    // WORLD POSITION AND ORIENTATION GETTERS

    Public.Getter("worldPosition", function () {
        if (!_mySceneNode) {
            Logger.error("World-related properties of widgets can only be retrieved after realization");
            return null;
        }
        return _mySceneNode.globalmatrix.getTranslation();
    });

    Public.Getter("worldScale", function () {
        if (!_mySceneNode) {
            Logger.error("World-related properties of widgets can only be retrieved after realization");
            return null;
        }
        return _mySceneNode.globalmatrix.getScale();
    });

    Public.Getter("worldRotation", function () {
        if (!_mySceneNode) {
            Logger.error("World-related properties of widgets can only be retrieved after realization");
            return null;
        }
        return _mySceneNode.globalmatrix.getRotation();
    });

    // WORLD BOUND GETTERS (XXX: ugly, only work after realization)
    Public.Getter("worldBounds", function () {
        if (!_mySceneNode) {
            Logger.error("World-related properties of widgets can only be retrieved after realization");
            return null;
        }
        var myBoundingBox = _mySceneNode.boundingbox;
        if (myBoundingBox + "" === "[]") {
            Logger.error("Widget does not have a bounding box yet.");
            return null;
        }
        return myBoundingBox.size;
    });

    Public.Getter("worldWidth", function () {
        return Public.worldBounds.x;
    });

    Public.Getter("worldHeight", function () {
        return Public.worldBounds.y;
    });

    Public.Getter("worldDepth", function () {
        return Public.worldBounds.z;
    });

    // STAGE
    /**
     * Access the stage for this widget.
     * 
     * Works by traversing the hierarchy upwards until a stage is found.
     */
    Public.__defineGetter__("stage", function () {
        var myCurrent = Public;
        while (myCurrent) {
            if ("Stage" in myCurrent._classes_) {
                return myCurrent;
            }
            myCurrent = myCurrent.parent;
        }
        Logger.fatal("Widget " + Public.name + " is not the child of a valid stage.");
        return null;
    });

    // STAGE EVENTS
    /**
     * Add an event listener.
     * 
     * This override is part of a hack to implement the somewhat unusual
     * behaviour of stage events. Contrary to other events, stage events
     * are never dispatched through the hierarchy. Instead, they execute
     * a full target phase for every registrant, forbidding capture.
     * 
     * This is needed because there is no "focus" to base stage event
     * dispatch on. Everyone just gets the events directly.
     */
    Base.addEventListener = Public.addEventListener;
    Public.addEventListener = function (theType, theListener, theUseCapture) {
        if (!("Stage" in Public._classes_)) {
            switch (theType) {
            case spark.StageEvent.FRAME:
            case spark.StageEvent.PRE_RENDER:
            case spark.StageEvent.POST_RENDER:
                if (theUseCapture) {
                    Logger.fatal("Capture of stage events is forbidden.");
                }
                Public.stage.addEventListener(
                    theType,
                    function (theEvent) {
                        Public.dispatchEvent(theEvent);
                    },
                    false
                );
                break;
            }
        }
        Base.addEventListener(theType, theListener, theUseCapture);
    };

    /**
     * Remove an event listener.
     * 
     * This override is a hack, symmetric to addEventListener above.
     */
    Base.removeEventListener = Public.removeEventListener;
    Public.removeEventListener = function (theType, theListener, theUseCapture) {
        switch (theType) {
        case spark.StageEvent.FRAME:
        case spark.StageEvent.PRE_RENDER:
        case spark.StageEvent.POST_RENDER:
            Logger.fatal("Removal of stage event listeners is not supported");
            break;
        default:
            Base.removeEventListener(theType, theListener, theUseCapture);
            break;
        }
    };

    // ALPHA
    function applyAlpha() {
        Public.propagateAlpha();
    }
    
    this.Property("alpha", Number, 1.0, applyAlpha);
    var _myActualAlpha = 1.0;
    
    Public.Getter("actualAlpha", function () {
        return _myActualAlpha;
    });

    Public.Getter("parentAlpha", function () {
        var myParentAlpha = 1.0;
        if ((Public.parent) && ("actualAlpha" in Public.parent)) {
            myParentAlpha = Public.parent.actualAlpha;
        }
        return myParentAlpha;
    });

    Public.propagateAlpha = function () {
        _myActualAlpha = Public.parentAlpha * Public.alpha;//_myAlpha;
        var myChildCount = Public.children.length;
        for (var i = 0; i < myChildCount; i++) {
            var myChild = Public.children[i];
            if ("propagateAlpha" in myChild) {
                myChild.propagateAlpha();
            }
        }
    };

    // VISIBILITY AND SENSIBLITY
    function applyVisibleAndSensible() {
        if (_mySceneNode) {
            _mySceneNode.insensible = !(Public.visible && Public.sensible);
            _mySceneNode.visible    = Public.visible;
        }
    }
    
    Public.Property("visible", Boolean, true, applyVisibleAndSensible);
    Public.Property("sensible", Boolean, true, applyVisibleAndSensible);

    // POSITION
    function applyPosition() {
        if (_mySceneNode) {
            _mySceneNode.position = Public.position;
        }
    }
    
    Public.Getter("position", function () {
        return new Vector3f(Public.x, Public.y, Public.z);
    });

    Public.Setter("position", function (theValue) {
        Public.x = theValue.x;
        Public.y = theValue.y;
        Public.z = theValue.z;
    });

    this.Property("x", Number, 0.0, applyPosition);
    this.Property("y", Number, 0.0, applyPosition);
    this.Property("z", Number, 0.0, applyPosition);

    // SCALE
    function applyScale() {
        if (_mySceneNode) {
            _mySceneNode.scale = Public.scale;
        }
    }
    
    Public.Getter("scale", function () {
        return new Vector3f(Public.scaleX, Public.scaleY, Public.scaleZ);
    });

    Public.Setter("scale", function (theValue) {
        Public.scaleX = theValue.x;
        Public.scaleY = theValue.y;
        Public.scaleZ = theValue.z;
    });

    this.Property("scaleX", Number, 1.0, applyScale);
    this.Property("scaleY", Number, 1.0, applyScale);
    this.Property("scaleZ", Number, 1.0, applyScale);

    // PIVOT
    function applyPivot() {
        if (_mySceneNode) {
            _mySceneNode.pivot = Public.pivot;
        }
    }
    
    Public.Getter("pivot", function () {
        return new Vector3f(Public.pivotX, Public.pivotY, Public.pivotZ);
    });

    Public.Setter("pivot", function (theValue) {
        Public.pivotX = theValue.x;
        Public.pivotY = theValue.y;
        Public.pivotZ = theValue.z;
    });

    this.Property("pivotX", Number, 0.0, applyPivot);
    this.Property("pivotY", Number, 0.0, applyPivot);
    this.Property("pivotZ", Number, 0.0, applyPivot);

    // ORIGIN
    // XXX: origins must be set up before realization
    //      this is not thought out well and has been
    //      introduced as a requirement-fullfilling hack.
    function applyOrigin() {
    }
    
    Public.Getter("origin", function () {
        return new Vector3f(Public.originX, Public.originY, Public.originZ);
    });

    Public.Setter("origin", function (theValue) {
        Public.originX = theValue.x;
        Public.originY = theValue.y;
        Public.originZ = theValue.z;
    });

    this.Property("originX", Number, 0.0, applyOrigin);
    this.Property("originY", Number, 0.0, applyOrigin);
    this.Property("originZ", Number, 0.0, applyOrigin);

    // ROTATION
    function applyRotation() {
        if (_mySceneNode) {
            var myRotation = new Vector3f(radFromDeg(Public.rotationX),
                                          radFromDeg(Public.rotationY),
                                          radFromDeg(Public.rotationZ));

            var myQuaternion = Quaternionf.createFromEuler(myRotation);
            _mySceneNode.orientation = myQuaternion;
        }
    }
    
    Public.Getter("rotation", function () {
        return new Vector3f(Public.rotationX, Public.rotationY, Public.rotationZ);
    });

    Public.Setter("rotation", function (theValue) {
        Public.rotationX = theValue.x;
        Public.rotationY = theValue.y;
        Public.rotationZ = theValue.z;
    });

    this.Property("rotationX", Number, 0.0, applyRotation);
    this.Property("rotationY", Number, 0.0, applyRotation);
    this.Property("rotationZ", Number, 0.0, applyRotation);

    // INTERNATIONALISATION HOOKS
    var _myI18nContext = null;

    Public.__defineGetter__("i18nContext", function () {
        if (_myI18nContext) {
            return _myI18nContext;
        } else {
            if (Public.parent) {
                return Public.parent.i18nContext;
            } else {
                return null;
            }
        }
    });

    Public.__defineSetter__("i18nContext", function (theValue) {
        _myI18nContext = theValue; // XXX: re-trigger i18n events? how?
    });

    Public.__defineGetter__("i18nContexts", function () {
        var myContexts = [];
        var myCurrent = Public;
        while (myCurrent) {
            if (myContexts.length > 0) {
                var myContext = myCurrent.i18nContext;
                if (myContexts[myContexts.length - 1] !== myContext) {
                    myContexts.push(myCurrent.i18nContext);
                }
            } else {
                myContexts.push(myCurrent.i18nContext);
            }
            myCurrent = myCurrent.parent;
        }
        return myContexts;
    });

    Public.getI18nItemByName = function (theName) {
        var myContexts = Public.i18nContexts;
        for (var i = 0; i < myContexts.length; i++) {
            var myContext = myContexts[i];
            var myItem = myContext.getChildByName(theName);
            if (myItem) {
                return myItem;
            }
        }
        return null;
    };

    // ANIMATION HELPERS
    Public.animateProperty = function (theDuration, theEasing, theProperty, theStart, theEnd) {
        return new GUI.PropertyAnimation(theDuration, theEasing, Public, theProperty, theStart, theEnd);
    };

    Public.animateFade = function(theDuration, theEnd, theEasing) {
        if(!theDuration) {
            theDuration = 250.0;
        }
        if(!theEnd) {
        	theEnd = 1.0;
        }
        if (!theEasing) {
            theEasing = null;
        }
        return Public.animateProperty(theDuration, theEasing, "alpha", Public.alpha, theEnd);
    };

    Public.animateFadeIn = function (theDuration, theEasing) {
        if (!theDuration) {
            theDuration = 250.0;
        }
        if (!theEasing) {
            theEasing = null;
        }
        return Public.animateProperty(theDuration, theEasing, "alpha", 0.0, 1.0);
    };

    Public.animateFadeOut = function (theDuration, theEasing) {
        if (!theDuration) {
            theDuration = 250.0;
        }
        if (!theEasing) {
            theEasing = null;
        }
        return Public.animateProperty(theDuration, theEasing, "alpha", 1.0, 0.0);
    };


    // REALIZATION
    Base.realize = Public.realize;
    Public.realize = function (theSceneNode) {
        _mySceneNode = theSceneNode;
        _mySceneNode.sticky = true;
        
        spark.sceneNodeMap[_mySceneNode.id] = Public;
        
        Base.realize();
        
        applyVisibleAndSensible();
        applyPosition();
        applyScale();
        applyRotation();
        applyPivot();
        applyAlpha();
        //Public.propagateAlpha();
    };

    Base.postRealize = Public.postRealize;
    Public.postRealize = function () {
        var myContextName = Protected.getString("i18nContext", "");
        if (myContextName !== "") {
            _myI18nContext = Public.getChildByName(myContextName);
        }
        Base.postRealize();
    };
};
