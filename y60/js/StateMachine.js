/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2011, ART+COM AG Berlin, Germany <www.artcom.de>
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

*/
/*jslint nomen:false plusplus:false*/
/*globals Logger*/

var SM = (function () {
    
    ////////////
    // Public //
    ////////////
    var State = function (theStateID) {
        this.Constructor(this, theStateID);
    };
    
    State.prototype.Constructor = function (self, theStateID) {
        var _id = theStateID;
        self.__defineGetter__('id', function () { 
            return _id;
        });
        self.toString = function toString() {
            return _id;
        };
        self.handler = {};
    };
    
    var Sub = function (theStateID, theSubMachine) {
        this.Constructor(this, theStateID, theSubMachine);
    };

    Sub.prototype.Constructor = function (self, theStateID, theSubMachine) {
        State.prototype.Constructor(self, theStateID);
        
        var _subMachine = theSubMachine;
        self._enter = function () {
            _subMachine.setup();
        };
        self._exit = function () {
            _subMachine.teardown();
        };
        self.terminateTransition = function () {
            _subMachine.terminateTransition.apply(_subMachine, arguments);
        };
        self.handleEvent = function () {
            _subMachine.handleEvent.apply(_subMachine, arguments);
        };
        self.toString = function toString() {
            return self.id + "/(" + _subMachine + ")"; 
        };
        self.__defineGetter__("subMachine", function () {
            return _subMachine;
        });
    };
    
    var Parallel = function (theStateID) {
        this.Constructor(this, theStateID, arguments);
    };
    
    Parallel.prototype.Constructor = function (self, theStateID, theArguments) {
        State.prototype.Constructor(self, theStateID);
        // Create a new array from the contents of arguments
        var args = Array.prototype.slice.call(theArguments);
        if (args.length < 2) {
            throw "not enough arguments";
        }
        var _id          = args.shift();
        var _subMachines = args;

        self.toString = function toString() {
            return _id + "/(" + _subMachines.join('|') + ")";
        };

        self._enter = function () {
            for (var i = 0; i < _subMachines.length; ++i) {
                _subMachines[i].setup();
            }
        };
        self._exit = function () {
            for (var i = 0; i < _subMachines.length; ++i) {
                _subMachines[i].teardown();
            }
        };
        self.terminateTransition = function () {
            for (var i = 0; i < _subMachines.length; ++i) {
                _subMachines[i].terminateTransition();
            }
        };
        self.handleEvent = function () {
            for (var i = 0; i < _subMachines.length; ++i) {
                _subMachines[i].handleEvent.apply(_subMachines[i], arguments);
            }
        };
    };

    var Transition = function (theNextState) {
        var self       = this;
        self.nextState = theNextState;
        self.travel = function () {
            self.switchState(self.nextState);
        };
        self.terminate = function () {
            self.switchState(self.nextState);
        };
    };
    
    var StateMachine = function () {
        this.Constructor(this, arguments);
    };

    StateMachine.prototype.Constructor = function (self, theArguments) {
        var _states       = {};
        self.initialState = theArguments[0].id;
        self.transition   = null;
        var _currentState = null;

        for (var i = 0; i  < theArguments.length; ++i) {
            _states[theArguments[i].id] = theArguments[i];
        }

        self.toString = function toString() {
            return _states[_currentState].toString();
        };
        /*self.__defineGetter__("states", function () {
            return _states;
        });*/
        self.__defineGetter__('stateObject', function () {
            return _states[_currentState];
        });

        function switchState(newState) {
            if (self.transition !== null) { 
                self.transition.switchState = function () {
                    Logger.debug("events from the past caught up with you");
                }; 
                self.transition = null;
            }
            Logger.debug("State transition '" + _currentState + "' => '" + newState + "'");
            // call old state's exit handler
            if (_currentState !== null && '_exit' in self.stateObject) {
                Logger.debug("exiting state '" + _currentState + "'");
                self.stateObject._exit(newState);
            }
            var oldState  = _currentState; 
            _currentState = newState;
            // call new state's enter handler
            if (_currentState !== null && '_enter' in self.stateObject) {
                Logger.debug("entering state '" + _currentState + "'");
                self.stateObject._enter(oldState);
            }
        }

        self.setup = function () {
            Logger.debug("setting initial state: " + self.initialState);
            _currentState = null;
            switchState(self.initialState);
        };
        self.teardown = function () {
            if (self.transition !== null) {
                Logger.debug("terminated transition on teardown");
                self.transition.terminate();
            }
            switchState(null);
        };

        self.terminateTransition = function (evt) {
            // Logger.debug("may terminate transition "+self.transition+" on "+evt+": "+_currentState, DEBUG_LEVELS.TRACE);
            if (self.transition !== null) {
                var nextState = self.transition.nextState;
                if (self.transition.terminate(arguments[0])) {
                    Logger.debug(arguments[0] + " terminated transition, skipping to " + nextState);
                    self.transition.switchState(nextState); // forces pending state-change immediately
                } else {
                    Logger.debug("dropped " + arguments[0] + " during transition");
                }
            } else {
                if ('terminateTransition' in self.stateObject) {
                    self.stateObject.terminateTransition(evt);
                }
            }
        };

        self.handleEvent = function () {
            if (self.transition !== null) {
                Logger.debug("dropped " + arguments[0] + " during handleEvent: transition still active: " + _currentState);
                return;
            }
            if (!_currentState) {
                Logger.debug("dropped " + arguments[0] + " during handleEvent: because current state is null");
                return;
            }
            var nextState = null;
            if (arguments[0] in self.stateObject.handler) {
                nextState = self.stateObject.handler[arguments[0]].apply(self.stateObject, arguments);
            }
            if (typeof(nextState) === "object" && nextState !== null) {
                self.transition             = nextState;
                self.transition.switchState = switchState;
                self.transition.travel();
            } else if (nextState !== null && nextState !== undefined) {
                switchState(nextState);
            } else {
                // we don't know self event (nextState == null), so bubble down to the children
                if ('handleEvent' in self.stateObject) {
                    self.stateObject.handleEvent.apply(self.stateObject, arguments);
                }
            }
        };
    };

    //////////////
    // Interface
    //////////////
    return {
        State        : State,
        Sub          : Sub,
        Parallel     : Parallel,
        Transition   : Transition,
        StateMachine : StateMachine
    };
}()); // execute outer function to produce our closure
