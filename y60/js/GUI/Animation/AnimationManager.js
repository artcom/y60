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
/*globals GUI, js, Logger*/

var ourCurrentAnimationTime = -1;

/**
 * The thing that runs animations.
 *
 * Instantiate yourself one and call its
 * doFrame() method on every frame.
 */
 
// TODO introduce namespaced animatons
/*
The idea here is that animations is not just a flat array but a deep object
with namespace names as the keys.
Per default a namespace 'default' is provided.
The AnimationManager then supports cancelling animation within a namespace.
Namespace can be nested and nested member animations would be cancelled then as well.
Namespace format is '<namespace_1>.<childnamespace>'
A Namespace can contain any number of child namespaces.
Namespaces are provided as String.

Also querying would then be interesting.
*/

GUI.AnimationManager = function () {
    this.Constructor(this, {});
};

GUI.AnimationManager.prototype.Constructor = function (Public, Protected) {
    var _ = {};
    
    /////////////////////
    // Private Members //
    /////////////////////

    _.animations = [];

    /////////////////////
    // Private Methods //
    /////////////////////

    _.removeFinished = function () {
        var myAnimation, i;
        for (i = 0; i < _.animations.length; i++) {
            myAnimation = _.animations[i];
            if (!myAnimation.running) {
                _.animations.splice(i, 1);
            }
        }
    };
    
    ////////////////////
    // Public Methods //
    ////////////////////

    Public.play = function (theAnimation) { /* theNamespace */
        if (!(theAnimation instanceof GUI.Animation)) {
            Logger.warning("<AnimationManager::play> Animation given (" + theAnimation + ") is not derived from GUI.Animation -> ABORTING");
            return;
        }
        Logger.info("<AnimationManager::play> playing '" + theAnimation + "'");
        if (js.array.indexOf(_.animations, theAnimation) < 0) {
            _.animations.push(theAnimation);
        }
        theAnimation.play();
    };

    Public.isPlaying = function () {
        return _.animations.length > 0;
    };
    
    Public.doFrame = function (theTime) {
        var myAnimation, i;
        ourCurrentAnimationTime = theTime * 1000;
        for (i = 0; i < _.animations.length; i++) {
            myAnimation = _.animations[i];
            if (myAnimation.running) {
                myAnimation.doFrame(theTime * 1000);
            }
        }
        _.removeFinished();
    };
    
    Public.__defineGetter__("animationCount", function () {
        return _.animations.length;
    });
};