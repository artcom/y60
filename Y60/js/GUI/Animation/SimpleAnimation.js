//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Animation.js");

function SimpleAnimation(theDuration) {
    this.Constructor(this, {}, theDuration);
}


SimpleAnimation.prototype.Constructor = function(Public, Protected, theDuration) {
    var Base = {};

    Animation.prototype.Constructor(Public, Protected);

	Public.setup = function() {
        
    }
    
    Base.setDuration = Public.setDuration;
    Public.setDuration = function(d) {
		Base.setDuration(d);
        Protected.propagateDurationChange();
	}

	
}