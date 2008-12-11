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

#include "RequestThread.h"
#include <y60/inet/Request.h>
#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>

#include <curl/curl.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace asl;
using namespace inet;

namespace y60 {

RequestThread::RequestThread(const std::string & theLocalPath, 
                      const std::string & theUsername, 
                      const std::string & thePassword,
                      const std::string & theSessionCookie,
                      const std::string & theUserAgent,
                      const std::string & theProxy,
                      const std::vector<std::pair<std::string, std::string> > & theOutdatedAssets,
                      unsigned int theMaxRequestCount,
                      bool theVerboseFlag) :
    _myLocalPath(theLocalPath), 
    _myUsername(theUsername), 
    _myPassword(thePassword), 
    _myUserAgent(theUserAgent),
    _myProxy(theProxy),
    _myOutdatedAssets(theOutdatedAssets),
    _myMaxRequestCount(theMaxRequestCount),
    _myVerboseFlag(theVerboseFlag),
    _remainingCount(theOutdatedAssets.size())
{
    if (!theSessionCookie.empty()) {
        addToCookieJar(theSessionCookie);
    }
    fillRequestQueue();
};


int 
RequestThread::getRemainingCount() const {
    asl::ScopeLocker(_remainingCountLock, false);
    return _remainingCount;
}

void 
RequestThread::setRemainingCount(unsigned int theNewCount) {
    asl::ScopeLocker(_remainingCountLock, true);
    _remainingCount = theNewCount;
}

void 
RequestThread::addToCookieJar(const std::string & theCookieString) {
    // JSESSIONID=c04c810430da7ff877e211624fbcb7e591c88151b82e.e34LaNmQaNeMci0Mb38LbhmPbxj0n6jAmljGr5XDqQLvpAe; path=/content
    string::size_type myEquals = theCookieString.find("=");
    if (myEquals == string::npos) {
        AC_WARNING << "illegal cookie header:" << theCookieString;
    }
    string myCookieName = theCookieString.substr(0, myEquals);
    string myCookieValue = theCookieString.substr(myEquals+1);

    string::size_type myDelimiter = myCookieValue.find(";");
    if (myDelimiter != string::npos) {
        myCookieValue = myCookieValue.substr(0, myDelimiter);
    }

    // now add name/value to map
    _myCookieJar[myCookieName] = myCookieValue;
}

bool 
RequestThread::handleRequests() {
    int myRunningCount = _myRequestManager.handleRequests(true);

    // TODO: Error statistics and handling
    AssetRequestMap::iterator myIter = _myAssetRequests.begin();
    while (myIter != _myAssetRequests.end()) {
        if (myIter->second->isDone()) {
            int myResponseCode = myIter->second->getResponseCode();
            if ( myResponseCode == 200) {
                std::string myFilename = _myLocalPath + "/" + myIter->first;
                time_t myTime = Request::getTimeFromHTTPDate(
                         myIter->second->getResponseHeader("Last-Modified"));
                AC_DEBUG << "Last-Modified Header '" << myIter->second->getResponseHeader("Last-Modified") << "'";
                AC_DEBUG << "$TZ '" << expandEnvironment("${TZ}") << "'";
                AC_DEBUG << "setLastModified for " << myFilename << " to " << myTime;
                setLastModified(myFilename, myTime);
                AC_DEBUG << "lastModified is now " << getLastModified(myFilename);
            }  else if ( myResponseCode == 304) { 
                // not modified, everything ok
            } else {
                string myReason = myIter->second->getResponseHeader("X-ORA-CONTENT-Info");
                    if (!myReason.empty()) {
                        AC_PRINT << "OCS server reason: '" << myReason << "' .";
                    }
                // TODO: retry handling
            }
            _myAssetRequests.erase( myIter++ );
        } else {
            AC_TRACE << "  still running:" << myIter->second->getResponseCode() << ":" << myIter->first;
            ++myIter;
        }
    }

    fillRequestQueue();
    //AC_WARNING << "queued:" << _myOutdatedAssets.size() << ", transferring" << _myAssetRequests.size();
    unsigned int remainingCount = _myOutdatedAssets.size() + _myAssetRequests.size();
    setRemainingCount(remainingCount);
    return (remainingCount == 0) && (myRunningCount == 0);
}

void
RequestThread::fillRequestQueue() {
    for (unsigned i = _myAssetRequests.size(); i < _myMaxRequestCount; ++i) {
        if ( ! _myOutdatedAssets.empty()) {
            addAssetRequest( _myOutdatedAssets.back().first, _myOutdatedAssets.back().second );
            _myOutdatedAssets.pop_back();
        } else {
            break;
        }
    }
}


void
RequestThread::addAssetRequest(const std::string & thePath, const std::string & theURI) {
    AC_INFO << "Fetching " << _myLocalPath + "/" + thePath;
    AssetRequestPtr myRequest;
    if (_myUserAgent.empty()) {
        myRequest = AssetRequestPtr(new AssetRequest( this, thePath, theURI, _myLocalPath, _myCookieJar));
    } else {
        myRequest = AssetRequestPtr(new AssetRequest( this, thePath, theURI, _myLocalPath, _myCookieJar, _myUserAgent));
    }
    if ( ! _myUsername.empty() ) {
        myRequest->setCredentials(_myUsername, _myPassword, DIGEST);
    }
    if (! _myProxy.empty()) {
        myRequest->setProxy(_myProxy,0);
    }
    myRequest->setVerbose(_myVerboseFlag);
    _myAssetRequests.insert(make_pair(thePath, myRequest));
    _myRequestManager.performRequest(myRequest);
}



void 
RequestThread::run () {
    while (!shouldTerminate()) {
        if (handleRequests()) {
            break;
        }
    }
}

}

