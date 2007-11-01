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
// Layout Widget base class.
//
// for a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
// 
// This widget represents a layout container. 
// 
// Properties:
// 
// - specifiedSize:  the size of a rectange children should be placed in. Will be taken
//                   from the "size" property during setup unless otherwise specified.
// 
// - intrinsicSize:  the size of this widget's representation, not taking child layouts into account.
//                   This is the minimum space a parent will allocate for this widget.
// 
// Values calculated during layout():
// 
// - layoutPosition: the position of this widget relative to its parent. if undefined,
//                   the parent is supposed to assigning one during its layout() run.
//                   the real position of a widget may differ from its layoutPosition
//                   e.g. during an animation sequence.
// 
// - layoutSize:     the calculated size of this widget. After layout(), this is the bounding box
//                   around all layouted children or the widget's intrinsicSize, whichever is larger.
// 
// Functions:
// 
// getAbsolutePosition(): calculates the absolute layout position of this widget w/respect
//                        to the layout root. Recursively walks up the parent chain and
//                        sums up the parents' absolute positions.
// 
// 
// layout():         layout() does a bottom-up recursive layout of a layout hierarchy.
//                   Children calculate their layoutSize during their layout-call first.
//                   By deleting their layoutPosition, they may indicate their request for
//                   a layout. The parent then assigns a position to every child lacking
//                   a valid layoutPosition.
// 
//                   This implementation does a top-aligned line layout:
//                   All children requesting layout are assigned consecutive positions with the same
//                   y value, until a child placement would exceed the layout size.
// 
//                   The next child will be placed at the beginning of the following line.
//                   The line height is height of the largest child in the previous line,
//                   regardless of whether the largest child was layouted or not.
// 
//                   Thus, children that are not assigned a position do contribute to the
//                   line height, but they do not occupy any horziontal space.
// 
//
// Style & Multilanguage text: Widgets of this class may use a "captions" array
//                             to store text in multiple languages.
//
//
// - text:      a string with the content of this widget. updated from "captions" in onLanguageChange.
//
// - captions:     the "captions" property is a mapping from language id to caption texts.
//
//                 When "onLanguageChange" is called with a string that exists
//                 in the "captions" map, the label's text is replaced for the
//                 corresponding value.
//
//                 Here's an example:
//
//                 {'de':'DeutscherText', 'en':'EnglischerText'}
//
//                 Multiple languages are supported as follows:
//
// - style:        an optional style object. style objects define the properties
//                  "font","color", "textColor", "selectedColor", "VTextAlign", "fontsize".
//                 Defaults to DEFAULT_BUTTON_STYLE, see below.
//                 NB: the "style" may also be a DOM node.
//
// Callbacks:
// 
// setText(): 
// 
//=============================================================================

var DEFAULT_WIDGET_STYLE = {
    font:              "FONTS/arial.ttf",
    textColor:         [1,0,0,1],
    color:             [1,1,1,1],
    selectedColor:     [0,1,0,1], 
    VTextAlign:        Renderer.CENTER_ALIGNMENT,
    HTextAlign:        Renderer.CENTER_ALIGNMENT,
    fontsize:          24,
}


use("WidgetBase.js");

function LayoutWidget() {
	this.Constructor(this);
}

LayoutWidget.prototype.Constructor = function(Public) {

    WidgetBase.prototype.Constructor(Public);

    // Widgets may do initialization stuff during early setup(), but they must
    // not use getAbsolutePosition() before a widget tree is completely layouted.
    //
    Public.getAbsolutePosition = function() {

        if ((!("layoutPosition" in Public)) || (Public.layoutPosition == undefined)) {
             Logger.error(Public.WIDGET_META.name + ": try to query position without a valid layout...");
        }

	if ("parent" in Public.WIDGET_META) {
	    var myParentPosition = [0,0];
	    if ("getAbsolutePosition" in Public.WIDGET_META.parent) {
		myParentPosition = Public.WIDGET_META.parent.getAbsolutePosition();
	    } else if ("layoutPosition" in Public.WIDGET_META.parent) {
		// fall back to parent's own position.
		Logger.warning(Public.WIDGET_META.name + ": parent has no absolute position");
		myParentPosition = Public.WIDGET_META.parent.layoutPosition;
	    } else { // parent has no clue about what a layout is.
		myParentPosition = [0,0]
            }
	    var myAbsolutePosition = [];

	    for (var i = 0; i < myParentPosition.length; i++) {
		myAbsolutePosition[i] = Public.layoutPosition[i] + myParentPosition[i];
	    }
	    Logger.trace(Public.WIDGET_META.name + ": myAbsolutePosition is " + myAbsolutePosition);
	    return myAbsolutePosition;
	} else {
	    Logger.trace(Public.WIDGET_META.name + ": my absolute position is " + Public.layoutPosition);
	    return clone(Public.layoutPosition);
	}
    }

    // We recursively layout a tree by first calculating the sizes bottom-up,
    // and then assigning positions to all children that need to be layouted.
    //
    // Since positions depend on their preceding siblings' sizes
    // and sizes may depend on childrens' positions, this is the only way to do it.
    // 
    Public.layout = function() {

	Logger.trace(Public.WIDGET_META.name + ".layout():");

        // 1. layout all children to have their sizes calculated.
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {
            Public.WIDGET_META.children[i].layout();
	}

	// 2. determine bounding box to place children in
	var myAvailableSize = [100000,100000];

	if ("specifiedSize" in Public && Public.specifiedSize) {
	    myAvailableSize = Public.specifiedSize;
	}

	// assign them a position
	var boundingBox = layoutChildren(myAvailableSize);

	// update my own size. 
	if ("intrinsicSize" in Public && Public.intrinsicSize) {
	    Public.layoutSize = [Math.max(boundingBox[0],Public.intrinsicSize[0]),
				 Math.max(boundingBox[1],Public.intrinsicSize[1])];
	} else {
	    Public.layoutSize = boundingBox;
	}

	Logger.trace(" --- layouting finished for " + Public.WIDGET_META.name + ". layoutsize = " + Public.layoutSize);
    }

    // assigns a position to every non-positioned child and returns a bounding box.
    //
    function layoutChildren(theAvailableSize) {

	if (Public.WIDGET_META.children.length > 0) {
	    Logger.info(Public.WIDGET_META.name + ": layouting children into canvas of " + theAvailableSize);
	}

        var myNextChildPos = [0,0];
        var myMaxHeightInThisLine = 0;
	for (var i = 0; i < Public.WIDGET_META.children.length; i++) {

            var curChild = Public.WIDGET_META.children[i];

            if (!("layoutPosition" in curChild) || !curChild.layoutPosition) {

		// curChild wants to be layouted.
		Logger.trace(" - " + curChild.WIDGET_META.name + " requests layout...");

                if ((!("layoutSize" in curChild)) || !curChild.layoutSize) {
		    Logger.fatal(curChild.WIDGET_META.name + " not implemented: layouting widgets without a size.");
		}

		// place child in this line unconditionally if it's the first in this line,
		// otherwise check if it fits into the size of this widget
                if ((myNextChildPos[0] == 0) || ((curChild.layoutSize[0] + myNextChildPos[0]) <= theAvailableSize[0])) {

		    if (curChild.layoutSize[1] > myMaxHeightInThisLine) {
			myMaxHeightInThisLine = curChild.layoutSize[1];
		    }

                    curChild.layoutPosition = clone(myNextChildPos);
		    myNextChildPos[0] += curChild.layoutSize[0];

                } else {

                    // put it at the beginning of the next line.

		    Logger.trace("    ! line break.");
		    myNextChildPos[1] += myMaxHeightInThisLine;
		    myMaxHeightInThisLine = 0;

                    curChild.layoutPosition = [0, myNextChildPos[1]];
		    myNextChildPos[0] = curChild.layoutSize[0];

		}
		Logger.trace("    myNextChildPos now = " + myNextChildPos);

		Logger.info(Public.WIDGET_META.name + ": Placed child " + curChild.WIDGET_META.name + " at " + curChild.layoutPosition);
                if (curChild.layoutSize[1] > myMaxHeightInThisLine) {
		   myMaxHeightInThisLine = curChild.layoutSize[1];
	        }
            } else {
		Logger.info(Public.WIDGET_META.name + ".layout(): skipping positioned " + curChild.WIDGET_META.name);
	    }
        }
	return [myNextChildPos[0], myNextChildPos[1] + myMaxHeightInThisLine];
    }

    // style handling: inherit styles from parent, cache locally.
    //
    var _myStyle;
    Public.style getter = function() {
	if (_myStyle) {
	    return _myStyle;
	}
	if ("parent" in Public.WIDGET_META && Public.WIDGET_META.style) {
	    _myStyle = Public.WIDGET_META.parent.style;
	    return _myStyle;
	}
	_myStyle = DEFAULT_WIDGET_STYLE;
	return DEFAULT_WIDGET_STYLE;
    }

    Public.style setter = function(theStyle) {
	// may happen during construction phase --> we may not been assigned a name yet.
	if ("name" in Public.WIDGET_META) {
		Logger.trace(Public.WIDGET_META.name + ": setting style to " + theStyle);
	} else {
	for(i in Public.WIDGET_META) {
		Logger.trace(Public.WIDGET_META[i]);
	}
        Logger.trace(": setting my individual style to " + theStyle);
	}
	_myStyle = theStyle;
    }

    // widget setup
    var myInitialPosition;
    Public.setup = function() {

	// save the initial value of the position attribute before someone defines a magic getter on it.
	// we'll re-use the value for our first layout in finish(),

	if ("position" in Public && Public.position) {
	    myInitialPosition = Public.position;
	}

	if ("size" in Public && Public.size) {
	    Public.specifiedSize = Public.size;
	}
    }

    Public.finish = function() {

	if (!("layoutPosition" in Public) || !Public.layoutPosition) {
	    if (!myInitialPosition) {

		// this may happen if the ancestor line in this widget's
		// layout subtree has its "position" property specified.
		// NB: A layout subtree is a subtree where the parent
		// of every LayoutWidget child is also a LayoutWidget.
		//
		Logger.error(Public.WIDGET_META.name + ": *** No layout position assigned");
	    }
	    Public.layoutPosition = myInitialPosition;
	    Logger.info("Layout root "+Public.WIDGET_META.name+" at " + Public.layoutPosition);
	}

	if (!("layoutSize" in Public) || (!Public.layoutSize)) {

	    // layout() goes children-first, but we must enter the procedure at
	    // the lowest possible tree level to get a valid layout.
	    //
	    // Since finish() is called on the parents first, this will do the job.
	    // 
	    Public.layout();
	    if (!("layoutSize" in Public) || !Public.layoutSize) {
		Logger.error(Public.WIDGET_META.name + ': *** Error: unable to calculate my own size! ');
		Public.layoutSize = Public.size;
	    }
	}
    }
}
