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
spark.enableTuio = function () {
    plug("TUIOClient");
    TUIOClient.listenToUDP();
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
 * Generic input events
 * 
 * Each of these are mapped onto specific mouse and cursor events.
 * 
 */
spark.GenericInputEvent = spark.Class("GenericInputEvent");

// XXX to be extended on demand
spark.GenericInputEvent.BUTTON_DOWN_ENTER   = "generic-button-down-enter";
spark.GenericInputEvent.BUTTON_UP_ENTER     = "generic-button-up-enter";
spark.GenericInputEvent.ENTER               = "generic-enter";
spark.GenericInputEvent.BUTTON_UP_LEAVE     = "generic-button-up-leave";
spark.GenericInputEvent.LEAVE               = "generic-leave";
spark.GenericInputEvent.MOVE                = "generic-move";

spark.GenericInputEvent.eventMapping = {};
spark.GenericInputEvent.eventMapping[spark.GenericInputEvent.BUTTON_DOWN_ENTER] = [spark.MouseEvent.BUTTON_DOWN, spark.CursorEvent.ENTER];
spark.GenericInputEvent.eventMapping[spark.GenericInputEvent.BUTTON_UP_ENTER]   = [spark.MouseEvent.BUTTON_UP, spark.CursorEvent.ENTER];
spark.GenericInputEvent.eventMapping[spark.GenericInputEvent.ENTER]             = [spark.MouseEvent.ENTER, spark.CursorEvent.ENTER];
spark.GenericInputEvent.eventMapping[spark.GenericInputEvent.BUTTON_UP_LEAVE]   = [spark.MouseEvent.BUTTON_UP, spark.CursorEvent.LEAVE];
spark.GenericInputEvent.eventMapping[spark.GenericInputEvent.LEAVE]             = [spark.MouseEvent.LEAVE, spark.CursorEvent.LEAVE];
spark.GenericInputEvent.eventMapping[spark.GenericInputEvent.MOVE]              = [spark.MouseEvent.MOVE, spark.CursorEvent.MOVE];

spark.GenericInputEvent.getMappedEvents = function(theEventType) {
    
    if (theEventType in spark.GenericInputEvent.eventMapping) {
        return spark.GenericInputEvent.eventMapping[theEventType];
    } else {
        return null;
    }
};
