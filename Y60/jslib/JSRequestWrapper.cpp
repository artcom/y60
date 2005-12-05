//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSRequestWrapper.cpp,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSRequestWrapper.h"
#include "JScppUtils.h"
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = JSRequestWrapper::getJSWrapper(cx,obj).getNative().getURL();
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
get(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Request a HTTP-GET.");
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::get,cx,obj,argc,argv,rval);
}

static JSBool
post(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Post a string as one block of data.");
    DOC_PARAM("theData", "", DOC_TYPE_STRING);
    DOC_END;
    return Method<inet::Request>::call(&inet::Request::post,cx,obj,argc,argv,rval);
}

static JSBool
postFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Post a file as one block of data.");
    DOC_PARAM("theFilename", "", DOC_TYPE_STRING);
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
    DOC_PARAM("theHeader", "", DOC_TYPE_STRING);
    DOC_RVAL("", DOC_TYPE_STRING);
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

JSFunctionSpec *
JSRequestWrapper::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"get",                  get,                     0},
        {"post",                 post,                    1},
        {"postFile",             postFile,                1},
        {"addHttpHeader",        addHttpHeader,           2},
        {"getResponseHeader",    getResponseHeader,       1},
        {"setTimeoutParams",     setTimeoutParams,        2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSRequestWrapper::Properties() {
    static JSPropertySpec myProperties[] = {
        {"responseCode", PROP_responseCode, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"responseString", PROP_responseString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"errorString", PROP_errorString, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {"URL", PROP_URL, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
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
    static JSFunctionSpec myFunctions[] = {{0}};
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
            case PROP_errorString:
                *vp = as_jsval(cx, getNative().getErrorString());
                return JS_TRUE;
            case PROP_URL:
                *vp = as_jsval(cx, getNative().getURL());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSRequestWrapper::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSRequestWrapper::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_responseCode:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
        default:
            JS_ReportError(cx,"JSRequestWrapper::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSRequestWrapper::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
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

        OWNERPTR myNewRequest = OWNERPTR(new JSRequest(myURL));

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

        }
                    
        myNewObject = new JSRequestWrapper(myNewRequest, &(*myNewRequest));
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
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
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
    JSObject * myReturnObject = JSRequestWrapper::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSRequestWrapper::OWNERPTR theOwner, JSRequestWrapper::NATIVE * theSerial) {
    JSObject * myObject = JSRequestWrapper::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

}
