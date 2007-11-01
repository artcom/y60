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
//
// This widget represents a Label.
// A LabelWidget displays a piece of text and may be used in a layout hierarchy.
//
// For a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
//
// Label Properties:
// 
// - position:     a vector2f describing the relative position of this button to its parent.
//
// - size:         a vector2f describing the size of this label.
//
// Functions:
// 
// fade: an animation effect. this function accepts one parameter between 0 and 1, indicating
//                 the new alpha value. if a second nonzero parameter is specified,
//                 the first parameter is ignored and the second one is interpreted as
//                 an offset to the current alpha value. in that case, returns true
//                 if finished.
// 
// move: an animation effect. this function accepts a parameter between 0 and 1, indicating
//                 an interpolation value between the widgets position and a target point.
//                 the target point is stored in move.to. the semantics of the second
//                 parameter is analogous to the effect above.
// 
// getAbsolutePosition():
//
//                 calculates the absolute position of this label by traversing the parent
//                 chain and summing up the positions of all parents.
// 
/* Example Tag:

    <style id="labelStyle" name="labelStyle"
        color='555555' selectedColor='000000' backgroundColor="999999" textColor='FFFFFF'
        VTextAlign="0" HTextAlign="0"
        font='FONTS/acswisslight.ttf' fontsize='24' />


 the most simple variant:

    <widget:LabelWidget size="[100,50]" style="#labelStyle" text="'blah" />


 a completely specified label, with internationalization and explicit positioning:

    <widget:LabelWidget id="myContent" position="[100, 200]" size="[500,100]"
                captions="{'de':'Ich bin kein Testfall.',
                           'en':'I am not a testcase.'}"
                style="../preceding::style[@name='labelStyle']"/>

*/
// 
//=============================================================================

use("LayoutWidget.js");
use("Label.js");

function LabelWidget() {
	this.Constructor(this);
}

LabelWidget.prototype.Constructor = function(Public) {

    EffectWidget.prototype.Constructor(Public);
    LayoutWidget.prototype.Constructor(Public);

    // required by "fade" transition from EffectWidget
    Public.alpha setter = function(theAlpha) {
	Public.node.alpha = theAlpha
    }

    var _myBaseSetup = Public.setup;
    Public.setup = function() {

	_myBaseSetup();

	// construct the geometry early, when we have no layout yet.
	// We provide a default position and update later.
	var Protected = {};
	if (!("text" in Public)) {
		Logger.warning("Created Label " + Public.WIDGET_META.name + " without text!");
		Public.text = Public.WIDGET_META.name;
	}
	Label.prototype.Constructor(Public, Protected, window.scene, Public.text, ("size" in Public && Public.size)?Public.size:undefined,
				    undefined, Public.style);

	Public.intrinsicSize = [Public.width,Public.height];
    }

    var _myBaseFinish = Public.finish;
    Public.finish = function() {

	_myBaseFinish();

	// now that layout has been calculated, update the overlay's size.
	Public.position = Public.getAbsolutePosition();
    }
}
