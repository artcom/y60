//==============================================================================
// Copyright (C) 2006, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#include <y60/Request.h>
#include <dom/Nodes.h>

namespace y60 {

class AssetRequest : public inet::Request {
    public:
        AssetRequest(dom::NodePtr theAssetNode,
                     const std::string & theBaseDir,
                     const std::string & theSessionCookie);

        bool isDone() const;

    protected:
        size_t onData( const char * theData, size_t theReceivedByteCount );
        bool onProgress(double theDownloadTotal, double theCurrentDownload,
            double theUploadTotal, double theCurrentUpdate);

        void onError(CURLcode theCode);
        void onDone();

    private:
        dom::NodePtr            _myAssetNode;
        std::string             _myLocalFile;
        asl::Ptr<std::ofstream> _myOutputFile;
        bool                    _myIsDoneFlag;
};

typedef asl::Ptr<AssetRequest> AssetRequestPtr;

}

