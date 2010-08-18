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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "Event.h"

#include "AxisEvent.h"
#include "ButtonEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "TouchEvent.h"
#include "GenericEvent.h"

#include <asl/dom/Nodes.h>

namespace y60 {

    Event::Event(Type theType) : type(theType), when()
    {
    };

    Event::Event(Type theType, const asl::Time & theTime) : type(theType), when(theTime) {
    }

    Event::Event(Type theType, const dom::NodePtr & theNode) :
        type(theType),
        when(theNode->getAttributeValue<unsigned long long>("when", when.millis()))
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
            myNode->appendAttribute("when", (unsigned long long)when.millis() );
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
