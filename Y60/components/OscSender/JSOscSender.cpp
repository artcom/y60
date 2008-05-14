//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSOscSender.h"

#include <y60/JSWrapper.impl>
#include <y60/JSNode.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;


const unsigned READ_BUFFER_SIZE = 20000;

#define DB(x) // x

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
/*    DOC_BEGIN("Returns information on socket connection."); DOC_END;
    const inet::Socket & mySocket = JSOscSender::getJSWrapper(cx,obj).getNative();
    std::string myStringRep = string("Socket [local ip: ") +
        as_dotted_address(mySocket.getLocalAddress()) + ", port: " +
        as_string(mySocket.getLocalPort()) + " | remote ip: " +
        as_dotted_address(mySocket.getRemoteAddress()) + ", port: " +
        as_string(mySocket.getRemotePort()) + "]";
    *rval = as_jsval(cx, myStringRep);*/
    return JS_TRUE;
}

static JSBool
close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Close connection."); DOC_END;
    return Method<JSOscSender::NATIVE>::call(&JSOscSender::NATIVE::close,cx,obj,argc,argv,rval);
}


static JSBool
write(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Write data to connected socket.");
    DOC_PARAM("theEvent", "", DOC_TYPE_NODE);
    DOC_END;
    try {
        if (argc != 1) {
            JS_ReportError(cx, "JSOscSender::write(): Wrong number of arguments, "
                               "expected one (node), got %d.", argc);
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx, "JSOscSender::write(): Argument #0 is undefined");
            return JS_FALSE;
        }
        
        dom::NodePtr myOscEvent;
        if (!convertFrom(cx, argv[0], myOscEvent)) {
            JS_ReportError(cx, "OscSender::write: argument #0 must be a node");
            return JS_FALSE;
        }


        unsigned myBytesWritten = JSOscSender::getJSWrapper(cx,obj).openNative().send(myOscEvent);
        JSOscSender::getJSWrapper(cx,obj).closeNative();

        *rval = as_jsval(cx, myBytesWritten);

        return JS_TRUE;
    }  HANDLE_CPP_EXCEPTION;
}


static JSBool
connect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Connect to socket.");
    DOC_PARAM("theReceiverIPAddress", "IP-address or hostname to connect to", DOC_TYPE_STRING);
    DOC_PARAM("theReceiverPort", "Port number to connect to", DOC_TYPE_INTEGER);
    DOC_PARAM("theSenderPort", "Local port number", DOC_TYPE_INTEGER);
    DOC_RVAL("True if succesful", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        if (argc != 3) {
            JS_ReportError(cx, "OscSender::connect: bad number of arguments: expected 3 "
                "(remote ip adress, remote port, local port), got %d", argc);
            return JS_FALSE;
        }

        if (JSVAL_IS_VOID(argv[0])) {
            JS_ReportError(cx,"Socket::connect: bad argument #1 (undefined)");
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[1])) {
            JS_ReportError(cx,"Socket::connect: bad argument #2 (undefined)");
            return JS_FALSE;
        }
        if (JSVAL_IS_VOID(argv[2])) {
            JS_ReportError(cx,"Socket::connect: bad argument #3 (undefined)");
            return JS_FALSE;
        }

        string myRemoteHostAddressString;
        if (!convertFrom(cx, argv[0], myRemoteHostAddressString)) {
            JS_ReportError(cx, "OscSender::connect: argument #1 must be a string (remote ip address)");
            return JS_FALSE;
        }

        unsigned short myRemotePort;
        if (!convertFrom(cx, argv[1], myRemotePort)) {
            JS_ReportError(cx, "OscSender::connect: argument #2 must be an unsigned short (remote port)");
            return JS_FALSE;
        }
        unsigned short myLocalPort;
        if (!convertFrom(cx, argv[2], myLocalPort)) {
            JS_ReportError(cx, "OscSender::connect: argument #3 must be an unsigned short (local port)");
            return JS_FALSE;
        }
        
        if (JSOscSender::getJSWrapper(cx,obj).openNative().connect(myRemoteHostAddressString, myRemotePort, myLocalPort) ) {
            return JS_TRUE;
        } else {
            return JS_FALSE;
        }
        return JS_TRUE;
    } HANDLE_CPP_EXCEPTION;
}


JSOscSender::~JSOscSender() {
}

JSFunctionSpec *
JSOscSender::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"write",                write,                   1},
        {"close",                close,                   0},
        {"connect",              connect,                 2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSOscSender::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSOscSender::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSOscSender::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSConstIntPropertySpec *
JSOscSender::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

// getproperty handling
JSBool
JSOscSender::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

// setproperty handling
JSBool
JSOscSender::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

JSBool
JSOscSender::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a OscSender. No connection is established.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    JSOscSender * myNewObject = 0;
        OWNERPTR myOscSender = OWNERPTR(new OscSender());
        myNewObject = new JSOscSender(myOscSender, &(*myOscSender));

    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}


JSObject *
JSOscSender::initClass(JSContext *cx, JSObject *theGlobalObject) {    
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    AC_PRINT << "JSOscSender::initClasses";
    DOC_MODULE_CREATE("Components", JSOscSender);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSOscSender::OWNERPTR & theNativePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSOscSender::NATIVE>::Class()) {
                theNativePtr = JSClassTraits<JSOscSender::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}


bool convertFrom(JSContext *cx, jsval theValue, JSOscSender::NATIVE & theNative) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSOscSender::NATIVE >::Class()) {
                theNative = JSClassTraits<JSOscSender::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSOscSender::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSOscSender::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSOscSender::OWNERPTR theOwner, JSOscSender::NATIVE * theSerial) {
    JSObject * myObject = JSOscSender::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
