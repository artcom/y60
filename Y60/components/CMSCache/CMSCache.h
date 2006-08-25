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

enum BackendTypeEnum {
    OCS,
    SVN,
    BackendTypeEnum_MAX
};

static const char * BackendTypeStrings[] = {
    "OCS",
    "SVN",
    ""
};

DEFINE_ENUM(BackendType, BackendTypeEnum);

class CMSCache {
    public:
        CMSCache(const std::string & theLocalPath,
                   dom::NodePtr thePresentationDocument,
                   BackendType theBackendType,
                   const std::string & theUsername = "",
                   const std::string & thePassword = "",
                   const std::string & theSessionCookie = "");
        ~CMSCache();
        void synchronize();
        bool isSynchronized();

        bool getVerboseFlag() const { return _myVerboseFlag; }
        void setVerboseFlag(bool theFlag) { _myVerboseFlag = theFlag; }

        bool getCleanupFlag() const { return _myCleanupFlag; }
        void setCleanupFlag(bool theFlag) { _myCleanupFlag = theFlag; }

        unsigned getMaxRequestCount() const { return _myMaxRequestCount; }
        void setMaxRequestCount(unsigned theMaxCount) { _myMaxRequestCount = theMaxCount; }

        std::string getUserAgent() const { return _myUserAgent; }
        void setUserAgent(const std::string & theUserAgent) { _myUserAgent = theUserAgent; }

        void dumpPresentation();
        void dumpPresentationToFile(const std::string & theFilename);

        dom::NodePtr getStatusReport();

        bool testConsistency();

    private:
        void login();
        void loginOCS();

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
        bool _myCleanupFlag;
        BackendType _myBackendType;
        unsigned _myMaxRequestCount;
        std::string _myUserAgent;
};

}
#endif



