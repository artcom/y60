// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: Event.h,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/03/30 15:54:32 $
//
//
//=============================================================================

#ifndef _Y60_INPUT_EVENT_INCLUDED_
#define _Y60_INPUT_EVENT_INCLUDED_

#include <asl/Ptr.h>
#include <asl/Time.h>
#include <functional>

namespace y60 {

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
			TOUCH
        };
        Event(Type theEventType);
        Event(Type theEventType, const asl::Time & theTime);
        virtual ~Event();
        const asl::Time when;
        const Type type;
        void * source; 
    };

    typedef asl::Ptr<Event> EventPtr;


    // Functor to compare two EventPtrs chronologically
    struct isEventAfter:std::binary_function<EventPtr,EventPtr,bool> {
        bool operator()(const EventPtr & x, const EventPtr & y) const {
            return x->when > y->when;
        }
    };

}

#endif

