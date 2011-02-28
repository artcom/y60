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

#ifndef __y60__RequestThread_h_included__
#define __y60__RequestThread_h_included__

#include "y60_cmscache_settings.h"

#include "AssetRequest.h"

#include <asl/dom/Nodes.h>
#include <asl/base/PosixThread.h>
#include <asl/base/ThreadFifo.h>
#include <asl/base/ReadWriteLock.h>
#include <y60/inet/RequestManager.h>

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

        std::string _myLocalPath;
        std::string _myUsername;
        std::string _myPassword;
        std::string _myUserAgent;
        std::string _myProxy;
        std::vector< std::pair<std::string, std::string> > _myOutdatedAssets;
        unsigned int _myMaxRequestCount;
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
