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

#include "JSOscSender.h"

#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSNode.h>

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
    DOC_PARAM("theSenderAddress", "(optional) Local address to use", DOC_TYPE_STRING);
    DOC_PARAM("theSenderPort", "Local port number", DOC_TYPE_INTEGER);
    DOC_RVAL("True if succesful", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        ensureParamCount(argc, 3, 4, PLUS_FILE_LINE);

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

        string myLocalAddress = "";
        unsigned short myLocalPort;
        if (argc == 3) {
            if (!convertFrom(cx, argv[2], myLocalPort)) {
             JS_ReportError(cx, "OscSender::connect: argument #3 must be an unsigned short (local port)");
             return JS_FALSE;
            }
        } else {
            if (!convertFrom(cx, argv[2], myLocalAddress)) {
             JS_ReportError(cx, "OscSender::connect: argument #3 must be a a string (local address)");
             return JS_FALSE;
            }
            if (!convertFrom(cx, argv[3], myLocalPort)) {
             JS_ReportError(cx, "OscSender::connect: argument #4 must be an unsigned short (local port)");
             return JS_FALSE;
            }
        }

        if (JSOscSender::getJSWrapper(cx,obj).openNative().connect(myRemoteHostAddressString, myRemotePort, myLocalAddress, myLocalPort) ) {
            return JS_TRUE;
        } else {
            return JS_FALSE;
        }
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
        myNewObject = new JSOscSender(myOscSender, myOscSender.get());

    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}


JSObject *
JSOscSender::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    AC_PRINT << "JSOscSender::initClasses";
    DOC_MODULE_CREATE("OscSender", JSOscSender);
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
    JSObject * myReturnObject = JSOscSender::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSOscSender::OWNERPTR theOwner, JSOscSender::NATIVE * theSerial) {
    JSObject * myObject = JSOscSender::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
