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
 * Animations that are time-compositions of their children.
 */
GUI.CompositeAnimation = {};

GUI.CompositeAnimation.Constructor = function(Public, Protected) {
    var Base = {};

    GUI.Animation.Constructor(Public, Protected);
	
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _children = [];

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////
		
    Public.children getter = function() {
        return [].concat(_children);
	};
    
    // add a child, also updating duration
    Public.add = function(theAnimation) {
		_children.push(theAnimation);
        theAnimation.parent = Public;
		Public.childDurationChanged();
	};

    // duration computation, should be overridden
    Public.childDurationChanged = function(theChild) {
    };
    
    // generic cancel
    Base.cancel = Public.cancel;
    Public.cancel = function() {
		for(var i = 0; i < _children.length; i++) {
			if(_children[i].running) {
				_children[i].cancel();
			}
		}
	    Base.cancel();
    };

    // generic finish
    Base.finish = Public.finish;
    Public.finish = function(){
		for(var i = 0; i < _children.length; i++) {
			if(_children[i].running) {
				_children[i].finish();
			}
		}
	    Base.finish();
	};
    
};
