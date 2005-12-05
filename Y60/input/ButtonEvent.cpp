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
//   $RCSfile: ButtonEvent.cpp,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#include "ButtonEvent.h"

#include <dom/Nodes.h>

namespace y60 {

    ButtonEvent::ButtonEvent(Type theEventType,
                  int myDevice,
                  int myButton)
        : Event (theEventType),
          device (myDevice),
          button (myButton)
    {}

    ButtonEvent::ButtonEvent(const dom::NodePtr & theNode)
        : Event(getType(theNode->getAttributeValue<int>("state")), theNode),
        device(theNode->getAttributeValue<int>("device", 0)),
        button(theNode->getAttributeValue<int>("button", 0))
    {
    }

    ButtonEvent::~ButtonEvent() {
    }

    dom::NodePtr ButtonEvent::asNode() const {
        dom::NodePtr myNode = Event::asNode();
        myNode->appendAttribute("device", device);
        myNode->appendAttribute("button", button);
        myNode->appendAttribute("state", (type == BUTTON_DOWN ? 1 : 0));

        return myNode;
    }

    Event::Type ButtonEvent::getType(int theState) const {
        return (theState ? BUTTON_DOWN : BUTTON_UP);
    }
}

