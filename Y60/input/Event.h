//=============================================================================
// Copyright (C) 2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INPUT_EVENT_INCLUDED_
#define _Y60_INPUT_EVENT_INCLUDED_

#include <asl/Ptr.h>
#include <asl/Time.h>
#include <functional>

#include <dom/typedefs.h>


namespace y60 {

    struct Event;
    typedef asl::Ptr<Event> EventPtr;

    struct Event {
        enum Type {
            KEY_UP,
            KEY_DOWN,
            MOUSE_MOTION,
            MOUSE_BUTTON_UP,
            MOUSE_BUTTON_DOWN,
            MOUSE_WHEEL,
            AXIS,
            BUTTON_UP,
            BUTTON_DOWN,
            RESIZE,
            QUIT,
            TOUCH,
            GENERIC
        };

        Event(Type theEventType);
        Event(Type theEventType, const asl::Time & theTime);
        Event(Type theEventType, const dom::NodePtr & theNode);

        virtual ~Event();

        const Type type;
        asl::Time when;
        void * source; 

        virtual EventPtr copy() const {
            return EventPtr(new Event(*this));
        }

        virtual dom::NodePtr asNode() const;

        // Event factory
        static EventPtr create(const dom::NodePtr & theNode);
    };

    // Functor to compare two EventPtrs chronologically
    struct isEventAfter : public std::binary_function<EventPtr,EventPtr,bool> {
        bool operator()(const EventPtr & x, const EventPtr & y) const {
            return x->when > y->when;
        }
    };
}

#endif
