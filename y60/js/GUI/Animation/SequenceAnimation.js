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
 * Play several animations in sequence.
 */
GUI.SequenceAnimation = function() {
    this.Constructor(this, {});
}

GUI.SequenceAnimation.prototype.Constructor = function(Public, Protected) {
    var Base = {};

    GUI.CompositeAnimation.Constructor(Public, Protected);

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _current = 0;

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    // duration = sum(map(children, duration))
    Base.childDurationChanged = Public.childDurationChanged;
    Public.childDurationChanged = function() {
        var d = 0.0;
        for(var i = 0; i < Public.children.length; i++) {
                d += Public.children[i].duration;
            }
        Protected.duration = d;
    };

    // start to play this animation
    // plays the first animation
    Base.play = Public.play;
    Public.play = function(theComeToAnEndFlag) {
        Base.play();
        if(!theComeToAnEndFlag) {
            _current = 0;
            if(Public.children.length >= 1) {
                Public.children[_current].play();
            }
        }
    };

    // iterate through child animations
    Public.doFrame = function(theTime) {
        if (_current >= Public.children.length) {
            return;
        }
        Public.children[_current].doFrame(theTime);
        if(!Public.children[_current].running) {
            _current++;
            if(_current < Public.children.length) {
                Public.children[_current].play();
            } else {
                Public.finish();
            }
        }
    };

    Public.toString = function() {
        return Protected.standardToString("SequenceAnimation");
    };

};
