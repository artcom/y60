//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
// widget base class.
//
// this is the base class for widgets.
// for a definition of what widgets are, see "WidgetFactory.js".
//
// this class defines sensible default event handlers for widgets
// propagating the calls to all children.
//
//=============================================================================

use("WidgetFactory.js");

function WidgetBase() {
    return this.Constructor(this);
}

WidgetBase.prototype.Constructor = function(Public) {

    Public.WIDGET_META = {};

    Public.toString = function() { return '[' + Public.WIDGET_META.name + ' ' + Public.WIDGET_META.className + "]"; };

    //  Will be called first in renderloop, has the time since application start
    Public.onFrame = function(theTime) {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onFrame" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onFrame(theTime);
		}
	}
    }

    // Will be called before rendering the frame
    Public.onPreRender = function() {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onPreRender" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onPreRender();
		}
	}
    }

    // Will be called after rendering the frame, but before swap buffer
    Public.onPostRender = function() {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onPostRender" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onPostRender();
		}
	}
    }

    // multilanguage text.
    //
    Public.onLanguageChange = function(theLanguage) {
	Logger.info(Public.WIDGET_META.name + ": onLanguageChange!");
        if (("captions" in Public) && Public.captions && (theLanguage in Public.captions)) {
	    Public.text = Public.captions[theLanguage];
	}
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onLanguageChange" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onLanguageChange(theLanguage);
		}
	}
    }

    // Will be called on a mouse button
    Public.onMouseButton = function(theButton, theState, theX, theY) {
	Logger.trace(Public.WIDGET_META.name + "::onMouseButton()");
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		Logger.trace("forwarding to " + Public.WIDGET_META.children[i].WIDGET_META.name);
		if ("onMouseButton" in Public.WIDGET_META.children[i]) {
			if (Public.WIDGET_META.children[i].onMouseButton(theButton, theState, theX, theY)) {
				break;
			}
		}
	}
    }

    // Will be called on a mouse move
    Public.onMouseMotion = function(theX, theY) {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onMouseMotion" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onMouseMotion(theX, theY);
		}
	}
    }

    // Will be called on a keyboard event
    Public.onKey = function(theKey, theKeyState, theX, theY,
                         theShiftFlag, theControlFlag, theAltFlag)
    {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onKey" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onKey(theKey, theKeyState, theX, theY,
        	                 theShiftFlag, theControlFlag, theAltFlag);
		}
	}
    }

    // Will be called on a mouse wheel event
    Public.onMouseWheel = function(theDeltaX, theDeltaY) {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onMouseWheel" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onMouseWheel(theDeltaX, theDeltaY);
		}
	}
    }

    // Will be called on a joystick axis event
    Public.onAxis = function(device, axis, value) {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onAxis" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onAxis(device, axis, value);
		}
	}
    }

    // Will be called on a joystick button event
    Public.onButton = function(theDevice, theButton, theState) {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onButton" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onButton(theDevice, theButton, theState);
		}
	}
    }

    // Will be called on a window reshape event
    Public.onResize = function(theNewWidth, theNewHeight) {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onResize" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onResize(theNewWidth, theNewHeight);
		}
	}
    }

    // Will be called before exit
    Public.onExit = function() {
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
		if ("onExit" in Public.WIDGET_META.children[i]) {
			Public.WIDGET_META.children[i].onExit();
		}
	}
    }

    Public.setup = function() {};

/*
    Public.bindFunction = function(theName, theArguments, theFunctionBody) {
	this[myNewFunction] = new Function(theArguments, theFunctionBody);
    }
*/
}
