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
//   $RCSfile: AxisEvent.cpp,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#include "AxisEvent.h"

#include <dom/Nodes.h>

namespace y60 {

    AxisEvent::AxisEvent(int myDevice, int myAxis, int myValue)
        : Event (AXIS),
        device (myDevice),
        axis (myAxis),
        value (myValue)
    {}

    AxisEvent::AxisEvent(const dom::NodePtr & theNode)
        : Event(AXIS, theNode),
        device(theNode->getAttributeValue<int>("device")),
        axis(theNode->getAttributeValue<int>("axis")),
        value(theNode->getAttributeValue<int>("value"))
    {
    }

    AxisEvent::~AxisEvent() {
    }

    dom::NodePtr AxisEvent::asNode() const {
        dom::NodePtr myNode = Event::asNode();
        myNode->appendAttribute("device", device);
        myNode->appendAttribute("axis", axis);
        myNode->appendAttribute("value", value);

        return myNode;
    }
}
