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
//  Vector4f            disabledColor
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
            Public.color = Public.style.selectedColor;
        } else {
            Public.color = Public.style.color;
        }
        Protected.isPressed = theFlag;
    }

    Public.onMouseButton = function(theState, theX, theY, theRadius) {
        if (Public.enabled && Protected.isVisible(Public.node)) {
            if (theState == MOUSE_UP && Protected.isPressed) {
                Public.setPressed(false);
                Public.onClick(Public);
            } else if (theState == MOUSE_DOWN && !Protected.isPressed && Public.touches(theX, theY, theRadius)) {
                Public.setPressed(true);
            }
        }
    }

    Public.enabled getter = function() {
        return Protected.enabled;
    }

    Public.enabled setter = function(theFlag) {
        if (Protected.enabled != theFlag) {
            Protected.enabled = theFlag;
            if (!theFlag && "disabledColor" in Public.style) {
                Public.color = Public.style.disabledColor;
            } else {
                Public.color = Public.style.color;
            }
        }
    }

    Public.setToggleGroup = function(theButtons) {
        // Replace the onMouseButton function with something more advanced
        Public.onMouseButton = function(theState, theX, theY, theRadius) {
            if (Public.enabled && Protected.isVisible(Public.node) && Public.touches(theX, theY, theRadius)) {
                if (theState == MOUSE_UP && Protected.isPressed) {
                    Public.onClick(this);
                } else  if (theState == MOUSE_DOWN && !Protected.isPressed) {
                    for (var i = 0; i < theButtons.length; ++i) {
                        if (theButtons[i].enabled && theButtons[i].isPressed()) {
                            theButtons[i].setPressed(false);
                        }
                    }
                    Public.setPressed(true);
                }
            }
        }
    }

    Public.setClickOnMouseDown = function() {
        // Replace the onMouseButton function with something more advanced
        Public.onMouseButton = function(theState, theX, theY, theRadius) {
            if (theState == MOUSE_UP) {
                Public.setPressed(false);
            } else if (theState == MOUSE_DOWN && Public.enabled && Protected.isVisible(Public.node) &&
                       !Protected.isPressed && Public.touches(theX, theY, theRadius))
            {
                Public.setPressed(true);
                Public.onClick(Public);
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Protected
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.name         = theId;
    Public.id           = theId;
    Protected.isPressed = false;
    Protected.enabled   = true;

    Protected.isVisible = function(theNode) {
        if (!theNode || theNode.nodeName != "overlay") {
            return true;
        } else {
            return (theNode.visible ? Protected.isVisible(theNode.parentNode) : false);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Private
    ///////////////////////////////////////////////////////////////////////////////////////////
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
            Public.color = Public.style.selectedColor;
            Public.setImage(theSources[1]);
        } else {
            Public.color = Public.style.color;
            Public.setImage(theSources[0]);
        }
        Protected.isPressed = theFlag;
    }
}

function ColorButton(theScene, theId, theSize, thePosition, theStyle, theParent) {
    var Public    = this;
    var Protected = {}
    ButtonBase(Public, Protected, theScene, theId, theSize,
               thePosition, theStyle, theParent);
}
