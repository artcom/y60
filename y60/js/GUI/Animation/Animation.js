/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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

    var _id = GUI.Animation.idCounter++;
    var _name = null;

    var _parent = null;
    var _duration = 100.0;
    var _loop = false;
    var _running = false;
    var _startTime = -1;
    var _progressTime = 0.0;
    var _progress = 0.0;
    var _finished = false;

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

    Public.easing setter = function(theEasing) {
        _easing = theEasing;
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

    Public.running getter = function()  {
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

    Public.onCancel getter = function() {
        return _onCancel;
    };

    Public.onCancel setter = function(f) {
        _onCancel = f;
    };

    Public.onFinish getter = function() {
        return _onFinish;
    };

    Public.onFinish setter = function(f) {
        _onFinish = f;
    };

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.play = function() {
        Logger.debug("Playing " + this);
        _startTime = ourCurrentAnimationTime;
        _progressTime = 0;
        _progress = _easing(0.0);
        _running = true;
        _finished = false;

        callOnPlay();

        Public.render();
    };

    Public.cancel = function() {
        Logger.debug("Cancelled " + this);
        _running = false;
        callOnCancel();
    };

    Public.finish = function() {
        Logger.debug("Finished " + this);
        callOnFinish();
        if(_loop) {
            Public.play();
        } else {
            _running = false;
            _finished = true;
        }
    };

    Public.comeToAnEnd = function() {
        Logger.debug("comeToAnEnd " + Public.name);
        if (_finished) {
            return;
        }
        if (!_running) {
            Public.play(true);
            Public.render();
        }
        _progressTime = _duration;
        _progress = 1.0;
        Public.render();
        Public.finish(true);

    };

    Public.doFrame = function(theTime) {
        if (_startTime == -1) {
            _startTime = ourCurrentAnimationTime;
        }
        _progressTime = (theTime - _startTime);
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

    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////

    function callOnPlay() {
        if(_onPlay != null) {
            _onPlay.call(Public);
        }
    };

    function callOnFinish() {
        if(_onFinish != null) {
            _onFinish.call(Public);
        }
    };

    function callOnCancel() {
        if(_onCancel != null) {
            _onCancel.call(Public);
        }
    };

};
