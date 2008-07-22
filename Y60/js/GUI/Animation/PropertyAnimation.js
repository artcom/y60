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

function PropertyAnimation(theDuration, theEasing, theObject, theProperty, theMin, theMax) {
    this.Constructor(this, {}, theDuration, theEasing, theObject, theProperty, theMin, theMax);
}


PropertyAnimation.prototype.Constructor = function(Public, Protected, theDuration, theEasing, 
                                                   theObject, theProperty, theMin, theMax) {
    var Base = {};

    SimpleAnimation.prototype.Constructor(Public, Protected, theEasing, theObject, theProperty, theMin, theMax);

    var _object = null;
	var _property = "";
	var _min = 0;
	var _max = 0;
		
    Base.setup = Public.setup;
    Public.setup = function() {
	    Base.setup();

		Public.setDuration(theDuration);
		if(theEasing != null) {
			Public.easing = theEasing;
		}
		_object = theObject;
		_property = theProperty;
		_min = theMin;
		_max = theMax;
    }
		
    Base.render = Public.render;
    Public.render = function() {
		_object[_property] = _min + ((_max - _min) * Public.progress);
	    //print(_object + "." + _property + " = " + _object[_property]);
	}
		
	Public.toString = function() {
		return "PropertyAnimation" + " on "  + _object + "." + _property;
	}
    
    Public.setup();
}