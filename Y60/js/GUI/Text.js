//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

function Text(theParent, theTextNode, theDepth) {
    this.Constructor(this, theParent, theTextNode, theDepth);
}

Text.prototype.Constructor = function(Public, theParent, theTextNode, theDepth) {
    
    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var _myName   = theTextNode.name;
    var _myNode   = theTextNode;
    var _myParent = theParent;
    var _myDepth  = theDepth;

    var _myText     = null;
    var _myStyle    = null;
    var _mySize     = null;
    var _myImage    = null;
    var _myMaterial = null;
    var _myQuad     = null;

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.text getter = function() {
        return _myText;
    }

    Public.text setter = function(theText) {
        _myText = theText;
        draw();
    }

    Public.style getter = function() {
        return _myStyle;
    }

    Public.style setter = function(theStyle) {
        _myStyle = theStyle;
        draw();
    }

    Public.bodies getter = function() {
        return [_myQuad.body];
    }

    Public.name getter = function() {
        return _myName;
    }

    Public.color setter = function(theColor) {
        _myStyle.textColor = theColor;
    }

    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////

    function draw() {
        textToImage(_myImage, _myText, _myStyle, _mySize);
    }
        
    function setup() {
        Logger.info("Setting up text " + _myName);

        if("text" in _myNode) {
            _myText = _myNode.text;
        } else {
            _myText = "";
        }

        _myStyle = fontStyleFromNode(_myNode);
        _mySize = new Vector2f(_myNode.width, _myNode.height);

        _myImage = textAsImage(_myText, _myStyle, _mySize);

        _myMaterial = Modelling.createUnlitTexturedMaterial(window.scene, _myImage);
        _myMaterial.transparent = true;

        _myQuad = createQuad(_myParent, _myName,
                             new Vector2f(_myNode.width,_myNode.height),
                             new Vector3f(_myNode.x,_myNode.y,_myDepth),
                             _myMaterial,
                             /* insensible */ true,
                             /* visible */    true);
    }
    
    setup();
}
