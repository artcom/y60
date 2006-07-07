//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSCMSCache.h"


#include <asl/PackageManager.h>

#include <iostream>

using namespace std;
using namespace y60;

#define DB(x) // x

namespace jslib {

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
isSynchronized(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    return Method<JSCMSCache::NATIVE>::call(&JSCMSCache::NATIVE::isSynchronized,cx,obj,argc,argv,rval);
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
        {"synchronize",     synchronize,                0},
        {"isSynchronized",     isSynchronized,            0},
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
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSCMSCache::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSCMSCache::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSCMSCache::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
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
    
    std::string myUsername;
    std::string myPassword;
    std::string myServerURL;
    std::string myLocalPath;
    std::string myPresentationFile;
        
    switch (argc) {
        case 5:
            convertFrom(cx, argv[3], myUsername);
            convertFrom(cx, argv[4], myPassword);
            // XXX don't break here
        case 3:
            convertFrom(cx, argv[0], myServerURL);
            convertFrom(cx, argv[1], myLocalPath);
            convertFrom(cx, argv[2], myPresentationFile);
            break;
        default:
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected three or five "
                    ", got %d", ClassName(), argc);
            return JS_FALSE;
    }
    
    switch (argc) {
        case 3:
            myNewNative = OWNERPTR(new CMSCache(myServerURL, myLocalPath, myPresentationFile));
            break;    
        case 5:
            myNewNative = OWNERPTR(new CMSCache(myServerURL, myLocalPath, myPresentationFile,
                                                  myUsername, myPassword));
            break;
    }

    JSCMSCache * myNewObject = new JSCMSCache(myNewNative, &(*myNewNative));
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
    JSObject * myReturnObject = JSCMSCache::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSCMSCache::OWNERPTR theOwner, JSCMSCache::NATIVE * theCMSCache) {
    JSObject * myObject = JSCMSCache::Construct(cx, theOwner, theCMSCache);
    return OBJECT_TO_JSVAL(myObject);
}

}
