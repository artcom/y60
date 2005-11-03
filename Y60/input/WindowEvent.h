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
//   $RCSfile: WindowEvent.h,v $
//   $Author: janbo $
//   $Revision: 1.3 $
//   $Date: 2004/10/18 16:22:57 $
//
//
//=============================================================================

#ifndef _Y60_INPUT_WINDOWEVENT_INCLUDED_
#define _Y60_INPUT_WINDOWEVENT_INCLUDED_

#include "Event.h"

namespace y60 {

    struct WindowEvent : public Event {
        WindowEvent(Type theEventType,
                int theWidth,
                int theHeight);
        WindowEvent(Type theEventType,
                bool theFullScreenFlag);
        virtual ~WindowEvent();
        const bool isFullScreen; 
        const int width;
        const int height;

        virtual EventPtr copy() const {
            return EventPtr(new WindowEvent(*this));
        }
    };
    typedef asl::Ptr<WindowEvent> WindowEventPtr;
}

#endif

