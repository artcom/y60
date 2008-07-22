//=============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function Animation(theDuration) {
    this.Constructor(this, {}, theDuration);
}

var ourAnimationIdCounter = 0;

Animation.prototype.Constructor = function(Public, Protected, theDuration) {

		
		var _animationId = 0;
		
		var _id = _animationId++;
		var _name = null;
		
		var _parent = null;
		var _easing  = function(v) { return v; };
		var _duration = theDuration;
		var _running = false;
		var _startTime = 0.0;
		var _progressTime = 0.0;
		var _progress = 0.0;
		
		var _onPlay        = null;
		var _onFrameBegin  = null;
		var _onFrameFinish = null;
		var _onFinish      = null;
		var _onCancel      = null;
		
		
        // getter / setter 
		Public.id getter = function() {
			return _id;
		}
		
		Public.path getter = function() {
			var r = "" + _id;
			var p = _parent;
			while(p) {
				r = p.id + "." + r;
				p = p.parent;
			}
			return r;
		}
		
		Public.name getter = function() {
			return _name;
		}
		
		Public.name setter = function(v) {
			_name = v;
		}
    
    	Public.progress getter = function() {
			return _progress;
		}
		
		Public.progress setter = function(p) {
			_progress = p;
		}
    
    
		
		Public.easing getter = function() {
            return _easing;
		}
		
		Public.getDuration = function() {
			return _duration;
		}
        
        Public.setDuration = function(d) {
            _duration = d;
		}
	
		Public.running getter = function()	{
			return _running;
		}
		
		Public.parent getter = function() {
			return _parent;
		}
		
		Public.parent setter = function(a) {
			_parent = a;
		}
		
		Public.onPlay getter = function() {
			return _onPlay;
		}
		
		Public.onFrameBegin setter =  function(f) {
			_onFrameBegin = f;
		}
		
		Public.onFrameBegin getter = function()	{
			return _onFrameBegin;
		}
		
		Public.onFrameFinish setter = function(f) {
			_onFrameFinish = f;
		}
		
		Public.onFrameFinish getter = function() {
			return _onFrameFinish;
		}
		
		Public.onPlay setter = function(f) {
			_onPlay = f;
		}
		
		Public.onCancel getter = function()	{
			return _onCancel;
		}
		
		Public.onCancel setter = function(f)
		{
			_onCancel = f;
		}
		
		Public.onFinish getter = function()	{
			return _onFinish;
		}
		
		Public.onFinish setter = function(f) {
			_onFinish = f;
		}
		
		Protected.propagateDurationChange = function()
		{
			if(_parent) {
				_parent.propagateDurationChange();
			}
		}
		
		Public.play = function()
		{
			
			_startTime = millisec();
			_progressTime = 0;
			_progress = _easing(0);
			_running = true;
			
			if(_onPlay != null) {
				_onPlay();
			}
			
			Public.render();
		}
		
		Public.cancel = function() {
			_running = false;
			if(_onCancel != null) {
				_onCancel();
			}
			Logger.debug(Public + " cancelled");
		}
		
		Public.finish = function()
		{
			_running = false;
			if(_onFinish != null) {
				_onFinish();
			}
			Logger.debug(Public + " finished");
		}
		
		Public.doFrame = function()
		{
			_progressTime = (millisec() - _startTime) / 1000.0;
			_progress = _easing(_progressTime / _duration);
			
			var finished = (_progressTime >= _duration);
			
			if(finished) {
				_progressTime = _duration;
				_progress = 1.0;
			}
			
			if(_onFrameBegin != null) {
				_onFrameBegin();
			}
			
			Public.render();
			
			if(_onFrameFinish != null) {
				_onFrameFinish();
			}
			
			if(finished) {
				Public.finish();
			}
		}
		
		Public.render = function()
		{
		}
		
		Protected.standardToString = function(cls) {
			return cls + " " + Public.path + ((Public.name != null) ? (" (" + Public.name + ") ") : "");
		}
		
		Public.toString = function()
		{
			return standardToString("Animation");
		}
	
}