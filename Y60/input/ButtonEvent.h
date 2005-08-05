// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ButtonEvent.h,v $
//   $Author: janbo $
//   $Revision: 1.2 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#ifndef _Y60_INPUT_BUTTONEVENT_INCLUDED_
#define _Y60_INPUT_BUTTONEVENT_INCLUDED_

#include "Event.h"

namespace y60 {

    struct ButtonEvent : public Event {
        ButtonEvent(Type theEventType,
                  int myDevice,
                  int myButton);
        virtual ~ButtonEvent();
        const int device;
        const int button;
    };
    typedef asl::Ptr<ButtonEvent> ButtonEventPtr;
}

#endif

