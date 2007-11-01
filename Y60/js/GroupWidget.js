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
// button group widget.
// 
// For a definition of what widgets are, see WidgetFactory.js and widgetBase.js.
//
// This widget class represents a button group
//
// On startup, it tries to register with every Button in the "members" array.
// When a registered Button is clicked, all other members are sent a "setPressed(false)"-call.
//
// More generally, this widget is a group of multicast notification clients. 
// Every JSObject that has a notion of "groups" can join this group.
// A nice builtin feature is that you overlapping groups can easily be handled:
//
// For example, if ButtonGroup A and ButtonGroup B have two overlapping members x and y,
// Then, you can deselect all Members of B that are not contained in A by writing:
//
//                              B.deactivateBut(A.members);
//
// The group can also be used to broadcast arbitrary function calls.
// See dispatch() and dispachFun() for details.
//
// Properties:
// 
// members:    an array of buttons.
//
// Functions:
//
// - deactivate():              calls setPressed(false) on all entries of "members"-array.
//
// - deactivateBut(theWidget):  calls setPressed(false) on all entries of "members"-array.
//                              that are not equal to theWidget.
//
//
// - dispatch(theFunctionName, theArguments, theException):
//                             for every member of members, calls member.theFunction(theArguments)
//                             except for any member of theException or the theException itself.
//
// - dispatchFun(theFunction, theArguments, theException):
//                             applies theFunction to every member not contained in or equal to theException.
//
// Notifications:
// on
//
//=============================================================================

function GroupWidget() {
    this.Constructor(this);
}

GroupWidget.prototype.Constructor = function(Public) {

    //////////////////////////////////////////////////////////////////////
    // Baseclass construction
    //////////////////////////////////////////////////////////////////////

    Public.deactivateBut = function(theClickedWidget) {
        for (item in Public.members) {
            if (theClickedWidget != Public.members[item] && "setPressed" in Public.members[item]) {
                Public.members[item].setPressed(false);
            }
        }
    }

    Public.deactivate = function() {
        Public.widgetClicked(undefined);
    }


    Public.finish = function() {
	for (item in Public.members) {
            var myButton = Public.members[item];
            if ("groups" in myButton) {
                myButton.groups.push(Public);
            }
        }
    }

    Public.dispatch = function(theFunctionString, theArguments, theExceptions) {
        for (item in Public.members) {
	    if (!(Public[item] == theExceptions) && !(Public[item] in theExceptions)) {
		Public[item][theFunctionString](theArguments);
	    }
        }
    }

    // this is untested.
    Public.dispatchFun = function(theFunction, theArguments, theExceptions) {
        for (item in Public.members) {
	    if (!(Public[item] == theExceptions) && !(Public[item] in theExceptions)) {
		theFunction.apply(Public[item], theArguments);
	    }
        }
    }
    Public.setup = function() {
    }
}
