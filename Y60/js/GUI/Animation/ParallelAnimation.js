//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/**
 * Play several animations in parallel.
 */
GUI.ParallelAnimation = function() {
    this.Constructor(this, {});
}

GUI.ParallelAnimation.prototype.Constructor = function(Public, Protected) {
    var Base = {};

    GUI.CompositeAnimation.Constructor.call(Public, Public, Protected);

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    // duration = max(map(children, duration))
    Base.childDurationChanged = Public.childDurationChanged;
    Public.childDurationChanged = function(theChild) {
    	var d = 0;
	    for(var i = 0; i < Public.children.length; i++) {
	        var c = Public.children[i].duration;
		    if(c > d) {
				d = c;
			}
		}
	    Protected.duration = d;
	};
	
    // start playing all children
    Base.play = Public.play;
	Public.play = function() {
		Base.play();
		for(var i = 0; i < Public.children.length; i++) {
			Public.children[i].play();
		}
	};
	
    // step all children forward
	Public.doFrame = function()	{
		var finished = true;
		for(var i = 0; i < Public.children.length; i++) {
			Public.children[i].doFrame();
			if(Public.children[i].running) {
				finished = false;
			}
		}
		
		if(finished) {
			Public.finish();
		}
	};
	
	Public.toString = function() {
		return "ParallelAnimation";
	};
	
};
