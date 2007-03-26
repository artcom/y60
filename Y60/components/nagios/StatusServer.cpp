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

#include "StatusServer.h"
#include <asl/string_functions.h>

using namespace asl;
using namespace std;

namespace inet {

ConduitServer<TCPPolicy>::Ptr 
StatusServer::create(TCPPolicy::Handle theHandle) {
    return ConduitServer<TCPPolicy>::Ptr(new StatusServer(theHandle));
}
    
StatusServer::StatusServer(TCPPolicy::Handle theHandle) : ConduitServer<TCPPolicy>(theHandle) 
{}; 

void 
StatusServer::sendSlowly(const std::string theData) {
    for (int i = 0; i < theData.length(); ++i) {
        this->sendData(&theData[i], 1);
        msleep(1);
    }
}

void 
StatusServer::sendResponseHeader(int theResponseCode) {
    sendResponseHeader(theResponseCode, "utf-8");
}

void
StatusServer::sendResponseHeader(int theResponseCode, const std::string & theCharset) {
    std::stringstream myDate;
    myDate << Time();
    sendSlowly("HTTP/1.1 "+as_string(theResponseCode)+"\n");
    sendSlowly("Date: "+myDate.str()+"\n");
    sendSlowly("Server: ASL Conduit\n");
    sendSlowly("Content-Type: text/plain; charset="+theCharset+"\n");
}

void 
StatusServer::sendResponseBody(const std::string & theBody) {
    sendSlowly("Content-Length: "+as_string(theBody.length())+"\n\n");
    sendSlowly(theBody);
}

std::string 
StatusServer::getUrl(const std::string & theRequest) {
    std::string::size_type getPos = theRequest.find("GET");
    std::string::size_type httpPos = theRequest.find("HTTP");
    std::string myUrl = theRequest.substr(getPos+5, httpPos-getPos-6);
    return myUrl;
}
    
bool 
StatusServer::processData() {
    CharBuffer myInputBuffer;
    if (this->receiveData(myInputBuffer)) {
        std::string s(&myInputBuffer[0], myInputBuffer.size());
        std::string myURL = getUrl(s);
        if (myURL == "status") {
            asl::Time t;
            long long myElapsed = t.millis() - StatusServer::readTick();
            if (myElapsed > 5000) {
                sendResponseHeader(500);
            } else {
                sendResponseHeader(200);
            }
            sendResponseBody(asl::as_string(myElapsed));
        } else {
            sendResponseHeader(404);
            sendResponseBody("not found");
        }
        return false;
    }
    return true;
}

long long StatusServer::_myTick(0);
asl::ReadWriteLock StatusServer::_myTickLock;

void
StatusServer::writeTick() {
    ScopeLocker myLock(_myTickLock, true);
    Time t;
    _myTick = t.millis();
}
long long
StatusServer::readTick() {
    ScopeLocker myLock(_myTickLock, false);
    return _myTick;
}

}


