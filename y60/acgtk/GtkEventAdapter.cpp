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
//   $RCSfile: GtkEventAdapter.cpp,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/19 15:00:20 $
//
//
//=============================================================================

//own header
#include "GtkEventAdapter.h"

#include <y60/input/EventDispatcher.h>
#include <y60/input/MouseEvent.h>
#include <y60/input/WindowEvent.h>

using namespace asl;
using namespace y60;

std::vector<y60::EventPtr>
GtkEventAdapter::poll() {
    std::vector<y60::EventPtr> myOutgoing = _myEventQueue;
    _myEventQueue.clear();
    return myOutgoing;
}

void
GtkEventAdapter::addWindowEvent(GdkEventConfigure * theEvent) {
    EventPtr myEvent = EventPtr(new WindowEvent(Event::RESIZE, theEvent->width, theEvent->height));
    _myEventQueue.push_back(myEvent);
    y60::EventDispatcher::get().dispatch();
}

void
GtkEventAdapter::addMouseScrollEvent(GdkEventScroll * theEvent) {
    int deltaX = 0;
    int deltaY = 0;
    switch (theEvent->direction) {
        case GDK_SCROLL_UP:
            deltaY = -1;
            break;
        case GDK_SCROLL_DOWN:
            deltaY = +1;
            break;
        case GDK_SCROLL_LEFT:
            deltaX = -1;
            break;
        case GDK_SCROLL_RIGHT:
            deltaX = +1;
            break;
    }
    EventPtr myEvent = EventPtr(new MouseEvent(Event::MOUSE_WHEEL,
                0 != (theEvent->state & GDK_BUTTON1_MASK),
                0 != (theEvent->state & GDK_BUTTON2_MASK),
                0 != (theEvent->state & GDK_BUTTON3_MASK), int(theEvent->x),
            int(theEvent->y), deltaX, deltaY ));

    _myEventQueue.push_back(myEvent);
    y60::EventDispatcher::get().dispatch();
}

void
GtkEventAdapter::addMouseMotionEvent(GdkEventMotion * theEvent) {
    EventPtr myEvent = EventPtr(new MouseEvent(Event::MOUSE_MOTION,
                0 != (theEvent->state & GDK_BUTTON1_MASK),
                0 != (theEvent->state & GDK_BUTTON2_MASK),
                0 != (theEvent->state & GDK_BUTTON3_MASK), int(theEvent->x),
            int(theEvent->y), 0,0));

    _myEventQueue.push_back(myEvent);
    y60::EventDispatcher::get().dispatch();
}

void
GtkEventAdapter::addMouseButtonEvent(GdkEventButton * theEvent) {
    Event::Type myType;
    switch (theEvent->type) {
        case GDK_BUTTON_PRESS:
            myType = Event::MOUSE_BUTTON_DOWN;
            break;
        case GDK_BUTTON_RELEASE:
            myType = Event::MOUSE_BUTTON_UP;
            break;
        default:
            // ignore double- and triple clicks
            return;
    }

    MouseEvent::Button myButton;

    switch (theEvent->button) {
        case 1:
            myButton = MouseEvent::LEFT;
            break;
        case 2:
            myButton = MouseEvent::MIDDLE;
            break;
        case 3:
            myButton = MouseEvent::RIGHT;
            break;
        default:
            myButton = static_cast<MouseEvent::Button>(theEvent->button);
    }
    EventPtr myEvent = EventPtr(new MouseEvent(myType,
                0 != (theEvent->state & GDK_BUTTON1_MASK),
                0 != (theEvent->state & GDK_BUTTON2_MASK),
                0 != (theEvent->state & GDK_BUTTON3_MASK),
                int(theEvent->x),int(theEvent->y),0,0, myButton));

    _myEventQueue.push_back(myEvent);
    y60::EventDispatcher::get().dispatch();
}
