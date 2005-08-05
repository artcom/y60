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
//   $RCSfile: IEventSource.h,v $
//   $Author: ulrich $
//   $Revision: 1.6 $
//   $Date: 2004/10/22 10:49:52 $
//
//=============================================================================
#ifndef _Y60_INPUT_IEVENTSOURCE_INCLUDED_
#define _Y60_INPUT_IEVENTSOURCE_INCLUDED_

#include "Event.h"
#include <asl/Ptr.h>
#include <vector>

namespace y60 {

    struct IEventSource {
			virtual ~IEventSource() {};
            virtual void init() {};
            virtual std::vector<EventPtr> poll()=0;
    };

    typedef asl::Ptr<IEventSource> IEventSourcePtr;
}

#endif
