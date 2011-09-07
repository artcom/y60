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

var ourButtons = [];

function TextButton(theParent, theTextButtonNode, theDepth) {
    this.Constructor(this, {}, theParent, theTextButtonNode, theDepth);
}

TextButton.prototype.Constructor = function(Public, Protected, theParent, theTextButtonNode, theDepth) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var Base = {};

    var _myName   = theTextButtonNode.name;
    var _myNode   = theTextButtonNode;
    var _myParent = theParent;

    var _myBackgroundNormal      = null;
    var _myBackgroundHighlighted = null;

    var _myEnabled   = true;
    var _myHighlight = false;

    TextWidget.prototype.Constructor(Public, Protected, theParent, theTextButtonNode, theDepth);

    Public.__defineGetter__("enabled", function() {
        return _myEnabled;
    });

    Public.__defineSetter__("enabled", function(theValue) {
        _myEnabled = theValue;
        Public.insensible = !theValue;
        if(theValue) {
            Modelling.setAlpha(Public.body, 1.0);
        } else {
            Modelling.setAlpha(Public.body, BUTTON_DISABLED_ALPHA);
        }
    });

    Public.__defineGetter__("highlight", function() {
        return _myHighlight;
    });

    Public.__defineSetter__("highlight", function(theHighlight) {
        _myHighlight = theHighlight;
        if(theHighlight && _myBackgroundHighlighted) {
            Public.background = _myBackgroundHighlighted;
        } else {
            Public.background = _myBackgroundNormal;
        }
    });


    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////

    Base.setup = Protected.setup;
    Protected.setup = function() {
        Logger.info("Setting up text button " + _myName);

        Base.setup();

        Public.insensible = false;

        registerBodyOwner(Public, Public.body);

        _myBackgroundNormal = Public.background;

        if("highlightImage" in _myNode) {
            var myHighlightImage = _myNode.highlightImage;
            _myBackgroundHighlighted = getCachedImage(CONTENT + "/" + myHighlightImage);
            _myBackgroundHighlighted.resize = "none";
        } else {
            _myBackgroundHighlighted = _myBackgroundNormal;
        }

        ourButtons.push(Public);
    }

    Protected.setup();
}
