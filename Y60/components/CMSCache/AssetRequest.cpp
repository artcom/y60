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

AssetRequest::AssetRequest(const std::string & theURL,
                           const std::string & theLocalFile,
                           const std::string & theSessionCookie) :
        // OCS doesn't like foreign user agents, that's why we claim to be wget! [jb,ds]
        inet::Request(theURL, "Wget/1.10.2"),
        _myLocalFile(theLocalFile),
        _myIsDoneFlag( false )
{
    setCookie( theSessionCookie, true);
}

size_t
AssetRequest::onData(const char * theData, size_t theReceivedByteCount) {
    long myResponseCode = getResponseCode();
    if (myResponseCode >= 400) {
        AC_WARNING << "Error:" << myResponseCode << ":" << getURL() << endl;
        return 0;
    }
    AC_PRINT << "save to: '" << _myLocalFile << "'";
    if ( ! _myOutputFile) {
        _myOutputFile = Ptr<ofstream>(new ofstream(_myLocalFile.c_str(),
                    ios::binary | ios::trunc | ios::out));
    }

    AC_TRACE << "onData called, byteCount=" << theReceivedByteCount;

    _myOutputFile->write(theData, theReceivedByteCount);
    return theReceivedByteCount;
}

void 
AssetRequest::onDone() {
    _myIsDoneFlag = true;
    _myOutputFile = asl::Ptr<ofstream>(0);
}
bool
AssetRequest::isDone() const {
    return _myIsDoneFlag;
}

}

