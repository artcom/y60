//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INPUT_MOUSEEVENT_INCLUDED_
#define _Y60_INPUT_MOUSEEVENT_INCLUDED_

#include "Event.h"

#include <asl/Vector234.h>

namespace y60 {

    struct MouseEvent : public Event {
        enum Button {
            NONE = 0,  // for mouse move events.
            // UH: 'None' is #defined by X11.
            // Warning: this is duplicated on the js side.
            LEFT = 1,
            RIGHT = 2,
            MIDDLE = 3,
            BUTTON4 = 4,
            BUTTON5 = 5
        };

        MouseEvent(Type theEventType,
                bool theLeftButtonState,
                bool theMiddleButtonState,
                bool theRightButtonState,
                int theXPosition,
                int theYPosition,
                int theXDelta,
                int theYDelta,
                Button theButton=NONE);
        MouseEvent(const dom::NodePtr & theNode);
        virtual ~MouseEvent();

        const bool leftButtonState;
        const bool middleButtonState;
        const bool rightButtonState;
        const asl::Vector2i position;
        const asl::Vector2i delta;
        const Button button; // only used in button events

        virtual EventPtr copy() const {
            return EventPtr(new MouseEvent(*this));
        }

        virtual dom::NodePtr asNode() const;

    private:
        Type getType(const dom::NodePtr & theNode) const;
    };
    typedef asl::Ptr<MouseEvent> MouseEventPtr;
}

#endif

