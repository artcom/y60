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

function CompositeAnimation() {
    this.Constructor(this, {});
}


CompositeAnimation.prototype.Constructor = function(Public, Protected) {
    var Base = {};

    Animation.prototype.Constructor(Public, Protected);
	
    var _children = [];
		
     Public.children getter = function() {
         return [].concat(_children);
	 }
		
    Public.add = function(theAnimation) {
		_children.push(theAnimation);
        theAnimation.parent = Public;
		Protected.propagateDurationChange();
	}
		
    Base.cancel = Public.cancel;
    Public.cancel = function() {
		for(var i = 0; i < _children.length; i++) {
			if(_children[i].running) {
				_children[i].cancel();
			}
		}
	    Base.cancel();
    }
		
    

    Base.finish = Public.finish;
    Public.finish = function(){
		for(var i = 0; i < _children.length; i++) {
			if(_children[i].running) {
				_children[i].finish();
			}
		}
	    Base.finish();
	}
    
}