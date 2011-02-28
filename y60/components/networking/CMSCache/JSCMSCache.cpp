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

#include "JSCMSCache.h"
#include <y60/jsbase/JSEnum.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSWrapper.impl>


#include <asl/zip/PackageManager.h>

#include <iostream>

using namespace std;
using namespace y60;

#define DB(x) // x

namespace jslib {

template class JSWrapper<y60::CMSCache, asl::Ptr<y60::CMSCache> , jslib::StaticAccessProtocol>;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    *rval = as_jsval(cx, "CMSCache");
    return JS_TRUE;
}

static JSBool
synchronize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCMSCache::NATIVE>::call(&JSCMSCache::NATIVE::synchronize,cx,obj,argc,argv,rval);
}
static JSBool
setProxy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCMSCache::NATIVE>::call(&JSCMSCache::NATIVE::setProxy,cx,obj,argc,argv,rval);
}

static JSBool
isSynchronized(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCMSCache::NATIVE>::call(&JSCMSCache::NATIVE::isSynchronized,cx,obj,argc,argv,rval);
}

static JSBool
testConsistency(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return JS_FALSE;
    // return Method<JSCMSCache::NATIVE>::call(&JSCMSCache::NATIVE::testConsistency,cx,obj,argc,argv,rval);
}

#define DEFINE_ORIENTATION_PROP(NAME) { #NAME, PROP_ ## NAME , CMSCache::NAME }

JSConstIntPropertySpec *
JSCMSCache::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSFunctionSpec *
JSCMSCache::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"synchronize",        synchronize,               0},
        {"isSynchronized",     isSynchronized,            0},
        {"testConsistency",    testConsistency,           0},
        {"setProxy",           setProxy,                  1},
        {"toString",           toString,                  0},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSCMSCache::StaticFunctions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSCMSCache::Properties() {
    static JSPropertySpec myProperties[] = {
        {"statusReport", PROP_statusReport,
                JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY},
        {"localPath", PROP_localPath,
                JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_READONLY},
        {"verbose", PROP_verbose, JSPROP_ENUMERATE | JSPROP_PERMANENT},
        {"cleanup", PROP_cleanup, JSPROP_ENUMERATE | JSPROP_PERMANENT},
        {"maxRequests", PROP_maxRequests, JSPROP_ENUMERATE | JSPROP_PERMANENT},
        {"userAgent", PROP_userAgent, JSPROP_ENUMERATE | JSPROP_PERMANENT},
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSCMSCache::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    switch (theID) {
        case PROP_statusReport:
            *vp = as_jsval( cx, static_cast_Ptr<dom::Node>( myObj.getNative().getStatusReport()));
            return JS_TRUE;
        case PROP_localPath:
            *vp = as_jsval( cx, myObj.getNative().getLocalPath());
            return JS_TRUE;
        case PROP_verbose:
            *vp = as_jsval( cx,  myObj.getNative().getVerboseFlag());
            return JS_TRUE;
        case PROP_cleanup:
            *vp = as_jsval( cx,  myObj.getNative().getCleanupFlag());
            return JS_TRUE;
        case PROP_maxRequests:
            *vp = as_jsval( cx,  myObj.getNative().getMaxRequestCount());
            return JS_TRUE;
        case PROP_userAgent:
            *vp = as_jsval( cx,  myObj.getNative().getUserAgent());
            return JS_TRUE;
        default:
            JS_ReportError(cx,"JSCMSCache::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSCMSCache::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    JSClassTraits<NATIVE>::ScopedNativeRef myObj(cx, obj);
    switch (theID) {
        case PROP_verbose:
            try {
                bool myVerboseFlag;
                convertFrom(cx, *vp, myVerboseFlag );
                myObj.getNative().setVerboseFlag( myVerboseFlag );
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_cleanup:
            try {
                bool myCleanupFlag;
                convertFrom(cx, *vp, myCleanupFlag );
                myObj.getNative().setCleanupFlag( myCleanupFlag );
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;        case PROP_maxRequests:
            try {
                unsigned myMaxRequestCount;
                convertFrom(cx, *vp, myMaxRequestCount );
                myObj.getNative().setMaxRequestCount( myMaxRequestCount );
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        case PROP_userAgent:
            try {
                string myUserAgent;
                convertFrom(cx, *vp, myUserAgent );
                myObj.getNative().setUserAgent( myUserAgent );
                return JS_TRUE;
            } HANDLE_CPP_EXCEPTION;
        default:
            JS_ReportError(cx,"JSCMSCache::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSCMSCache::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    OWNERPTR myNewNative;

    std::string myLocalPath;
    dom::NodePtr myPresentationDoc;
    y60::BackendType myBackendType;
    std::string myUsername;
    std::string myPassword;
    std::string mySessionCookie;

    switch (argc) {
        case 6:
            convertFrom(cx, argv[5], mySessionCookie);
        case 5:
            convertFrom(cx, argv[3], myUsername);
            convertFrom(cx, argv[4], myPassword);
            // XXX don't break here
        case 3:
            convertFrom(cx, argv[0], myLocalPath);
            convertFrom(cx, argv[1], myPresentationDoc);
            convertFrom(cx, argv[2], myBackendType);
            break;
        default:
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected three or five "
                    ", got %d", ClassName(), argc);
            return JS_FALSE;
    }

    switch (argc) {
        case 3:
            myNewNative = OWNERPTR(new CMSCache(myLocalPath, myPresentationDoc,
                                                myBackendType));
            break;
        case 5:
            myNewNative = OWNERPTR(new CMSCache(myLocalPath, myPresentationDoc,
                                                myBackendType, myUsername, myPassword));
            break;
        case 6:
            myNewNative = OWNERPTR(new CMSCache(myLocalPath, myPresentationDoc,
                                                myBackendType, myUsername, myPassword,
                                                mySessionCookie));
            break;
    }

    JSCMSCache * myNewObject = new JSCMSCache(myNewNative, myNewNative.get());
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSObject *
JSCMSCache::initClass(JSContext *cx, JSObject *theGlobalObject) {
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties(), 0, StaticFunctions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSCMSCache::OWNERPTR & theCMSCache) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSCMSCache::NATIVE >::Class()) {
                theCMSCache = JSClassTraits<JSCMSCache::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSCMSCache::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSCMSCache::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSCMSCache::OWNERPTR theOwner, JSCMSCache::NATIVE * theCMSCache) {
    JSObject * myObject = JSCMSCache::Construct(cx, theOwner, theCMSCache);
    return OBJECT_TO_JSVAL(myObject);
}

}
