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

// own header
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
TestServer::sendSlowly(const std::string & theData) {
    for (std::string::size_type i = 0; i < theData.length(); ++i) {
        this->sendData(&theData[i], 1);
        msleep(1);
    }
}

void
TestServer::sendResponseHeader(int theResponseCode) {
    sendResponseHeader(theResponseCode, "utf-8");
}

void
TestServer::sendResponseHeader(int theResponseCode, const std::string & theCharset) {
    std::stringstream myDate;
    myDate << Time();
    sendSlowly("HTTP/1.1 "+as_string(theResponseCode)+"\n");
    sendSlowly("Date: "+myDate.str()+"\n");
    sendSlowly("Server: ASL Conduit\n");
    sendSlowly("Content-Type: text/plain; charset="+theCharset+"\n");
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
        } else if (myURL == "utf8") {
            sendResponseHeader(200, "utf-8");
            sendResponseBody("\xc3\x84"); // set A-Umlaut
        } else {
            sendResponseHeader(404);
            sendResponseBody("not found");
        }
        return false;
    }
    return true;
}

}

