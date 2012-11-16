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

#ifndef _Y60_INPUT_MOUSEEVENT_INCLUDED_
#define _Y60_INPUT_MOUSEEVENT_INCLUDED_

#include "y60_input_settings.h"

#include "Event.h"

#include <asl/math/Vector234.h>

namespace y60 {

    struct Y60_INPUT_DECL MouseEvent : public Event {
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

        virtual EventPtr copy() const {
            return EventPtr(new MouseEvent(*this));
        }
        virtual dom::NodePtr asNode() const;

    public:
        const bool leftButtonState;
        const bool middleButtonState;
        const bool rightButtonState;
        const asl::Vector2i position;
        const asl::Vector2i delta;
        const Button button; // only used in button events



    private:
        Type getType(const dom::NodePtr & theNode) const;
    };
    typedef asl::Ptr<MouseEvent> MouseEventPtr;
}

#endif

