//==============================================================================
//
// Copyright (C) 2006, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#ifndef __y60__RequestThread_h_included__
#define __y60__RequestThread_h_included__

#include "AssetRequest.h"

#include <dom/Nodes.h>
#include <asl/PosixThread.h>
#include <asl/ThreadFifo.h>
#include <asl/ReadWriteLock.h>
#include <y60/RequestManager.h>

namespace y60 {

struct StatusMsg {
    StatusMsg(const std::string & thePath, const std::string & theAttribute, const std::string & theValue) :
        path(thePath), attribute(theAttribute), value(theValue) {};
    std::string path;
    std::string attribute;
    std::string value;
};

class RequestThread : public asl::PosixThread {
    public:
        RequestThread( const std::string & theLocalPath, 
                      const std::string & theUsername, 
                      const std::string & thePassword,
                      const std::string & theSessionCookie,
                      const std::string & theUserAgent,
                      const std::string & theProxy,
                      const std::vector<std::pair<std::string, std::string> > & theOutdatedAssets,
                      unsigned int theMaxRequestCount,
                      bool theVerboseFlag);

        void addToCookieJar(const std::string & theCookieString);
        int getRemainingCount() const;

        void sendStatusMsg(const StatusMsg & theMessage) {
            _myMessageQueue.push(theMessage);
        }

        bool popStatusMsg(StatusMsg & theMessage) {
            if (_myMessageQueue.empty()) {
                return false;
            }
            theMessage = _myMessageQueue.pop();
            return true;
        }

    protected:
        void setRemainingCount(unsigned int theNewCount);

    private:
        bool handleRequests();
        virtual void run ();
        void addAssetRequest(const std::string & thePath, const std::string & theURI);
        void fillRequestQueue();

        inet::RequestManager _myRequestManager;
        typedef std::map<std::string, AssetRequestPtr> AssetRequestMap;
        AssetRequestMap _myAssetRequests;

        std::vector< std::pair<std::string, std::string> > _myOutdatedAssets;

        unsigned int _myMaxRequestCount;
        std::string _myUserAgent;
        std::string _myLocalPath;
        std::string _myProxy;
        std::string _myUsername;
        std::string _myPassword;
        bool _myVerboseFlag;
        std::map<std::string,std::string> _myCookieJar;

        // mutex-protected members
        mutable asl::ReadWriteLock  _remainingCountLock;
        unsigned int _remainingCount;

        asl::ThreadFifo<StatusMsg> _myMessageQueue;
};

typedef asl::Ptr<RequestThread> RequestThreadPtr;

}

#endif
