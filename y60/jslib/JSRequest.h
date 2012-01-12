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

#ifndef _Y60_ACXPSHELL_JSREQUEST_H_INCLUDED_
#define _Y60_ACXPSHELL_JSREQUEST_H_INCLUDED_

#include "y60_jslib_settings.h"

#include <y60/jsbase/JScppUtils.h>

#include <y60/inet/Request.h>

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

        void onError(CURLcode theError, long theHttpStatus);
        bool onProgress(double theDownloadTotal, double theCurrentDownload,
                double theUploadTotal, double theCurrentUpload);
        void onDone();
        size_t onData(const char * theData, size_t theReceivedByteCount);

        void removeFromRoot();
    private:
        JSRequest();
        bool hasCallback(const char * theName);
        JSObject * _myJSListener;
        JSContext * _myJSContext;
};

typedef asl::Ptr<JSRequest> JSRequestPtr;

}

#endif
