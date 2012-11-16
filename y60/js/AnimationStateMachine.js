/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
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
//
//   $RCSfile: StateMachine.js,v $
//   $Author: christian $
//   $Revision: 1.9 $
//   $Date: 2004/05/04 16:49:58 $
//
//
//=============================================================================
use("AnimationManager.js");

function StateMachine(theInitalState, theDefaultState, theAnimationMgr, theCharacterName) {
    this.Constructor(this, theInitalState, theDefaultState, theAnimationMgr, theCharacterName);
}

StateMachine.prototype.Constructor = function(obj, theInitalState, theDefaultState, theAnimationMgr, theCharacterName) {
    var _myAnimationManager     = theAnimationMgr;
    var _myCharacterName        = theCharacterName;

    var _myStateQueue           = new Array(theInitalState);

    var _myStateAnims           = [];  // a array of possible states and animations

    var _myStateChangeAnims     = [];  // a array of possible statechanges and animations
    var _myStateChangeParams    = [];  // a array of possible statechanges and params
    var _mySyncClips            = [];  // Clips that are played back synchronized in another character

    var _myDefaultState         = theDefaultState;

    const TRIM_QUEUE_LENGTH = 2;

    // add a state and a corresponding loop animation.
    // theLoopAnimation can be undefined or a string or a list of animations,
    // devided by '|'
    obj.addState = function(theState, theLoopAnimation) {
        var myAnimations = null;
        if (theLoopAnimation) {
            myAnimations = theLoopAnimation.split("|");
            if (myAnimations.length == 0) {
                myAnimations = [theLoopAnimation];
            }
        }
        _myStateAnims[theState] = myAnimations;
    }

    // Allows you to register clips that are started syncronously with the main clip
    obj.connectClip = function(theClip, theOtherCharacter, theOtherClip) {
        _mySyncClips[theClip] = {character: theOtherCharacter, clip: theOtherClip};
    }

    obj.pushState = function(theDestState) {
        var myPreviousState = _myStateQueue[_myStateQueue.length - 1];
        //print("push state: " + myPreviousState + " -> "+ theDestState);

        if (myPreviousState in _myStateChangeAnims &&
            theDestState in _myStateChangeAnims[myPreviousState])
        {
            // A fitting state change is registered
            _myStateQueue.push(theDestState);
        } else if (theDestState in _myStateChangeAnims[_myDefaultState]) {
            // A default state change is registered
            _myStateQueue.push(_myDefaultState);
            _myStateQueue.push(theDestState);
        } else {
            Logger.warning("Impossible state change from: " + myPreviousState + " to " + theDestState);
        }

        if (_myStateQueue.length > TRIM_QUEUE_LENGTH) {
            trimQueue();
        }
        //print("queue: " + _myStateQueue);
    }

    obj.onFrame = function() {
        if (!_myAnimationManager.isCharacterActive(_myCharacterName)) {
            setNextState();
        }
    }

    obj.switchToState = function(theState) {
        //print("Switch to state: " + theState);
        _myAnimationManager.stop(_myCharacterName);
        _myStateQueue = new Array(theState);
    }

    // add a statechange and a corresponding animation
    obj.addStateChange = function(theSrcState, theDestState, theAnimation, theParam) {
        if (!(theSrcState in _myStateChangeAnims)) {
            _myStateChangeAnims[theSrcState] = new Array();
            _myStateChangeParams[theSrcState] = new Array();
        }
        _myStateChangeAnims[theSrcState][theDestState] = theAnimation;
        _myStateChangeParams[theSrcState][theDestState] = theParam;
    }

    // reset this beast
    obj.reset = function() {
        _myStateQueue =  new Array(theInitalState);
    }

    obj.getCurrentState = function() {
        return _myStateQueue[0];
    }

    /////////////////////////////////////
    // private methods
    /////////////////////////////////////

    function setNextState() {
        var myQueueLength = _myStateQueue.length;
        if (myQueueLength == 1) {
            var myState = _myStateQueue[0];
            if (myState in _myStateAnims && _myStateAnims[myState]) {
                changeClip(randomElement(_myStateAnims[myState]));
            }
            //print("quequed state: "+myState);
        } else {
            var mySrcState  = _myStateQueue.shift();
            var myDestState = _myStateQueue[0];
            if (mySrcState in _myStateChangeAnims && myDestState in _myStateChangeAnims[mySrcState]) {
                //print("  Statechange: " + mySrcState + " -> " + myDestState);
                changeClip(_myStateChangeAnims[mySrcState][myDestState], _myStateChangeParams[mySrcState][myDestState]);
            } else {
                print("### WARNING Statechange from: " + mySrcState + " to: " + myDestState + " does not have a clip");
            }
        }
    }

    function changeClip(theAnimName, theReverseState) {
        if (theAnimName != "") {
            //print("change clip " + theAnimName);
            if (theReverseState == undefined || !theReverseState) {
                startClip(theAnimName, true)
            } else {
                startClip(theAnimName, false)
            }
        }
    }

    function startClip(theAnimName, theDirection) {
        //print("startClip " + theAnimName);
        _myAnimationManager.setClipForwardDirection(_myCharacterName, theAnimName, theDirection);
        _myAnimationManager.setClipLoops(_myCharacterName, theAnimName, 1);
        _myAnimationManager.startClip(_myCharacterName, theAnimName);

        if (theAnimName in _mySyncClips) {
            var mySyncClip = _mySyncClips[theAnimName];
            _myAnimationManager.setClipForwardDirection(mySyncClip.character, mySyncClip.clip, theDirection);
            _myAnimationManager.setClipLoops(mySyncClip.character, mySyncClip.clip, 1);
            _myAnimationManager.startClip(mySyncClip.character, mySyncClip.clip);
        }
    }

    function stateChangeExist(theSourceState, theDestState) {
        // Standard state changes from source to destination
        if (theSourceState in _myStateChangeAnims &&
            theDestState in _myStateChangeAnims[theSourceState])
        {
            return true;
        }

        // check if there is a statechange from theSourceState to defaultstate
        if (theSourceState in _myStateChangeAnims &&
            _myDefaultState in _myStateChangeAnims[theSourceState]) {

        }
        return false;
    }

    function trimQueue() {
        // The queue of animations is getting too long. Throw away animations without
        // causing jumps.
        var myLastState = _myStateQueue[_myStateQueue.length-1];
        for (var i = 1; i < _myStateQueue.length-1; ++i) {
            if (myLastState == _myStateQueue[i]) {
                _myStateQueue.splice(i, _myStateQueue.length-i-1);
                break;
            }
        }
    }

}
