//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: RequestManager.cpp,v $
//   $Author: pavel $
//   $Revision: 1.5 $
//   $Date: 2005/04/24 00:41:19 $
//
//
//=============================================================================

#include "RequestManager.h"

#include <asl/string_functions.h>
#include <asl/Logger.h>

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

    void
    RequestManager::removeRequest(Request* theRequest) {
        vector<RequestPtr>::iterator it;
        
        for (it=_myRequests.begin();  it != _myRequests.end(); ++it) {
            if (&(*(*it)) == theRequest) {
                curl_multi_remove_handle(_myCurlMultiHandle, theRequest->getHandle());
                _myRequests.erase(it);
                return;
            }
        }
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
                    if (myMessage->data.result == CURLE_OK) {
                        myRequest->onDone();
                    } else {
                        myRequest->onError(myMessage->data.result);
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

