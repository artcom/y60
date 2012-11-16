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

#include "JSTCPServer.h"
#include "JSSocket.h"

#include <asl/net/net.h>
#include <asl/net/TCPServer.h>
#include <asl/net/TCPSocket.h>
#include <asl/base/os_functions.h>
#include <asl/net/net_functions.h>
#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;


#define DB(x) // x

namespace jslib {
    template class JSWrapper<inet::TCPServer, asl::Ptr<inet::TCPServer>,
             jslib::StaticAccessProtocol>;
}

static JSBool
waitForConnection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Starts a blocking waiting for connections");
    DOC_END;
    const inet::TCPServer & myServer = JSTCPServer::getJSWrapper(cx,obj).getNative();
    asl::Ptr<inet::Socket> mySocket(myServer.waitForConnection());
    if(mySocket) {
    	*rval = as_jsval(cx, mySocket);
    } else {
    	*rval = JSVAL_NULL;
    }
    return JS_TRUE;
}

static JSBool
setBlockingMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Set blocking mode on connection");
    DOC_PARAM("isBlocking", "If false, a read call on the socket will only return if a packet has been received", DOC_TYPE_BOOLEAN);
    DOC_END;
    return Method<JSTCPServer::NATIVE>::call(&JSTCPServer::NATIVE::setBlockingMode,cx,obj,argc,argv,rval);
}


JSTCPServer::~JSTCPServer() {
}

JSFunctionSpec *
JSTCPServer::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"waitForConnection",    waitForConnection,       0},
        {"setBlockingMode",      setBlockingMode,         1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSTCPServer::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSTCPServer::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSTCPServer::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSBool
JSTCPServer::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a TCP Server. Does not start the server.");
    DOC_PARAM("theIPAddress", "The ipadress or hostname the server is started on", DOC_TYPE_STRING);
    DOC_PARAM("thePort", "The tcp-port the server is listening at", DOC_TYPE_INTEGER);
    DOC_PARAM_OPT("theReusePortFlag", "If true, the server does not open a new port for each connection", DOC_TYPE_BOOLEAN, false);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    if (argc < 2 || argc > 3) {
        JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected 2 or 3"
            "(host, port, reuse port flag) got %d", ClassName(), argc);
        return JS_FALSE;
    }

    string myHostAddressString;
    if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myHostAddressString)) {
        JS_ReportError(cx, "JSTCPServer::Constructor: argument #1 must be a string (ip address)");
        return JS_FALSE;
    }

    unsigned short myPort;
    if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myPort)) {
        JS_ReportError(cx, "JSTCPServer::Constructor: argument #2 must be an unsigned short (port)");
        return JS_FALSE;
    }

    bool myReusePortFlag = false;
    if (argc > 2) {
        if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myReusePortFlag)) {
            JS_ReportError(cx, "JSTCPServer::Constructor: argument #3 must be a bool (reuse port flag)");
            return JS_FALSE;
        }
    }
    unsigned long myHostAddress = asl::hostaddress(myHostAddressString);
    OWNERPTR myNewTCPServer = OWNERPTR(new inet::TCPServer(myHostAddress, myPort, myReusePortFlag));
    JSTCPServer * myNewObject = new JSTCPServer(myNewTCPServer, myNewTCPServer.get());

    if (myNewObject) {
        JS_SetPrivate(cx, obj, myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSTCPServer::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSTCPServer::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSObject *
JSTCPServer::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("Network", JSTCPServer);
        return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSTCPServer::NATIVE & theTCPServer) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSTCPServer::NATIVE >::Class()) {
                theTCPServer = JSClassTraits<JSTCPServer::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSTCPServer::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSTCPServer::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSTCPServer::OWNERPTR theOwner, JSTCPServer::NATIVE * theTCPServer) {
    JSObject * myObject = JSTCPServer::Construct(cx, theOwner, theTCPServer);
    return OBJECT_TO_JSVAL(myObject);
}
