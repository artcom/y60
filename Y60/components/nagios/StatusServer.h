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
*/

#ifndef y60_inet_statusserver_included
#define y60_inet_statusserver_included

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

