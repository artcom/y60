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
// Widget of this type can be animated using fade() and move().
// 
// animation effects are scheduled from outside by calling an effect function
// with one or two parameters. the first parameter has a range between 0 and 1,
// the second parameter supersedes the first and is in a range between -1 and 1.
//
// If the second parameter is nonzero, an incremental variant of the effect
// is supposed to be executed. In this case, the first parameter should be ignored
// if possible.
//
// For fade(), this is an alpha offset; for move, this is a movement distance on
// a trajectory between the original position and the destination.
//
// Effects may require additional parameters, which are set on the function object
// itself from the caller. fade() does not require a parameter, move() expects
// a destination coordinate in its "to" property.
//
// Properties:
//
// fade():    
//
//
//
// move():    
//
//
//
//=============================================================================

function EffectWidget() {
	this.Constructor(this);
}

EffectWidget.prototype.Constructor = function(Public) {

    Public.fade = function(theAbsValue,theRelValue) {
        if (theRelValue) {
            var myNewAlpha = clamp(Public.node.alpha + theRelValue,0,1);
	    Public.node.alpha = myNewAlpha;
            if ((myNewAlpha == 1) || (myNewAlpha == 0)) {
               return true;
            };
            return false;
        } else {
            Public.alpha = theAbsValue;
	}
    }

    Public.fadeReverse = function(theAbsValue,theRelValue) {
	return Public.fade(1 - theAbsValue, theRelValue);
    }

    var myAnimationOffset = 0;
    Public.move = function(theDistance, theDistanceIncrement) {

        var mydx = Public.move.to[0] - Public.getAbsolutePosition()[0];
        var mydy = Public.move.to[1] - Public.getAbsolutePosition()[1];

	var isFinished = false;

        if (theDistanceIncrement) {
	if (theDistanceIncrement < -1 || theDistanceIncrement > 1) {
		Logger.error("distance increment " + theDistanceIncrement + " out of range!");
	}

        myAnimationOffset += theDistanceIncrement;

	if (myAnimationOffset <= 0 || myAnimationOffset >= 1) {
	    myAnimationOffset = clamp(myAnimationOffset,0,1);
	    isFinished = true;
        }
            theDistance = myAnimationOffset;
	} else {
	}

	Public.position = [Public.getAbsolutePosition()[0] + mydx * theDistance,
			   Public.getAbsolutePosition()[1] + mydy * theDistance];
	return isFinished;
    }

    // initialize the "to" parameter on the "move" effect to a dummy value
    // to make the function object writeable.
    Public.move.to = [1,2];


    Public.moveReverse = function(theAbsValue,theRelValue) {
	return Public.move(1 - theAbsValue, theRelValue);
    }
    Public.moveReverse.to = [1,2];
}
