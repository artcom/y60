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

#ifndef _Y60_INPUT_EVENT_INCLUDED_
#define _Y60_INPUT_EVENT_INCLUDED_

#include "y60_input_settings.h"

#include <asl/base/Ptr.h>
#include <asl/base/Time.h>
#include <functional>

#include <asl/dom/typedefs.h>


namespace y60 {

    struct Event;
    typedef asl::Ptr<Event> EventPtr;

    struct Y60_INPUT_DECL Event {
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
        double simulation_time;
        void * source;

        virtual EventPtr copy() const {
            return EventPtr(new Event(*this));
        }

        virtual dom::NodePtr asNode() const;

        // Event factory
        static EventPtr create(const dom::NodePtr & theNode);
    };

    // Functor to compare two EventPtrs chronologically
    struct isEventBefore : public std::binary_function<EventPtr,EventPtr,bool> {
        bool operator()(const EventPtr & x, const EventPtr & y) const {
            return x->when < y->when;
        }
    };
}

#endif
