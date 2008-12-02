//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSRequest.h"

#include <iostream>

#define DB(x) //x;

using namespace std;

namespace jslib {

JSRequest::JSRequest(const std::string & theURL) :
    Request(theURL), _myJSContext(0), _myJSListener(0)
{
}

JSRequest::JSRequest(const std::string & theURL, const std::string & theUserAgent) :
    Request(theURL, theUserAgent), _myJSContext(0), _myJSListener(0)
{
}

JSRequest::~JSRequest() {
}

void JSRequest::setJSListener(JSContext * theContext, JSObject * theListener) {
    _myJSContext = theContext;
    _myJSListener = theListener;
}

void
JSRequest::onError(CURLcode theErrorCode) {
    DB(AC_TRACE << "onError for '" << getURL() << "' triggered" << endl); 
    if (hasCallback("onError")) {
        jsval argv[1], rval;
        argv[0] = as_jsval(_myJSContext, theErrorCode);

        /*JSBool ok =*/ JSA_CallFunctionName(_myJSContext, _myJSListener, "onError", 1, argv, &rval);
    }
}


bool 
JSRequest::hasCallback(const char * theName) {
    jsval myValue;
    DB(AC_TRACE << "hasCallback('" << theName << "') called for Context=" << _myJSContext 
            << ", Listener=" << _myJSListener << endl);
    jsval myListenerValue = OBJECT_TO_JSVAL(_myJSListener);
    DB(AC_TRACE << "myListenerValue=" << myListenerValue << endl);
    /*JSType myType =*/ JS_TypeOfValue(_myJSContext, myListenerValue);
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

}
