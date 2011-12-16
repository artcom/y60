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

/*jslint nomen:false, plusplus:false, forin:true*/
/*globals GUI, js, Logger*/

var ourCurrentAnimationTime = -1;

/**
 * The thing that runs animations.
 *
 * Instantiate yourself one and call its
 * doFrame() method on every frame.
 */
GUI.AnimationManager = function () {
    this.Constructor(this, {});
};

GUI.AnimationManager.prototype.Constructor = function (Public, Protected) {
    var _ = {};
    
    ///////////////////
    // Inner Classes //
    ///////////////////
    
    _.Namespace = function Namespace(theName, theParent) {
        this.name = theName;
        this.parent = theParent;
        this.animations = [];
        this.subNamespaces = {};
    };
    
    _.Namespace.prototype.toString = function () {
        return "Namespace with name: '" + this.name + "' - animations: " + this.animations.length; //'[" + this.animations.join(", ") + "]'";
    };
    
    _.Namespace.prototype.forEachNamespaceDo = function (theFunction) {
        var subNamespace;
        theFunction(this);
        for (subNamespace in this.subNamespaces) {
            this.subNamespaces[subNamespace].forEachNamespaceDo(theFunction);
        }
    };
    
    _.Namespace.prototype.__defineGetter__("hasAnimations", function () {
        var hasAnimations = false;
        this.forEachNamespaceDo(function (theNamespace) {
            if (theNamespace.animations.length > 0) {
                hasAnimations = true;
            }
        });
        
        return hasAnimations;
    });
    
    /////////////////////
    // Private Members //
    /////////////////////

    _.namespaces = {
        'default' : new _.Namespace("default", null)
    };

    /////////////////////
    // Private Methods //
    /////////////////////
    
    _.getNamespaceParts = function (theNamespace) {
        if (theNamespace.parent !== null) {
            return _.getNamespaceParts(theNamespace.parent) + "." + theNamespace.name;
        } else {
            return theNamespace.name;
        }
    };
    
    _.prepareNamespace = function (theNamespace) {
        var myNamespaceSegments = theNamespace.split(".");
        if (myNamespaceSegments.length > 0 &&
            !(myNamespaceSegments[0] in _.namespaces)) {
            _.namespaces[myNamespaceSegments[0]] = new _.Namespace(myNamespaceSegments[0], null);
        }
        
        var i;
        var currentNamespace = _.namespaces[myNamespaceSegments[0]];
        if (myNamespaceSegments.length > 1) {
            for (i = 1; i < myNamespaceSegments.length; i++) {
                if (!(myNamespaceSegments[i] in currentNamespace.subNamespaces)) {
                    currentNamespace.subNamespaces[myNamespaceSegments[i]] = new _.Namespace(myNamespaceSegments[i], currentNamespace);
                }
                currentNamespace = currentNamespace.subNamespaces[myNamespaceSegments[i]];
            }
        }
        return currentNamespace;
    };
    
    ////////////////////
    // Public Methods //
    ////////////////////

    Public.play = function (theAnimation, theNamespace) {
        if (!(theAnimation instanceof GUI.Animation)) {
            Logger.warning("<AnimationManager::play> Animation given (" + theAnimation + ") is not derived from GUI.Animation -> ABORTING");
            return;
        }
        if (theAnimation.running) {
            Logger.warning("<AnimationManager::play> The animation '" + theAnimation + "' is already playing (potentially in the context of another AnimationManager) -> ABORTING");
            return;
        }
        theNamespace = theNamespace || 'default';
        
        var myNamespace = _.prepareNamespace(theNamespace);
        var namespace;
        var isAlreadyPlaying = false;
        
        var checkIsAlreadyPlayingFunc = function (theCurrentNamespace) {
            if (js.array.indexOf(theCurrentNamespace.animations, theAnimation) > -1) {
                Logger.warning("<AnimationManager::play> The animation: '" + theAnimation + "' is already playing in namespace: '" + theCurrentNamespace + "'");
                isAlreadyPlaying = true;
            }
        };
        
        for (namespace in _.namespaces) {
            _.namespaces[namespace].forEachNamespaceDo(checkIsAlreadyPlayingFunc);
            if (isAlreadyPlaying) {
                return;
            }
        }
        Logger.info("<AnimationManager::play> playing '" + theAnimation + "' in namespace: '" + _.getNamespaceParts(myNamespace) + "'");
        myNamespace.animations.push(theAnimation);
        theAnimation.play();
    };

    Public.isPlaying = function () {
        return Public.animationCount > 0;
    };
    
    Public.doFrame = function (theTime) {
        ourCurrentAnimationTime = theTime * 1000;
        
        var onFramePerNamespaceFunc = function (theNamespace) {
            var myAnimation, i;
            for (i = 0; i < theNamespace.animations.length; i++) {
                myAnimation = theNamespace.animations[i];
                if (myAnimation.running) {
                    myAnimation.doFrame(theTime * 1000);
                } else {
                    Logger.info("<AnimationManager::doFrame> removing animation '" + myAnimation + "' from namespace: '" + _.getNamespaceParts(theNamespace) + "' since it is done playing!");
                    theNamespace.animations.splice(i, 1);
                    i--; // We mutate the array as we walk it and have to mitigate this.
                }
            }
        };
        
        var namespace;
        for (namespace in _.namespaces) {
            _.namespaces[namespace].forEachNamespaceDo(onFramePerNamespaceFunc);
        }
    };
    
    Public.__defineGetter__("animationCount", function () {
        var namespace;
        var animationsCount = 0;
        
        var animationCountingFunc = function (theNamespace) {
            animationsCount += theNamespace.animations.length;
        };
        
        for (namespace in _.namespaces) {
            _.namespaces[namespace].forEachNamespaceDo(animationCountingFunc);
        }
        return animationsCount;
    });
    
    // Additional Public Interface for performing tasks on animations via namespaces.
    
    Public.getAllAnimationsForNamespace = function (theNamespaceString) {
        var myNamespace = Public.getNamespace(theNamespaceString);
        if (myNamespace) {
            return myNamespace.animations;
        } else {
            Logger.warning("<AnimationManager::getAllAnimationsForNamespace> Namespace '" + theNamespaceString + "' does not exist - returning empty array.");
            return [];
        }
    };
    
    Public.cancelAllAnimationsForNamespace = function (theNamespaceString, theDoNotCancelAnimation) {
        Logger.info("<AnimationManager::cancelAllAnimationsForNamespace> Cancelling all animations for namespace: '" + theNamespaceString + "'");
        var myNamespace = Public.getNamespace(theNamespaceString);
        var animationsCancelled = 0;
        if (myNamespace) {
            myNamespace.forEachNamespaceDo(function (theNamespace) {
                var myAnimation, i;
                for (i = 0; i < theNamespace.animations.length; i++) {
                    if (theNamespace.animations[i] !== theDoNotCancelAnimation) {        
                        theNamespace.animations[i].cancel();
                        animationsCancelled += 1;
                    }
                }
            });
        } else {
            Logger.warning("<AnimationManager::cancelAllAnimationsForNamespace> Namespace '" + theNamespaceString + "' does not exist.");
        }
        return animationsCancelled;
    };
    
    Public.getNamespace = function (theNamespaceString) {
        var myNamespaceSegments = theNamespaceString.split(".");
        if (myNamespaceSegments.length > 0 &&
            !(myNamespaceSegments[0] in _.namespaces)) {
            return null;
        }
        var i;
        var currentNamespace = _.namespaces[myNamespaceSegments[0]];
        if (myNamespaceSegments.length > 1) {
            for (i = 1; i < myNamespaceSegments.length; i++) {
                if (myNamespaceSegments[i] in currentNamespace.subNamespaces) {
                    currentNamespace = currentNamespace.subNamespaces[myNamespaceSegments[i]];
                } else {
                    currentNamespace = null;
                    return currentNamespace;
                }
            }
        }
        
        return currentNamespace;
    };
    
    // TODO Potentially cleanup existing namespaces if they are empty after cleanup (and their subnamespaces are also empty)
};
