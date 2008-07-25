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
 * Abstract base class for animations.
 */
GUI.Animation = {};

// global counter for generating animation ids
GUI.Animation.idCounter = 0;

GUI.Animation.Constructor = function(Public, Protected) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _id = Animation.idCounter++;
    var _name = null;
		
    var _parent = null;
    var _duration = 100.0;
    var _loop = false;
    var _running = false;
    var _startTime = 0.0;
    var _progressTime = 0.0;
    var _progress = 0.0;

    var _easing  = function(v) {
        return v;
    };
		
    var _onPlay        = null;
    var _onFinish      = null;
    var _onCancel      = null;

    ////////////////////////////////////////
    // Properties
    ////////////////////////////////////////		
		
    // getter / setter 
    Public.id getter = function() {
        return _id;
	};
		
    Public.path getter = function() {
        var r = "" + _id;
	    var p = _parent;
		while(p) {
			r = p.id + "." + r;
		    p = p.parent;
	    }
	    return r;
	};
		
    Public.name getter = function() {
		return _name;
	};
		
    Public.name setter = function(v) {
		_name = v;
	};
    
    Public.progress getter = function() {
		return _progress;
	};
		
    Public.progress setter = function(p) {
		_progress = p;
	};
		
    Public.easing getter = function() {
        return _easing;
	};
		
    Public.duration getter = function() {
        return _duration;
	};
    
    Protected.duration setter = function(d) {
        _duration = d;
        Protected.durationChanged();
    };

    Public.loop getter = function() {
        return _loop;
    };

    Public.loop setter = function(l) {
        _loop = l;
    };
    	
    Public.running getter = function()	{
		return _running;
	};
		
    Public.parent getter = function() {
		return _parent;
	};
		
    Public.parent setter = function(a) {
		_parent = a;
	};
		
    Public.onPlay getter = function() {
		return _onPlay;
	};
		
    Public.onPlay setter = function(f) {
		_onPlay = f;
	};
		
    Public.onCancel getter = function()	{
		return _onCancel;
	};
		
    Public.onCancel setter = function(f) {
		_onCancel = f;
	};
		
    Public.onFinish getter = function()	{
		return _onFinish;
	};
		
    Public.onFinish setter = function(f) {
		_onFinish = f;
	};

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.play = function() {
		_startTime = millisec();
	    _progressTime = 0;
		_progress = _easing(0.0);
	    _running = true;
		
	    if(_onPlay != null) {
	        _onPlay();
		}
	    
	    Public.render();
	};
		
    Public.cancel = function() {
		_running = false;
	    if(_onCancel != null) {
			_onCancel();
		}
	};
		
    Public.finish = function() {
		if(_onFinish != null) {
			_onFinish();
		}
        if(_loop) {
            Public.play();
        } else {
            _running = false;
        }
	};
		
    Public.doFrame = function() {
        _progressTime = (millisec() - _startTime);
		_progress = _easing(_progressTime / _duration);
	    
	    var finished = (_progressTime >= _duration);
	    
	    if(finished) {
			_progressTime = _duration;
		    _progress = 1.0;
		}
	    
	    Public.render();
	    
	    if(finished) {
			Public.finish();
		}
	};
		
    Public.render = function() {
    };

    Public.toString = function() {
		return Protected.standardToString("Animation");
	};

    ////////////////////////////////////////
    // Protected
    ////////////////////////////////////////

    Protected.durationChanged = function() {
		if(_parent) {
			_parent.childDurationChanged(Public);
		}
    };
		
    Protected.standardToString = function(cls) {
		return cls + " " + Public.path + ((Public.name != null) ? (" (" + Public.name + ") ") : "");
	};

};
