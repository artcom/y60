/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_numeric_functions.tst.cpp,v $
//
//   $Revision: 1.5 $
//
// Description: Test for EventDispatcher
//
//
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
                if (!_myTestEvents.empty()) {
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
            ENSURE( theEvent == _myExpectedEvents.front());
            _myExpectedEvents.pop_front();
        }

        void startDispatchTest(vector<EventPtr> & theTestEvents) {
            deque<EventPtr> myEventQueue(theTestEvents.begin(),theTestEvents.end());
            _myTestSource.setEvents(myEventQueue);

            _myExpectedEvents.assign(theTestEvents.begin(),theTestEvents.end());

            for (int i=0; i<theTestEvents.size(); i++) {
                _testDispatcher.dispatch();
            }
            ENSURE(_myExpectedEvents.empty());
            ENSURE(_myTestSource._myTestEvents.empty());
        }

        void run() {
            vector<EventPtr> myTestEvents;

            _testDispatcher.addSource(&_myTestSource);
            _testDispatcher.addSink(this);
            // very simple test
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP)));
            startDispatchTest(myTestEvents);

            // simple presorted test
            myTestEvents.clear();
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,1)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,2)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,3)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,4)));
            startDispatchTest(myTestEvents);

            // simple unsorted test
            myTestEvents.clear();
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,3)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,1)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,4)));
            myTestEvents.push_back(EventPtr(new Event(Event::KEY_UP,2)));
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
