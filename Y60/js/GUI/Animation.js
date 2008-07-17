//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function Animation(theManager, theFunction, theDuration, theObjects, theContinuation) {
    this.Constructor(this, {}, theManager, theFunction, theDuration, theObjects, theContinuation);
}

var ourAnimationIdCounter = 0;

Animation.prototype.Constructor = function(Public, Protected, theManager, theFunction, theDuration, theObjects, theContinuation) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _myId            = ourAnimationIdCounter++;

    var _myManager       = theManager;

    var _myFunction      = theFunction;
    var _myDuration      = theDuration;
    var _myObjects       = theObjects;
    var _myContinuation  = theContinuation;

    var _myActive        = false;
    var _myStartTime     = 0.0;

    var _myIteration     = 0;

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.id getter = function() {
        return _myId;
    }

    Public.active getter = function() {
        return _myActive;
    }

    Public.duration getter = function() {
        return _myDuration;
    }

    Public.iteration getter = function() {
        return _myIteration;
    }
    Public.iteration setter = function(theIter) {
        _myIteration = theIter;
    }
    
    Public.onFrame = function(theTime) {
        var myTime = millisec() - _myStartTime;

        if(myTime < _myDuration) {
            var myProgress = myTime/_myDuration;
            _myFunction(Public, myProgress, _myObjects);
        } else {
            _myFunction(Public, 1.0, _myObjects);
            Public.finish();
        }
    }

    Public.start = function() {
        Logger.info("Starting animation " + _myId + " for " + theDuration + " msecs.");

        _myStartTime = millisec();

        _myDuration       = theDuration;
        _myObjects        = theObjects;
        _myContinuation   = theContinuation;
        _myFunction       = theFunction;

        _myActive = true;

        _myFunction(Public, 0.0, _myObjects);
    }

    Public.finish = function() {
        _myActive = false;
        Logger.info("Animation " + _myId + " finished.");
        _myContinuation();
    }

}

Animation.fadeIn = function(theAnimation, theProgress, theObjects) {
    for(var i = 0; i < theObjects.length; i++) {
        Modelling.setAlpha(theObjects[i], theProgress);
    }
}

// array format: [object-array alpha-goal object-array alpha-goal ...]
Animation.fadeInTowards = function(theAnimation, theProgress, theArray) {
    for(var i = 0; i < theArray.length; i += 2) {
        var myObjects = theArray[i+0];
        var myAlpha = theArray[i+1];

        for(var j = 0; j < myObjects.length; j++) {
            Modelling.setAlpha(myObjects[j], theProgress * myAlpha);
        }
    }
}

Animation.fadeOut = function(theAnimation, theProgress, theObjects) {
    for(var i = 0; i < theObjects.length; i++) {
        Modelling.setAlpha(theObjects[i], (theProgress*-1.0)+1.0);
    }
}

// array format: [object-array current-alpha object-array current-alpha ...]
Animation.fadeOutTowards = function(theAnimation, theProgress, theArray) {
    for(var i = 0; i < theArray.length; i += 2) {
        var myObjects = theArray[i+0];
        var myAlpha = theArray[i+1];

        for(var j = 0; j < myObjects.length; j++) {
            Modelling.setAlpha(myObjects[j], ((theProgress*-1.0)+1.0) * myAlpha);
        }
    }
}

Animation.click = function(theAnimation, theProgress, theObjects) {
    for(var i = 0; i < theObjects.length; i++) {
        var myObject = theObjects[i];
        if(theProgress == 1.0) {
            myObject.highlight = false;
        } else {
            myObject.highlight = true;
        }
    }
}

Animation.blink = function(theInterval) {
    return function(theAnimation, theProgress, theObjects) {
        var myFrac = 1 / (theAnimation.duration / theInterval);
        var myIter = Math.round(theProgress/myFrac);
        if (theAnimation.iteration < myIter) {
            theAnimation.iteration = myIter;
            for (var i = 0; i < theObjects.length; i++) {
                theObjects[i].bodies[0].visible = (theAnimation.iteration%2 == 0);
            }
        }
    }
}

Animation.delay = function(theAnimation, theProgress, theObjects) {
}

function AnimationManager() {
    this.Constructor(this);
}

AnimationManager.prototype.Constructor = function(Public) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _myAnimations = {};

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.onFrame = function(theTime) {
        for(var myAnimationId in _myAnimations) {
            var myAnimation = _myAnimations[myAnimationId];
            myAnimation.onFrame(theTime);
            if(!myAnimation.active) {
                delete _myAnimations[myAnimationId];
            }
        }
    }

    Public.animate = function(theFunction, theDuration, theObjects, theContinuation) {
        var myAnimation = new Animation(Public, theFunction, theDuration, theObjects, theContinuation);
        myAnimation.start();
        _myAnimations[myAnimation.id] = myAnimation;
        return myAnimation.id;
    }

    Public.abort = function(theId) {
        if (theId in _myAnimations) {
            delete _myAnimations[theId];
            Logger.info("Animation " + theId + " aborted.");
        }
    }
}
