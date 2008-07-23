//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("SimpleAnimation.js");

/**
 * Animations implemented with a closure function.
 */
function ClosureAnimation(theDuration, theEasing, theFunction) {
    this.Constructor(this, {}, theDuration, theEasing, theFunction);
}

ClosureAnimation.prototype.Constructor = function(Public, Protected, theDuration, theEasing, theFunction) {
    var Base = {};

    SimpleAnimation.Constructor.call(Public, Public, Protected);
    
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _function = null;
    
    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////
    
    // initialize from arguments
    Public.setup = function() {
		Protected.duration = theDuration;

		if(theEasing != null) {
			Public.easing = theEasing;
		}

		_function = theFunction;
    };
    
    // call the closure with the current progress value
    Base.render = Public.render;
    Public.render = function() {
        if(_function != null) {
            _function(Public.progress);
        }
	};
    
	Public.toString = function() {
		return "ClosureAnimation" + _function;
	};
    
    Public.setup();
};
