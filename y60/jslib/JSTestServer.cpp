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
#include "JSTestServer.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

template class JSWrapper<asl::ConduitAcceptor<asl::TCPPolicy>,
        asl::Ptr<asl::ConduitAcceptor<asl::TCPPolicy> >, StaticAccessProtocol>;

typedef JSTestServer::NATIVE NATIVE;

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns the name of the test server");
    DOC_RVAL("The string", DOC_TYPE_STRING);
    DOC_END;
    std::string myStringRep = string("Test Server - ") + as_string(JSTestServer::getJSWrapper(cx,obj).getNative().getActiveServerCount());
    *rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

static JSBool
start(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Start the acceptor. It will begin accepting incoming connections & spawning servers.");
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
    DOC_BEGIN("Creates a new test server");
    DOC_PARAM("theURL", "IP-Adress of the server", DOC_TYPE_STRING);
    DOC_PARAM("thePort", "Port number on which the server is listening", DOC_TYPE_INTEGER);
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

        myNewObject = new JSTestServer(myNewTestServer, myNewTestServer.get());
    } else {
        JS_ReportError(cx,"Constructor for %s: bad number of arguments: expected 2 (URL, port) %d",ClassName(), argc);
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
    JSObject * myReturnObject = JSTestServer::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSTestServer::OWNERPTR theOwner, JSTestServer::NATIVE * theSerial) {
    JSObject * myObject = JSTestServer::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}

}

