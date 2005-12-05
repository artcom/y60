//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: MouseEvent.cpp,v $
//   $Author: janbo $
//   $Revision: 1.3 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#include "MouseEvent.h"

#include <asl/Vector234.h>
#include <dom/Nodes.h>

namespace y60 {

    MouseEvent::MouseEvent(Type theEventType,
                            bool theLeftButtonState,
                            bool theMiddleButtonState,
                            bool theRightButtonState,
                            int theXPosition,
                            int theYPosition,
                            int theXDelta,
                            int theYDelta,
                            Button theButton)
        : Event(theEventType),
        leftButtonState(theLeftButtonState),
        middleButtonState(theMiddleButtonState),
        rightButtonState(theRightButtonState),
        position(asl::Vector2i(theXPosition,theYPosition)),
        delta(asl::Vector2i(theXDelta, theYDelta)),
        button(theButton)
    {}

    MouseEvent::MouseEvent(const dom::NodePtr & theNode)
        : Event(getType(theNode), theNode),
        button(Button(theNode->getAttributeValue<int>("button"))),
        leftButtonState(theNode->getAttributeValue<int>("leftbuttonstate")),
        middleButtonState(theNode->getAttributeValue<int>("middlebuttonstate")),
        rightButtonState(theNode->getAttributeValue<int>("rightbuttonstate")),
        position(theNode->getAttributeValue<asl::Vector2i>("position")),
        delta(theNode->getAttributeValue<asl::Vector2i>("delta"))
    {
    }
        
    MouseEvent::~MouseEvent() {
    }

    dom::NodePtr MouseEvent::asNode() const {
        dom::NodePtr myNode = Event::asNode();

        if (type == MOUSE_BUTTON_UP || type == MOUSE_BUTTON_DOWN) {
            myNode->appendAttribute("type", "button");
        } else {
            myNode->appendAttribute("type", (type == MOUSE_WHEEL ? "wheel" : "motion"));
        }
        myNode->appendAttribute("state", (type == MOUSE_BUTTON_DOWN ? 1 : 0));

        myNode->appendAttribute("button", button);
        myNode->appendAttribute("leftbuttonstate", leftButtonState);
        myNode->appendAttribute("middlebuttonstate", middleButtonState);
        myNode->appendAttribute("rightbuttonstate", rightButtonState);
        myNode->appendAttribute("position", position);
        myNode->appendAttribute("delta", delta);

        return myNode;
    }

    Event::Type MouseEvent::getType(const dom::NodePtr & theNode) const {

        const std::string myType = theNode->getAttributeValue<std::string>("type");
        if (myType == "button") {
            return (theNode->getAttributeValue<int>("state") ? MOUSE_BUTTON_DOWN : MOUSE_BUTTON_UP);
        } else if (myType == "wheel") {
            return MOUSE_WHEEL;
        } else {
            return MOUSE_MOTION;
        }
    }
}
