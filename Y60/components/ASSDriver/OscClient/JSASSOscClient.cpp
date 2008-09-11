//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSASSOscClient.cpp,v $
//   $Author: christian $
//   $Revision: 1.12 $
//   $Date: 2005/04/28 17:12:56 $
//
//
//=============================================================================

#include "JSASSOscClient.h"

#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSNode.h>

using namespace std;
using namespace asl;
using namespace y60;
using namespace dom;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    *rval = as_jsval(cx, "ASSOscClient");
    return JS_TRUE;
}

static JSBool
Poll(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<ASSOscClient>::ScopedNativeRef myObj(cx, obj);

        if (argc != 0) {
            JS_ReportError(cx, "JSASSOscClient::poll(): Wrong number of arguments, "
                    "expected 0, got %d.", argc);
            return JS_FALSE;
        }

        ASSOscClient & myOscClient = myObj.getNative();
        myOscClient.poll();

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}

static JSBool
OnUpdateSettings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        JSClassTraits<ASSOscClient>::ScopedNativeRef myObj(cx, obj);

        if (argc != 1) {
            JS_ReportError(cx, "JSASSOscClient::onUpdateSettings(): Wrong number of arguments, "
                    "expected 1 (theSettings), got %d.", argc);
            return JS_FALSE;
        }

        dom::NodePtr mySettings;
        convertFrom(cx, argv[0], mySettings);

        ASSOscClient & myOscClient = myObj.getNative();
        myOscClient.onUpdateSettings( mySettings );

        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JSConstIntPropertySpec *
JSASSOscClient::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSFunctionSpec *
JSASSOscClient::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"poll",                 Poll,                    0},
        {"onUpdateSettings",     OnUpdateSettings,        1},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSASSOscClient::StaticFunctions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}


JSPropertySpec *
JSASSOscClient::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

// getproperty handling
JSBool
JSASSOscClient::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSASSOscClient::getProperty: index %d out of range", theID);
            return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSASSOscClient::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSASSOscClient::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSASSOscClient::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    OWNERPTR myNewNative;
    switch (argc) {
        case 0:
            myNewNative = OWNERPTR(new ASSOscClient());
            break;
        case 1:
            AC_PRINT << "TODO: parse port number";
            myNewNative = OWNERPTR(new ASSOscClient());
            break;
        default:
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected none or one "
                    "(filepath), got %d", ClassName(), argc);
            return JS_FALSE;
    }

    JSASSOscClient * myNewObject = new JSASSOscClient(myNewNative, &(*myNewNative));
    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}

JSObject *
JSASSOscClient::initClass(JSContext *cx, JSObject *theGlobalObject) {
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
            Properties(), Functions(), ConstIntProperties(), 0, StaticFunctions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSASSOscClient::OWNERPTR & theASSOscClient) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSASSOscClient::NATIVE >::Class()) {
                theASSOscClient = JSClassTraits<JSASSOscClient::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSASSOscClient::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSASSOscClient::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSASSOscClient::OWNERPTR theOwner,
        JSASSOscClient::NATIVE * theASSOscClient)
{
    JSObject * myObject = JSASSOscClient::Construct(cx, theOwner, theASSOscClient);
    return OBJECT_TO_JSVAL(myObject);
}

}
