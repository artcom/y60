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

#ifndef y60_inet_testserver_included
#define y60_inet_testserver_included

#include <asl/ConduitAcceptor.h>
#include <asl/TCPPolicy.h>

namespace inet {
    
class TestServer : public asl::ConduitServer<asl::TCPPolicy> {
    public:
        static asl::ConduitServer<asl::TCPPolicy>::Ptr create(asl::TCPPolicy::Handle theHandle);
    private:
        TestServer(asl::TCPPolicy::Handle theHandle);
        void sendSlowly(const std::string theData);
        void sendResponseHeader(int theResponseCode);
        void sendResponseHeader(int theResponseCode, const std::string & theCharset);
        void sendResponseBody(const std::string & theBody);
        std::string getUrl(const std::string & theRequest);
        virtual bool processData();
};

}

#endif
