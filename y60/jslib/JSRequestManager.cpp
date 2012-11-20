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
#include "JSRequestManager.h"

#include "JSRequestWrapper.h"
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<jslib::JSRequestManagerAdapter, asl::Ptr<jslib::JSRequestManagerAdapter>, StaticAccessProtocol >;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("a string repesentation of the request manager.");
    DOC_END;
    std::string myStringRep = as_string(JSRequestManager::getJSWrapper(cx,obj).getNative().getActiveCount());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
performRequest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Add HTTP-Request to request queue.");
    DOC_PARAM("theRequest", "", DOC_TYPE_REQUEST);
    DOC_END;
    return Method<inet::RequestManager>::call(&inet::RequestManager::performRequest,cx,obj,argc,argv,rval);
}

static JSBool
handleRequests(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Process handle request routines, must be called in a loop, until all 'activeCount' requests are handled.");
    DOC_END;
    ensureParamCount(argc, 0,1);
    JSRequestManager::NATIVE * myNative = 0;
    if (!convertFrom(cx, OBJECT_TO_JSVAL(obj), myNative)) {
        JS_ReportError(cx, "JSRequestManager::handleRequests: self is not a RequestManager");
        return JS_FALSE;
    }
    if (argc == 0 ) {
        myNative->handleRequests();
    } else {
        bool myBlockingFlag = false;
        if ( ! convertFrom(cx, argv[1], myBlockingFlag)) {
            JS_ReportError(cx, "argument 1 must be a bool");
            return JS_FALSE;
        }
        myNative->handleRequests(myBlockingFlag);
    }
    return JS_TRUE;
}

JSFunctionSpec *
JSRequestManager::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"performRequest",       performRequest,          1},
        {"handleRequests",       handleRequests,          1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSRequestManager::Properties() {
    static JSPropertySpec myProperties[] = {
        {"activeCount", PROP_ACTIVE_COUNT, JSPROP_READONLY|JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED},
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSRequestManager::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSRequestManager::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSRequestManager::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}

// getproperty handling
JSBool
JSRequestManager::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case PROP_ACTIVE_COUNT:
                *vp = as_jsval(cx, getNative().getActiveCount());
                return JS_TRUE;
            default:
                JS_ReportError(cx,"JSRequestManager::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSRequestManager::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case PROP_ACTIVE_COUNT:
            //jsval dummy;
            //return Method<NATIVE>::call(&NATIVE::isOpen, cx, obj, 1, vp, &dummy);
            return JS_FALSE;
        default:
            JS_ReportError(cx,"JSRequestManager::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSRequestManager::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Construct a Request Manager.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSRequestManager * myNewObject = 0;

    if (argc == 0) {
        OWNERPTR myNewManager = OWNERPTR(new jslib::JSRequestManagerAdapter());
        myNewObject = new JSRequestManager(myNewManager, myNewManager.get());
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 0, got %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSRequestManager::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSRequestManager::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());

    DOC_CREATE(JSRequestManager);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSRequestManager::NATIVE & theRequest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRequestManager::NATIVE >::Class()) {
                theRequest = JSClassTraits<JSRequestManager::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}
bool convertFrom(JSContext *cx, jsval theValue, JSRequestManager::NATIVE *& theRequest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRequestManager::NATIVE >::Class()) {
                theRequest = JSClassTraits<JSRequestManager::NATIVE>::getNativeOwner(cx,myArgument).get();
                return true;
            }
        }
    }
    return false;
}

bool convertFrom(JSContext *cx, jsval theValue, JSRequestManager::OWNERPTR & theRequest) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSRequestManager::NATIVE >::Class()) {
                theRequest = JSClassTraits<JSRequestManager::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSRequestManager::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSRequestManager::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSRequestManager::OWNERPTR theOwner, JSRequestManager::NATIVE * theSerial) {
    JSObject * myObject = JSRequestManager::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

}
