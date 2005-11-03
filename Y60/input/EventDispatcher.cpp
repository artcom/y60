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
//   $RCSfile: EventDispatcher.cpp,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/03/30 15:54:32 $
//
//
//=============================================================================

#include "EventDispatcher.h"
#include "Event.h"

#include <queue>

using namespace std;

namespace y60 {

    EventDispatcher::EventDispatcher() {
    }

    EventDispatcher::~EventDispatcher() {
    }

    void
    EventDispatcher::dispatch() {
        std::priority_queue<EventPtr,EventPtrList,isEventAfter> sortedEvents;

        for (unsigned i = 0; i < _myEventSources.size(); ++i) {
            EventPtrList curEvents = _myEventSources[i]->poll();
            for (unsigned j = 0; j < curEvents.size(); ++j) {
                curEvents[j]->source = &(*_myEventSources[i]);
                sortedEvents.push(curEvents[j]);
            }
        }

        while (!sortedEvents.empty()) {
            EventPtr curEvent = sortedEvents.top();
            sortedEvents.pop();
            for (unsigned i = 0; i < _myEventSinks.size(); ++i) {
                _myEventSinks[i]->handle(curEvent);
            }
        }
    }

    void
    EventDispatcher::addSource(IEventSource * theSource) {
        _myEventSources.push_back(theSource);
        theSource->init();
    }

    void
    EventDispatcher::removeSource(IEventSource * theSource) {
        _myEventSources.erase(remove(_myEventSources.begin(),
                    _myEventSources.end(), theSource));
    }

    void
    EventDispatcher::addSink(IEventSink * theSink) {
        _myEventSinks.push_back(theSink);
    }
    void
    EventDispatcher::removeSink(IEventSink * theSink) {
        _myEventSinks.erase(remove(_myEventSinks.begin(),
                    _myEventSinks.end(), theSink));
    }
}
