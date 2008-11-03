//==============================================================================
// Copyright (C) 2006, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#ifndef __y60__AssetRequest__included
#define __y60__AssetRequest__included

#include <asl/base/Enum.h>
#include <y60/inet/Request.h>
#include <asl/dom/Nodes.h>

namespace y60 {

class RequestThread;    

class AssetRequest : public inet::Request {
    public:
        AssetRequest(RequestThread * theParent,
                     const std::string & theLocalPath,
                     const std::string & theRemoteURI,
                     const std::string & theBaseDir,
                     const inet::CookieJar & theCookies,
                     // OCS doesn't like foreign user agents, that's why we claim to be wget! [jb,ds]
                     const std::string & theUserAgent = "Wget/1.10.2");
        bool isDone() const;

    protected:
        size_t onData( const char * theData, size_t theReceivedByteCount );
        bool onProgress(double theDownloadTotal, double theCurrentDownload,
            double theUploadTotal, double theCurrentUpdate);

        void onError(CURLcode theCode);
        void onDone();
        bool onResponseHeader(const std::string & theHeader);

    private:
        RequestThread *         _myParent;
        unsigned int            _myTotalReceived;
        std::string             _myLocalFile;
        std::string             _myPartialFile;
        std::string             _myLocalPath;
        asl::Ptr<std::ofstream> _myOutputFile;
        bool                    _myIsDoneFlag;
};

typedef asl::Ptr<AssetRequest> AssetRequestPtr;

}

#endif
