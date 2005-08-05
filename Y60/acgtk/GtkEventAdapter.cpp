//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: GtkEventAdapter.cpp,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/11/19 15:00:20 $
//
//
//=============================================================================

#include "GtkEventAdapter.h"
#include <y60/EventDispatcher.h>
#include <y60/MouseEvent.h>
#include <y60/WindowEvent.h>

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
                theEvent->state & GDK_BUTTON1_MASK, 
                theEvent->state & GDK_BUTTON2_MASK,
                theEvent->state & GDK_BUTTON3_MASK, int(theEvent->x),
            int(theEvent->y), deltaX, deltaY ));

    _myEventQueue.push_back(myEvent);
    y60::EventDispatcher::get().dispatch();
}

void
GtkEventAdapter::addMouseMotionEvent(GdkEventMotion * theEvent) {
    EventPtr myEvent = EventPtr(new MouseEvent(Event::MOUSE_MOTION, 
                theEvent->state & GDK_BUTTON1_MASK, 
                theEvent->state & GDK_BUTTON2_MASK,
                theEvent->state & GDK_BUTTON3_MASK, int(theEvent->x),
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
                theEvent->state & GDK_BUTTON1_MASK, 
                theEvent->state & GDK_BUTTON2_MASK,
                theEvent->state & GDK_BUTTON3_MASK, 
                int(theEvent->x),int(theEvent->y),0,0, myButton));

    _myEventQueue.push_back(myEvent);
    y60::EventDispatcher::get().dispatch();
}
