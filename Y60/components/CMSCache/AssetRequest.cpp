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

#include "RequestThread.h"
#include <asl/Logger.h>
#include <asl/file_functions.h>
#include <asl/MappedBlock.h>

using namespace std;
using namespace asl;

namespace y60 {

AssetRequest::AssetRequest(RequestThread * theParent,
                           const std::string & theLocalPath,
                           const std::string & theRemoteURI,
                           const std::string & theBaseDir,
                           const inet::CookieJar & theCookies,
                           const string & theUserAgent) :
        _myParent(theParent),
        inet::Request(theRemoteURI, theUserAgent),
        _myIsDoneFlag( false ),
        _myTotalReceived (0),
        _myLocalPath (theLocalPath)
{
    if ( ! theCookies.empty() ) {
        setCookies( theCookies );
    }
    _myLocalFile = theBaseDir + "/" + _myLocalPath;
    _myPartialFile = theBaseDir + "/" + _myLocalPath+".partial";
    verifyPeer(false); // don't bother to authenticate server, just encrypt
    if (fileExists(_myLocalFile)) {
        time_t myLastModified = getLastModified(_myLocalFile);
        if (_myLocalPath == "C_produktion/1_inhalte/IMG/emptypath.png") {
            AC_DEBUG << "If-modified-Since " << myLastModified << " for " << _myLocalPath; 
        }
        addHttpHeaderAsDate("If-Modified-Since", myLastModified);
    }
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
        _myParent->sendStatusMsg(StatusMsg(_myLocalPath, "status", "downloading"));
        _myParent->sendStatusMsg(StatusMsg(_myLocalPath, "progress", "0.0"));
        _myOutputFile = asl::Ptr<ofstream>(new ofstream(_myPartialFile.c_str(),
                    ios::binary | ios::trunc | ios::out));
    }

    AC_TRACE << "onData called, # of bytes read: " << theReceivedByteCount;

    _myOutputFile->write(theData, theReceivedByteCount);
    _myTotalReceived += theReceivedByteCount;
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
    if (_myOutputFile) {
        _myOutputFile = asl::Ptr<ofstream>(0);
        deleteFile(_myLocalFile);
        moveFile(_myPartialFile, _myLocalFile);
    }
    _myParent->sendStatusMsg(StatusMsg(_myLocalPath, "status", "done"));
    _myParent->sendStatusMsg(StatusMsg(_myLocalPath, "progress", as_string(_myTotalReceived)));
}

bool
AssetRequest::onResponseHeader(const string & theHeader) {
    string::size_type myColon = theHeader.find(":");
    if (myColon != string::npos) {
        string myKey = theHeader.substr(0,myColon);
        string myValue = theHeader.substr(myColon+2);
        if (myKey == "Set-Cookie") {
            _myParent->addToCookieJar(myValue);
        }
    }
    return Request::onResponseHeader(theHeader);
}

bool
AssetRequest::onProgress(double theDownloadTotal, double theCurrentDownload,
            double theUploadTotal, double theCurrentUpdate)
{
    if ( theDownloadTotal > 0) {
        _myParent->sendStatusMsg(StatusMsg(_myLocalPath, "total", as_string(theDownloadTotal)));
    }
    _myParent->sendStatusMsg(StatusMsg(_myLocalPath, "progress", as_string(theCurrentDownload)));

    return Request::onProgress(theDownloadTotal, theCurrentDownload,
                                theUploadTotal, theCurrentUpdate);
}

bool
AssetRequest::isDone() const {
    return _myIsDoneFlag;
}

}

