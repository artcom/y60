//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Event.h"
#include "AxisEvent.h"
#include "ButtonEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "TouchEvent.h"
#include "GenericEvent.h"

#include <dom/Nodes.h>

namespace y60 {

    Event::Event(Type theType) : type(theType), when()
    {
    };

    Event::Event(Type theType, const asl::Time & theTime) : type(theType), when(theTime) {
    }

    Event::Event(Type theType, const dom::NodePtr & theNode) :
        type(theType),
        when(theNode->getAttributeValue<double>("when", double(asl::Time())))
    {
    }

    Event::~Event() {
    }

    dom::NodePtr Event::asNode() const {

        dom::Node * myNodeP = 0;
        switch (type) {
            case AXIS:
                myNodeP = new dom::Node("<axis/>");
                break;
            case BUTTON_UP:
            case BUTTON_DOWN:
                myNodeP = new dom::Node("<button/>");
                break;
            case KEY_UP:
            case KEY_DOWN:
                myNodeP = new dom::Node("<key/>");
                break;
            case MOUSE_MOTION:
            case MOUSE_BUTTON_UP:
            case MOUSE_BUTTON_DOWN:
            case MOUSE_WHEEL:
                myNodeP = new dom::Node("<mouse/>");
                break;
            case Event::TOUCH:
                myNodeP = new dom::Node("<touch/>");
                break;
            default:
                AC_ERROR << "Unsupported event type " << type;
                break;
        }

        dom::NodePtr myNode(myNodeP);
        if (myNode) {
            myNode = myNode->firstChild();
            myNode->appendAttribute("when", (double)when);
        }

        return myNode;
    }

    EventPtr Event::create(const dom::NodePtr & theNode) {

        Event * myEventP = 0;
        const std::string myType = theNode->nodeName();
        if (myType == "axis") {
            myEventP = new AxisEvent(theNode);
        } else if (myType == "button") {
            myEventP = new ButtonEvent(theNode);
        } else if (myType == "key") {
            myEventP = new KeyEvent(theNode);
        } else if (myType == "mouse") {
            myEventP = new MouseEvent(theNode);
        } else if (myType == "touch") {
            myEventP = new TouchEvent(theNode);
        } else if (myType == "generic") {
            myEventP = new GenericEvent(theNode);
        } else {
            AC_ERROR << "Unsupported event type '" << myType << "'";
        }

        EventPtr myEvent(myEventP);
        return myEvent;
    }
}
