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

#ifndef __y60__CMSCache_h_included__
#define __y60__CMSCache_h_included__

#include "y60_cmscache_settings.h"

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

const char * const BackendTypeStrings[] = {
    "OCS",
    "SVN",
    ""
};

#define NOPARAM
DEFINE_ENUM(BackendType, BackendTypeEnum,NOPARAM);
#undef NOPARAM

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

        std::string _myLocalPath;
        dom::NodePtr _myPresentationDocument;
        std::string _myUsername;
        std::string _myPassword;
        dom::DocumentPtr _myStatusDocument;
        bool _myVerboseFlag;
        BackendType _myBackendType;
        unsigned _myMaxRequestCount;
        bool _myCleanupFlag;
        std::string _mySessionCookie;
        std::string _myProxy;

        unsigned         _myReportNodeVersion;
        dom::NodePtr     _myReportNode;
        dom::NodePtr     _myAssetReportNode;
        dom::NodePtr     _myStalledFilesNode;
        std::map<std::string, dom::NodePtr> _myAssets;
        std::string _myUserAgent;
};

}
#endif
