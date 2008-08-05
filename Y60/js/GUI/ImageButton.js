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

function ImageButton(theParent, theImageButtonNode, theDepth) {
    this.Constructor(this, theParent, theImageButtonNode, theDepth);
}

ImageButton.prototype.Constructor = function(Public, theParent, theImageButtonNode, theDepth) {
        
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var Base = {};

    var _myName   = theImageButtonNode.name;
    var _myNode   = theImageButtonNode;
    var _myParent = theParent;

    var _myImageNormal      = null;
    var _myImageHighlighted = null;

    var _myEnabled   = true;
    var _myHighlight = false;

    Image.prototype.Constructor(Public, theParent, theImageButtonNode, theDepth);

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
        if(theHighlight && _myImageHighlighted) {
            Public.image = _myImageHighlighted;
        } else {
            Public.image = _myImageNormal;
        }
    }
    
    Public.imageNormal getter = function() {
        return _myImageNormal;
    }

    Public.imageHighlighted getter = function() {
        return _myImageHighlighted;
    }

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
