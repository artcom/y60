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

#include "JSHttpServer.h"
#include "NetAsync.h"

#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSNode.h>

#include <iostream>

#include <netsrc/spidermonkey/jsfun.h>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;


const unsigned READ_BUFFER_SIZE = 20000;

#define DB(x) // x

static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
/*    DOC_BEGIN("Returns information on socket connection."); DOC_END;
    const inet::Socket & mySocket = JSHttpServer::getJSWrapper(cx,obj).getNative();
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
    return Method<JSHttpServer::NATIVE>::call(&JSHttpServer::NATIVE::close,cx,obj,argc,argv,rval);
}

static JSBool
registerCallback(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Register a callback function for an uri"); DOC_END;

    std::string myUri;
    if (!convertFrom(cx, argv[0], myUri)) {
        JS_ReportError(cx, "HttpServer::registerCallback: argument #1 is not a string");
        return JS_FALSE;
    }

    JSObject* myCallingObject;
    if (!JSVAL_IS_OBJECT(argv[1])) {
        JS_ReportError(cx, "HttpServer::registerCallback: argument #2 is not an object");
        return JS_FALSE;
    } 
    JS_ValueToObject(cx,argv[1],&myCallingObject);

    if (!JS_ValueToFunction(cx, argv[2])) {
        JS_ReportError(cx, "Argument 3 is not a function!" );
        return JS_FALSE;
    }
    
    std::string myContentType = "text/plain";
    if (argc > 3 && !convertFrom(cx, argv[3], myContentType)) {
        JS_ReportError(cx, "HttpServer::registerCallback: argument #4 is not a string");
        return JS_FALSE;
    }

    async::http::JSCallback myCallback;
    myCallback.context = cx;
    myCallback.object = myCallingObject;
    myCallback.functionValue = argv[2];
    myCallback.contentType = myContentType;

    async::http::Server & myNative = JSHttpServer::getJSWrapper(cx, obj).openNative();
    myNative.registerCallback( obj, myUri, myCallback );

    return JS_TRUE;
}


static JSBool
requestsPending( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("check if there are any requests waiting to be processed"); DOC_END;

    return Method<JSHttpServer::NATIVE>::call(&JSHttpServer::NATIVE::requestsPending, cx, obj, argc, argv, rval);
}

static JSBool
handleRequests(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("handle requests..."); DOC_END;
               
    return Method<JSHttpServer::NATIVE>::call(&JSHttpServer::NATIVE::handleRequest,cx,obj,argc,argv,rval);

}


static JSBool
start(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Connect to socket.");
    DOC_PARAM("theReceiverIPAddress", "IP-address or hostname to connect to", DOC_TYPE_STRING);
    DOC_PARAM("theReceiverPort", "Port number to connect to", DOC_TYPE_INTEGER);
    DOC_PARAM("theSenderPort", "Local port number", DOC_TYPE_INTEGER);
    DOC_RVAL("True if succesful", DOC_TYPE_BOOLEAN);
    DOC_END;
    try {
        if (argc != 2) {
            JS_ReportError(cx, "HttpServer::start: bad number of arguments: expected 2 "
                "(remote ip adress, remote port, local port), got %d", argc);
            return JS_FALSE;
        }

        string myRemoteHostAddressString;
        if (!convertFrom(cx, argv[0], myRemoteHostAddressString)) {
            JS_ReportError(cx, "HttpServer::start: argument #1 must be a string (remote ip address)");
            return JS_FALSE;
        }

        string myRemotePort;
        if (!convertFrom(cx, argv[1], myRemotePort)) {
            JS_ReportError(cx, "HttpServer::start: argument #2 must be a string (remote port)");
            return JS_FALSE;
        }
        
        if (JSHttpServer::getJSWrapper(cx,obj).openNative().start( myRemoteHostAddressString, myRemotePort ) ) {
            return JS_TRUE;
        } else {
            return JS_FALSE;
        }
    } HANDLE_CPP_EXCEPTION;
} 


JSHttpServer::~JSHttpServer() {
}

JSFunctionSpec *
JSHttpServer::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {"close",                close,                   0},
        {"start",                start,                   2},
        {"requestsPending",      requestsPending,         0},
        {"handleRequests",       handleRequests,          0},
        {"registerCallback",     registerCallback,        2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSHttpServer::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSHttpServer::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSHttpServer::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSConstIntPropertySpec *
JSHttpServer::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

// getproperty handling
JSBool
JSHttpServer::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

// setproperty handling
JSBool
JSHttpServer::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}

JSBool
JSHttpServer::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a HttpServer.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    asl::Ptr<NetAsync> parentPlugin = dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName));

    JSHttpServer * myNewObject = 0;
    OWNERPTR myHttpServer = OWNERPTR(new async::http::Server(cx, parentPlugin->io_service()));
    myNewObject = new JSHttpServer(myHttpServer, myHttpServer.get());

    JS_SetPrivate(cx, obj, myNewObject);
    return JS_TRUE;
}


JSObject *
JSHttpServer::initClass(JSContext *cx, JSObject *theGlobalObject) {    
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("HttpServer", JSHttpServer);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSHttpServer::OWNERPTR & theNativePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSHttpServer::NATIVE>::Class()) {
                theNativePtr = JSClassTraits<JSHttpServer::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSHttpServer::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSHttpServer::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSHttpServer::OWNERPTR theOwner, JSHttpServer::NATIVE * theSerial) {
    JSObject * myObject = JSHttpServer::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
