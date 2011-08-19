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

function TextBox(theParent, theTextBoxNode, theDepth) {
    this.Constructor(this, {}, theParent, theTextBoxNode, theDepth);
}

TextBox.prototype.Constructor = function(Public, Protected, theParent, theTextBoxNode, theDepth) {

    const EM = 25; // XXX: this should come from the font renderer

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var Base = {};

    var _myName   = theTextBoxNode.name;
    var _myNode   = theTextBoxNode;
    var _myParent = theParent;

    var _myCursorY      = Number(_myNode.cursorY);
    var _myCursorHeight = Number(_myNode.cursorHeight);
    var _myCursorWidth  = Number(_myNode.cursorWidth);
    var _myCursorOffset = Number(_myNode.cursorOffset);

    var _myCursorColor  = asColor(_myNode.cursorColor);

    var _myCursorPosition = null;

    var _myTextX = Number(_myNode.textX);
    var _myTextY = Number(_myNode.textY);
    var _myTextWidth = Number(_myNode.textWidth);
    var _myTextHeight = Number(_myNode.textHeight);
    var _myTextSurfaceWidth = _myTextWidth;
    if("textSurfaceWidth" in _myNode) {
        _myTextSurfaceWidth = Number(_myNode.textSurfaceWidth);
    }

    var _myOnTextChange = null;


    TextWidget.prototype.Constructor(Public, Protected, theParent, theTextBoxNode, theDepth);

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.onKey = function(theKey) {
        switch(theKey) {
        case "<--":
            removeFinalCharacter();
            break;
        case "SPACE":
            appendCharacter(" ");
            break;
        default:
            appendCharacter(theKey);
            break;
        }
    }

    Public.__defineGetter__("onTextChange", function() {
        return _myOnTextChange;
    });

    Public.__defineSetter__("onTextChange", function(theHandler) {
        _myOnTextChange = theHandler;
    });

    ////////////////////////////////////////
    // Protected
    ////////////////////////////////////////

    Base.draw = Protected.draw;
    Protected.draw = function() {
        var myContext = Protected.cairoContext;
        var myCursor  = Protected.cursorPosition;

        Base.draw();

        var myCursorX = myCursor[0];
        if(myCursorX > _myTextWidth) {
            myCursorX = _myTextWidth;
        }
        myContext.setSourceRGB(_myCursorColor[0],_myCursorColor[1],_myCursorColor[2]);
        myContext.setLineWidth(_myCursorWidth);
        myContext.moveTo(_myTextX + myCursorX + _myCursorOffset, _myCursorY);
        myContext.lineTo(_myTextX + myCursorX + _myCursorOffset, _myCursorY + _myCursorHeight);
        myContext.stroke();

        // hold on to cursor position to prevent textbox overflow
        _myCursorPosition = myCursor;
    }

    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////

    function appendCharacter(theCharacter) {
        if(_myCursorPosition) {
            if(_myTextSurfaceWidth-_myCursorPosition[0] <= 1*EM) {
                return false;
            }
        }

        Public.text += theCharacter;

        textChanged();

        return true;
    }

    function removeFinalCharacter() {
        var myOld = Public.text;
        if(myOld.length > 0) {
            Public.text = myOld.substring(0, myOld.length-1);
        }
        textChanged();
    }

    function textChanged() {
        if(_myOnTextChange) {
            _myOnTextChange(Public);
        }
    }


    ////////////////////////////////////////
    // setup
    ////////////////////////////////////////

    Base.setup = Protected.setup;
    Protected.setup = function() {
        Logger.info("Setting up text box " + _myName);

        Base.setup();
    }

    Protected.setup();
}
