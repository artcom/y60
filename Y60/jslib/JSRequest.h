//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_ACXPSHELL_JSREQUEST_H_INCLUDED_
#define _Y60_ACXPSHELL_JSREQUEST_H_INCLUDED_

#include <y60/JScppUtils.h> 

#include <y60/Request.h>

#include <string>

// TODO: change the name JSRequest to something less confusing, e.g. RequestAdapter
// TODO: because it is not a wrapper for request as the name JSRequest suggests

namespace jslib {

class JSRequest : public inet::Request {
    public: 
        JSRequest(const std::string & theURL);
        JSRequest(const std::string & theURL, const std::string & theUserAgent);
        virtual ~JSRequest();

        void setJSListener(JSContext * theContext, JSObject * theListener);
       
        void onError(CURLcode theErrorCode);
        bool onProgress(double theDownloadTotal, double theCurrentDownload,
                double theUploadTotal, double theCurrentUpload);
        void onDone();
    private:
        JSRequest();
        bool hasCallback(const char * theName);
        JSObject * _myJSListener;
        JSContext * _myJSContext;
};

typedef asl::Ptr<JSRequest> JSRequestPtr;

}

#endif
