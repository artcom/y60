//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Timeout.h"

namespace jslib {
   
    using namespace std;

    Timeout::Timeout() {}

    Timeout::Timeout(const string & theCommand, double theDuration,
            asl::Time theCurrentTime, bool theIsInterval)
        : _myCommand(theCommand),
          _myDuration(theDuration),
          _isInterval(theIsInterval),
          _myActivationTime(theCurrentTime + theDuration / 1000.0)
        {
        }

    Timeout::~Timeout() {}

    const string & Timeout::getCommand() const {
        return _myCommand;
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

    unsigned long TimeoutQueue::addTimeout(const string & theCommand,
            asl::Time theCurrentTime, double theDuration, bool isInterval)
    {
        TimeoutPtr myTimeout(new Timeout(theCommand, theDuration, 
                    theCurrentTime, isInterval));
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
