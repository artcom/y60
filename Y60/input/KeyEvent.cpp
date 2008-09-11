//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "KeyEvent.h"

#include <asl/dom/Nodes.h>

namespace y60 {

    KeyEvent::KeyEvent(Type theEventType,
               unsigned char theScanCode,
               KeyCode theKeyCode,
               const char * theKeyString,
               unsigned int theModifiers) 
        : Event(theEventType), scanCode(theScanCode), keyString(theKeyString),
          keyCode(theKeyCode), modifiers(theModifiers)
    {}

    KeyEvent::KeyEvent(const dom::NodePtr & theNode)
        : Event(getType(theNode->getAttributeValue<int>("state")), theNode),
        scanCode(theNode->getAttributeValue<unsigned char>("scancode")),
        keyString(theNode->getAttributeValue<std::string>("keystring")),
        keyCode(KeyCode(theNode->getAttributeValue<int>("keycode"))),
        modifiers(theNode->getAttributeValue<int>("modifiers"))
    {
    }

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
