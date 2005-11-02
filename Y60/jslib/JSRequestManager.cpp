//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: JSRequestManager.cpp,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2005/04/28 17:12:58 $
//
//
//=============================================================================

#include "JSRequestWrapper.h"
#include "JSRequestManager.h"
#include "JScppUtils.h"
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = as_string(JSRequestManager::getJSWrapper(cx,obj).getNative().getActiveCount());
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
performRequest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Add HTTP-Request to request queue.");
    DOC_PARAM("theRequest", "", DOC_TYPE_REQUEST);    
    DOC_END;
    return Method<JSRequestManager::NATIVE>::call(&JSRequestManager::NATIVE::performRequest,cx,obj,argc,argv,rval);
}

static JSBool
handleRequests(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Process handle request routines, must be called in a loop, til all 'activeCount' requests are handled.");
    DOC_END;
    return Method<JSRequestManager::NATIVE>::call(&JSRequestManager::NATIVE::handleRequests,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSRequestManager::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"performRequest",       performRequest,          1},
        {"handleRequests",       handleRequests,          0},
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
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSRequestManager * myNewObject = 0;

    if (argc == 0) {
        OWNERPTR myNewManager = OWNERPTR(new inet::RequestManager());
        myNewObject = new JSRequestManager(myNewManager, &(*myNewManager));
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
    JSObject * myReturnObject = JSRequestManager::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSRequestManager::OWNERPTR theOwner, JSRequestManager::NATIVE * theSerial) {
    JSObject * myObject = JSRequestManager::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

}
