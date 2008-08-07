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
 * Rotate an object using degrees as unit, normalizing the set angles.
 */
GUI.RotationAnimation = function(theDuration, theEasing, theObject, theProperty, theStart, theRotation) {
    this.Constructor(this, {}, theDuration, theEasing, theObject, theProperty, theStart, theRotation);
}

GUI.RotationAnimation.prototype.Constructor = function(Public, Protected, theDuration, theEasing, 
                                                       theObject, theProperty, theStart, theRotation) {
    var Base = {};

    GUI.SimpleAnimation.Constructor.call(Public, Public, Protected);
    
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _object = null;
	var _property = "";
	var _start = 0;
	var _rotation = 0;
	
	Public.object getter = function() {
	    return _object;
	}
	
	Public.property getter = function() {
	    return _property;
	}
	
	Public.start getter = function() {
	    return _start;
	}
	
	Public.rotation getter = function() {
	    return _rotation;
	}
    
    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////
    
    // initialize from arguments
    Public.setup = function() {
		Protected.duration = theDuration;

		if(theEasing != null) {
			Public.easing = theEasing;
		}

		_object = theObject;
		_property = theProperty;
		_start = theStart;
		_rotation = theRotation;
    };
    
    // set the current value
    Base.render = Public.render;
    Public.render = function() {
        var myValue = _start + Public.progress * _rotation;
        myValue %= 360.0;
        if(_object instanceof Array) {
            for (var object in _object) {
		        object[_property] = myValue;
            }
        } else {
	        _object[_property] = myValue;
        }
	};
    
	Public.toString = function() {
		return "RotationAnimation" + " on "  + _object.name + "." + _property;
	};
    
    Public.setup();
};
