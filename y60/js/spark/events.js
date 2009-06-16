/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2009, ART+COM AG Berlin, Germany <www.artcom.de>
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

spark.EventPhase = {
    IDLE:      "idle",
    CAPTURING: "capturing",
    TARGET:    "target",
    BUBBLING:  "bubbling"
};


spark.Event = spark.Class("Event");

spark.Event.Constructor = function(Protected, theType, theBubbles, theCancelable) {
    var Public = this;


    var _myBubbles = theBubbles != null ? theBubbles : false;

    Public.bubbles getter = function() {
        return _myBubbles;
    };

    var _myCancelable = theCancelable != null ? theCancelable : false;

    Public.cancelable getter = function() {
        return _myCancelable;
    };

    var _myCurrentTarget = null;

    Public.currentTarget getter = function() {
        return _myCurrentTarget;
    };

    var _myCurrentPhase = spark.EventPhase.IDLE;

    Public.currentPhase getter = function() {
        return _myCurrentPhase;
    };

    var _myTarget = null;

    Public.target getter = function() {
        return _myTarget;
    };

    var _myType = theType;

    Public.type getter = function() {
        return _myType;
    };

    var _myDispatching = false;
    
    Public.dispatching getter = function() {
        return _myDispatching;
    };

    Public.startDispatch = function(theTarget) {
        _myDispatching = true;
        _myTarget = theTarget;
        _myCurrentPhase  = spark.EventPhase.IDLE;
        _myCurrentTarget = null;
    };
    
    Public.cancelDispatch = function() {
        if(_myCancelable) {
            Public.finishDispatch();
        } else {
            Logger.fatal("event of type " + _myType + " is not cancelable");
        }
    };
    
    Public.finishDispatch = function() {
        _myDispatching = false;
        _myCurrentPhase = spark.EventPhase.IDLE;
        _myCurrentTarget = null;
    };

    Public.dispatchTo = function(theCurrentTarget, theCurrentPhase) {
        _myCurrentTarget = theCurrentTarget;
        _myCurrentPhase  = theCurrentPhase;
    };

};

spark.EventDispatcher = spark.AbstractClass("EventDispatcher");

spark.EventDispatcher.Constructor = function(Protected) {
    var Public = this;

    var _myListenersByType = {};

    Public.addEventListener = function(theType, theListener, theUseCapture) {
        if(theUseCapture == null) {
            theUseCapture = false;
        }

        var myListener = {
            type:       theType,
            listener:   theListener,
            useCapture: theUseCapture
        };

        if(! (theType in _myListenersByType)) {
            _myListenersByType[theType] = [];
        }

        _myListenersByType[theType] = _myListenersByType[theType].concat(myListener);
    };

    Public.getEventListeners = function(theType) {
        if(theType in _myListenersByType) {
            var myListeners = _myListenersByType[theType];
            if(myListeners.length > 0) {
                return myListeners;
            }
        }
        return [];
    };

    Public.dispatchEvent = function(theEvent) {
        theEvent.startDispatch(Public);

        // collect events to capture on
        var myCaptureList = [Public];
        while(myCaptureList[0].parent != null) {
            myCaptureList.unshift(myCaptureList[0].parent);
        }
        myCaptureList.pop();

        // capture phase
        for(var i = 0; i < myCaptureList.length; i++) {
            var myCurrentTarget = myCaptureList[i];
            if(myCurrentTarget.hasEventListener(theEvent.type)) {
                theEvent.dispatchTo(myCurrentTarget, spark.EventPhase.CAPTURING);
                var myListeners = myCurrentTarget.getEventListeners(theEvent.type);
                for(var j = 0; j < myListeners.length; j++) {
                    var myListener = myListeners[j];
                    if(myListener.useCapture) {
                        myListener.listener.call(myCurrentTarget, theEvent);
                        if(!theEvent.dispatching) {
                            return;
                        }
                    }
                }
            }
        }

        // target phase
        theEvent.dispatchTo(Public, spark.EventPhase.TARGET);
        if(Public.hasEventListener(theEvent.type)) {
            var myListeners = Public.getEventListeners(theEvent.type);
            for(var j = 0; j < myListeners.length; j++) {
                var myListener = myListeners[j];
                myListener.listener.call(Public, theEvent);
                if(!theEvent.dispatching) {
                    return;
                }
            }
        }

        // bubbling phase
        if(theEvent.bubbles) {
            Logger.fatal("spark event bubbling has not been implemented yet");
        }
        
        theEvent.finishDispatch();        
    };

    Public.hasEventListener = function(theType) {
        if(theType in _myListenersByType) {
            var myListeners = _myListenersByType[theType];
            if(myListeners.length > 0) {
                return true;
            }
        }
        return false;
    };

    Public.removeEventListener = function(theType, theListener, theUseCapture) {
        var myDidRemove = false;

        if(theUseCapture == null) {
            theUseCapture = false;
        }

        var myListeners = _myListenersByType[theType];
        for(var i = 0; i < myListeners.length; i++) {
            var myListener = myListeners[i];
            if(myListener.type == theType 
               && myListener.listener == theListener
               && myListener.useCapture == theUseCapture) {
                myListeners.splice(i, 1);
                myDidRemove = true;
                break;
            }
        }

        if(!myDidRemove) {
            Logger.error("Tried to remove nonexistent handler on " + Public.name + "." + theType);
        }
    };

    Public.willTrigger = function(theType) {
        if(Public.hasEventListener(theType)) {
            return true;
        }
        for(var i = 0; i < Public.children.length; i++) {
            var myChild = Public.children[i];
            if(myChild.willTrigger(theType)) {
                return true;
            }
        }
        return false;
    };

};

spark.MouseEvent = spark.Class("MouseEvent");

spark.MouseEvent.MOVE   = "mouse-move";
spark.MouseEvent.CLICK  = "mouse-click";
spark.MouseEvent.BUTTON_DOWN = "mouse-button-down";
spark.MouseEvent.BUTTON_UP   = "mouse-button-up";
spark.MouseEvent.ENTER  = "mouse-enter";
spark.MouseEvent.LEAVE  = "mouse-leave";
spark.MouseEvent.SCROLL = "mouse-scroll";

spark.MouseEvent.Constructor = function(Protected, theType, theX, theY, theAmountX, theAmountY) {
    var Public = this;
    
    this.Inherit(spark.Event, theType);
    
    var _myStageX = theX;
    
    Public.stageX getter = function() {
        return _myStageX;
    };
    
    var _myStageY = theY;

    Public.stageY getter = function() {
        return _myStageY;
    };
    
    var _myAmountX = theAmountX;
    
    Public.amountX getter = function() {
        return _myAmountX;
    };
    
    var _myAmountY = theAmountY;
    
    Public.amountY getter = function() {
        return _myAmountY;
    };
    
};

spark.Keyboard = {};

spark.Keyboard.SHIFT      = 1;
spark.Keyboard.ALT        = 2;
spark.Keyboard.ALT_SHIFT  = spark.Keyboard.ALT | spark.Keyboard.SHIFT;
spark.Keyboard.CTRL       = 4;
spark.Keyboard.CTRL_SHIFT = spark.Keyboard.CTRL | spark.Keyboard.SHIFT;
spark.Keyboard.CTRL_ALT   = spark.Keyboard.CTRL | spark.Keyboard.ALT;


spark.KeyboardEvent = spark.Class("KeyboardEvent");

spark.KeyboardEvent.KEY_DOWN = "keybord-key-down";
spark.KeyboardEvent.KEY_UP   = "keybord-key-up";

spark.KeyboardEvent.Constructor = function(Protected, theType, theKey, theModifiers) {
    var Public = this;
    
    this.Inherit(spark.Event, theType);
    
    var _myKey = theKey;
    
    Public.key getter = function() {
        return _myKey;
    };
    
    var _myModifiers = theModifiers;
    
    Public.modifiers getter = function() {
        return _myModifiers;
    };
    
    Public.keyString getter = function() {
        var myString =
              _myModifiers & spark.Keyboard.CTRL  ? "ctrl+"  : ""
            + _myModifiers & spark.Keyboard.ALT   ? "alt+"   : ""
            + _myModifiers & spark.Keyboard.SHIFT ? "shift+" : ""
            + _myKey;
        return myString;
    };
    
};


spark.StageEvent = spark.Class("StageEvent");

spark.StageEvent.FRAME       = "stage-frame";
spark.StageEvent.PRE_RENDER  = "stage-pre-render";
spark.StageEvent.POST_RENDER = "stage-post-render";

spark.StageEvent.Constructor = function(Protected, theType, theStage, theTime, theDeltaT) {
    var Public = this;
    
    this.Inherit(spark.Event, theType);
    
    var _myStage = theStage;
    var _myTime = theTime != null ? theTime : 0.0;
    var _myDeltaT = theDeltaT != null ? theDeltaT : 0.0;
    
    Public.stage getter = function() {
        return _myStage;
    };    
    
    Public.currenttime getter = function() {
        return _myTime;
    };  
    
    Public.deltaT getter = function() {
        return _myDeltaT;
    };
    
};
