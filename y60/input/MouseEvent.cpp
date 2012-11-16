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
*/
//
//   $RCSfile: MouseEvent.cpp,v $
//   $Author: janbo $
//   $Revision: 1.3 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

// own header
#include "MouseEvent.h"


#include <asl/math/Vector234.h>
#include <asl/dom/Nodes.h>

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
        leftButtonState(0 != theNode->getAttributeValue<int>("leftbuttonstate")),
        middleButtonState(0 != theNode->getAttributeValue<int>("middlebuttonstate")),
        rightButtonState(0 != theNode->getAttributeValue<int>("rightbuttonstate")),
        position(theNode->getAttributeValue<asl::Vector2i>("position")),
        delta(theNode->getAttributeValue<asl::Vector2i>("delta")),
        button(Button(theNode->getAttributeValue<int>("button")))
    {}

    MouseEvent::~MouseEvent() {}

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
