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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "StatusServer.h"
#include <asl/base/string_functions.h>
#include <asl/base/buildinfo.h>

using namespace asl;
using namespace std;
using asl::build_information;

namespace inet {

ConduitServer<TCPPolicy>::Ptr
StatusServer::create(TCPPolicy::Handle theHandle) {
    return ConduitServer<TCPPolicy>::Ptr(new StatusServer(theHandle));
}

StatusServer::StatusServer(TCPPolicy::Handle theHandle) : ConduitServer<TCPPolicy>(theHandle)
{
};

void
StatusServer::sendString(const std::string theData) {
    this->sendData(theData.c_str(), theData.length());
}

void
StatusServer::sendResponseHeader(int theResponseCode) {
    sendResponseHeader(theResponseCode, "utf-8");
}

void
StatusServer::sendResponseHeader(int theResponseCode, const std::string & theCharset) {
    std::stringstream myDate;
    myDate << Time();
    sendString("HTTP/1.1 "+as_string(theResponseCode)+"\n");
    sendString("Date: "+myDate.str()+"\n");
    sendString("Server: ASL Conduit\n");
    sendString("Content-Type: text/plain; charset="+theCharset+"\n");
}

void
StatusServer::sendResponseBody(const std::string & theBody) {
    sendString("Content-Length: "+as_string(theBody.length())+"\n\n");
    sendString(theBody);
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
        asl::build_target_info const& executable = asl::build_information::get().executable();
        std::string myName = executable.name();
        std::string myHistoryId = executable.history_id();
        std::string myRepositoryId = executable.repository_id();
        std::string myScmName = executable.scm_name();
        std::string myBuildDate = executable.build_date();
        std::string myBuildTime = executable.build_time();
        std::string myCompiler = executable.compiler();
        std::string myCompilerVersion = executable.compiler_version();
        if (myURL == "status") {
            asl::Time t;
            asl::Signed64 myElapsed = t.millis() - StatusServer::readTick();
            if (myElapsed > _myFrameTimeout) {
                sendResponseHeader(500);
            } else {
                sendResponseHeader(200);
            }

            std::string myText = myName + string(" rev: ") + myHistoryId.substr(0,6) + string(" ")
                                 + myScmName + string(" repo: ") + myRepositoryId
                                 + string(" builddate: ") + myBuildDate + string(" ") + myBuildTime
                                 + string(" ") + myCompiler + string(":") + myCompilerVersion;
            std::string myStatusText = readStatusText();
            if (myText == "" && !myStatusText.empty()) {
                myText = myStatusText;
            }
            myText += ", perf: "+asl::as_string(myElapsed);
            sendResponseBody(myText);
        } else if (myURL == "revision") {
            sendResponseHeader(200);
            sendResponseBody(myHistoryId);
        } else {
            sendResponseHeader(404);
            sendResponseBody("not found");
        }
        this->disconnect();
        return false;
    }
    return true;
}

asl::Signed64 StatusServer::_myTick(0);
asl::ReadWriteLock StatusServer::_myTickLock;

void
StatusServer::writeTick() {
    ScopeLocker myLock(_myTickLock, true);
    Time t;
    _myTick = t.millis();
}
asl::Signed64
StatusServer::readTick() {
    ScopeLocker myLock(_myTickLock, false);
    return _myTick;
}

asl::Signed32 StatusServer::_myFrameTimeout(100);
asl::ReadWriteLock StatusServer::_myFrameTimeoutLock;
std::string StatusServer::_myStatusText("Y60");
asl::ReadWriteLock StatusServer::_myStatusTextLock;

void
StatusServer::writeFrameTimeout(asl::Signed32 theTimeout) {
    ScopeLocker myLock(_myFrameTimeoutLock, true);
    _myFrameTimeout = theTimeout;
}
asl::Signed32
StatusServer::readFrameTimeout() {
    ScopeLocker myLock(_myFrameTimeoutLock, false);
    return _myFrameTimeout;
}

void
StatusServer::writeStatusText(const std::string & theStatusText) {
    ScopeLocker myLock(_myStatusTextLock, true);
    _myStatusText = theStatusText;
}

std::string
StatusServer::readStatusText() {
    ScopeLocker myLock(_myStatusTextLock, false);
    return _myStatusText;
}
}


