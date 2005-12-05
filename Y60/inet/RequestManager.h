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
//   $RCSfile: RequestManager.h,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2004/10/08 21:30:46 $
//
//
//=============================================================================
#ifndef _Y60_INET_REQUESTManager_INCLUDED_
#define _Y60_INET_REQUESTManager_INCLUDED_

#include "Request.h"

#include <vector>

namespace inet {


    class RequestManager {
        public:
            RequestManager();
            ~RequestManager();
            void performRequest(RequestPtr theRequest);
            int handleRequests();
            int getActiveCount() const { return _myRequests.size(); };
        private:
            CURLM * getHandle() { return _myCurlMultiHandle; };
            void checkCurlStatus(CURLMcode theStatusCode, 
                    const std::string & theWhere);
            void removeRequest(Request* theRequest);
            void handleMessages();
            std::vector<RequestPtr> _myRequests;
            CURLM * _myCurlMultiHandle;
    };
}

#endif

