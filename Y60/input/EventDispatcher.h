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
//   $RCSfile: EventDispatcher.h,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2004/11/11 20:18:14 $
//
//=============================================================================
#ifndef _Y60_INPUT_EVENTDISPATCHER_INCLUDED_
#define _Y60_INPUT_EVENTDISPATCHER_INCLUDED_

#include <asl/Singleton.h>

#include "IEventSink.h"
#include "IEventSource.h"

#include <vector>

namespace y60 {

    class EventDispatcher : public asl::Singleton<EventDispatcher> {
    public:
        virtual ~EventDispatcher();
        void dispatch();
        void addSource(IEventSource * theSource);
        void removeSource(IEventSource * theSource);
        void addSink(IEventSink * theSink);
        void removeSink(IEventSink * theSink);

    protected:
        friend class asl::SingletonManager;
        EventDispatcher();

    private:
        std::vector<IEventSource*> _myEventSources;
        std::vector<IEventSink*> _myEventSinks;
    };
}

#endif

