//==============================================================================
// Copyright (C) 2006, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//==============================================================================

#include "AssetRequest.h"

#include <asl/Logger.h>

using namespace std;
using namespace asl;

namespace y60 {

AssetRequest::AssetRequest(dom::NodePtr theAssetNode,
                           const std::string & theBaseDir,
                           const std::string & theSessionCookie,
                           const string & theUserAgent) :
        inet::Request(theAssetNode->getAttributeString("uri"), theUserAgent),
        _myIsDoneFlag( false ),
        _myAssetNode( theAssetNode )
{
    if ( ! theSessionCookie.empty() ) {
        setCookie( theSessionCookie, true );
    }
    _myLocalFile = theBaseDir + "/" + theAssetNode->getAttributeString("path");
    //setVerbose(true);
    //setTimeoutParams(100, 60);
}

size_t
AssetRequest::onData(const char * theData, size_t theReceivedByteCount) {
    long myResponseCode = getResponseCode();
    if (myResponseCode >= 400 /*&& myResponseCode != 401*/) {
        AC_WARNING << "Error code " << myResponseCode << " returned for '" << getURL() << "'." << endl;
        return 0;
    }
    if ( ! _myOutputFile) {
        _myOutputFile = asl::Ptr<ofstream>(new ofstream(_myLocalFile.c_str(),
                    ios::binary | ios::trunc | ios::out));
    }

    AC_TRACE << "onData called, # of bytes read: " << theReceivedByteCount;

    _myOutputFile->write(theData, theReceivedByteCount);
    return theReceivedByteCount;
}

void
AssetRequest::onError(CURLcode theCode) {
    if (_myIsDoneFlag) {
        AC_INFO << "aborted download.";
        return;
    }
    _myIsDoneFlag = true;
    Request::onError(theCode);
    if (getResponseCode() == 401) {
        AC_ERROR << "Access denied for '" << getURL() << "'.";
    } else {
        AC_ERROR << "CURLerror for URL '" << getURL() << "': " << getErrorString();
        switch (theCode) {
            case CURLE_URL_MALFORMAT:
            case CURLE_COULDNT_RESOLVE_HOST:
            case CURLE_COULDNT_CONNECT:
            case CURLE_PARTIAL_FILE:
            case CURLE_OPERATION_TIMEOUTED:
            //case CURLE_HTTP_RANGE_ERROR:
            //case CURLE_BAD_PASSWORD_ENTERED:
            //case CURLE_LOGIN_DENIED:
            case CURLE_GOT_NOTHING:
            case CURLE_RECV_ERROR:
            case CURLE_BAD_CONTENT_ENCODING:
            case CURLE_FILESIZE_EXCEEDED:
                break;
        }
    }
}    


void 
AssetRequest::onDone() {
    _myIsDoneFlag = true;
    _myOutputFile = asl::Ptr<ofstream>(0);
    _myAssetNode->getAttribute("status")->nodeValue("done");
    if ( _myAssetNode->getAttribute("total") ) {
        _myAssetNode->getAttribute("progress")->nodeValue( _myAssetNode->getAttributeString("total") );
    }

    AC_TRACE << "Request done: " << _myAssetNode->getAttributeString("path");
}

bool
AssetRequest::onProgress(double theDownloadTotal, double theCurrentDownload,
            double theUploadTotal, double theCurrentUpdate)
{
    if ( ! _myAssetNode->getAttribute("total") && theDownloadTotal > 0) {
        _myAssetNode->appendAttribute("total", as_string(theDownloadTotal));
    }

    _myAssetNode->getAttribute("progress")->nodeValue(as_string( theCurrentDownload ));

    return Request::onProgress(theDownloadTotal, theCurrentDownload,
                                theUploadTotal, theCurrentUpdate);
}

bool
AssetRequest::isDone() const {
    return _myIsDoneFlag;
}

}

