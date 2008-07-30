//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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

    Public.enabled getter = function() {
        return _myEnabled;
    }

    Public.enabled setter = function(theValue) {
        _myEnabled = theValue;
        Public.insensible = !theValue;
        if(theValue) {
            Modelling.setAlpha(Public.body, 1.0);
        } else {
            Modelling.setAlpha(Public.body, BUTTON_DISABLED_ALPHA);
        }
    }

    Public.highlight getter = function() {
        return _myHighlight;
    }

    Public.highlight setter = function(theHighlight) {
        _myHighlight = theHighlight;
        if(theHighlight && _myBackgroundHighlighted) {
            Public.background = _myBackgroundHighlighted;
        } else {
            Public.background = _myBackgroundNormal;
        }
    }


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
