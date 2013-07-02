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

#include "JSWebSocketClient.h"

#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSBlock.h>
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
    DOC_BEGIN("Returns information on websocket client."); DOC_END;
    const async::websocket::Client & myClient = JSWebSocketClient::getJSWrapper(cx,obj).getNative();
    *rval = as_jsval(cx, myClient.debugIdentifier);
    return JS_TRUE;
}

JSWebSocketClient::~JSWebSocketClient() {
}

JSFunctionSpec *
JSWebSocketClient::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSWebSocketClient::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSPropertySpec *
JSWebSocketClient::StaticProperties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSFunctionSpec *
JSWebSocketClient::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        {0}
    };
    return myFunctions;
}

JSConstIntPropertySpec *
JSWebSocketClient::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
};

// getproperty handling
JSBool
JSWebSocketClient::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    switch (theID) {
            default:
                JS_ReportError(cx,"JSRequestWrapper::getProperty: index %d out of range", theID);
                return JS_FALSE;
    }
    return JS_TRUE;
}

// setproperty handling
JSBool
JSWebSocketClient::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_TRUE;
}


JSBool
JSWebSocketClient::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates a WebSocketClient.");
    DOC_END;
    if (JSA_GetClass(cx,obj) != Class()) {
        JS_ReportError(cx,"Constructor for %s bad object; did you forget a 'new'?", ClassName());
        return JS_FALSE;
    }

    JSObject* optsObject;
    if (!JSVAL_IS_OBJECT(argv[0])) {
        JS_ReportError(cx, "WebSocketClient::Construct: argument #1 is not an object");
        return JS_FALSE;
    } 
    JS_ValueToObject(cx,argv[0],&optsObject);
    
    JSWebSocketClient * myNewObject = 0;
    OWNERPTR myWebSocketClient = OWNERPTR(new async::websocket::Client(cx, optsObject));
    myNewObject = new JSWebSocketClient(myWebSocketClient, myWebSocketClient.get());
    JS_SetPrivate(cx, obj, myNewObject);
    myWebSocketClient->setWrapper(obj);
    // now stick the opts object to the JSWebSocketClient wrapper so it's not GC'ed
    jsval optsValue = OBJECT_TO_JSVAL(optsObject);
    JS_SetProperty(cx, obj, "_opts", &optsValue);

    return JS_TRUE;
}


JSObject *
JSWebSocketClient::initClass(JSContext *cx, JSObject *theGlobalObject) {    
    JSObject *myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), Functions(), ConstIntProperties());
    DOC_MODULE_CREATE("WebSocketClient", JSWebSocketClient);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, JSWebSocketClient::OWNERPTR & theNativePtr) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSWebSocketClient::NATIVE>::Class()) {
                theNativePtr = JSClassTraits<JSWebSocketClient::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSWebSocketClient::OWNERPTR theOwner) {
    JSObject * myReturnObject = JSWebSocketClient::Construct(cx, theOwner, theOwner.get());
    return OBJECT_TO_JSVAL(myReturnObject);
}


jsval as_jsval(JSContext *cx, JSWebSocketClient::OWNERPTR theOwner, JSWebSocketClient::NATIVE * theSerial) {
    JSObject * myObject = JSWebSocketClient::Construct(cx, theOwner, theSerial);
    return OBJECT_TO_JSVAL(myObject);
}
