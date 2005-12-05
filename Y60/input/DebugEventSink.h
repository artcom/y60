//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: DebugEventSink.h,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2005/02/01 11:18:35 $
//
//
//=============================================================================
#ifndef _Y60_INPUT_DEBUGEVENTSINK_INCLUDED_
#define _Y60_INPUT_DEBUGEVENTSINK_INCLUDED_

#include "Event.h"
#include "IEventSink.h"

#include <asl/Ptr.h>

namespace y60 {

    class DebugEventSink: public y60::IEventSink {
        public:
            DebugEventSink();
            virtual void handle(EventPtr theEvent);
    };

}

#endif
