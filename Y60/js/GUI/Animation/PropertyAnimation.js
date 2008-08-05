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
 * Animate a javascript property on one or more objects.
 */
GUI.PropertyAnimation = function(theDuration, theEasing, theObject, theProperty, theMin, theMax) {
    this.Constructor(this, {}, theDuration, theEasing, theObject, theProperty, theMin, theMax);
}

GUI.PropertyAnimation.prototype.Constructor = function(Public, Protected, theDuration, theEasing, 
                                                       theObject, theProperty, theMin, theMax) {
    var Base = {};

    GUI.SimpleAnimation.Constructor.call(Public, Public, Protected);
    
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _object = null;
	var _property = "";
	var _min = 0;
	var _max = 0;
	
	Public.object getter = function() {
	    return _object;
	}
	
	Public.property getter = function() {
	    return _property;
	}
	
	Public.min getter = function() {
	    return _min;
	}
	
	Public.max getter = function() {
	    return _max;
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
		_min = theMin;
		_max = theMax;
    };
    
    // set the current value
    Base.render = Public.render;
    Public.render = function() {
        var myValue = _min + ((_max - _min) * Public.progress);
        if(_object instanceof Array) {
            for (var object in _object) {
		        object[_property] = myValue;
            }
        } else {
	        _object[_property] = myValue;
        }
	};
    
	Public.toString = function() {
		return "PropertyAnimation" + " on "  + _object.name + "." + _property;
	};
    
    Public.setup();
};
