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
//   $RCSfile: JSTCPServer.cpp,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/04/28 17:12:57 $
//
//
//=============================================================================

#include "JSTCPServer.h"
#include "JSSocket.h"

#include <asl/net.h>
#include <asl/TCPServer.h>
#include <asl/TCPSocket.h>
#include <asl/os_functions.h>
#include <asl/net_functions.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;


#define DB(x) // x

static JSBool
waitForConnection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Starts a blocking waiting for connections");
    DOC_END;
    const inet::TCPServer & myServer = JSTCPServer::getJSWrapper(cx,obj).getNative();
    asl::Ptr<inet::Socket> mySocket(myServer.waitForConnection());
    *rval = as_jsval(cx, mySocket);
    return JS_TRUE;
}


JSTCPServer::~JSTCPServer() {
}

JSFunctionSpec *
JSTCPServer::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"waitForConnection",    waitForConnection,       0},
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
    JSTCPServer * myNewObject = new JSTCPServer(myNewTCPServer, &(*myNewTCPServer));

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
        DOC_MODULE_CREATE("Components", JSTCPServer);
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
    JSObject * myReturnObject = JSTCPServer::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSTCPServer::OWNERPTR theOwner, JSTCPServer::NATIVE * theTCPServer) {
    JSObject * myObject = JSTCPServer::Construct(cx, theOwner, theTCPServer);
    return OBJECT_TO_JSVAL(myObject);
}
