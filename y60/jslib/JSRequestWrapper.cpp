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

// own header
#include "JSRequestWrapper.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSEnum.h>
#include <y60/jsbase/JSBlock.h>
#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/ArgumentHolder.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<JSRequest, JSRequestPtr, StaticAccessProtocol>;
template struct Y60_JSLIB_DECL ArgumentHolder<JSRequestPtr const &>;
template struct Y60_JSLIB_DECL ArgumentHolder<inet::RequestPtr>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the url of the request.");
    DOC_RVAL("The string", DOC_TYPE_STRING);
    DOC_END;
    std::string myStringRep = JSRequestWrapper::getJSWrapper(cx,obj).getNative().getURL();
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
get(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Request a HTTP-GET.");
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::get,cx,obj,argc,argv,rval);
}

static JSBool
head(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Request a HTTP-HEAD.");
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::head,cx,obj,argc,argv,rval);
}

static JSBool
http_delete(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Request a HTTP-DELETE.");
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::http_delete,cx,obj,argc,argv,rval);
}
    
static JSBool
post(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Post a string as one block of data.");
    DOC_PARAM("theData", "The data to post", DOC_TYPE_STRING);
    DOC_RVAL("The size of the post buffer", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::post,cx,obj,argc,argv,rval);
}

static JSBool
put(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Put a string as one block of data.");
    DOC_PARAM("theData", "The data to post", DOC_TYPE_STRING);
    DOC_RVAL("The size of the put buffer", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::put,cx,obj,argc,argv,rval);
}

static JSBool
putBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Put a string as one block of data.");
    DOC_PARAM("theData", "The data to post", DOC_TYPE_STRING);
    DOC_RVAL("The size of the putBlock buffer", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::putBlock,cx,obj,argc,argv,rval);
}
    
static JSBool
postFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Post a file as one block of data.");
    DOC_PARAM("theFilename", "The filename to post", DOC_TYPE_STRING);
    DOC_RVAL("The size of the post buffer", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::postFile,cx,obj,argc,argv,rval);
}

static JSBool
addHttpHeader(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Adds a entry to the HTTP header");
    DOC_PARAM("theKey", "", DOC_TYPE_STRING);
    DOC_PARAM("theValue", "", DOC_TYPE_STRING);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::addHttpHeader,cx,obj,argc,argv,rval);
}

static JSBool
getResponseHeader(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the HTTP Response header field with the name given by theHeader.");
    DOC_PARAM("theHeader", "Name of the header field", DOC_TYPE_STRING);
    DOC_RVAL("The requested http response header field", DOC_TYPE_STRING);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::getResponseHeader,cx,obj,argc,argv,rval);
}

static JSBool
setTimeoutParams(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the LowSpeedLimit (Bytes/s) and LowSpeedTimeout (sec) to detect transfer timeouts.");
    DOC_PARAM("theBytesPerSec", "", DOC_TYPE_INTEGER);
    DOC_PARAM("theSeconds", "", DOC_TYPE_INTEGER);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::setTimeoutParams,cx,obj,argc,argv,rval);
}

static JSBool
setCredentials(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets the username and password for the base and digest authentication methods.");
    DOC_PARAM("theUsername", "", DOC_TYPE_STRING);
    DOC_PARAM("thePassword", "", DOC_TYPE_STRING);
    DOC_PARAM_OPT("theAuthentMethod", "The authentication methode.", DOC_TYPE_STRING, "ANY");
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::setCredentials,cx,obj,argc,argv,rval);
}

static JSBool
setCookie(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets a cookie with the given string.");
    DOC_PARAM("theString", "", DOC_TYPE_STRING);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::setCookie,cx,obj,argc,argv,rval);
}

static JSBool
setProxy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Sets a HTTP Proxy.");
    DOC_PARAM("theProxyHost", "", DOC_TYPE_STRING);
    DOC_PARAM("theTunnelFlag", "", DOC_TYPE_BOOLEAN);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::setProxy,cx,obj,argc,argv,rval);
}


static JSBool
getTimeFromHTTPDate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Converts a HTTP Date String to a Time object");
    DOC_PARAM("theHTTPDate", "", DOC_TYPE_STRING);
    DOC_RVAL("Time object", DOC_TYPE_OBJECT);
    DOC_END;
    try {
        ensureParamCount(argc, 1);
        std::string myHTTPDateString;
        convertFrom(cx, argv[0], myHTTPDateString);

        time_t myTime = inet::Request::getTimeFromHTTPDate( myHTTPDateString );

        *rval = as_jsval(cx, myTime);
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
getAllResponseHeaders(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Get all response headers matching theKey");
    DOC_PARAM("theKey", "", DOC_TYPE_STRING);
    DOC_RVAL("VectorOfString", DOC_TYPE_OBJECT);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::getAllResponseHeaders,cx,obj,argc,argv,rval);

}

JSFunctionSpec *
JSRequestWrapper::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",          toString,            0},
        {"get",               get,                 0},
        {"head",              head,                1},
        {"put",               put,                 1},
        {"putBlock",          putBlock,            1},
        {"post",              post,                1},
        {"postFile",          postFile,            1},
        {"http_delete",       http_delete,         0},
        {"addHttpHeader",     addHttpHeader,       2},
        {"getResponseHeader", getResponseHeader,   1},
        {"getAllResponseHeaders", getAllResponseHeaders,   1},
        {"setTimeoutParams",  setTimeoutParams,    2},
        {"setCredentials",    setCredentials,      3},
        {"setCookie",         setCookie,      1},
        {"setProxy",          setProxy,       2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSRequestWrapper::Properties() {
    static JSPropertySpec myProperties[] = {
        {"responseCode", PROP_responseCode, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"responseString", PROP_responseString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"responseBlock", PROP_responseBlock, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"errorString", PROP_errorString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"verbose", PROP_verbose, JSPROP_ENUMERATE | JSPROP_PERMANENT},
        {"verifyPeer", PROP_verifyPeer, JSPROP_ENUMERATE | JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSRequestWrapper::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSRequestWrapper::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSRequestWrapper::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {"getTimeFromHTTPDate",         getTimeFromHTTPDate,         1},
        {0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSRequestWrapper::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_responseCode:
                *vp = as_jsval(cx, getNative().getResponseCode());
                return JS_TRUE;
            case PROP_responseString:
                *vp = as_jsval(cx, getNative().getResponseString());
                return JS_TRUE;
            case PROP_responseBlock:
                {
                    asl::Ptr<asl::Block> myBlock(new Block(getNative().getResponseBlock()));
                    *vp = as_jsval(cx, myBlock);
                }
                return JS_TRUE;
            case PROP_errorString:
                *vp = as_jsval(cx, getNative().getErrorString());
                return JS_TRUE;
            case PROP_URL:
                *vp = as_jsval(cx, getNative().getURL());
                return JS_TRUE;
            case PROP_verbose:
                *vp = as_jsval(cx, getNative().getVerbose());
                return JS_TRUE;
            case PROP_verifyPeer:
                *vp = as_jsval(cx, getNative().getVerifyPeer());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSRequestWrapper::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSRequestWrapper::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    switch (theID) {
        case PROP_responseCode:
            return JS_FALSE;
        case PROP_verbose:
            try {
                bool myVerboseFlag;
                convertFrom(cx, *vp, myVerboseFlag );
                myObj.getNative().setVerbose( myVerboseFlag );
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_verifyPeer:
            try {
                bool myFlag;
                convertFrom(cx, *vp, myFlag );
                myObj.getNative().verifyPeer( myFlag );
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            JS_ReportError(cx,"JSRequestWrapper::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSRequestWrapper::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a new RequestWrapper");
    DOC_RESET;
    DOC_PARAM("theUrl", "The url for the reqest", DOC_TYPE_STRING);
    DOC_PARAM_OPT("theUserAgent", "The user agent", DOC_TYPE_STRING, "");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSRequestWrapper * myNewObject = 0;

    if (argc >= 1) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSRequestWrapper::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        std::string myURL = "";
        if (!convertFrom(cx, argv[0], myURL)) {
            JS_ReportError(cx, "JSRequestWrapper::Constructor: argument #1 must be an URL");
            return JS_FALSE;
        }


        OWNERPTR myNewRequest;
        if (argc == 2) {
            if (JSVAL_IS_VOID(argv[1])) {
                JS_ReportError(cx,"JSRequestWrapper::Constructor: bad argument #2 (undefined)");
                return JS_FALSE;
            }

            std::string myUserAgent = "";
            if (!convertFrom(cx, argv[1], myUserAgent)) {
                JS_ReportError(cx, "JSRequestWrapper::Constructor: argument #2 must be an UserAgent");
                return JS_FALSE;
            }

            myNewRequest = OWNERPTR(new JSRequest(myURL, myUserAgent));

        } else {
            myNewRequest = OWNERPTR(new JSRequest(myURL));
        }

        myNewObject = new JSRequestWrapper(myNewRequest, myNewRequest.get());
        myNewRequest->setJSListener(cx, obj);
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1 (URL) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRequestWrapper::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSRequestWrapper::initClass(JSContext *cx, JSObject *theGlobalObject) {
    DOC_CREATE(JSRequestWrapper);
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(),
                ConstIntProperties(), 0, StaticFunctions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSRequestWrapper::NATIVE & theRequest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRequestWrapper::NATIVE >::Class()) {
                theRequest = JSClassTraits<JSRequestWrapper::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, JSRequestWrapper::OWNERPTR & theRequest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRequestWrapper::NATIVE >::Class()) {
                theRequest = JSClassTraits<JSRequestWrapper::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, inet::RequestPtr & theRequest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRequestWrapper::NATIVE >::Class()) {
                theRequest = JSClassTraits<JSRequestWrapper::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSRequestWrapper::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSRequestWrapper::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSRequestWrapper::OWNERPTR theOwner, JSRequestWrapper::NATIVE * theSerial) {
    JSObject * myObject = JSRequestWrapper::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

}
