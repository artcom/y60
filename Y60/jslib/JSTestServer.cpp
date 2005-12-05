//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSTestServer.h"
#include "JScppUtils.h"
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

typedef JSTestServer::NATIVE NATIVE;
    
static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    std::string myStringRep = string("Test Server - ") + as_string(JSTestServer::getJSWrapper(cx,obj).getNative().getActiveServerCount());
    JSString * myString = JS_NewStringCopyN(cx,myStringRep.c_str(),myStringRep.size());
    *rval = STRING_TO_JSVAL(myString);
    return JS_TRUE;
}

static JSBool
start(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("start the acceptor. It will begin accepting incoming connections & spawning servers.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::start,cx,obj,argc,argv,rval);
}

static JSBool
stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("stop the acceptor. Any active server threads will be stopped.");
    DOC_END;
    return Method<NATIVE>::call(&NATIVE::stop,cx,obj,argc,argv,rval);
}

JSFunctionSpec *
JSTestServer::Functions() {
    AC_DEBUG << "Registering class '"<<ClassName()<<"'"<<endl;
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"start",                start,                     0},
        {"stop",                 stop,                    1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSTestServer::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSTestServer::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSTestServer::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSFunctionSpec *
JSTestServer::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {{0}};
    return myFunctions;
}
 
// getproperty handling
JSBool
JSTestServer::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            case 0:
            default:
                JS_ReportError(cx,"JSTestServer::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
}

// setproperty handling
JSBool
JSTestServer::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
        case 0:
        default:
            JS_ReportError(cx,"JSTestServer::setPropertySwitch: index %d out of range", theID);
            return JS_FALSE;
    }
}

JSBool
JSTestServer::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }
    JSTestServer * myNewObject = 0;

    if (argc >= 2) {
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"JSTestServer::Constructor: bad argument #1 (undefined)");
            return JS_FALSE;
        }

        std::string myAddress = "";
        if (!convertFrom(cx, argv[0], myAddress)) {
            JS_ReportError(cx, "JSTestServer::Constructor: argument #1 must be an Address");
            return JS_FALSE;
        }

        asl::Unsigned16 myPort = 0;
        if (!convertFrom(cx, argv[1], myPort)) {
            JS_ReportError(cx, "JSTestServer::Constructor: argument #2 must be an 16-bit unsigned (port)");
            return JS_FALSE;
        }

        OWNERPTR myNewTestServer = OWNERPTR(new NATIVE(TCPPolicy::Endpoint(myAddress.c_str(), myPort), inet::TestServer::create));

        myNewObject = new JSTestServer(myNewTestServer, &(*myNewTestServer));
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 1 (URL) %d",ClassName(), argc);
        return JS_FALSE;
    }

    if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSTestServer::Constructor: bad parameters");
    return JS_FALSE;
}

JSObject *
JSTestServer::initClass(JSContext *cx, JSObject *theGlobalObject) {
    DOC_CREATE(JSTestServer);
    return Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions());
}

bool convertFrom(JSContext *cx, jsval theValue, JSTestServer::OWNERPTR & theTestServer) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSTestServer::NATIVE >::Class()) {
                theTestServer = JSClassTraits<JSTestServer::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSTestServer::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSTestServer::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSTestServer::OWNERPTR theOwner, JSTestServer::NATIVE * theSerial) {
    JSObject * myObject = JSTestServer::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

}

