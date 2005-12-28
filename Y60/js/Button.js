//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////////////////
//
//  Button Style properties:
//
//  string              font          (Path to font)
//  int                 fontsize      (Size of font)
//  Vector4f            textColor
//  enum                HTextAlign    (Renderer.CENTER_ALIGNMENT | LEFT_ALIGNMENT | RIGHT_ALIGNMENT)
//  enum                VTextAlign    (Renderer.TOP_ALIGNMENT | CENTER_ALIGNMENT | BOTTOM_ALIGNMENT)
//  Vector4f            color
//  Vector4f            selectedColor
//  int                 topPad        (optional)
//  int                 bottomPad     (optional)
//  int                 rightPad      (optional)
//  int                 leftPad       (optional)
//
///////////////////////////////////////////////////////////////////////////////////////////

use("Label.js");

///////////////////////////////////////////////////////////////////////////////////////////
//
// Button Base
//
///////////////////////////////////////////////////////////////////////////////////////////

var ourButtonCounter = 0;

var MOUSE_UP   = 0;
var MOUSE_DOWN = 1;

function ButtonBase(Public, Protected, theScene, theId,
                    theSize, thePosition, theStyle, theParent)
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Inheritance
    ///////////////////////////////////////////////////////////////////////////////////////////

    LabelBase(Public, Protected, theScene,
              theSize, thePosition, theStyle, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////
    Public.onClick = function(theButton) {
        print ("Clicked " + Public.name + " - Override me!");
    }

    Public.isPressed = function(theButton) {
        return Protected.isPressed;
    }

    Public.setPressed = function(theFlag) {
        if (theFlag) {
            Public.color = Protected.style.selectedColor;
        } else {
            Public.color = Protected.style.color;
        }
        Protected.isPressed = theFlag;
    }

    Public.onMouseButton = function(theState, theX, theY) {
        if (Public.enabled && isVisible(Public.node)) {
            if (theState == MOUSE_UP && Protected.isPressed) {
                Public.setPressed(false);
                Public.onClick(Public);
            } else if (theState == MOUSE_DOWN && !Protected.isPressed && Public.touches(theX, theY)) {
                Public.setPressed(true);
            }
        }
    }

    Public.enabled = true;

    Public.setToggleGroup = function(theButtons) {
        // Replace the onMouseButton function with something more advanced
        Public.onMouseButton = function(theState, theX, theY) {
            if (Public.enabled && isVisible(Public.node) && theState == MOUSE_UP && Protected.isPressed && Public.touches(theX, theY)) {
                Public.onClick(this);
            } else  if (Public.enabled && isVisible(Public.node) && theState == MOUSE_DOWN && !Protected.isPressed && Public.touches(theX, theY)) {
                for (var i = 0; i < theButtons.length; ++i) {
                    if (theButtons[i].enabled && theButtons[i].isPressed()) {
                        theButtons[i].setPressed(false);
                    }
                }
                Public.setPressed(true);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.name         = theId;
    Public.id           = theId;
    Protected.isPressed = false;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////

    function isVisible(theNode) {
        if (!theNode || theNode.nodeName != "overlay") {
            return true;
        } else {
            return (theNode.visible ? isVisible(theNode.parentNode) : false);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// TextButton
//
///////////////////////////////////////////////////////////////////////////////////////////

function TextButton(theScene, theId, theText,
                    theSize, thePosition, theStyle, theParent)
{
    var Public    = this;
    var Protected = {}
    TextButtonBase(Public, Protected, theScene, theId, theText,
                   theSize, thePosition, theStyle, theParent);
}

function TextButtonBase(Public, Protected, theScene, theId, theText,
                        theSize, thePosition, theStyle, theParent)
{
    ButtonBase(Public, Protected, theScene, theId,
               theSize, thePosition, theStyle, theParent);
    Public.setText(theText);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// ImageButton
//
///////////////////////////////////////////////////////////////////////////////////////////

function ImageButton(theScene, theId, theSource,
                     thePosition, theStyle, theParent)
{
    var Public    = this;
    var Protected = {}
    ButtonBase(Public, Protected, theScene, theId,
               [1,1], thePosition, theStyle, theParent);
    Public.setImage(theSource);
}

function DualImageButton(theScene, theId, theSources,
                         thePosition, theStyle, theParent) {
    var Base      = {};
    var Public    = this;
    var Protected = {};
    ButtonBase(Public, Protected, theScene, theId,
               [1,1], thePosition, theStyle, theParent);
    Public.setImage(theSources[0]);

    Base.setPressed = Public.setPressed;
    Public.setPressed = function(theFlag) {
        if (theFlag) {
            Public.color = Protected.style.selectedColor;
            Public.setImage(theSources[1]);
        } else {
            Public.color = Protected.style.color;
            Public.setImage(theSources[0]);
        }
        Base.setPressed(theFlag);
    }
}

// [CH]: The only way to get the background transparent.
function TransparentTextButtonBase(Public, Protected, theScene, theId, theText, theSize, thePosition, theStyle, theParent) {
    // Create a button as child of an background overlay
    var _myBackground = new Overlay(window.scene, theStyle.color, thePosition, theSize);
    TextButtonBase(Public, Protected, theScene, theId, theText, theSize, [0, 0], theStyle, _myBackground.node);
    Public.color = new Vector4f(1,1,1,1);

    Public.setPressed = function(theFlag) {
        if (theFlag) {
            _myBackground.color = Protected.style.selectedColor;
        } else {
            _myBackground.color = Protected.style.color;
        }
        Protected.isPressed = theFlag;
    }

    Public.visible getter = function() { return _myBackground.visible; }
    Public.visible setter = function(theArgument) { _myBackground.visible = theArgument; }
    Public.alpha getter = function() { return _myBackground.alpha; }
    Public.alpha setter = function(theArgument) { _myBackground.alpha = theArgument; Public.node.alpha = theArgument; }
    Public.width getter = function() { return _myBackground.width; }
    Public.width setter = function(theArgument) { _myBackground.width = theArgument; Public.node.width = theArgument; }
    Public.height getter = function() { return _myBackground.height; }
    Public.height setter = function(theArgument) { _myBackground.height = theArgument; Public.node.height = theArgument; }
    Public.name getter = function() { return _myBackground.name; }
    Public.name setter = function(theArgument) { _myBackground.name = theArgument; }

    var BaseRemoveFromScene = Public.removeFromScene;
    Public.removeFromScene = function() {
        BaseRemoveFromScene();
        _myBackground.removeFromScene();
    }
}

function TransparentTextButton(theScene, theId, theText, theSize, thePosition, theStyle, theParent) {
    var Public    = this;
    var Protected = {};
    TransparentTextButtonBase(Public, Protected, theScene, theId, theText, theSize, thePosition, theStyle, theParent);
}
