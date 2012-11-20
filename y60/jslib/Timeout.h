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

#ifndef __included_Timeout_h__
#define __included_Timeout_h__

#include "y60_jslib_settings.h"

#include <asl/base/Time.h>
#include <asl/base/Ptr.h>

#include <y60/jsbase/jssettings.h>

#ifdef USE_LEGACY_SPIDERMONKEY
#include <js/spidermonkey/jsapi.h>
#else
#include <js/jsapi.h>
#endif

#include <string>
#include <map>

namespace jslib {

class Timeout {
    public:
        Timeout();
        Timeout(const std::string & theCommand, double theDuration,
                asl::Time theCurrentTime, bool theIsInterval = false, JSObject * theObjectToCall = 0);
        virtual ~Timeout();
        const std::string & getCommand() const;
        const double getActivationTime() const;
        JSObject * getObjectToCall() const;
        bool isInterval() const;
        void resetInterval();
    private:
        std::string      _myCommand;
        double           _myDuration;
        double           _myActivationTime;
        bool             _isInterval;
        JSObject *       _myObjectToCall;
};

typedef asl::Ptr<Timeout> TimeoutPtr;

typedef std::multimap<double, unsigned long> TimeoutMultiMap;
typedef std::map<unsigned long, TimeoutPtr>  TimeoutMap;

class TimeoutQueue {
    public:
        TimeoutQueue();

        unsigned long addTimeout(const std::string & theCommand,
                asl::Time theCurrentTime, double theDuration, bool isInterval = false, JSObject * theObjectToCall = 0);
        void clearTimeout(unsigned long theId);
        bool isShowTime(double theTime);
        TimeoutPtr popTimeout();

    private:
        TimeoutMap      _myTimeoutMap;
        TimeoutMultiMap _myTimeoutQueue;
        static unsigned long ourNextTimeoutId;
};

}

#endif

