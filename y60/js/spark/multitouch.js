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
*/

/*jslint nomen: false*/
/*globals spark, use, ASSManager, plug, TUIOClient, Logger, Point2f*/

/**
 * Load the proximatrix driver
 * 
 * After doing so, the main window will receive
 * proximatrix events. A toplevel Window instance
 * will then dispatch these as cursor events.
 * 
 * This must be given a reference to the stage
 * due to the baroque API of ASSManager.
 */
spark.enableProximatrix = function (theStage) {
    use("ASSManager.js");
    spark.proximatrix = new ASSManager(theStage);
};

/**
 * Load the TUIO receiver
 * 
 * After doing so, the main window will receive
 * TUIO events. A toplevel Window instance will
 * then dispatch these as cursor events.
 */
spark.enableTuio = function (theStage) {
    var myPlugIn = plug("TUIOClient");
    TUIOClient.listenToUDP();
    if (!theStage) {
        Logger.warning("for the settings listener the Stage as argument is mandatory");
    } else {
        theStage.registerSettingsListener(myPlugIn, "TUIO");
    }
};

var TUIO_OBJECT_CLASS = "object";
var TUIO_CURSOR_CLASS = "cursor";

/**
 * Multitouch cursors
 * 
 * This class represents the retained state of
 * a single multitouch cursor.
 */
spark.Cursor = spark.Class("Cursor");
spark.Cursor.Constructor = function (Protected, theId) {
    
    var Public = this;
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myId         = theId;
    var _myActive     = false;
    var _myGrabHolder = null;
    var _myHovered    = null;
    var _myLastStagePosition = new Point2f();
    var _myStagePosition     = new Point2f();

    ////////////////////
    // Public Methods //
    ////////////////////

    /**
     * ID of this cursor.
     * 
     * Any creator of cursors shall ensure uniqueness
     * (within application runtime) of these.
     */
    Public.__defineGetter__("id", function () {
        return _myId;
    });

    /**
     * Whether this cursor still exists.
     * 
     * This will be true for as long as the sensoric
     * keeps track of this cursor. Can be used to
     * identify "dead" cursors when dealing with
     * cursor collections.
     */
    Public.__defineGetter__("active", function () {
        return _myActive;
    });
    
    /**
     * Current grab holder of this cursor.
     */
    Public.__defineGetter__("grabHolder", function () {
        return _myGrabHolder;
    });

    /**
     * True when this cursor is under grab.
     */
    Public.__defineGetter__("grabbed", function () {
        return (_myGrabHolder !== null);
    });

    /**
     * Returns widget currently being hovered.
     * 
     * This is always updated, even when under grab.
     */
    Public.__defineGetter__("hovered", function () {
        return _myHovered;
    });

    /**
     * Returns widget currently being focused.
     * 
     * Depending on grab state, this is either
     * the grab holder or the currently hovered widget.
     */
    Public.__defineGetter__("focused", function () {
        if (_myGrabHolder) {
            return _myGrabHolder;
        } else {
            return _myHovered;
        }
    });
    
    /**
     * Last stage position as cloned Point2f.
     */
    Public.__defineGetter__("lastStagePosition", function () {
        return _myLastStagePosition.clone();
    });

    /**
     * Current position as cloned Point2f.
     */
    Public.__defineGetter__("stagePosition", function () {
        return _myStagePosition.clone();
    });

    /**
     * Current horizontal position.
     */
    Public.__defineGetter__("stageX", function () {
        return _myStagePosition.x;
    });

    /**
     * Current vertical position.
     */
    Public.__defineGetter__("stageY", function () {
        return _myStagePosition.y;
    });

    /**
     * Internal: update position and focus of cursor.
     */
    Public.update = function (theHovered, theStagePosition) {
        _myHovered = theHovered;
        _myLastStagePosition = Public.stagePosition;
        _myStagePosition = theStagePosition.clone();
    };

    /**
     * Internal: called when cursor is created.
     */
    Public.activate = function () {
        _myActive = true;
    };

    /**
     * Internal: called when cursor vanishes.
     */
    Public.deactivate = function () {
        _myActive = false;
    };

    /**
     * Grab this cursor, sending all its future events to HOLDER.
     */
    Public.grab = function (theHolder) {
        _myGrabHolder = theHolder;
    };

    /**
     * Cancel a grab, returning the cursor to normal event flow.
     */
    Public.ungrab = function () {
        _myGrabHolder = null;
    };

};

/**
 * Cursor events
 * 
 * Each of these represents a change in state
 * on the given cursor.
 */
spark.CursorEvent = spark.Class("CursorEvent");

/**
 * Appear event: "new cursor created here"
 * 
 * Symmetric to VANISH.
 */
spark.CursorEvent.APPEAR = "cursor-appear";

/**
 * Vanish event: "cursor ceases to exist"
 * 
 * Symmetric to APPEAR.
 */
spark.CursorEvent.VANISH = "cursor-vanish";

/**
 * Move event: "cursor changed position"
 */
spark.CursorEvent.MOVE   = "cursor-move";

/**
 * Enter event: "cursor entered widget"
 * 
 * Symmetric to LEAVE.
 */
spark.CursorEvent.ENTER  = "cursor-enter";

/**
 * Leave event: "cursor left widget"
 * 
 * Symmetric to ENTER.
 */
spark.CursorEvent.LEAVE  = "cursor-leave";


/**
 *  Aggerated events
 *
 *  used to compensate for touch jitter
 */
spark.CursorEvent.APPEAR_ENTER = "cursor-appear-enter";
spark.CursorEvent.VANISH_LEAVE = "cursor-vanish-leave";

spark.CursorEvent.eventMapping = {};
spark.CursorEvent.eventMapping[spark.CursorEvent.APPEAR_ENTER] = 
    [spark.CursorEvent.APPEAR, spark.CursorEvent.ENTER];
spark.CursorEvent.eventMapping[spark.CursorEvent.VANISH_LEAVE] = 
    [spark.CursorEvent.VANISH, spark.CursorEvent.LEAVE];

spark.CursorEvent.getMappedEvents = function (theEventType) {
    if (theEventType in spark.CursorEvent.eventMapping) {
        return spark.CursorEvent.eventMapping[theEventType];
    }
};

spark.CursorEvent.Constructor = function (Protected, theType, theCursor) {
    var Public = this;
    this.Inherit(spark.Event, theType);

    /////////////////////
    // Private Members //
    /////////////////////

    var _myCursor = theCursor;

    ////////////////////
    // Public Methods //
    ////////////////////

    /**
     * Reference to the cursor this event concerns
     */
    Public.__defineGetter__("cursor", function () {
        return _myCursor;
    });

    /**
     * Convenience: horizontal position of cursor
     */
    Public.__defineGetter__("stageX", function () {
        return _myCursor.stageX;
    });

    /**
     * Convenience: vertical position of cursor
     */
    Public.__defineGetter__("stageY", function () {
        return _myCursor.stageY;
    });

    /**
     * Convenience: intensity of cursor
     */
    Public.__defineGetter__("intensity", function () {
        return _myCursor.intensity;
    });

};

spark.ObjectEvent = spark.Class("ObjectEvent");

/**
 * Appear event: "new object created here"
 * 
 * Symmetric to VANISH.
 */
spark.ObjectEvent.APPEAR = "object-appear";

/**
 * Vanish event: "object ceases to exist"
 * 
 * Symmetric to APPEAR.
 */
spark.ObjectEvent.VANISH = "object-vanish";

/**
 * Move event: "object changed position"
 */
spark.ObjectEvent.MOVE   = "object-move";

/**
 * Enter event: "object entered widget"
 * 
 * Symmetric to LEAVE.
 */
spark.ObjectEvent.ENTER  = "object-enter";

/**
 * Leave event: "object left widget"
 * 
 * Symmetric to ENTER.
 */
spark.ObjectEvent.LEAVE  = "object-leave";

spark.ObjectEvent.Constructor = function (Protected, theType, theObject, theEvent) {
    var Public = this;
    this.Inherit(spark.CursorEvent, theType, theObject);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myEvent = theEvent;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /**
     * Rotation of the object
     */
    Public.__defineGetter__("rotation", function () {
        return _myEvent.rotation;
    });
    
    /**
     * Rotation speed of the object
     */
    Public.__defineGetter__("rotationspeed", function () {
        return _myEvent.rotationspeed;
    });
    
    /**
     * Rotation acceleration of the object
     */
    Public.__defineGetter__("rotationaccel", function () {
        return _myEvent.rotationaccel;
    });
    
    /**
     * Symbolid of the object
     */
    Public.__defineGetter__("symbolid", function () {
        return _myEvent.symbolid;
    });

};

/**
 * Multitouch gestures
 * 
 * This class represents 
 * multitouch gesture events
 */

var ASS_BASE_EVENT  = 0;
var TUIO_BASE_EVENT = 1;

spark.GestureEvent = spark.Class("GestureEvent");
spark.GestureEvent.Constructor = function (Protected, theType, theBaseEvent, theCursor) {
    this.Inherit(spark.CursorEvent, theType, theCursor);
};

/**
 * wipe event: "the distance between the last two move cursors
 * is bigger than the given threshold"
 */
spark.GestureEvent.WIPE  = "gesture-wipe";
spark.WipeGestureEvent = spark.Class("WipeGestureEvent");
spark.WipeGestureEvent.Constructor = function (Protected, theType, theBaseEvent, theCursor, theDirection, theVelocity) {
    var Public = this;
    this.Inherit(spark.GestureEvent, theType, theBaseEvent, theCursor);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myDirection = theDirection;
    var _myVelocity = theVelocity;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /**
     * Direction vector of the wipe event
     */
    Public.__defineGetter__("direction", function () {
        return _myDirection;
    });
    
    /**
     * Velocity of the movement
     */
    Public.__defineGetter__("velocity", function () {
        return _myVelocity;
    });

};

/**
 * tap event: "the distance between the last two move cursors
 * is smaller than the given threshold"
 */
spark.GestureEvent.TAP  = "gesture-tap";
spark.TapGestureEvent = spark.Class("TapGestureEvent");
spark.TapGestureEvent.Constructor = function (Protected, theType, theBaseEvent, theCursor, thePosition) {
    var Public = this;
    this.Inherit(spark.GestureEvent, theType, theBaseEvent, theCursor);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myPosition = thePosition;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /**
     * Position of the tap event
     */
    Public.__defineGetter__("position", function () {
        return _myPosition;
    });
};

/**
 * events with two cursors
 */
spark.GestureEvent.CURSOR_PAIR_START  = "gesture-cursor-pair-start";
spark.GestureEvent.CURSOR_PAIR_FINISH = "gesture-cursor-pair-finish";

spark.MultiCursorGestureEvent = spark.Class("MultiCursorGestureEvent");
spark.MultiCursorGestureEvent.Constructor = function (Protected, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance) {
    var Public = this;
    this.Inherit(spark.GestureEvent, theType, theBaseEvent, theMainCursor);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myPartnerCursor = thePartnerCursor;
    var _myCenterPoint   = theCenterpoint;
    var _myDistance      = theDistance;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /**
     * get partner cursor
     */
    Public.__defineGetter__("partnerCursor", function () {
        return _myPartnerCursor;
    });
    
    /**
     * centerpoint between the two cursors
     */
    Public.__defineGetter__("centerPoint", function () {
        return _myCenterPoint;
    });

    /**
     * current distance between zoom partners
     */
    Public.__defineGetter__("distance", function () {
        return _myDistance;
    });
};

/**
 * zoom event: "two cursors which change their distance"
 */
spark.GestureEvent.ZOOM  = "gesture-zoom";

spark.ZoomGestureEvent = spark.Class("ZoomGestureEvent");
spark.ZoomGestureEvent.Constructor = function (Protected, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance, theLastDistance, theInitialDistance, theZoomFactor) {
    var Public = this;
    this.Inherit(spark.MultiCursorGestureEvent, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myLastDistance    = theLastDistance;
    var _myZoomFactor      = theZoomFactor;
    var _myInitialDistance = theInitialDistance;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /**
     * current distance between zoom partners
     */
    Public.__defineGetter__("lastDistance", function () {
        return _myLastDistance;
    });
    
    /**
     * initial distance between zoom partners
     */
    Public.__defineGetter__("initialDistance", function () {
        return _myInitialDistance;
    });
    
    /**
     * current zoom factor
     */
    Public.__defineGetter__("zoomFactor", function () {
        return _myZoomFactor;
    });
};

/**
 * rotate event: "two cursor with changing angle"
 */
spark.GestureEvent.ROTATE  = "gesture-rotate";
spark.RotateGestureEvent = spark.Class("RotateGestureEvent");
spark.RotateGestureEvent.Constructor = function (Protected, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance, theAngle) {
    var Public = this;
    this.Inherit(spark.MultiCursorGestureEvent, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance);
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var _myAngle = theAngle;
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    /**
     * angle 
     */
    Public.__defineGetter__("angle", function () {
        return _myAngle;
    });
};

