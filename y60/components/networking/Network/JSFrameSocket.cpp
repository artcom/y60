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

#include "JSFrameSocket.h"
#include "JSSocket.h"
#include <asl/net/Socket.h>

#include <y60/jsbase/JSWrapper.impl>

#include <iostream>

using namespace std;
using namespace asl;
using namespace jslib;


#define DB(x) // x

namespace jslib {
    template class JSWrapper<inet::FrameSocket, asl::Ptr<inet::FrameSocket>,
             jslib::StaticAccessProtocol>;
}

static JSBool
sendFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Send a frame");
    DOC_END;
    inet::FrameSocket & myServer = JSFrameSocket::getJSWrapper(cx,obj).openNative();

    checkArguments("sendFrame", argc, argv, 1);

    std::string myFrame;
    convertFrom(cx, argv[0], myFrame);

    myServer.sendFrame(myFrame);

    *rval = JSVAL_NULL;

    return JS_TRUE;
}

static JSBool
receiveFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Receive a frame");
    DOC_END;
    inet::FrameSocket & myServer = JSFrameSocket::getJSWrapper(cx,obj).openNative();

    std::string myFrame;
    bool myReceived;

    myReceived = myServer.receiveFrame(myFrame);

    if(!myReceived) {
    	*rval = JSVAL_NULL;
    } else {
    	*rval = as_jsval(cx, myFrame);
    }

    return JS_TRUE;
}


JSFrameSocket::~JSFrameSocket() {
}

JSFunctionSpec *
JSFrameSocket::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"sendFrame",    sendFrame,       1},
        {"receiveFrame", receiveFrame,    1},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSFrameSocket::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSFrameSocket::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSFrameSocket::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSBool
JSFrameSocket::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a frame socket");
    DOC_PARAM("theSocket", "Underlying socket", DOC_TYPE_OBJECT);
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    if (argc < 1 ) {
        JS_ReportError(cx, "Constructor for %s: bad number of arguments: expected 2 or 3"
            "(host, port, reuse port flag) got %d", ClassName(), argc);
        return JS_FALSE;
    }

    inet::SocketPtr mySocket;
    if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], mySocket)) {
        JS_ReportError(cx, "JSFrameSocket::Constructor: argument #1 must be a tcp socket");
        return JS_FALSE;
    }

    inet::TCPSocketPtr myTCPSocket = dynamic_cast_Ptr<inet::TCPSocket>(mySocket);
    if(!myTCPSocket) {
    	JS_ReportError(cx, "JSFrameSocket::Constructor: argument #1 must be a tcp socket");
    	return JS_FALSE;
    }

    OWNERPTR myNewFrameSocket = OWNERPTR(new inet::FrameSocket(myTCPSocket));
    JSFrameSocket * myNewObject = new JSFrameSocket(myNewFrameSocket, myNewFrameSocket.get());

    if (myNewObject) {
        JS_SetPrivate(cx, obj, myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSFrameSocket::Constructor: bad parameters");
    return JS_FALSE;
}

JSConstIntPropertySpec *
JSFrameSocket::ConstIntProperties() {

    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

JSObject *
JSFrameSocket::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("Network", JSFrameSocket);
        return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSFrameSocket::NATIVE & theFrameSocket) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSFrameSocket::NATIVE >::Class()) {
                theFrameSocket = JSClassTraits<JSFrameSocket::NATIVE>::getNativeRef(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSFrameSocket::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSFrameSocket::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}

jsval as_jsval(JSContext *cx, JSFrameSocket::OWNERPTR theOwner, JSFrameSocket::NATIVE * theFrameSocket) {
    JSObject * myObject = JSFrameSocket::Construct(cx, theOwner, theFrameSocket);
    return OBJECT_TO_JSVAL(myObject);
}
