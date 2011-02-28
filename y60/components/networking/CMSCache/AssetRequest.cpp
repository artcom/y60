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

#include "AssetRequest.h"

#include "RequestThread.h"
#include <asl/base/Logger.h>
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>

using namespace std;
using namespace asl;

namespace y60 {

AssetRequest::AssetRequest(RequestThread * theParent,
                           const std::string & theLocalPath,
                           const std::string & theRemoteURI,
                           const std::string & theBaseDir,
                           const inet::CookieJar & theCookies,
                           const string & theUserAgent)
        : inet::Request(theRemoteURI, theUserAgent),
        _myParent(theParent),
        _myTotalReceived (0),
        _myLocalPath (theLocalPath),
        _myIsDoneFlag( false )
{
    if ( ! theCookies.empty() ) {
        setCookies( theCookies );
    }
    _myLocalFile = theBaseDir + "/" + _myLocalPath;
    _myPartialFile = theBaseDir + "/" + _myLocalPath+".partial";
    setVerbose(false);
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
AssetRequest::onError(CURLcode theCode, long theHttpStatus) {
    if (_myIsDoneFlag) {
        AC_INFO << "aborted download.";
        return;
    }
    _myIsDoneFlag = true;
    Request::onError(theCode, theHttpStatus);
    switch (theCode) {
        case CURLE_OK:
            break;
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
            {
                AC_ERROR << "CURLerror for URL '" << getURL() << "': " << getErrorString();
            }
            break;
        default:
            {
                AC_ERROR << "CURLerror for URL '" << getURL() << "': " << getErrorString();        
            }
    }
}    


void 
AssetRequest::onDone() {
    _myIsDoneFlag = true;
    if (getResponseCode() >= 399 ) {
        AC_ERROR << "HTTP Error " << getResponseCode() << " for '" <<  getURL() << "'.";
        return;
    }
    if (_myOutputFile) {
        _myOutputFile = asl::Ptr<ofstream>();
        if (asl::fileExists(_myLocalFile)) {
            deleteFile(_myLocalFile);
        }
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

