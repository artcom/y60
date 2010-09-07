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
// Description: Event handling
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

/*jslint nomen: false, plusplus: false, white:false, bitwise:false*/
/*globals spark, Logger, getter*/

/**
 * SPARK Event Handling
 * 
 * Events in SPARK are heavily inspired by DOM events.
 * The API, however, is based on Adobe Flash.
 * 
 * Events are dispatched in a hierarchy of instances of EventDispatcher.
 * All such instances must follow the component hierarchy protocol,
 * meaning that they have to provide "parent" and "children" properties.
 * There is, however, no obligation for all children and the parent to
 * inherit EventDispatcher. Instances not doing so will be skipped during
 * event dispatch.
 * 
 * As in DOM events, dispatch works in three phases: Capture, Target
 * and Bubbling. Bubbling is currently unimplemented because there
 * never was a need for it. It is believed likely that this will
 * never change. For details on how dispatch works, see the DOM Level 3
 * Events specification.
 * 
 * Dispatch may be cancelled by any participant in the event flow IF
 * the event specifies that it may be cancelled. Doing so will stop
 * propagation after the current handler.
 * 
 */

/**
 * Enumeration of event dispatch phases.
 */
spark.EventPhase = {
    IDLE      : "idle",
    CAPTURING : "capturing",
    TARGET    : "target",
    BUBBLING  : "bubbling"
};

/**
 * Base class for events.
 * 
 * Constructor takes three arguments:
 * 
 *   TYPE (string)     - type of event, identifying handler to call
 *   BUBBLES (bool)    - if true, the event will bubble after the target phase
 *   CANCELABLE (bool) - if true, the event can be canceled with cancelDispatch
 * 
 */
spark.Event = spark.Class("Event");

spark.Event.Constructor = function (Protected, theType, theBubbles, theCancelable) {
    var Public = this;

    var _myBubbles       = !!theBubbles;
    var _myCancelable    = !!theCancelable;
    var _myCurrentTarget = null;
    var _myCurrentPhase  = spark.EventPhase.IDLE;
    var _myTarget        = null;
    var _myType          = theType;
    var _myDispatching   = false;


    /**
     * The type of this event.
     */
    Public.__defineGetter__("type", function () {
        return _myType;
    });

    /**
     * Whether this event participates in bubbling.
     */
    Public.__defineGetter__("bubbles", function () {
        return _myBubbles;
    });

    /**
     * Whether this event allows cancellation.
     */
    Public.__defineGetter__("cancelable", function () {
        return _myCancelable;
    });

    /**
     * Whether this event is currently being dispatched.
     * 
     * While true, the following properties are valid.
     */
    Public.__defineGetter__("dispatching", function () {
        return _myDispatching;
    });

    /**
     * The final target of this event in the current flow.
     */
    Public.__defineGetter__("target", function () {
        return _myTarget;
    });

    /**
     * Always reflects the current target during event flow.
     */
    Public.__defineGetter__("currentTarget", function () {
        return _myCurrentTarget;
    });

    /**
     * Always reflects the current phase during event flow.
     */
    Public.__defineGetter__("currentPhase", function () {
        return _myCurrentPhase;
    });

    /**
     * Cancel dispatch of event.
     * 
     * This may be called by the client to abort the
     * event flow for this event.
     * 
     * This is, however, only allowed on events that allow
     * cancellation on creation.
     */
    Public.cancelDispatch = function () {
        if (_myCancelable) {
            Public.finishDispatch();
        } else {
            Logger.fatal("event of type " + _myType + " is not cancelable");
        }
    };

    /**
     * Internal: update state of event to reflect start of dispatch
     */
    Public.startDispatch = function (theTarget) {
        _myDispatching = true;
        _myTarget = theTarget;
        _myCurrentPhase  = spark.EventPhase.IDLE;
        _myCurrentTarget = null;
    };

    /**
     * Internal: update state of event to reflect end of dispatch
     */
    Public.finishDispatch = function () {
        _myDispatching = false;
        _myCurrentPhase = spark.EventPhase.IDLE;
        _myCurrentTarget = null;
    };

    /**
     * Internal: update state of event to reflect dispatch to given target
     */
    Public.dispatchTo = function (theCurrentTarget, theCurrentPhase) {
        _myCurrentTarget = theCurrentTarget;
        _myCurrentPhase  = theCurrentPhase;
    };

};

/**
 * Mixin class for event dispatchers.
 * 
 * This class may be mixed into anything that should offer
 * an event dispatch protocol (dispatchEvent, addEventListener...).
 * 
 * Mixing in this class imposes a hierarchy protocol requirement,
 * meaning that instances of this class must, by other means,
 * provide a "parent" and a "children" property. These may be
 * implemented as stubs, returning null and [] respectively.
 */
spark.EventDispatcher = spark.AbstractClass("EventDispatcher");

spark.EventDispatcher.Constructor = function (Protected) {
    var Public = this;

    var _myListenersByType = {};

    /**
     * Register an event listener with this dispatcher.
     * 
     * This method should be called to register event handlers.
     * 
     * Event handlers are unary functions and will be called
     * with the dispatched event as their sole argument.
     * 
     * The THIS of the handler will be the current target
     * in the event flow.
     * 
     * Arguments:
     *   TYPE (string)       - type of event to handle
     *   LISTENER (function) - unary event handler function
     *   USECAPTURE (bool)   - if the handler belongs to the Capture phase
     */
    Public.addEventListener = function (theType, theListener, theUseCapture) {
        var mappedEvents = spark.CursorEvent.getMappedEvents(theType);
        
        if(mappedEvents) {
            for(var i=0; i<mappedEvents.length; ++i) {
                Public.addEventListener(mappedEvents[i], theListener, theUseCapture);
            }
        } else {
            // XXX: this is not really acceptable.
            //      need to figure out something better.
            //      click events DO still make sense, after all.
            //      maybe these should be implemented at widget level.
            if (theType === spark.MouseEvent.CLICK) {
                Logger.warning("MouseEvent.CLICK is deprecated; use MouseEvent.BUTTON_UP or MouseEvent.BUTTON_DOWN");
            }
            theUseCapture = theUseCapture || false;
            
            var myListener = {
                type:       theType,
                listener:   theListener,
                useCapture: theUseCapture
            };

            if (!(theType in _myListenersByType)) {
                _myListenersByType[theType] = [];
            }

            _myListenersByType[theType] = _myListenersByType[theType].concat(myListener);
        }
    };

    Public.addEventListenerInFront = function(theType, theListener, theUseCapture) {
        theUseCapture = theUseCapture || false;

        var myListener = {
            type:       theType,
            listener:   theListener,
            useCapture: theUseCapture
        };

        if (!(theType in _myListenersByType)) {
            _myListenersByType[theType] = [];
        }

        _myListenersByType[theType].unshift(myListener);
    };

    /**
     * Get all event listeners for the given type.
     * 
     * This is internal, right? -IA 20100219
     */
    Public.getEventListeners = function (theType) {
        if (theType in _myListenersByType) {
            var myListeners = _myListenersByType[theType];
            if (myListeners.length > 0) {
                return myListeners;
            }
        }
        return [];
    };

    /**
     * Dispatch the given event to the hierarchy.
     * 
     * This method is the actual driver of event dispatch.
     * 
     * It adheres to the following protocol:
     * 
     *  - call startDispatch
     *  - for each capturing listener, call it
     *  - call listeners on target
     *  - bubble the event upwards (not implemented)
     *  - call finishDispatch
     * 
     * For each event target (in each phase), dispatchTo
     * will be called on the event to update its currentTarget
     * and currentPhase, making it reflect dispatch progress.
     * 
     */
    Public.dispatchEvent = function (theEvent) {
        var myListeners, j, myListener;
        theEvent.startDispatch(Public);

        // collect dispatchers to capture on
        var myCurrent = Public;
        var myCaptureList = [Public];
        while (myCurrent.parent != null) {
            myCurrent = myCurrent.parent;
            if ("EventDispatcher" in myCurrent._classes_) {
                myCaptureList.unshift(myCurrent);
            }
        }
        myCaptureList.pop();

        // capture phase
        for (var i = 0; i < myCaptureList.length; i++) {
            var myCurrentTarget = myCaptureList[i];
            if (myCurrentTarget.hasEventListener(theEvent.type)) {
                theEvent.dispatchTo(myCurrentTarget, spark.EventPhase.CAPTURING);
                myListeners = myCurrentTarget.getEventListeners(theEvent.type);
                for (j = 0; j < myListeners.length; j++) {
                    myListener = myListeners[j];
                    if (myListener.useCapture) {
                        myListener.listener.call(myCurrentTarget, theEvent);
                        if (!theEvent.dispatching) {
                            return;
                        }
                    }
                }
            }
        }

        // target phase
        theEvent.dispatchTo(Public, spark.EventPhase.TARGET);
        if (Public.hasEventListener(theEvent.type)) {
            myListeners = Public.getEventListeners(theEvent.type);
            for (j = 0; j < myListeners.length; j++) {
                myListener = myListeners[j];
                myListener.listener.call(Public, theEvent);
                if (!theEvent.dispatching) {
                    return;
                }
            }
        }

        // bubbling phase
        if (theEvent.bubbles) {
            Logger.fatal("spark event bubbling has not been implemented yet");
        }

        theEvent.finishDispatch();
    };

    /**
     * Check if this dispatcher has listeners for the given event type.
     * 
     * This is non-recursive, checking only the given dispatcher.
     */
    Public.hasEventListener = function(theType) {
        if (theType in _myListenersByType) {
            var myListeners = _myListenersByType[theType];
            if (myListeners.length > 0) {
                return true;
            }
        }
        return false;
    };

    /**
     * Remove the given event listener.
     * 
     * Care must be taken to call this method with the exact same
     * arguments as in the corresponding addEventListener call.
     * 
     * The listener function will be compared by identity.
     * Therefore, removing a listener that is a closure may fail.
     * 
     */
    Public.removeEventListener = function(theType, theListener, theUseCapture) {
        var myDidRemove = false;
        theUseCapture = theUseCapture || false;

        var myListeners = _myListenersByType[theType];
        for (var i = 0; i < myListeners.length; i++) {
            var myListener = myListeners[i];
            if (myListener.type == theType &&
                myListener.listener == theListener &&
                myListener.useCapture == theUseCapture) {
                myListeners.splice(i, 1);
                myDidRemove = true;
                break;
            }
        }

        if (!myDidRemove) {
            Logger.error("Tried to remove nonexistent handler on " + Public.name + "." + theType);
        }
    };

    Public.moveEventListenerToFront = function(theType, theListener, theUseCapture) {
        var myDidMove = false;
        theUseCapture = theUseCapture || false;

        var myListeners = _myListenersByType[theType];
        var myListenerToMove = null;
        for (var i = 0; i < myListeners.length; i++) {
            var myListener = myListeners[i];
            if (myListener.type == theType &&
                myListener.listener == theListener &&
                myListener.useCapture == theUseCapture)
            {
                myListenerToMove = myListeners[i];
                myListeners.splice(i, 1);
                break;
            }
        }
        if (myListenerToMove) {
            myListeners.unshift(myListenerToMove);
            myDidMove = true;
        }
        if (!myDidMove) {
            Logger.error("Tried to move nonexistent handler on " + Public.name + "." + theType);
        }
    };

    /**
     * Recursively check for listeners of a given event type.
     * 
     * This can be used to decide whether creating and dispatching
     * an event is worth the effort.
     */
    Public.willTrigger = function(theType) {
        if (Public.hasEventListener(theType)) {
            return true;
        }
        for (var i = 0; i < Public.children.length; i++) {
            var myChild = Public.children[i];
            if (myChild.willTrigger(theType)) {
                return true;
            }
        }
        return false;
    };

};


spark.Mouse = {};

spark.Mouse.PRIMARY   = "primary";
spark.Mouse.SECONDARY = "secondary";
spark.Mouse.TERTIARY  = "tertiary";

spark.Mouse.buttonFromId = function(theId) {
    switch(theId) {
    case 1:
        return spark.Mouse.PRIMARY;
    case 2:
        return spark.Mouse.SECONDARY;
    case 3:
        return spark.Mouse.TERTIARY;
    default:
        Logger.fatal("Unknown mouse button " + theId);
    }
};


spark.MouseEvent = spark.Class("MouseEvent");

spark.MouseEvent.MOVE   = "mouse-move";
spark.MouseEvent.CLICK  = "mouse-click";
spark.MouseEvent.BUTTON_DOWN = "mouse-button-down";
spark.MouseEvent.BUTTON_UP   = "mouse-button-up";
spark.MouseEvent.ENTER  = "mouse-enter";
spark.MouseEvent.LEAVE  = "mouse-leave";
spark.MouseEvent.SCROLL = "mouse-scroll";

spark.MouseEvent.Constructor = function(Protected, theType, theX, theY, theAmountX, theAmountY, theButton, theButtonStates) {
    var Public = this;

    this.Inherit(spark.Event, theType);

    var _myStageX = theX;

    Public.__defineGetter__("stageX", function () {
        return _myStageX;
    });

    var _myStageY = theY;

    Public.__defineGetter__("stageY", function () {
        return _myStageY;
    });

    var _myAmountX = theAmountX;

    Public.__defineGetter__("amountX", function () {
        return _myAmountX;
    });

    var _myAmountY = theAmountY;

    Public.__defineGetter__("amountY", function () {
        return _myAmountY;
    });

    var _myButton = theButton;

    Public.__defineGetter__("button", function () {
        return _myButton;
    });

    var _myButtonStates = theButtonStates;

    Public.__defineGetter__("buttonStates", function () {
        return _myButtonStates;
    });
};

spark.Keyboard = {};

spark.Keyboard.SHIFT      = 1;
spark.Keyboard.ALT        = 2;
spark.Keyboard.ALT_SHIFT  = spark.Keyboard.ALT | spark.Keyboard.SHIFT;
spark.Keyboard.CTRL       = 4;
spark.Keyboard.CTRL_SHIFT = spark.Keyboard.CTRL | spark.Keyboard.SHIFT;
spark.Keyboard.CTRL_ALT   = spark.Keyboard.CTRL | spark.Keyboard.ALT;
spark.Keyboard.CTRL_ALT_SHIFT = spark.Keyboard.ALT | spark.Keyboard.SHIFT | spark.Keyboard.CTRL;

spark.KeyboardEvent = spark.Class("KeyboardEvent");

spark.KeyboardEvent.KEY_DOWN = "keybord-key-down";
spark.KeyboardEvent.KEY_UP   = "keybord-key-up";

spark.KeyboardEvent.Constructor = function(Protected, theType, theKey, theModifiers) {
    var Public = this;

    this.Inherit(spark.Event, theType);

    var _myKey = theKey;

    Public.__defineGetter__("key", function() {
        return _myKey;
    });

    var _myModifiers = theModifiers;

    Public.__defineGetter__("modifiers", function() {
        return _myModifiers;
    });

    Public.__defineGetter__("keyString", function() {
        var myString = (_myModifiers & spark.Keyboard.CTRL  ? "ctrl+"  : "") + 
                       (_myModifiers & spark.Keyboard.ALT   ? "alt+"   : "") + 
                       (_myModifiers & spark.Keyboard.SHIFT ? "shift+" : "") + 
                       _myKey;
        return myString;
    });
};

spark.StageEvent = spark.Class("StageEvent");

spark.StageEvent.PROTO_FRAME = "stage-proto-frame";
spark.StageEvent.FRAME       = "stage-frame";
spark.StageEvent.PRE_RENDER  = "stage-pre-render";
spark.StageEvent.POST_RENDER = "stage-post-render";

spark.StageEvent.Constructor = function(Protected, theType, theStage, theTime, theDeltaT) {
    var Public = this;

    this.Inherit(spark.Event, theType);

    var _myStage = theStage;
    var _myTime = theTime != null ? theTime : 0.0;
    var _myDeltaT = theDeltaT != null ? theDeltaT : 0.0;

    Public.__defineGetter__("stage", function() {
        return _myStage;
    });

    Public.__defineGetter__("currenttime", function() {
        return _myTime;
    });

    Public.__defineGetter__("deltaT", function() {
        return _myDeltaT;
    });
};

