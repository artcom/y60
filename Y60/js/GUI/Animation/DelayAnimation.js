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
 * Does nothing for the time of the animation
 */
GUI.DelayAnimation = function(theDuration) {
    this.Constructor(this, {}, theDuration);
}

GUI.DelayAnimation.prototype.Constructor = function(Public, Protected, theDuration) {
    var Base = {};

    GUI.SimpleAnimation.Constructor.call(Public, Public, Protected);
    
    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////
    
    // initialize from arguments
    Public.setup = function() {
		Protected.duration = theDuration;
    };
    
    // does noting
    Base.render = Public.render;
    Public.render = function() {
        
	};
    
	Public.toString = function() {
		return "DelayAnimation";
	};
    
    Public.setup();
};
