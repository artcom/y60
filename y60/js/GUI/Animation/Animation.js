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

/*jslint nomen:false, plusplus:false*/
/*globals GUI, Logger, ourCurrentAnimationTime, Exception*/

/**
 * Abstract base class for animations.
 */
GUI.Animation = function Animation() {
    throw new Exception("<Animation> Abstract Base class cannot be instantiated");
};

// global counter for generating animation ids
GUI.Animation.idCounter = 0;

GUI.Animation.Constructor = function (Public, Protected) {
    var _ = {};
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    _.id           = GUI.Animation.idCounter++;
    _.name         = null;
    
    _.parent       = null;
    _.duration     = 100.0;
    _.loop         = false;
    _.running      = false;
    _.startTime    = -1;
    _.progressTime = 0.0;
    _.progress     = 0.0;
    _.finished     = false;

    _.easing  = function (theValue) {
        return theValue;
    };

    _.callbacks = {
        onPlay   : null,
        onFinish : null,
        onCancel : null
    };

    /////////////////////
    // Private Methods //
    /////////////////////

    _.callOnPlay = function () {
        if (_.callbacks.onPlay !== null) {
            _.callbacks.onPlay.call(Public);
        }
    };

    _.callOnFinish = function () {
        if (_.callbacks.onFinish !== null) {
            _.callbacks.onFinish.call(Public);
        }
    };

    _.callOnCancel = function () {
        if (_.callbacks.onCancel !== null) {
            _.callbacks.onCancel.call(Public);
        }
    };

    ///////////////////////
    // Protected Methods //
    ///////////////////////

    Protected.finish = function () {
        Logger.debug("Finished " + Public);
        _.progressTime = _.duration;
        _.progress = 1.0;
        Public.render();
        _.callOnFinish();
        if (_.loop) {
            Public.play();
        } else {
            _.running = false;
            _.finished = true;
        }
    };

    Protected.durationChanged = function () {
        if (_.parent) {
            _.parent.childDurationChanged(Public);
        }
    };

    Protected.standardToString = function (cls) {
        return cls + " " + Public.path + ((Public.name !== null) ? (" (" + Public.name + ") ") : "") + " running: " + _.running;
    };

    ////////////////////
    // Public Methods //
    ////////////////////

    Public.play = function () {
        Logger.debug("Playing " + Public);
        _.startTime    = ourCurrentAnimationTime;
        _.progressTime = 0;
        _.progress     = _.easing(0.0);
        _.running      = true;
        _.finished     = false;
        
        _.callOnPlay();
        Public.render();
    };

    Public.cancel = function () {
        Logger.debug("Cancelled " + Public);
        _.running  = false;
        _.finished = false;
        _.callOnCancel();
    };

    Public.finish = function () {
        Logger.debug("force finish of " + Public);
        if (_.finished) {
            return;
        }
        if (!_.running) {
            Public.play(true);
        }
        Protected.finish();
    };

    Public.doFrame = function (theTime) {
        if (_.startTime === -1) {
            _.startTime = ourCurrentAnimationTime;
        }
        _.progressTime = (theTime - _.startTime);
        _.progress = _.easing(_.progressTime / _.duration);

        var finished = (_.progressTime >= _.duration);
        if (finished) {
            Protected.finish();
        } else {
            Public.render();
        }
    };

    Public.render = function () {
    };

    Public.toString = function () {
        return Protected.standardToString("Animation");
    };

    ////////////////
    // Properties //
    ////////////////

    // getter / setter
    Public.__defineGetter__("id", function () {
        return _.id;
    });

    Public.__defineGetter__("path", function () {
        var r = "" + _.id;
        var p = _.parent;
        while (p) {
            r = p.id + "." + r;
            p = p.parent;
        }
        return r;
    });

    Public.__defineGetter__("name", function () {
        return _.name;
    });

    Public.__defineSetter__("name", function (theName) {
        _.name = theName;
    });

    Public.__defineGetter__("progress", function () {
        return _.progress;
    });

    Public.__defineSetter__("progress", function (theProgress) {
        // TODO validate theProgress to be numeric (between 0 and 1 probably)
        _.progress = theProgress;
    });

    Public.__defineGetter__("easing", function () {
        return _.easing;
    });

    Public.__defineSetter__("easing", function (theEasing) {
        // TODO validate the Easing to be a proper Easing (function or even instance of something)
        _.easing = theEasing;
    });

    Public.__defineGetter__("duration", function () {
        return _.duration;
    });

    Protected.__defineSetter__("duration", function (theDuration) {
        // TODO validate theDuration to be numeric
        _.duration = theDuration;
        Protected.durationChanged();
    });

    Public.__defineGetter__("loop", function () {
        return _.loop;
    });

    Public.__defineSetter__("loop", function (theLoop) {
        // TODO validate to be a boolean flag (?)
        _.loop = theLoop;
    });

    Public.__defineGetter__("running", function ()  {
        return _.running;
    });

    Public.__defineGetter__("parent", function () {
        return _.parent;
    });

    Public.__defineSetter__("parent", function (theParent) {
        // TODO validate to be another Animation
        _.parent = theParent;
    });

    Public.__defineGetter__("onPlay", function () {
        return _.callbacks.onPlay;
    });

    Public.__defineSetter__("onPlay", function (theFunction) {
        // TODO check to be a function
        _.callbacks.onPlay = theFunction;
    });

    Public.__defineGetter__("onCancel", function () {
        return _.callbacks.onCancel;
    });

    Public.__defineSetter__("onCancel", function (theFunction) {
        // TODO check to be a function
        _.callbacks.onCancel = theFunction;
    });

    Public.__defineGetter__("onFinish", function () {
        return _.callbacks.onFinish;
    });

    Public.__defineSetter__("onFinish", function (theFunction) {
        // TODO check to be a function
        _.callbacks.onFinish = theFunction;
    });
};