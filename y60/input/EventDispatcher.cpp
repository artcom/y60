/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: EventDispatcher.cpp,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/03/30 15:54:32 $
//
//
//=============================================================================

// own header
#include "EventDispatcher.h"
#include <asl/base/Logger.h>

#include "Event.h"

#include <queue>
#include <algorithm>

using namespace std;

namespace y60 {
    isEventBefore EventDispatcher::_myEventSort = isEventBefore();
        
    EventDispatcher::EventDispatcher() {
    }

    EventDispatcher::~EventDispatcher() {
    }

    void
    EventDispatcher::dispatch() {
        std::deque<EventPtr> sortedEvents;

        for (unsigned i = 0; i < _myEventSources.size(); ++i) {
            EventPtrList curEvents = _myEventSources[i]->poll();
            for (unsigned j = 0; j < curEvents.size(); ++j) {
                curEvents[j]->source = _myEventSources[i];
                sortedEvents.push_back(curEvents[j]);
            }
        }
        std::stable_sort(sortedEvents.begin(), sortedEvents.end(),_myEventSort);
        while (!sortedEvents.empty()) {
            EventPtr curEvent = sortedEvents.front();
            sortedEvents.pop_front();
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
        _myEventSources.erase(std::remove(_myEventSources.begin(), _myEventSources.end(), theSource), _myEventSources.end());
    }

    void
    EventDispatcher::addSink(IEventSink * theSink) {
        _myEventSinks.push_back(theSink);
    }
    void
    EventDispatcher::removeSink(IEventSink * theSink) {
        _myEventSinks.erase(std::remove(_myEventSinks.begin(), _myEventSinks.end(), theSink), _myEventSinks.end());
    }
}
