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
*/

/*jslint nomen:false*/
/*globals GUI*/

/**
 * Animate a javascript property on one or more objects.
 */
GUI.PropertyAnimation = function (theDuration, theEasing, theObject,
                                  theProperty, theMin, theMax) {
    this.Constructor(this, {}, theDuration, theEasing, theObject,
                               theProperty, theMin, theMax);
};

GUI.PropertyAnimation.prototype.__proto__ = GUI.SimpleAnimation.prototype;
GUI.PropertyAnimation.prototype.Constructor = function (Public, Protected, theDuration, theEasing,
                                                       theObject, theProperty, theMin, theMax) {
    var Base = {};
    var _ = {};
    
    GUI.SimpleAnimation.Constructor(Public, Protected);

    /////////////////////
    // Private Members //
    /////////////////////

    _.object = null;
    _.property = "";
    _.min = 0; // Numeric
    _.max = 0; // Numeric

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
        _.min = theMin;
        _.max = theMax;
    };

    // set the current value
    Base.render = Public.render;
    Public.render = function () {
        var object;
        var myValue = _.min + ((_.max - _.min) * Public.progress);
        if (_.object instanceof Array) {
            for (object in _.object) {
                object[_.property] = myValue;
            }
        } else {
            _.object[_.property] = myValue;
        }
    };

    Public.toString = function () {
        return Protected.standardToString("PropertyAnimation" + " on "  + _.object.name + "." + _.property);
    };
    
    Public.__defineGetter__("object", function () {
        return _.object;
    });

    Public.__defineGetter__("property", function () {
        return _.property;
    });

    Public.__defineGetter__("min", function () {
        return _.min;
    });
    Public.__defineSetter__("min", function (theValue) {
        // TODO validate theValue to be numeric
        _.min = theValue;
    });

    Public.__defineGetter__("max", function () {
        return _.max;
    });
    Public.__defineSetter__("max", function (theValue) {
        // TODO validate theValue to be numeric
        _.max = theValue;
    });

    Public.setup();
};