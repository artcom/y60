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
//   $RCSfile: KeyEvent.h,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2004/11/27 10:14:52 $
//
//
//=============================================================================

#ifndef _Y60_INPUT_KEYEVENT_INCLUDED_
#define _Y60_INPUT_KEYEVENT_INCLUDED_

#include "Event.h"
#include "KeyCodes.h"

namespace y60 {

    struct KeyEvent : public Event {
        KeyEvent(Type theEventType,
                unsigned char theScanCode,
                KeyCode theKeyCode,
                const char * theKeyString,
                unsigned int theModifiers);
        KeyEvent(const dom::NodePtr & theNode);
        virtual ~KeyEvent();

        const unsigned char scanCode; 
        const KeyCode keyCode;
        const std::string keyString;
        const unsigned int modifiers;

        virtual EventPtr copy() const {
            return EventPtr(new KeyEvent(*this));
        }

        virtual dom::NodePtr asNode() const;

    private:
        Type getType(int theState) const;
    };
    typedef asl::Ptr<KeyEvent> KeyEventPtr;
}

#endif

