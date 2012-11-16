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

#include "Timeout.h"

namespace jslib {

    using namespace std;

    Timeout::Timeout() {}

    Timeout::Timeout(const string & theCommand,
                     double theDuration,
                     asl::Time theCurrentTime,
                      bool theIsInterval,
                      JSObject * theObjectToCall)
        : _myCommand(theCommand),
        _myDuration(theDuration),
        _myActivationTime(theCurrentTime + theDuration / 1000.0),
        _isInterval(theIsInterval),
        _myObjectToCall(theObjectToCall)
    {}

    Timeout::~Timeout() {}

    const string & Timeout::getCommand() const {
        return _myCommand;
    }

    JSObject * Timeout::getObjectToCall() const {
        return _myObjectToCall;
    }

    const double Timeout::getActivationTime() const {
        return _myActivationTime;
    }

    bool Timeout::isInterval() const {
        return _isInterval;
    }

    void Timeout::resetInterval() {
        _myActivationTime += double(_myDuration) / 1000.0;
    }

    TimeoutQueue::TimeoutQueue() {}

    unsigned long TimeoutQueue::ourNextTimeoutId = 0;

    unsigned long TimeoutQueue::addTimeout(const string & theCommand,
            asl::Time theCurrentTime, double theDuration, bool isInterval, JSObject * theObjectToCall)
    {
        TimeoutPtr myTimeout(new Timeout(theCommand, theDuration,
                    theCurrentTime, isInterval, theObjectToCall));
        _myTimeoutMap[ourNextTimeoutId] = myTimeout;
        _myTimeoutQueue.insert(std::pair<double, unsigned long>(myTimeout->getActivationTime(),
                    ourNextTimeoutId));
        return ourNextTimeoutId++;
    }

    void TimeoutQueue::clearTimeout(unsigned long theId) {
        _myTimeoutMap.erase(theId);

        TimeoutMultiMap::iterator it;
        for (it = _myTimeoutQueue.begin(); it != _myTimeoutQueue.end(); ++it) {
            if (it->second == theId) {
                _myTimeoutQueue.erase(it);
                break;
            }
        }
    }

    bool TimeoutQueue::isShowTime(double theTime) {
        return (!_myTimeoutQueue.empty() &&
                theTime >= _myTimeoutQueue.begin()->first);
    }

    TimeoutPtr TimeoutQueue::popTimeout() {
        unsigned long myTimeoutId = (_myTimeoutQueue.begin())->second;
        _myTimeoutQueue.erase(_myTimeoutQueue.begin());

        TimeoutPtr myTimeout = _myTimeoutMap[myTimeoutId];

        if (myTimeout->isInterval()) {
            myTimeout->resetInterval();
            _myTimeoutQueue.insert(std::pair<double, unsigned long>(
                        myTimeout->getActivationTime(), myTimeoutId));
        } else {
            _myTimeoutMap.erase(myTimeoutId);
        }

        return myTimeout;
    }
}
