/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "TestServer.h"

using namespace asl;
using namespace std;

namespace inet {

ConduitServer<TCPPolicy>::Ptr 
TestServer::create(TCPPolicy::Handle theHandle) {
    return ConduitServer<TCPPolicy>::Ptr(new TestServer(theHandle));
}
    
TestServer::TestServer(TCPPolicy::Handle theHandle) : ConduitServer<TCPPolicy>(theHandle) 
{}; 

void 
TestServer::sendSlowly(const std::string theData) {
    for (int i = 0; i < theData.length(); ++i) {
        this->sendData(&theData[i], 1);
        msleep(1);
    }
}

void 
TestServer::sendResponseHeader(int theResponseCode) {
    std::stringstream myDate;
    myDate << Time();
    sendSlowly("HTTP/1.1 "+as_string(theResponseCode)+"\n");
    sendSlowly("Date: "+myDate.str()+"\n");
    sendSlowly("Server: ASL Conduit\n");
    sendSlowly("Content-Type: text/plain\n");
}

void 
TestServer::sendResponseBody(const std::string & theBody) {
    sendSlowly("Content-Length: "+as_string(theBody.length())+"\n\n");
    sendSlowly(theBody);
}

std::string 
TestServer::getUrl(const std::string & theRequest) {
    std::string::size_type getPos = theRequest.find("GET");
    std::string::size_type httpPos = theRequest.find("HTTP");
    std::string myUrl = theRequest.substr(getPos+5, httpPos-getPos-6);
    return myUrl;
}
    
bool 
TestServer::processData() {
    CharBuffer myInputBuffer;
    if (this->receiveData(myInputBuffer)) {
        std::string s(&myInputBuffer[0], myInputBuffer.size());
        std::string myURL = getUrl(s);
        if (myURL == "ShortRequest") {
            sendResponseHeader(200);
            sendResponseBody("SHORT");
        } else if (myURL == "LongRequest") {
            sendResponseHeader(200);
            sendResponseBody(string(1000,'L'));
        } else if (myURL == "Timeout") {
            asl::msleep(10*1000);
            sendResponseHeader(200);
            sendResponseBody("timeout");
        } else {
            sendResponseHeader(404);
            sendResponseBody("not found");
        }
        return false;
    }
    return true;
}

}

