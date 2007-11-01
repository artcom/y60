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
// Button Widget.
//
// this widget represents a button. 
//
// for a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
//
// ButtonWidget inherits from LayoutWidget and exhibits all its properties and functions.
// Inherited Properties and Functions are:
//
// - text:         a string with the initial contents of this Label.
// - position:     a vector2f describing the relative position of this button to its parent.
// - size:         a vector2f describing the size of this label.
// - captions:     the "captions" property is a mapping from language id to caption texts.
// - style:        an optional style object, defining the properties of rendered text.
//                 default is DEFAULT_LABELWIDGET_STYLE.
//                 NB: the "style"-property may also be a DOM Node.
// 
// New Properties of Button:
// 
// - image:        an image that is displayed when the button is not active.
// - pressedImage: while the button is pressed down, display this image instead.
// 
// 
// onClick():      issued when a release event inside the region of the button widget
//                 after a press event inside the region of the button widget is received.
//                 if a nonzero value is returned, event propagation will be stopped.
// 
// Example:
/*
        <style id="style###" name="aButtonStyle"
               color='555555' selectedColor='FFFFFF' backgroundColor="999999" textColor='FFFFFF'
               VTextAlign="0" HTextAlign="0"
               font='FONTS/arial.ttf' fontsize='20' />

        <widget:ButtonWidget id="###" position="[100,100]" size="[100, 30]"
                text="'Sample Text"
                style="#style###"
	        image = "'TEX/button.png"
	        pressedImage="'TEX/button_pressed.png"
                captions="{'de':'Klick mich!',
                          'en':'Click me!'}"
                onClick="print('do something');return true;"/>
*/
//=============================================================================

use("LayoutWidget.js");
use("EffectWidget.js");
use("Button.js");

function ButtonWidget() {
	this.Constructor(this);
}

ButtonWidget.prototype.Constructor = function(Public) {

    EffectWidget.prototype.Constructor(Public);
    LayoutWidget.prototype.Constructor(Public);

    Public.WIDGET_META.groups = [];

    // default property values
    Public.image = "TEX/small_button_unpressed.png";
    Public.pressedImage = "TEX/small_button.png";

    // button setup

    var _myBaseSetup = Public.setup;
    Public.setup = function() {

	// Call base class before polluting my Namespace.
	_myBaseSetup();

	var Protected = {};

        // onClick will be overwritten by Button Constructor, 
	// so save the callback before calling its constructor.
        var myOnClick = undefined;
	if ("onClick" in Public) {
	    myOnClick = Public.onClick;
	}

	if (!"text" in Public) {
	    Public.text = Public.WIDGET_META.name;
	}
	// construct with default position
        DualImageTextButton.prototype.Constructor(Public, Protected, window.scene, Public.id,
                                                  [Public.image, Public.pressedImage], undefined,
                                                  Public.style, Public.text);

	if (myOnClick) {
	    
	    // inform button groups about the click event and call custom onClick.

	    Public.onClick = function() { 
		Logger.trace(Public.WIDGET_META.name + ": onClick()");
		for (groupWidgetID in Public.WIDGET_META.groups) {
		    var myGroup = Public.WIDGET_META.groups[groupWidgetID];
		    if ("deactivateBut" in myGroup) {
			myGroup.deactivateBut(Public);
		    }
		}
		return myOnClick();
	    }
	}  else {

	    // inform button groups about the click event

	    Public.onClick = function() { 
		Logger.trace(Public.WIDGET_META.name + ": onClick()");
		for (groupWidgetID in Public.WIDGET_META.groups) {
		    var myGroup = Public.WIDGET_META.groups[groupWidgetID];
		    if ("deactivateBut" in myGroup) {
			myGroup.deactivateBut(Public);
		    }
		}
		return false;
	    }
	}

	Public.intrinsicSize = [Public.width,Public.height];
    }

    var _myBaseFinish = Public.finish;
    Public.finish = function() {
	_myBaseFinish();
	Public.position = Public.getAbsolutePosition();
    }
}

