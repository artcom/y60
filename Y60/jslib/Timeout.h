//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef __included_Timeout_h__
#define __included_Timeout_h__

#include <asl/Time.h>
#include <asl/Ptr.h>

#include "jssettings.h"
#include <js/jsapi.h>

#include <string>
#include <map>

namespace jslib {

static unsigned long ourNextTimeoutId = 0;

class Timeout {
    public:
        Timeout();
        Timeout(const std::string & theCommand, double theDuration,
                asl::Time theCurrentTime, bool theIsInterval = false);
        virtual ~Timeout(); 
        const std::string & getCommand() const; 
        const double getActivationTime() const; 
        bool isInterval() const; 
        void resetInterval(); 
    private:
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
        TimeoutQueue();

        unsigned long addTimeout(const std::string & theCommand,
                asl::Time theCurrentTime, double theDuration, bool isInterval = false);
        void clearTimeout(unsigned long theId); 
        bool isShowTime(double theTime); 
        TimeoutPtr popTimeout(); 

    private:
        TimeoutMap      _myTimeoutMap;
        TimeoutMultiMap _myTimeoutQueue;
};

}

#endif

