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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __

*/

#include "EventDispatcher.h"
#include "Event.h"

#include <asl/base/UnitTest.h>
#include <iostream>
#include <deque>
#include <algorithm>

using namespace y60;
using namespace asl;
using namespace std;

#define DB(x) x

namespace y60 {
    class TestEventSource : public IEventSource {
        public:
            TestEventSource() {
            }

            void setEvents(const deque<EventPtr> & theEvents) {
                _myTestEvents = theEvents;
            }

            virtual vector<EventPtr> poll() {
                vector<EventPtr> curEvents;
                while (!_myTestEvents.empty()) {
                    EventPtr myNextEvent = _myTestEvents.front();
                    _myTestEvents.pop_front();
                    curEvents.push_back(myNextEvent);
                }
                return curEvents;
            };

            deque<EventPtr> _myTestEvents;
    };

}

class EventDispatcherTest : public UnitTest, IEventSink {

    public:
        EventDispatcher& _testDispatcher;
        TestEventSource _myTestSource;
        deque<EventPtr> _myExpectedEvents;

        EventDispatcherTest() :
            UnitTest("EventDispatcherTest"),
            _testDispatcher(EventDispatcher::get()) {
        }

        virtual void handle(EventPtr theEvent) {
            DB (cerr << "Handling " << theEvent->when
                    << ", should be " << _myExpectedEvents.front()->when << endl);
            ENSURE( theEvent->when == _myExpectedEvents.front()->when);
            ENSURE( theEvent->type == _myExpectedEvents.front()->type);
            _myExpectedEvents.pop_front();
        }

        void startDispatchTest(vector<EventPtr> & theTestEvents) {
            deque<EventPtr> myEventQueue(theTestEvents.begin(),theTestEvents.end());
            _myTestSource.setEvents(myEventQueue);

            _testDispatcher.dispatch();
            
            ENSURE(_myExpectedEvents.empty());
            ENSURE(_myTestSource._myTestEvents.empty());
        }

        void run() {
            vector<EventPtr> myTestEvents;
            
            _testDispatcher.addSource(&_myTestSource);
            _testDispatcher.addSink(this);
            // very simple test
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP)));
            _myExpectedEvents.assign(myTestEvents.begin(), myTestEvents.end());
            startDispatchTest(myTestEvents);
            // simple presorted test
            myTestEvents.clear();
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,1)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,2)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,3)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,4)));
            _myExpectedEvents.assign(myTestEvents.begin(), myTestEvents.end());
            startDispatchTest(myTestEvents);
            // simple unsorted test
            myTestEvents.clear();
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,3)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,1)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,4)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,2)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP,1)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP,2)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP,3)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP,4)));
            startDispatchTest(myTestEvents);
            //test sorting of events with same timestamp
            myTestEvents.clear();
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            myTestEvents.push_back(EventPtr(new Event(Event::MOUSE_MOTION, 2)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP, 1)));
                
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP, 1)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::KEY_UP, 2)));
            _myExpectedEvents.push_back(EventPtr(new Event(Event::MOUSE_MOTION, 2)));
            startDispatchTest(myTestEvents);
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
        void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new EventDispatcherTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

    try {
        mySuite.run();
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
