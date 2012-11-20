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

// own header
#include "EventRecorder.h"


#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>

#include <asl/dom/Nodes.h>

#include "Event.h"

#define DB(x) x
#define DB2(x) //x

using namespace y60;

// Event compare functor, ascending order
struct compareEvent : public std::binary_function<EventPtr,EventPtr,bool> {
    bool operator()(const EventPtr & x, const EventPtr & y) const {
        return (x->when < y->when);
    }
};

EventRecorder::EventRecorder() {
    _myMode = STOP;
}

EventRecorder::~EventRecorder() {
}

void EventRecorder::init() {
}

EventPtrList EventRecorder::poll() {

    EventPtrList myEvents;

    if (_myMode == PLAY) {
        asl::Time myTime = asl::Time() - _myStartTime;
        while (_myEventIter != _myEvents.end() && (*_myEventIter)->when <= myTime) {
            EventPtr myEvent = *(_myEventIter++);
            DB(AC_TRACE << "pop node=" << *(myEvent->asNode()));
            myEvents.push_back(myEvent);
        }

        if (_myEventIter == _myEvents.end()) {
            AC_DEBUG << "Stopping event playback";
            setMode(STOP);
        }
    }

    return myEvents;
}

void EventRecorder::handle(EventPtr theEvent) {

    if (_myMode == RECORD && filterEvent(theEvent) == false) {
        EventPtr myEvent = theEvent->copy();
        myEvent->when = asl::Time() - _myStartTime;
        DB(AC_TRACE << "push node=" << *myEvent->asNode());
        _myEvents.push_back(myEvent);
    }
}

void EventRecorder::setMode(Mode theMode, bool theDiscardFlag) {

    if (_myMode == theMode) {
        return;
    }

    // discard events?
    if (theMode == RECORD && theDiscardFlag) {
        AC_DEBUG << "Discarding events";
        _myEvents.clear();
    }

    _myMode = theMode;
    _myStartTime = asl::Time();
    _myEventIter = _myEvents.begin();

    const char * myModeNames[] = { "STOP", "PLAY", "RECORD" };
    AC_DEBUG << "EventRecorder mode=" << asl::getStringFromEnum(_myMode, myModeNames) << " events=" << _myEvents.size();
}

bool EventRecorder::load(const std::string & theFilename, bool theDiscardFlag) {

    std::string myFile = asl::readFile(theFilename);
    if (myFile.empty()) {
        AC_ERROR << "Unable to open '" << theFilename << "'";
        return false;
    }

    dom::Document myDoc;
    myDoc.parseAll(myFile.c_str());
    if (!myDoc) {
        AC_ERROR << "Unable to parse '" << theFilename << "'";
        return false;
    }

    const dom::NodePtr & myEvents = myDoc.childNode("events");
    if (!myEvents) {
        AC_ERROR << "No 'events' node found in '" << theFilename << "'";
        return false;
    }

    // load or merge
    if (theDiscardFlag) {
        AC_DEBUG << "Discarding events";
        _myEvents.clear();
    }

    // load events into list
    for (unsigned i = 0; i < myEvents->childNodesLength(); ++i) {
        const dom::NodePtr & myEventNode = myEvents->childNode(i);
        DB2(AC_TRACE << "parsed " << *myEventNode);
        EventPtr myEvent = Event::create(myEventNode);
        if (myEvent) {
            DB2(AC_TRACE << "event " << *myEvent->asNode());
            _myEvents.push_back(myEvent);
        }
    }
    AC_INFO << "Loaded " << _myEvents.size() << " events from '" << theFilename << "'";

    // sort list by time
    std::sort(_myEvents.begin(), _myEvents.end(), compareEvent());

    setMode(STOP);

    return true;
}

void EventRecorder::save(const std::string & theFilename) const {

    if (_myEvents.size() == 0) {
        AC_DEBUG << "Nothing to save";
        return;
    }

    dom::NodePtr myEventsNode = dom::NodePtr(new dom::Node("<events/>"));
    myEventsNode = myEventsNode->childNode("events");

    for (EventQueue::const_iterator it = _myEvents.begin(); it != _myEvents.end(); ++it) {
        dom::NodePtr myNode = (*it)->asNode();
        myEventsNode->appendChild(myNode);
    }

    asl::writeFile(theFilename, asl::as_string(*myEventsNode));
    AC_INFO << "Saved " << _myEvents.size() << " events to '" << theFilename << "'";
}

bool EventRecorder::filterEvent(const EventPtr & theEvent) const {

    if (theEvent->source == (void*)this) {
        return true;
    }

    return false;
}
