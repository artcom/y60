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
//
//   $RCSfile: RequestManager.cpp,v $
//   $Author: pavel $
//   $Revision: 1.5 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

// own header
#include "RequestManager.h"


#include <asl/base/string_functions.h>
#include <asl/base/Logger.h>

#include <curl/curl.h>

#include <iostream>

#define DB(x) // x;

using namespace std;


namespace inet {

    void
    RequestManager::checkCurlStatus(CURLMcode theStatusCode, const std::string & theWhere) {
        if (theStatusCode != CURLM_OK && theStatusCode != CURLM_CALL_MULTI_PERFORM) {
            throw INetException(std::string("Curl Multi Error: ") + asl::as_string(theStatusCode), theWhere);
        }
    }

    RequestManager::RequestManager() {
        curl_global_init(CURL_GLOBAL_ALL);
        _myCurlMultiHandle = curl_multi_init();
    }

    RequestManager::~RequestManager() {
        while (!_myRequests.empty()) {
            RequestPtr myRequest = _myRequests.back();
            curl_multi_remove_handle(_myCurlMultiHandle, myRequest->getHandle());
            _myRequests.pop_back();
        }

        CURLMcode myStatus = curl_multi_cleanup(_myCurlMultiHandle);
        checkCurlStatus(myStatus, PLUS_FILE_LINE);
    }

    void
    RequestManager::performRequest(RequestPtr theRequest) {
        theRequest->onStart();
        curl_multi_add_handle(_myCurlMultiHandle, theRequest->getHandle());
        _myRequests.push_back(theRequest);

    }

    bool 
    RequestManager::removeRequest(Request* theRequest) {
        vector<RequestPtr>::iterator it;

        for (it=_myRequests.begin();  it != _myRequests.end(); ++it) {
            if (&(*(*it)) == theRequest) {
                curl_multi_remove_handle(_myCurlMultiHandle, theRequest->getHandle());
                _myRequests.erase(it);
                return true;
            }
        }  
        return false;
    }

    void
    RequestManager::handleMessages() {
        int myMessageCount = 0;
        CURLMsg * myMessage = 0;
        do {
            myMessage = curl_multi_info_read(_myCurlMultiHandle, &myMessageCount);
            if (myMessage) {
                CURL * myEasyHandle = myMessage->easy_handle;
                Request * myRequest = 0;
                curl_easy_getinfo(myEasyHandle, CURLINFO_PRIVATE, &myRequest);
                if (myMessage->msg == CURLMSG_DONE) {
                    CURLcode myResult = myMessage->data.result;
                    if (myResult == CURLE_OK) {
                        long myResponseCode = myRequest->getResponseCode();
                        if (myResponseCode / 100 == 2) {
                            myRequest->onDone();
                        } else {
                            myRequest->onError(myResult, myResponseCode);
                        }
                    } else {
                        AC_ERROR << "curl request failed with error: " << curl_easy_strerror(myResult);
                        myRequest->onError(myResult, 0);
                    }
                    removeRequest(myRequest);
                } else {
                    throw INetException("RequestManager::handleMessages(): unknown curl msg", PLUS_FILE_LINE);
                }
            }
        } while (myMessage);


    }

    int
    RequestManager::handleRequests(bool theBlockingFlag) {
        CURLMcode myStatus;

        AC_TRACE << "handleRequests("<<theBlockingFlag<<");";

        if (theBlockingFlag) {
            fd_set myReadHandles;
            fd_set myWriteHandles;
            fd_set myExceptHandles;
            int myMaxFd;

            struct timeval tv;
            /* Wait up to 0.1 sec */
            tv.tv_sec = 0;
            tv.tv_usec = 100000;

            FD_ZERO(&myReadHandles);
            FD_ZERO(&myWriteHandles);
            FD_ZERO(&myExceptHandles);
            myStatus = curl_multi_fdset(_myCurlMultiHandle,
                    &myReadHandles, &myWriteHandles,&myExceptHandles,
                    &myMaxFd);
            checkCurlStatus(myStatus, PLUS_FILE_LINE);
            if (myMaxFd != -1) {
                select(myMaxFd+1, &myReadHandles, &myWriteHandles, &myExceptHandles, &tv);
            }
         }
        int myRunningHandles;
        do {
            myStatus = curl_multi_perform(_myCurlMultiHandle, &myRunningHandles);
            checkCurlStatus(myStatus, PLUS_FILE_LINE);
            DB(AC_TRACE << "curl multi perform returned " << myRunningHandles << " running handles. More Data..." << endl);

            handleMessages();

        } while (myStatus == CURLM_CALL_MULTI_PERFORM);
        return myRunningHandles;
    }
}

