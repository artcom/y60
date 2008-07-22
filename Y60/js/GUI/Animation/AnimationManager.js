//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function AnimationManager() {
    this.Constructor(this, {});
}


AnimationManager.prototype.Constructor = function(Public, Protected) {

    var _animations = [];
		
		
    Public.play = function(a)
    {
		_animations.push(a);
	    a.play(0);
	}
		
    Public.isPlaying = function() {
		return _animations.length > 0;	
	}
		
    Public.doFrame = function() {
		removeFinished();
	    for(var i = 0; i < _animations.length; i++) {
		var a = _animations[i];
		if(a.running) {
			a.doFrame();
			}
		}
	    removeFinished();
    }
		
    function removeFinished() {
        for(var i = 0; i < _animations.length; i++) {
			var a = _animations[i];
		    if(!a.running) {
				_animations.splice(i, 1);
			}
		}
	}
	
}