/*jslint nomen:false*/
/*globals spark, Point2f*/

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

    var _myId = theId;
    var _myActive = false;
    var _myGrabHolder = null;
    var _myHovered = null;
    var _myStagePosition = new Point2f();
    
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