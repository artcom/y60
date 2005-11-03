// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INPUT_BUTTONEVENT_INCLUDED_
#define _Y60_INPUT_BUTTONEVENT_INCLUDED_

#include "Event.h"

namespace y60 {

    struct ButtonEvent : public Event {
        ButtonEvent(Type theEventType, int myDevice, int myButton);
        ButtonEvent(const dom::NodePtr & theNode);
        virtual ~ButtonEvent();

        const int device;
        const int button;

        virtual EventPtr copy() const {
            return EventPtr(new ButtonEvent(*this));
        }

        virtual dom::NodePtr asNode() const;

    private:
        Type getType(int theState) const;
    };
    typedef asl::Ptr<ButtonEvent> ButtonEventPtr;
}

#endif

