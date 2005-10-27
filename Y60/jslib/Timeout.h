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
//   $RCSfile: Timeout.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2004/10/04 13:36:42 $
//
//
//=============================================================================

#ifndef __included_Timeout_h__
#define __included_Timeout_h__

#include <asl/Time.h>
#include <asl/Ptr.h>

#include <js/jsapi.h>

#include <string>
#include <map>
#include <queue>
#include <iostream>

namespace jslib {

static unsigned long ourNextTimeoutId = 0;

class Timeout {
    public:
        Timeout() {}

        Timeout(JSObject * theObject, const std::string & theCommand, double theDuration,
                        asl::Time theCurrentTime, bool theIsInterval = false) :
            _myJSObject(theObject),
            _myCommand(theCommand),
            _myDuration(theDuration),
            _isInterval(theIsInterval),
            _myActivationTime(theCurrentTime + theDuration / 1000.0)
        {
        }

        virtual ~Timeout() {}

        const std::string & getCommand() const {
            return _myCommand;
        }

        const double getActivationTime() const {
            return _myActivationTime;
        }

        bool isInterval() const {
            return _isInterval;
        }

        void resetInterval() {
            _myActivationTime += double(_myDuration) / 1000.0;
        }

        JSObject * getJSObject() const {
            return _myJSObject;
        }
    private:
        JSObject *       _myJSObject;
        std::string      _myCommand;
        double           _myDuration;
        double           _myActivationTime;
        bool             _isInterval;
};

typedef asl::Ptr<Timeout> TimeoutPtr;

typedef std::multimap<double, unsigned long> TimeoutMultiMap;
typedef std::map<unsigned long, TimeoutPtr>  TimeoutMap;

class TimeoutQueue {
    public:
        TimeoutQueue() {}

        unsigned long addTimeout(JSObject * theObject, const std::string & theCommand,
                asl::Time theCurrentTime, double theDuration, bool isInterval = false)
        {
            TimeoutPtr myTimeout(new Timeout(theObject, theCommand, theDuration, 
                        theCurrentTime, isInterval));
            _myTimeoutMap[ourNextTimeoutId] = myTimeout;
            _myTimeoutQueue.insert(std::pair<double, unsigned long>(myTimeout->getActivationTime(),
                ourNextTimeoutId));
            return ourNextTimeoutId++;
        }

        void clearTimeout(unsigned long theId) {
            _myTimeoutMap.erase(theId);

            TimeoutMultiMap::iterator it;
            for (it = _myTimeoutQueue.begin(); it != _myTimeoutQueue.end(); ++it) {
                if (it->second == theId) {
                    _myTimeoutQueue.erase(it);
                    break;
                }
            }
        }

        bool isShowTime(double theTime) {
            return (!_myTimeoutQueue.empty() &&
                     theTime >= _myTimeoutQueue.begin()->first);
        }

        TimeoutPtr popTimeout() {
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

    private:
        TimeoutMap      _myTimeoutMap;
        TimeoutMultiMap _myTimeoutQueue;
};

}

#endif

