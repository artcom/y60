// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: AxisEvent.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#ifndef _Y60_INPUT_AXISEVENT_INCLUDED_
#define _Y60_INPUT_AXISEVENT_INCLUDED_

#include "Event.h"

namespace y60 {

    struct AxisEvent : public Event {
        AxisEvent(int myDevice,
                  int myAxis,
                  int myValue);
        AxisEvent(const dom::NodePtr & theNode);
        virtual ~AxisEvent();

        const int device;
        const int axis;
        const int value;   //Range: -32767 to 32767

        virtual EventPtr copy() const {
            return EventPtr(new AxisEvent(*this));
        }

        virtual dom::NodePtr asNode() const;
    };
    typedef asl::Ptr<AxisEvent> AxisEventPtr;
}

#endif

