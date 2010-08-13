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
// Description: Multitouch support
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

/*jslint nomen: false*/
/*globals spark, use, ASSManager, plug, TUIOClient*/

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

spark.enableTuio = function(theStage) {
    var myPlugIn = plug("TUIOClient");
    theStage.registerSettingsListener( myPlugIn, "TUIO" );
    TUIOClient.listenToUDP();
};

/**
 * Multitouch cursors
 * 
 * This class represents the retained state of
 * a single multitouch cursor.
 */
spark.Cursor = spark.Class("Cursor");

spark.Cursor.Constructor = function(Protected, theId) {
    var Public = this;

    var _myId = theId;

    /**
     * ID of this cursor.
     * 
     * Any creator of cursors shall ensure uniqueness
     * (within application runtime) of these.
     */
    Public.id getter = function() {
        return _myId;
    };

    var _myActive = false;

    /**
     * Whether this cursor still exists.
     * 
     * This will be true for as long as the sensoric
     * keeps track of this cursor. Can be used to
     * identify "dead" cursors when dealing with
     * cursor collections.
     */
    Public.active getter = function() {
        return _myActive;
    };


    var _myGrabHolder = null;

    /**
     * Current grab holder of this cursor.
     */
    Public.grabHolder getter = function() {
        return _myGrabHolder;
    };

    /**
     * True when this cursor is under grab.
     */
    Public.grabbed getter = function() {
        return (_myGrabHolder != null);
    };


    var _myHovered = null;

    /**
     * Returns widget currently being hovered.
     * 
     * This is always updated, even when under grab.
     */
    Public.hovered getter = function() {
        return _myHovered;
    };

    /**
     * Returns widget currently being focused.
     * 
     * Depending on grab state, this is either
     * the grab holder or the currently hovered widget.
     */
    Public.focused getter = function() {
        if(_myGrabHolder) {
            return _myGrabHolder;
        } else {
            return _myHovered;
        }
    };
    
    var _myLastStagePosition = new Point2f();
    /**
     * Last stage position as cloned Point2f.
     */
    Public.lastStagePosition getter = function() {
        return _myLastStagePosition.clone();
    };

    var _myStagePosition = new Point2f();

    /**
     * Current position as cloned Point2f.
     */
    Public.stagePosition getter = function() {
        return _myStagePosition.clone();
    };

    /**
     * Current horizontal position.
     */
    Public.stageX getter = function() {
        return _myStagePosition.x;
    };

    /**
     * Current vertical position.
     */
    Public.stageY getter = function() {
        return _myStagePosition.y;
    };

    /**
     * Internal: update position and focus of cursor.
     */
    Public.update = function(theHovered, theStagePosition) {
        _myHovered = theHovered;
        _myLastStagePosition = Public.stagePosition;
        _myStagePosition = theStagePosition.clone();
    };

    /**
     * Internal: called when cursor is created.
     */
    Public.activate = function() {
        _myActive = true;
    };

    /**
     * Internal: called when cursor vanishes.
     */
    Public.deactivate = function() {
        _myActive = false;
    };

    /**
     * Grab this cursor, sending all its future events to HOLDER.
     */
    Public.grab = function(theHolder) {
        _myGrabHolder = theHolder;
    };

    /**
     * Cancel a grab, returning the cursor to normal event flow.
     */
    Public.ungrab = function() {
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

spark.CursorEvent.Constructor = function (Protected, theType, theCursor) {
    var Public = this;

    this.Inherit(spark.Event, theType);

    var _myCursor = theCursor;

    /**
     * Reference to the cursor this event concerns
     */
    Public.cursor getter = function () {
        return _myCursor;
    };

    /**
     * Convenience: horizontal position of cursor
     */
    Public.stageX getter = function () {
        return _myCursor.stageX;
    };

    /**
     * Convenience: vertical position of cursor
     */
    Public.stageY getter = function () {
        return _myCursor.stageY;
    };

    /**
     * Convenience: intensity of cursor
     */
    Public.intensity getter = function () {
        return _myCursor.intensity;
    };
};

/**
 * Generic cursor events
 * 
 * Each of these are mapped onto specific mousecursor and touchcursor events.
 * 
 */
spark.GenericCursorEvent = spark.Class("GenericCursorEvent");

spark.GenericCursorEvent.APPEAR = "generic-cursor-appear";
spark.GenericCursorEvent.ENTER  = "generic-cursor-enter";
spark.GenericCursorEvent.LEAVE  = "generic-cursor-leave";
spark.GenericCursorEvent.VANISH = "generic-cursor-vanish";
spark.GenericCursorEvent.MOVE   = "generic-cursor-move";
spark.GenericCursorEvent.APPEAR_TOUCHENTER = "generic-cursor-appear-touchenter";
spark.GenericCursorEvent.VANISH_TOUCHLEAVE = "generic-cursor-vanish-touchleave";

spark.GenericCursorEvent.eventMapping = {};
spark.GenericCursorEvent.eventMapping[spark.GenericCursorEvent.APPEAR]  = [spark.MouseCursorEvent.APPEAR, spark.CursorEvent.APPEAR];
spark.GenericCursorEvent.eventMapping[spark.GenericCursorEvent.ENTER]   = [spark.MouseCursorEvent.ENTER,  spark.CursorEvent.ENTER];
spark.GenericCursorEvent.eventMapping[spark.GenericCursorEvent.LEAVE]   = [spark.MouseCursorEvent.LEAVE,  spark.CursorEvent.LEAVE];
spark.GenericCursorEvent.eventMapping[spark.GenericCursorEvent.VANISH]  = [spark.MouseCursorEvent.VANISH, spark.CursorEvent.VANISH];
spark.GenericCursorEvent.eventMapping[spark.GenericCursorEvent.MOVE]    = [spark.MouseCursorEvent.MOVE,   spark.CursorEvent.MOVE];
spark.GenericCursorEvent.eventMapping[spark.GenericCursorEvent.APPEAR_TOUCHENTER] = [spark.MouseCursorEvent.APPEAR,
                                                                                     spark.CursorEvent.APPEAR,
                                                                                     spark.CursorEvent.ENTER];
spark.GenericCursorEvent.eventMapping[spark.GenericCursorEvent.VANISH_TOUCHLEAVE] = [spark.MouseCursorEvent.VANISH,
                                                                                     spark.CursorEvent.VANISH,
                                                                                     spark.CursorEvent.LEAVE];


spark.GenericCursorEvent.getMappedEvents = function(theEventType) {
    if (theEventType in spark.GenericCursorEvent.eventMapping) {
        return spark.GenericCursorEvent.eventMapping[theEventType];
    } else {
        return null;
    }
};


/**
 * Multitouch gestures
 * 
 * This class represents 
 * multitouch gesture events
 */

const ASS_BASE_EVENT  = 0;
const TUIO_BASE_EVENT = 1;

spark.GestureEvent = spark.Class("GestureEvent");
spark.GestureEvent.Constructor = function(Protected, theType, theBaseEvent, theCursor) {
    this.Inherit(spark.CursorEvent, theType, theCursor);
};

/**
 * wipe event: "the distance between the last two move cursors
 * is bigger than the given threshold"
 */
spark.GestureEvent.WIPE  = "gesture-wipe";
spark.WipeGestureEvent = spark.Class("WipeGestureEvent");
spark.WipeGestureEvent.Constructor = function(Protected, theType, theBaseEvent, theCursor, theDirection, theMagnitude) {
    var Public = this;

    this.Inherit(spark.GestureEvent, theType, theBaseEvent, theCursor);
    
    /**
     * Direction vector of the wipe event
     */
    var _myDirection = theDirection;

    Public.direction getter = function() {
        return _myDirection;
    };
    
    /**
     * Magnitude of the direction vector
     */
    var _myMagnitude = theMagnitude;

    Public.magnitude getter = function () {
        return _myMagnitude;
    };

};

/**
 * events with two cursors
 */
spark.GestureEvent.CURSOR_PAIR_START  = "gesture-cursor-pair-start";
spark.GestureEvent.CURSOR_PAIR_FINISH = "gesture-cursor-pair-finish";

spark.MultiCursorGestureEvent = spark.Class("MultiCursorGestureEvent");
spark.MultiCursorGestureEvent.Constructor = function(Protected, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance) {
    var Public = this;

    this.Inherit(spark.GestureEvent, theType, theBaseEvent, theMainCursor);
    
    
    /**
     * get partner cursor
     */
    var _myPartnerCursor = thePartnerCursor;

    Public.partnerCursor getter = function () {
        return _myPartnerCursor;
    };
    
    /**
     * centerpoint between the two cursors
     */
    var _myCenterPoint = theCenterpoint;

    Public.centerPoint getter = function () {
        return _myCenterPoint;
    };

    /**
     * current distance between zoom partners
     */
    var _myDistance = theDistance;

    Public.distance getter = function() {
        return _myDistance;
    };
};



/**
 * zoom event: "two cursors which change their distance"
 */
spark.GestureEvent.ZOOM  = "gesture-zoom";

spark.ZoomGestureEvent = spark.Class("ZoomGestureEvent");
spark.ZoomGestureEvent.Constructor = function(Protected, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance, theLastDistance, theInitialDistance, theZoomFactor) {
    var Public = this;

    this.Inherit(spark.MultiCursorGestureEvent, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance);
    
    /**
     * current distance between zoom partners
     */
    var _myLastDistance = theLastDistance;

    Public.lastDistance getter = function() {
        return _myLastDistance;
    };
    
    /**
     * initial distance between zoom partners
     */
    var _myInitialDistance = theInitialDistance;

    Public.initialDistance getter = function() {
        return _myInitialDistance;
    };
    
    /**
     * current zoom factor
     */
    var _myZoomFactor = theZoomFactor;

    Public.zoomFactor getter = function() {
        return _myZoomFactor;
    };
};

/**
 * rotate event: "two cursor with changing angle"
 */
spark.GestureEvent.ROTATE  = "gesture-rotate";
spark.RotateGestureEvent = spark.Class("RotateGestureEvent");
spark.RotateGestureEvent.Constructor = function(Protected, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance, theAngle) {
    var Public = this;

    this.Inherit(spark.MultiCursorGestureEvent, theType, theBaseEvent, theMainCursor, thePartnerCursor, theCenterpoint, theDistance);
    
    /**
     * angle 
     */
    var _myAngle = theAngle;

    Public.angle getter = function () {
        return _myAngle;
    };
};
