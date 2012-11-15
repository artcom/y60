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

function ButtonBase(theScene, theId, theSize, thePosition, theStyle, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, theId,
                     theSize, thePosition, theStyle, theParent);
};

ButtonBase.prototype.Constructor = function(Public, Protected, theScene, theId,
                    theSize, thePosition, theStyle, theParent) {

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Inheritance
    ///////////////////////////////////////////////////////////////////////////////////////////

    LabelBase.prototype.Constructor(Public, Protected, theScene,
              theSize, thePosition, theStyle, theParent);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Public
    ///////////////////////////////////////////////////////////////////////////////////////////

    Public.onClick = function(theButton) {
        //print ("Clicked " + Public.name + " - Override me!");
    }

    Public.isPressed = function(theButton) {
        return Protected.isPressed;
    }

    Public.setPressed = function(theFlag) {
    Logger.trace(Public.id + ": setPressed("+theFlag+")");
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
                Logger.trace("Unpressing button " + Public.id);
                Public.setPressed(false);
                if (Public.touches(theX, theY, theRadius)) {
                    Public.onClick(Public);
                    return true;
                }
            } else if (theState == MOUSE_DOWN && !Protected.isPressed && Public.touches(theX, theY, theRadius)) {
                Logger.trace(Public.id + ": registered mouse press at coords("+theX+","+theY+").");
                Public.setPressed(true);
            } else {
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
                    return Public.onClick(this);
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
                return Public.onClick(Public);
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
            // check visibility of parent chain recursively
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
    var Protected = {}
    this.Constructor(this, Protected, theScene, theId, theText,
                     theSize, thePosition, theStyle, theParent);
}

TextButton.prototype.Constructor = function(Public, Protected, theScene, theId, theText,
                                            theSize, thePosition, theStyle, theParent) {
    ButtonBase.prototype.Constructor(Public, Protected, theScene, theId,
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
    var Protected = {}
    this.Constructor(this, Protected, theScene, theId, theSource,
                     thePosition, theStyle, theParent);
}

ImageButton.prototype.Constructor = function(Public, Protected, theScene, theId,
                                             theSource, thePosition, theStyle, theParent) {

    ButtonBase.prototype.Constructor(Public, Protected, theScene, theId,
               [1,1], thePosition, theStyle, theParent);
    Public.setImage(theSource);
}

function DualImageButton(theScene, theId, theSources,
                         thePosition, theStyle, theParent) {

    var Protected = [];
    this.Constructor(this, Protected, theScene, theId, theSources,
                     thePosition, theStyle, theParent);
}

DualImageButton.prototype.Constructor = function(Public, Protected, theScene, theId, theSources,
                                     thePosition, theStyle, theParent) {
    var Base      = [];
    ButtonBase.prototype.Constructor(Public, Protected, theScene, theId,
               [1,1], thePosition, theStyle, theParent);
    Protected.myCachedImages = [];
    for (var myIndex = 0; myIndex < theSources.length; myIndex++) {
        var myImage = 0;
        if (typeof(theSources[myIndex]) == "object") {
            myImage = theSources[myIndex];
        } else {
            myImage = theScene.images.appendChild(new Node("<image/>").firstChild);
            myImage.resize = "pad";
            myImage.src = theSources[myIndex]
            var mySize = getImageSize(myImage);
            Public.width  = mySize.x;
            Public.height = mySize.y;
            Public.srcsize.x = 1;
            Public.srcsize.y = 1;
        }
        Protected.myCachedImages.push(myImage);

    }
    Public.setImage(Protected.myCachedImages[0]);

    Base.setPressed = Public.setPressed;
    Public.setPressed = function(theFlag) {
        if (theFlag) {
            Logger.trace("setting selected color to " + Public.style.selectedColor);
            Public.color = Public.style.selectedColor;
            //Logger.trace("setting image to " +Protected.myCachedImages[1]);
            Public.image = Protected.myCachedImages[1];

        } else {
            //Logger.trace("setUnPressed");
            Public.color = Public.style.color;
            Public.image = Protected.myCachedImages[0];
        }
        Protected.isPressed = theFlag;
    }
}

function ColorButton(theScene, theId, theSize, thePosition, theStyle, theParent) {
    var Protected = [];
    this.Constructor(this, Protected, theScene, theId, theSize, thePosition, theStyle, theParent);
};

ColorButton.prototype.Constructor = function(Public, Protected, theScene, theId,
                        theSize, thePosition, theStyle, theParent) {

    ButtonBase.prototype.Constructor(Public, Protected, theScene, theId, theSize,
               thePosition, theStyle, theParent);
};

function DualImageTextButton(theScene, theId, theSources,
                         thePosition, theStyle, theText, theParent) {
     var Protected = [];
     return this.Constructor(this, Protected, theScene, theId, theSources,
                         thePosition, theStyle, theText, theParent);
}

DualImageTextButton.prototype.Constructor = function(Public, Protected, theScene, theId, theSources,
                                                     thePosition, theStyle, theText, theParent) {

    ButtonBase.prototype.Constructor(Public, Protected, theScene, theId,
               [1,1], thePosition, theStyle, theParent);

    Protected.myCachedImages = [];
    for (var myIndex = 0; myIndex < theSources.length; myIndex++) {
        var myImage = 0;
        if (typeof(theSources[myIndex]) == "object") {
            myImage = theSources[myIndex];
            //Protected.addTexture(myImage.id);
        } else {
            myImage = theScene.images.appendChild(new Node("<image/>").firstChild);
            myImage.resize = "pad";
            myImage.src = theSources[myIndex]
            var mySize = getImageSize(myImage);
            Public.width  = mySize.x;
            Public.height = mySize.y;
            Public.srcsize.x = 1;
            Public.srcsize.y = 1;
        }
        Protected.myCachedImages.push(myImage);

    }
    Public.setActive = function(theState) {
        Public.image = Protected.myCachedImages[theState?1:0];
        Public.enabled = !theState;
    }
    Public.setImage(Protected.myCachedImages[0]);
    var myImageSize = getImageSize(Public.image);
    Public._myTextlabel = new Label(theScene, theText, myImageSize, [0,0], theStyle, Public);
    Public.setText = Public._myTextlabel.setText;

}

