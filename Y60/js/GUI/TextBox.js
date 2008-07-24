//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

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

    Public.onTextChange getter = function() {
        return _myOnTextChange;
    }

    Public.onTextChange setter = function(theHandler) {
        _myOnTextChange = theHandler;
    }

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
        Logger.info("Setting up textbox " + _myName);
        
        Base.setup();
    }  
    
    Protected.setup();
}
