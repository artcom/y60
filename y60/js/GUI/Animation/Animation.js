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
    Public.__defineGetter__("id", function() {
        return _id;
    });

    Public.__defineGetter__("path", function() {
        var r = "" + _id;
        var p = _parent;
        while(p) {
            r = p.id + "." + r;
            p = p.parent;
        }
        return r;
    });

    Public.__defineGetter__("name", function() {
        return _name;
    });

    Public.__defineSetter__("name", function(v) {
        _name = v;
    });

    Public.__defineGetter__("progress", function() {
        return _progress;
    });

    Public.__defineSetter__("progress", function(p) {
        _progress = p;
    });

    Public.__defineGetter__("easing", function() {
        return _easing;
    });

    Public.__defineSetter__("easing", function(theEasing) {
        _easing = theEasing;
    });

    Public.__defineGetter__("duration", function() {
        return _duration;
    });

    Protected.__defineSetter__("duration", function(d) {
        _duration = d;
        Protected.durationChanged();
    });

    Public.__defineGetter__("loop", function() {
        return _loop;
    });

    Public.__defineSetter__("loop", function(l) {
        _loop = l;
    });

    Public.__defineGetter__("running", function()  {
        return _running;
    });

    Public.__defineGetter__("parent", function() {
        return _parent;
    });

    Public.__defineSetter__("parent", function(a) {
        _parent = a;
    });

    Public.__defineGetter__("onPlay", function() {
        return _onPlay;
    });

    Public.__defineSetter__("onPlay", function(f) {
        _onPlay = f;
    });

    Public.__defineGetter__("onCancel", function() {
        return _onCancel;
    });

    Public.__defineSetter__("onCancel", function(f) {
        _onCancel = f;
    });

    Public.__defineGetter__("onFinish", function() {
        return _onFinish;
    });

    Public.__defineSetter__("onFinish", function(f) {
        _onFinish = f;
    });

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.play = function() {
        Logger.debug("Playing " + Public);
        _startTime = ourCurrentAnimationTime;
        _progressTime = 0;
        _progress = _easing(0.0);
        _running = true;
        _finished = false;

        callOnPlay();

        Public.render();
    };

    Public.cancel = function() {
        Logger.debug("Cancelled " + Public);
        _running = false;
        _finished = false;
        callOnCancel();
    };

    Public.finish = function() {
        Logger.debug("force finish of " + Public);
        if (_finished) {
            return;
        }
        if (!_running) {
            Public.play(true);
        }
        Protected.finish();
    };

    Public.doFrame = function(theTime) {
        if (_startTime == -1) {
            _startTime = ourCurrentAnimationTime;
        }
        _progressTime = (theTime - _startTime);
        _progress = _easing(_progressTime / _duration);

        var finished = (_progressTime >= _duration);

        if (finished) {
            Protected.finish();
        } else {
            Public.render();
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

    Protected.finish = function() {
        Logger.debug("Finished " + Public);
        _progressTime = _duration;
        _progress = 1.0;
        Public.render();
        callOnFinish();
        if(_loop) {
            Public.play();
        } else {
            _running = false;
            _finished = true;
        }
    };

    Protected.durationChanged = function() {
        if(_parent) {
            _parent.childDurationChanged(Public);
        }
    };

    Protected.standardToString = function(cls) {
        return cls + " " + Public.path + ((Public.name != null) ? (" (" + Public.name + ") ") : "") + " running: " + _running;
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
