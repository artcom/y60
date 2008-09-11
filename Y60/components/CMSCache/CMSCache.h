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

#include "RequestThread.h"

#include <asl/dom/Nodes.h>

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
        bool isSynchronized() {
            if (! _myRequestThread ) {
                return true;
            }
            return _myRequestThread->getRemainingCount() == 0;
        };

        std::string getLocalPath() const { return _myLocalPath; }

        bool getVerboseFlag() const { return _myVerboseFlag; }
        void setVerboseFlag(bool theFlag) { _myVerboseFlag = theFlag; }

        bool getCleanupFlag() const { return _myCleanupFlag; }
        void setCleanupFlag(bool theFlag) { _myCleanupFlag = theFlag; }

        void setProxy(const std::string & theProxy);

        unsigned getMaxRequestCount() const { return _myMaxRequestCount; }
        void setMaxRequestCount(unsigned theMaxCount) { _myMaxRequestCount = theMaxCount; }

        std::string getUserAgent() const { return _myUserAgent; }
        void setUserAgent(const std::string & theUserAgent) { _myUserAgent = theUserAgent; }

        void dumpPresentation();
        void dumpPresentationToFile(const std::string & theFilename);

        dom::NodePtr getStatusReport();

        // bool testConsistency();

    private:
        RequestThreadPtr _myRequestThread;
        // void login();
        // void loginOCS();

        void collectExternalAssetList();
        void collectAssets(dom::NodePtr theParent);
        void collectOutdatedAssets(std::vector<std::pair<std::string, std::string> > & theOutdatedAssets);
        bool isOutdated( dom::NodePtr theAsset );
        void updateDirectoryHierarchy();
        void removeStalledAssets();
        void scanStalledEntries(const std::string & thePath);

        std::string _mySessionCookie;

        dom::NodePtr _myPresentationDocument;
        dom::DocumentPtr _myStatusDocument;

        dom::NodePtr     _myReportNode;
        unsigned         _myReportNodeVersion;

        dom::NodePtr     _myAssetReportNode;
        dom::NodePtr     _myStalledFilesNode;
        std::map<std::string, dom::NodePtr> _myAssets;

        bool _myVerboseFlag;
        bool _myCleanupFlag;
        BackendType _myBackendType;
        unsigned _myMaxRequestCount;

        std::string _myUserAgent;
        std::string _myLocalPath;
        std::string _myUsername;
        std::string _myPassword;
        std::string _myProxy;

};

}
#endif
