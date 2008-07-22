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

function ParallelAnimation() {
    this.Constructor(this, {});
}


ParallelAnimation.prototype.Constructor = function(Public, Protected) {
    var Base = {};

    CompositeAnimation.prototype.Constructor(Public, Protected);

	

    Base.propagateDurationChange = Protected.propagateDurationChange;
    Protected.propagateDurationChange = function() {
    	var d = 0;
	    for(var i = 0; i < Public.children.length; i++) {
	        var c = Public.children[i].getDuration();
		    if(c > d) {
				d = c;
			}
		}
	    Public.setDuration(d);
	    Base.propagateDurationChange();
	}
	
    Base.play = Public.play;
	Public.play =function()
	{
		Base.play();
		for(var i = 0; i < Public.children.length; i++) {
			Public.children[i].play();
		}
	}
	
	Public.doFrame = function()	{
		if(Public.onFrameBegin != null) {
			Public.onFrameBegin();
		}
		
		var finished = true;
		for(var i = 0; i < Public.children.length; i++) {
			Public.children[i].doFrame();
			if(Public.children[i].running) {
				finished = false;
			}
		}
		
		if(Public.onFrameFinish != null) {
			Public.onFrameFinish();
		}
		
		if(finished) {
			Public.finish();
		}
	}
	
	Public.toString = function()
	{
		return "ParallelAnimation";
	}
	
}