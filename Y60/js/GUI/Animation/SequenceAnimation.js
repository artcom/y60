//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("CompositeAnimation.js");

/**
 * Play several animations in sequence.
 */
function SequenceAnimation() {
    this.Constructor(this, {});
}

SequenceAnimation.prototype.Constructor = function(Public, Protected) {
    var Base = {};

    CompositeAnimation.Constructor(Public, Protected);

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _current = 0;
	
    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    // duration = sum(map(children, duration))
    Base.childDurationChanged = Public.childDurationChanged;
    Public.childDurationChanged = function() {
		var d = 0.0;
	    for(var i = 0; i < Public.children.length; i++) {
				d += Public.children[i].duration;
			}
		Protected.duration = d;
	};
	
    // start to play this animation
    // plays the first animation
    Base.play = Public.play;
	Public.play = function()
	{
		_current = 0;
	    if(Public.children.length >= 1) {
			Public.children[_current].play();
		}
	    Base.play();	
    };
	
    // iterate through child animations
    Public.doFrame = function() {
	    Public.children[_current].doFrame();
	    if(!Public.children[_current].running) {
			_current++;
			if(_current < Public.children.length) {
				Public.children[_current].play();
			} else {
				Public.finish();
			}
		}
	};
	
	Public.toString = function() {
		return Protected.standardToString("SequenceAnimation");
	};

};
