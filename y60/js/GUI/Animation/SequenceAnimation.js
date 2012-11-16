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
/*globals GUI*/
/**
 * Play several animations in sequence.
 */
GUI.SequenceAnimation = function SequenceAnimation() {
    this.Constructor(this, {});
};

GUI.SequenceAnimation.prototype.__proto__ = GUI.CompositeAnimation.prototype;
GUI.SequenceAnimation.prototype.Constructor = function (Public, Protected) {
    var Base = {};
    var _ = {};

    GUI.CompositeAnimation.Constructor(Public, Protected);

    /////////////////////
    // Private Members //
    /////////////////////

    _.current = 0;

    ////////////////////
    // Public Methods //
    ////////////////////

    // duration = sum(map(children, duration))
    Base.childDurationChanged = Public.childDurationChanged;
    Public.childDurationChanged = function () {
        var i;
        var d = 0.0;
        for (i = 0; i < Public.children.length; i++) {
            d += Public.children[i].duration;
        }
        Protected.duration = d;
    };

    // start to play this animation
    // plays the first animation
    Base.play = Public.play;
    Public.play = function (theComeToAnEndFlag) {
        Base.play();
        if (!theComeToAnEndFlag && Public.running) {
            _.current = 0;
            if (Public.children.length >= 1) {
                Public.children[_.current].play();
            }
        }
    };

    // iterate through child animations
    Public.doFrame = function (theTime) {
        if (_.current >= Public.children.length) {
            Protected.finish();
            return;
        }
        Public.children[_.current].doFrame(theTime);
        if (!Public.children[_.current].running && Public.running) {
            _.current++;
            if (_.current < Public.children.length) {
                Public.children[_.current].play();
            } else {
                Protected.finish();
            }
        }
    };

    Public.toString = function () {
        return Protected.standardToString("SequenceAnimation");
    };
};