//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
// 
// State Machine Widget
// 
// For a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
// 
// This widget represents a state machine. It accepts events via event(theEvent) and
// holds a state in its "state" property. For every state, a transition table exists
// in the properties, each of which are expected to provide a mapping from events to
// new states.
// 
// On event(X), Public["state"][X] is looked up and assigned to Public.state,
// and the relevant callbacks (see below) are executed.
// 
// Properties:
// 
// - state:        carries the current state as a string.
// - initialstate: carries the state which will be used in reset().
// 
// - laststate:    carries the previous state as a string.
// - state.*:      every state maps events to new states.
//                 when event(theEvent) is called and the current state is CURRENTSTATE,
//                 the mapping named stateCURRENTSTATE is looked up. if the value under
//                 stateCURRENTSTATE[theEvent] is a state NEWSTATE for which a stateNEWSTATE
//                 attribute exists, onLeaveCURRENTSTATE is executed, state is set to
//                 NEWSTATE, and onEnterNEWSTATE is executed.
// 
// Functions:
// 
// - enter(NEWSTATE): executes onLeaveCURRENTSTATE, onLeave(CURRENTSTATE),
//                    onEnterNEWSTATE, onEnter(NEWSTATE).
// - event(EVENT):   looks up stateCURRENTSTATE for EVENT and if its value is NEWSTATE
//                   and stateNEWSTATE exists, sets state to NEWSTATE and executes
//                   onEnterNEWSTATE(), onEnter(NEWSTATE).
// - reset():        executes enter(initialState)
// 
// Notifications:
// 
// onEnterSTATE():   issued when state STATE is entered.
// onEnter(STATE):   issued when state STATE is entered
// onEvent(theEvent):issued on incoming event theEvent.
// onTransition(theEvent,theNewState):issued on incoming event theEvent if that event triggers
//                   a state transition, which may be the same state as before.
// 
// onError(theEvent):issued on incoming event theEvent if that state transition is not defined.
// 
// onLeaveSTATE():   issued when state STATE is left.
// onLeave(STATE):   issued when state STATE is left.
// 
// Example:
/*
   <widget:StateMachineWidget id="theGlobalState"       state0="{'this':'state1','that':'state1'}"
                                                        state1="{'this':'state2','that':'state2'}"
                                                        state2="{'jenes':'state0','that':'state0'}"
                                                        onEnterstate0="print(theState,theEvent);"
                                                        onLeavestate1="print(theState,theEvent);"
                                                        onTransition="print(theState)"
                              onCreate="enter('state0');/>
*/
//=============================================================================

function StateMachineWidget() {
	return this.Constructor(this);
}

StateMachineWidget.prototype.Constructor = function(Public) {

    WidgetBase.prototype.Constructor(Public);

    Public.WIDGET_META = {};

    Public.WIDGET_META.callbackParams = { "onEvent":["theEvent"],
                                          "onTransition":["theEvent","theState","theOldState"],
                                          "onEnter":["theState"],
                                          "onLeave":["theState"] };

    Public.setup = function() {
        if (Public.state == undefined) {

            if (("initialstate" in Public) && Public.initialstate.length) {
                Public.reset();
            } else {
                var minState = undefined;

                for (i in Public) {
                    if (i.substring(5) == "state" && i.length > 5) {
                        if ((minState == undefined) || (i > minState)) {
                            minState = i;
                        }
                    }
                }
            }
            Logger.warning(Public.WIDGET_META.name + ": undefined initial state. setting to " + minState);
            Public.state = minState;
        }
    }

    Public.enter = function(theState) {
        if ((theState) in Public) {
            if ("onTransition" in Public) {
                Public.onTransition(undefined, theState, Public.state);
            }
            if ((("onLeave"+Public.state) in Public)) {
                Public[onLeave+Public.state]();
            }
            if ("onLeave" in Public) {
                Public.onLeave(Public.state);
            }
            Public.state = theState;
            if ((("onEnter"+theState) in Public)) {
                Public["onEnter"+theState]();
            }
            if ("onEnter" in Public) {
                Public.onEnter(theState);
            }
        }
    }

    Public.reset = function() {
        Public.enter(Public.initialstate);
    }

    Public.event = function(theEvent) {
        if ("onEvent" in Public) {
            Public.onEvent(theEvent);
        }
        if (Public.state == undefined) {
	    Logger.warning(Public.WIDGET_META.name + ": received event " + theEvent + " in undefined state!");
        } else if (((Public.state) in Public) && (theEvent in Public[Public.state])) {
            if ("onTransition" in Public) {
                Public.onTransition(theEvent, theNewStateString);
            }
            var theNewStateString = Public[Public.state][theEvent];
            if ((("onLeave"+Public.state) in Public)) {
                Public[onLeave+Public.state]();
            }
            if ("onLeave" in Public) {
                Public.onLeave(Public.state);
            }
            Public.state = theNewStateString;
            if ((("onEnter"+theNewStateString) in Public)) {
                Public["onEnter"+theNewStateString]();
            }
            if ("onEnter" in Public) {
                Public.onEnter(theNewStateString);
            }
        } else {
	    Logger.warning(Public.WIDGET_META.name + ": state transition " + theEvent + " not allowed from " + Public.state);
            if ("onError" in Public) {
                Public.onError(theEvent, Public.state);
            }
        }
    }
}