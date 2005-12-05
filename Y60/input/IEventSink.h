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
//   $RCSfile: IEventSink.h,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/02/01 11:18:35 $
//
//
//=============================================================================

#ifndef _Y60_INPUT_IEVENTSINK_INCLUDED_
#define _Y60_INPUT_IEVENTSINK_INCLUDED_

#include "Event.h"

#include <asl/Ptr.h>

namespace y60 {

    class IEventSink {
        public:
            virtual void handle(EventPtr theEvent) = 0;
    };
}
#endif

