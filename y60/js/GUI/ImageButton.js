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

function ImageButton(theParent, theImageButtonNode, theDepth) {
    this.Constructor(this, theParent, theImageButtonNode, theDepth);
}

ImageButton.prototype.Constructor = function(Public, theParent, theImageButtonNode, theDepth) {

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var Base = {};

    var _myName   = theImageButtonNode.name;

    var _myButtonName = ("button" in theImageButtonNode)?theImageButtonNode.button:theImageButtonNode.name;
    var _myNode   = theImageButtonNode;
    var _myParent = theParent;

    var _myImageNormal      = null;
    var _myImageHighlighted = null;

    var _myEnabled   = true;
    var _myHighlight = false;

    Image.prototype.Constructor(Public, theParent, theImageButtonNode, theDepth);

    Public.__defineGetter__("enabled", function() {
        return _myEnabled;
    });

    Public.__defineSetter__("enabled", function(theValue) {
        _myEnabled = theValue;
        Public.insensible = !theValue;
        Public.visible = theValue;
    });

    Public.__defineGetter__("buttonname", function() {
        return _myButtonName;
    });

    Public.__defineGetter__("highlight", function() {
        return _myHighlight;
    });

    Public.__defineSetter__("highlight", function(theHighlight) {
        _myHighlight = theHighlight;
        if(theHighlight && _myImageHighlighted) {
            Public.image = _myImageHighlighted;
        } else {
            Public.image = _myImageNormal;
        }
    });

    Public.__defineGetter__("imageNormal", function() {
        return _myImageNormal;
    });

    Public.__defineGetter__("imageHighlighted", function() {
        return _myImageHighlighted;
    });

    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////

    Base.setup = Public.setup;
    Public.setup = function() {
        Logger.info("Setting up image button " + _myName);

        Public.insensible = false;

        registerBodyOwner(Public, Public.body);

        _myImageNormal = Public.image;

        if("highlightImage" in _myNode) {
            var myHighlightImage = _myNode.highlightImage;
            _myImageHighlighted = getCachedImage(CONTENT + "/" + myHighlightImage);
            _myImageHighlighted.resize = "none";
        } else {
            _myImageHighlighted = _myImageNormal;
        }

        ourButtons.push(Public);
    }

    Public.setup();
}
