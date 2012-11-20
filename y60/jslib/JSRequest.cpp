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
*/

// own header
#include "JSRequest.h"
#include <y60/jsbase/JSBlock.h>

#include <iostream>

#define DB(x) //x;

using namespace std;

namespace jslib {

JSRequest::JSRequest(const std::string & theURL) :
    Request(theURL), _myJSListener(0), _myJSContext(0)
{   
    AC_TRACE << "constructing JSRequest with URL = " << theURL;
}

JSRequest::JSRequest(const std::string & theURL, const std::string & theUserAgent) :
    Request(theURL, theUserAgent), _myJSListener(0), _myJSContext(0)
{
    AC_TRACE << "constructing JSRequest with URL = " << theURL << " user agent = " << theUserAgent;
}

JSRequest::~JSRequest() {
}

void JSRequest::setJSListener(JSContext * theContext, JSObject * theListener) {
    // remove old object if we update the listener.
    removeFromRoot();
    _myJSListener = theListener;
    _myJSContext = theContext;
    // add root to prevent garbage collection of listener object
    if(!JS_AddRoot(theContext, &_myJSListener)) {
        AC_WARNING << "failed to root request object!";
    }
}

void
JSRequest::onError(CURLcode theError, long theHttpStatus) {
    DB(AC_TRACE << "onError for '" << getURL() << "' triggered" << endl);
    if (hasCallback("onError")) {
        jsval argv[2], rval;
        argv[0] = as_jsval(_myJSContext, theError);
        argv[1] = as_jsval(_myJSContext, theHttpStatus);

        /*JSBool ok =*/ JSA_CallFunctionName(_myJSContext, _myJSListener, "onError", 2, argv, &rval);
    }
}


bool
JSRequest::hasCallback(const char * theName) {
    jsval myValue;
    DB(AC_TRACE << "hasCallback('" << theName << "') called for Context=" << _myJSContext
            << ", Listener=" << _myJSListener << endl);
    jsval myListenerValue = OBJECT_TO_JSVAL(_myJSListener);
    DB(AC_TRACE << "myListenerValue=" << myListenerValue << endl);
    /* JSType myType = */ JS_TypeOfValue(_myJSContext, myListenerValue);
    DB(AC_TRACE << "myListenerType=" << myType << endl);
    if (JS_GetProperty(_myJSContext, _myJSListener, theName, &myValue)) {
         DB(AC_TRACE << "property for '" << theName << "' found" << endl);
        if (JS_TypeOfValue(_myJSContext, myValue) == JSTYPE_FUNCTION) {
            DB(AC_TRACE << "callback function for '" << theName << "' found" << endl);
            return true;
        }
        DB(AC_TRACE << "Property '" << theName << "' is not a function" << endl);
    }
    DB(AC_TRACE << "no callback for '" << theName << "' found." << endl);
    return false;
}

bool
JSRequest::onProgress(double theDownloadTotal, double theCurrentDownload,
                double theUploadTotal, double theCurrentUpload) {
    DB(AC_TRACE << "onProgress for '" << getURL() << "' triggered" << endl);

    bool myContinueFlag = true;

    if (hasCallback("onProgress")) {
        jsval argv[4], rval;
        argv[0] = as_jsval(_myJSContext, theDownloadTotal);
        argv[1] = as_jsval(_myJSContext, theCurrentDownload);
        argv[2] = as_jsval(_myJSContext, theUploadTotal);
        argv[3] = as_jsval(_myJSContext, theCurrentUpload);

        JSBool ok = JSA_CallFunctionName(_myJSContext, _myJSListener, "onProgress", 4, argv, &rval);
        if (ok) {
            if (!convertFrom(_myJSContext, rval, myContinueFlag)) {
                std::cerr << "#ERROR: request: onProgress returned a bad result (not a bool)" << std::endl;
                myContinueFlag = true;
            }
        }
    }
    return myContinueFlag;
}

void
JSRequest::onDone() {
    DB(AC_TRACE << "onDone for '" << getURL() << "' triggered" << endl);
    if (hasCallback("onDone")) {
        jsval argv[1], rval;
        /*JSBool ok =*/ JSA_CallFunctionName(_myJSContext, _myJSListener, "onDone", 0, argv, &rval);
    }
}

size_t
JSRequest::onData(const char * theData, size_t theReceivedByteCount) {
    size_t myByteCount = Request::onData(theData, theReceivedByteCount);
    if (hasCallback("onData")) {
        jsval argv[1], rval;
        //argv[0] = as_jsval(_myJSContext, getResponseBlock());
        /*JSBool ok =*/ JSA_CallFunctionName(_myJSContext, _myJSListener, "onData", 0, argv, &rval);
    }
    return myByteCount;
}

void 
JSRequest::removeFromRoot() {
    if(_myJSListener && _myJSContext) {    
        JS_RemoveRoot(_myJSContext, &_myJSListener);
        AC_TRACE << "removing root";
    }
}

}
