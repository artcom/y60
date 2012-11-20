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
        AC_WARNING << "Settings: " << *mySettings;

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
        default:
            JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected none or one "
                    "(filepath), got %d", ClassName(), argc);
            return JS_FALSE;
    }

    JSASSOscClient * myNewObject = new JSASSOscClient(myNewNative, myNewNative.get());
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
    JSObject * myReturnObject = JSASSOscClient::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSASSOscClient::OWNERPTR theOwner,
        JSASSOscClient::NATIVE * theASSOscClient)
{
    JSObject * myObject = JSASSOscClient::Construct(cx, theOwner, theASSOscClient);
    return OBJECT_TO_JSVAL(myObject);
}

}
