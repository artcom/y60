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

#ifndef y60_inet_statusserver_included
#define y60_inet_statusserver_included

#include "y60_nagios_settings.h"

#include <asl/ipc/ConduitAcceptor.h>
#include <asl/ipc/TCPPolicy.h>
#include <asl/base/ReadWriteLock.h>

namespace inet {

class StatusServer : public asl::ConduitServer<asl::TCPPolicy> {
    public:
        static asl::ConduitServer<asl::TCPPolicy>::Ptr create(asl::TCPPolicy::Handle theHandle);
        static void writeTick();
        static asl::Signed64 readTick();

        static void writeFrameTimeout(asl::Signed32);
        static asl::Signed32 readFrameTimeout();

        static void writeStatusText(const std::string & theStatusText);
        static std::string readStatusText();
    private:
        StatusServer(asl::TCPPolicy::Handle theHandle);
        void sendString(const std::string theData);
        void sendResponseHeader(int theResponseCode);
        void sendResponseHeader(int theResponseCode, const std::string & theCharset);
        void sendResponseBody(const std::string & theBody);
        std::string getUrl(const std::string & theRequest);
        virtual bool processData();

        static asl::Signed64 _myTick;
        static asl::ReadWriteLock _myTickLock;

        static asl::Signed32 _myFrameTimeout;
        static asl::ReadWriteLock _myFrameTimeoutLock;

        static std::string _myStatusText;
        static asl::ReadWriteLock _myStatusTextLock;
};

}

#endif

