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

function SequenceAnimation() {
    this.Constructor(this, {});
}


SequenceAnimation.prototype.Constructor = function(Public, Protected) {
    var Base = {};

    CompositeAnimation.prototype.Constructor(Public, Protected);

    var _current = 0;
	
    Base.propagateDurationChange = Protected.propagateDurationChange;
    Protected.propagateDurationChange = function() {
		var d:Number = 0;
	    for(var i = 0; i < Public.children.length; i++) {
				d += Public.children[i].duration;
			}
		Public.setDuration(d);
	    Base.propagateDurationChange();
	}
		
    Base.play = Public.play;
	Public.play =function()
	{
		_current = 0;
	    if(Public.children.length >= 1) {
			Public.children[_current].play();
		}
	    Base.play();	
    }
		
    Public.doFrame = function() {
		if(Public.onFrameBegin != null) {
			Public.onFrameBegin();
		}
		
	    Public.children[_current].doFrame();
	    if(!Public.children[_current].running) {
			_current++;
			if(_current < Public.children.length) {
				Public.children[_current].play();
			} else {
				Public.finish();
			}
		}
		
		if(Public.onFrameFinish != null) {
			Public.onFrameFinish();
		}
	}
	
	Public.toString = function() {
		return standardToString("SequenceAnimation");
	}
}