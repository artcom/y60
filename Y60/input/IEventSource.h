//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INPUT_IEVENTSOURCE_INCLUDED_
#define _Y60_INPUT_IEVENTSOURCE_INCLUDED_

#include "Event.h"
#include <asl/base/Ptr.h>
#include <vector>

namespace y60 {

    typedef std::vector<EventPtr> EventPtrList;

    struct IEventSource {
        virtual ~IEventSource() {}
        virtual void init() {}
        virtual EventPtrList poll() = 0;
    };

    typedef asl::Ptr<IEventSource> IEventSourcePtr;
}

#endif
