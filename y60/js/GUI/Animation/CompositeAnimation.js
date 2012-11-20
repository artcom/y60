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

/*jslint nomen:false, plusplus:false*/
/*globals GUI, Exception*/

/**
 * Animations that are time-compositions of their children.
 */
GUI.CompositeAnimation =  function () {
    throw new Exception("<CompositeAnimation> Abstract Base class cannot be instantiated");
};

GUI.CompositeAnimation.prototype.__proto__ = GUI.Animation.prototype;
GUI.CompositeAnimation.Constructor = function (Public, Protected) {
    var Base = {};
    var _ = {};

    GUI.Animation.Constructor(Public, Protected);

    ////////////////////
    // Private Member //
    ////////////////////

    _.children = [];

    ////////////////////
    // Public Methods //
    ////////////////////

    // add a child, also updating duration
    Public.add = function (theAnimation) {
        // TODO check if instanceof Animation
        _.children.push(theAnimation);
        theAnimation.parent = Public;
        Public.childDurationChanged();
    };
    
    Public.remove = function (theAnimation) {
        // TODO check if instanceof Animation
        var index = js.array.indexOf (_.children, theAnimation);
        _.children.splice(index,1);
        Public.childDurationChanged();
    };

    // duration computation, should be overridden
    Public.childDurationChanged = function (theChild) {
    };

    // generic cancel
    Base.cancel = Public.cancel;
    Public.cancel = function () {
        var i;
        for (i = 0; i < _.children.length; i++) {
            if (_.children[i].running) {
                _.children[i].cancel();
            }
        }
        Base.cancel();
    };

    // generic finish
    Base.finish = Public.finish;
    Public.finish = function () {
        var i;
        for (i = 0; i < _.children.length; i++) {
            _.children[i].finish();
        }
        Base.finish();
    };
    
    Public.__defineGetter__("children", function () {
        return _.children;
    });
};