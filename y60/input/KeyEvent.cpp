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

// own header
#include "KeyEvent.h"
#include <asl/dom/Nodes.h>

namespace y60 {

    KeyEvent::KeyEvent(Type theEventType,
               unsigned char theScanCode,
               KeyCode theKeyCode,
               const char * theKeyString,
               unsigned int theModifiers)
        : Event(theEventType),
        scanCode(theScanCode),
        keyCode(theKeyCode),
        keyString(theKeyString),
        modifiers(theModifiers)
    {}

    KeyEvent::KeyEvent(const dom::NodePtr & theNode)
        : Event(getType(theNode->getAttributeValue<int>("state")), theNode),
        scanCode(theNode->getAttributeValue<unsigned char>("scancode")),
        keyCode(KeyCode(theNode->getAttributeValue<int>("keycode"))),
        keyString(theNode->getAttributeValue<std::string>("keystring")),
        modifiers(theNode->getAttributeValue<int>("modifiers"))
    {}

    KeyEvent::~KeyEvent() {
    }

    dom::NodePtr KeyEvent::asNode() const {
        dom::NodePtr myNode = Event::asNode();
        myNode->appendAttribute("state", (type == KEY_DOWN ? 1 : 0));
        myNode->appendAttribute("scancode", (int)scanCode);
        myNode->appendAttribute("keystring", keyString);
        myNode->appendAttribute("keycode", keyCode);
        myNode->appendAttribute("modifiers", modifiers);
        return myNode;
    }

    Event::Type KeyEvent::getType(int theState) const {
        return (theState ? KEY_DOWN : KEY_UP);
    }
}
