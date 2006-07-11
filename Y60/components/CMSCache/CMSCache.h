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

#ifndef __y60__CMSCache_h_included__
#define __y60__CMSCache_h_included__

#include "AssetRequest.h"

#include <y60/RequestManager.h>
#include <dom/Nodes.h>

#include <map>

DEFINE_EXCEPTION (CMSCacheException, asl::Exception);

namespace y60 {

class CMSCache {
    public:
        CMSCache(const std::string & theLocalPath,
                   dom::NodePtr thePresentationDocument,
                   const std::string & theUsername = "",
                   const std::string & thePassword = "");
        ~CMSCache();
        void synchronize();
        bool isSynchronized();

        bool getVerboseFlag() const { return _myVerboseFlag; }
        void setVerboseFlag(bool theFlag) { _myVerboseFlag = theFlag; }

        dom::NodePtr getStatusReport();

        enum RequestCount {
            MAX_REQUESTS = 10
        };
    private:
        void login();
        void loginCMS();
        void collectExternalAssetList();
        void collectAssets(dom::NodePtr theParent);
        void addAssetRequest(dom::NodePtr theAsset);
        void collectOutdatedAssets();
        bool isOutdated( dom::NodePtr theAsset );
        void fillRequestQueue();
        void updateDirectoryHierarchy();
        void removeStalledAssets();
        void scanStalledEntries(const std::string & thePath);
    
        std::string _myLocalPath;
        std::string _myUsername;
        std::string _myPassword;
        
        std::string _mySessionCookie;
        inet::RequestManager _myRequestManager;
        
        dom::NodePtr _myPresentationDocument;
        dom::DocumentPtr _myStatusDocument;
        dom::NodePtr     _myAssetReportNode;
        dom::NodePtr     _myStalledFilesNode;
        std::map<std::string, dom::NodePtr> _myAssets;
        std::vector<dom::NodePtr> _myOutdatedAssets;
        
        typedef std::map<dom::Node *, AssetRequestPtr> AssetRequestMap;
        AssetRequestMap _myAssetRequests;

        bool _myVerboseFlag;
};

}
#endif



