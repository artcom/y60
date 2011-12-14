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

/*jslint nomen:false*/
/*globals GUI, Quaternionf, Vector3f*/

/**
 * Rotate an object using degrees as unit, normalizing the set angles.
 */
GUI.QuaternionAnimation = function (theDuration, theEasing, theObject, theProperty, theStart, theEnd) {
    this.Constructor(this, {}, theDuration, theEasing, theObject, theProperty, theStart, theEnd);
};

GUI.QuaternionAnimation.prototype.__proto__ = GUI.SimpleAnimation.prototype;
GUI.QuaternionAnimation.prototype.Constructor = function (Public, Protected, theDuration, theEasing,
                                                       theObject, theProperty, theStart, theEnd) {
    var Base = {};
    var _ = {};
    
    GUI.SimpleAnimation.Constructor(Public, Protected);

    /////////////////////
    // Private Members //
    /////////////////////

    _.object = theObject;
    _.property = "";
    _.start = 0;
    _.end = Quaternionf.createFromEuler(new Vector3f(0, 0, 0));

    ////////////////////
    // Public Methods //
    ////////////////////

    // initialize from arguments
    Public.setup = function () {
        Protected.duration = theDuration;
        if (theEasing) {
            Public.easing = theEasing;
        }
        _.object = theObject;
        _.property = theProperty;
        _.start = new Quaternionf(theStart); // take a copy, just in case the user
                                            // supplied a refernce? [DS]
        _.end = theEnd;
    };

    // set the current value
    Base.render = Public.render;
    Public.render = function () {
        var myValue = Quaternionf.slerp(_.start, _.end, Public.progress);
        _.object[_.property] = myValue;
    };

    Public.toString = function () {
        return Protected.standardToString("QuaternionAnimation" + " on "  + _.object.name + "." + _.property);
    };

    Public.__defineGetter__("object", function () {
        return _.object;
    });

    Public.__defineGetter__("property", function () {
        return _.property;
    });

    Public.__defineGetter__("start", function () {
        return _.start;
    });

    Public.__defineGetter__("end", function () {
        return _.end;
    });

    Public.setup();
};